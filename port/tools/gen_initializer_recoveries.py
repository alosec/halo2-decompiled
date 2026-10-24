#!/usr/bin/env python3
"""Write port/recovered/retail_initializers_generated.cpp: the retail C++
dynamic initialisers (the CRT's __xc table) and h2_run_retail_initializers,
which runs them in table order.

Retail's CRT runs the table before main(); the whole image only runs its own
static constructors (.guest_init), so globals that upstream declares without
an initial value stay zero (the default field of view, g_5234dc, set by
0x3763b0, is the classic one). The table lives in retail .data:

  0x461180  0                    start marker
  0x461184  __mtinit, __ioinit   the CRT's own (skipped)
  0x46118c  0, 0                 end / start markers
  0x461194  415 C++ initialisers (TABLE_START..TABLE_END, this script)
  0x461810  0, 0                 end / start markers
  0x461818  5 C initialisers returning int (XOnline's, ___onexitinit,
            ___initstdio, ___initmbctable, the unhandled-exception filter;
            skipped: library internals)
  0x461834  ___endstdio; 0x461840 0x328168; 0x46184c 0x3b0272 (pre-
            terminators and terminators; never run by the port)

Each initialiser is interpreted symbolically, one straight-line x86 function
at a time, into C++:
  - loads from read-only retail data (.rdata) become literals, with the
    address they came from in a comment; loads from writable data are read at
    run time into a local first (an earlier initialiser may have set them)
  - stores become writes through a pointer cast of the retail address
  - sse arithmetic (subss/mulss/divss/addss) becomes float arithmetic, and
    fld/fcos/fstp becomes (float)__builtin_cos
  - `mov ecx, G; push args; call C` (or `mov eax/esi/edi, G; call/jmp C`, the
    register-`this` constructors of the game's link-time code generation)
    becomes h2_retail_C(G, args), the native function recovered for C
  - `push D; call _atexit` (0x320fb4) is dropped: the port never runs exit
    handlers, so the destructor D is only named in a comment
Anything the interpreter does not handle (loops, call results, code addresses
as arguments) must be listed in HAND (port/recovered/retail_initializers.cpp)
or SKIP, or the script stops.

usage: gen_initializer_recoveries.py [--xbe orig/game/default.xbe]"""
import argparse
import csv
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
from retail_dis import OBJDUMP, read, sections  # noqa: E402

OUTPUT = ROOT / "port/recovered/retail_initializers_generated.cpp"
TABLE_START = 0x461194
TABLE_END = 0x461810  # the zero word after the last entry
ATEXIT = 0x320fb4

# initialisers already recovered elsewhere under their own @retail tag: the
# runner calls them through h2_retail_<address>
EXISTING = {
    0x378ad0: "havok_base_system.cpp (singleton node)",
    0x378af0: "havok_base_system.cpp (singleton node)",
    0x378b10: "havok_base_system.cpp (singleton node)",
    0x378ba0: "havok_base_system.cpp (singleton node)",
    0x378c00: "havok_base_system.cpp (singleton node)",
    0x379630: "havok_base_system.cpp (singleton node)",
    0x379650: "havok_base_system.cpp (singleton node)",
    0x379670: "havok_base_system.cpp (singleton node)",
    0x3772d0: "havok_memory_stack.cpp (the Havok memory's stack area)",
}

# hand-written in port/recovered/retail_initializers.cpp as
# retail_initializer_<address>
HAND = {
    0x376bc0: "a port number through htons (XNet 0x3ce184)",
    0x376be0: "a port number through htons (XNet 0x3ce184)",
    0x3769c0: "a loop clearing one byte of 16 records",
    0x377ef0: "the main thread's id (GetCurrentThreadId)",
    0x3781d1: "vector_constructor_iterator with a constructor's code address",
    0x3781e8: "vector_constructor_iterator with a constructor's code address",
}

# library initialisers in the table, not run by the port
SKIP = {
    0x40a04a: "XDK (XPP section): fills ten words of its own data with one constant",
    0x37daca: "XDK (DSOUND section): DirectSound's own function-pointer words",
    0x37dad5: "XDK (DSOUND section): DirectSound's own function-pointer words",
    0x3808a8: "XDK (DSOUND section): DirectSound's own function-pointer words",
    0x3808b3: "XDK (DSOUND section): DirectSound's own function-pointer words",
    0x41130f: "XDK (XPP section): the peripheral port's device records",
    0x41133f: "XDK (XPP section): the peripheral port's device records",
    0x3f76e0: "XDK (D3D section): Direct3D's default render-state block",
}

GPR32 = ["eax", "ecx", "edx", "ebx", "esi", "edi"]
SUB = {"ax": "eax", "cx": "ecx", "dx": "edx", "bx": "ebx", "si": "esi", "di": "edi",
       "al": "eax", "cl": "ecx", "dl": "edx", "bl": "ebx"}
WIDTH = {"dword": ("unsigned int", 0xffffffff), "word": ("unsigned short", 0xffff),
         "byte": ("unsigned char", 0xff)}
FLOAT_OPS = {"addss": "+", "subss": "-", "mulss": "*", "divss": "/"}


class Unsupported(Exception):
    pass


def float_literal(bits):
    value = struct.unpack("<f", struct.pack("<I", bits))[0]
    if value != value or value in (float("inf"), float("-inf")):
        return f"float_bits(0x{bits:08x})"
    for precision in range(1, 10):
        text = f"{value:.{precision}g}"
        try:
            if struct.unpack("<I", struct.pack("<f", float(text)))[0] == bits:
                break
        except OverflowError:  # rounded past the largest float
            continue
    text = repr(float(text))  # the shortest round-tripping digits, as 40.0 not 4e+01
    if text.endswith(".0") and "e" in text:
        text = text[:-2]
    return text + "f"


def double_literal(raw):
    value = struct.unpack("<d", raw)[0]
    return repr(value)


class Image:
    def __init__(self, xbe_path):
        self.xbe = xbe_path.read_bytes()
        self.secs = sections(self.xbe)
        base = struct.unpack_from("<I", self.xbe, 0x104)[0]
        headers = struct.unpack_from("<I", self.xbe, 0x120)[0] - base
        self.flags = []
        for index in range(len(self.secs)):
            flags, va, vsize = struct.unpack_from("<III", self.xbe, headers + index * 56)
            self.flags.append((va, va + vsize, flags))
        with (ROOT / "config/functions.csv").open(newline="") as handle:
            self.rows = {int(row["va"], 16): row for row in csv.DictReader(handle)}

    def section(self, address):
        for (name, va, vsize, _, _), (_, _, flags) in zip(self.secs, self.flags):
            if va <= address < va + vsize:
                return name, flags
        return None, 0

    def writable(self, address):
        return bool(self.section(address)[1] & 1)

    def is_code(self, value):
        name, flags = self.section(value)
        return bool(flags & 4) and name not in (".rdata",) and value in self.rows

    def dword(self, address):
        return struct.unpack("<I", read(self.xbe, self.secs, address, 4))[0]

    def raw(self, address, size):
        return read(self.xbe, self.secs, address, size)

    def disassemble(self, address):
        size = int(self.rows[address]["size"])
        code = self.raw(address, size)
        with tempfile.TemporaryDirectory() as temp:
            raw = Path(temp) / "code.bin"
            raw.write_bytes(code)
            elf = Path(temp) / "code.elf"
            subprocess.run([str(Path(OBJDUMP).with_name("llvm-objcopy")), "-I", "binary", "-O",
                            "elf32-i386", "--change-section-address", f".data=0x{address:x}",
                            raw, elf], check=True)
            out = subprocess.run([OBJDUMP, "-D", "-j", ".data", "--no-show-raw-insn", "-M", "intel",
                                  elf], capture_output=True, text=True, check=True).stdout
        result = []
        for line in out.splitlines():
            m = re.match(r"\s+([0-9a-f]+):\s+(.*)", line)
            if m:
                result.append((int(m.group(1), 16), re.sub(r"\s*<[^>]*>", "", m.group(2)).strip()))
        return result


class Value:
    """an integer (const known, or base expression + offset) or a float
    expression"""

    def __init__(self, kind, text=None, const=None, offset=0, note=None):
        self.kind, self.text, self.const, self.offset = kind, text, const, offset
        self.note = note  # the read-only retail data a literal came from

    def int_text(self, mask=0xffffffff):
        if self.kind != "int":
            raise Unsupported("float value used as an integer")
        if self.const is not None:
            return f"0x{self.const & mask:x}"
        if self.offset:
            return f"({self.text} + {self.offset})"
        return self.text


class Interpreter:
    def __init__(self, image, address, calls):
        self.image, self.address, self.calls = image, address, calls
        self.lines = []
        self.regs = {}
        self.xmm = {}
        self.x87 = []
        self.pushes = []
        self.set_since_call = []
        self.temps = 0

    def temp(self, ctype, expression, comment):
        name = f"{'f' if ctype == 'float' else 'v'}{self.temps}"
        self.temps += 1
        self.lines.append(f"\t{ctype} const {name} = {expression}; // {comment}")
        return name

    def mem(self, operand):
        m = re.fullmatch(r"(dword|word|byte|qword) ptr \[0x([0-9a-f]+)\]", operand)
        if not m:
            raise Unsupported(f"operand {operand}")
        return m.group(1), int(m.group(2), 16)

    def load_int(self, width, address):
        ctype, mask = WIDTH[width]
        if self.image.writable(address):
            return Value("int", self.temp(ctype, f"*({ctype} *)0x{address:x}", f"g_{address:x}"))
        value = int.from_bytes(self.image.raw(address, 4), "little") & mask
        if self.image.is_code(value):
            raise Unsupported(f"code address 0x{value:x} loaded from 0x{address:x}")
        return Value("int", const=value, note=f"[0x{address:x}]")

    def load_float(self, address):
        if self.image.writable(address):
            return Value("float", self.temp("float", f"*(float *)0x{address:x}", f"g_{address:x}"))
        return Value("float", f"{float_literal(self.image.dword(address))} /* [0x{address:x}] */")

    def reg_value(self, name):
        full = SUB.get(name, name)
        if full not in self.regs:
            raise Unsupported(f"read of unset {name}")
        value = self.regs[full]
        if value is None:
            raise Unsupported(f"read of {name} after a call")
        return value

    def source(self, operand, width):
        if operand.startswith("0x") or operand.startswith("-0x") or operand.isdigit():
            value = int(operand, 16) if "x" in operand else int(operand)
            value &= 0xffffffff
            if self.image.is_code(value):
                raise Unsupported(f"code address 0x{value:x} as a value")
            return Value("int", const=value)
        if operand in GPR32 or operand in SUB:
            return self.reg_value(operand)
        return self.load_int(width, self.mem(operand)[1])

    def store(self, operand, value, comment=""):
        width, address = self.mem(operand)
        if value.kind == "float":
            if width != "dword":
                raise Unsupported("float store not a dword")
            self.lines.append(f"\t*(float *)0x{address:x} = {value.text};{comment}")
            return
        ctype, mask = WIDTH[width]
        if value.note:
            comment += f" // {value.note}"
        self.lines.append(f"\t*({ctype} *)0x{address:x} = {value.int_text(mask)};{comment}")

    def xmm_source(self, operand):
        if operand.startswith("xmm"):
            return self.xmm[operand]
        width, address = self.mem(operand)
        return self.load_float(address)

    def call(self, target, tail=False):
        if target == ATEXIT:
            if not self.pushes:
                raise Unsupported("_atexit without an argument")
            destructor = self.pushes.pop()
            self.lines.append(f"\t// retail registers the destructor 0x{destructor.const:x} with _atexit;"
                              f" the port never runs exit handlers")
            self.after_call()
            return
        self_value = None
        if "ecx" in self.set_since_call:
            self_value = self.regs["ecx"]
        else:
            for reg in ("eax", "esi", "edi"):
                if reg in self.set_since_call:
                    self_value = self.regs[reg]
        args = list(reversed(self.pushes))
        if any(a.kind != "int" for a in args):
            raise Unsupported("a code address or float as an argument")
        self.pushes = []
        if self_value is not None and self_value.const is None:
            raise Unsupported("constructor on a computed object")
        parameters = (["void *self"] if self_value is not None else []) + \
                     [f"unsigned int arg{i}" for i in range(len(args))]
        signature = ", ".join(parameters) or "void"
        known = self.calls.setdefault(target, signature)
        if known != signature:
            raise Unsupported(f"0x{target:x} called with differing signatures")
        arguments = ([f"(void *)0x{self_value.const:x}"] if self_value is not None else []) + \
                    [a.int_text() for a in args]
        self.lines.append(f"\th2_retail_{target:x}({', '.join(arguments)});")
        self.after_call()

    def after_call(self):
        for reg in ("eax", "ecx", "edx"):
            self.regs[reg] = None
        self.set_since_call = []
        self.xmm = {}

    def set_reg(self, name, value):
        if name not in GPR32:
            raise Unsupported(f"write of {name}")
        self.regs[name] = value
        if name not in self.set_since_call:
            self.set_since_call.append(name)

    def run(self):
        saved = set()
        instructions = self.image.disassemble(self.address)
        for index, (pc, text) in enumerate(instructions):
            parts = text.split(None, 1)
            op = parts[0]
            operands = [o.strip() for o in parts[1].split(",")] if len(parts) > 1 else []
            if op == "ret":
                if operands or index != len(instructions) - 1:
                    raise Unsupported("ret not at the end")
                return self.lines
            if op == "push":
                (a,) = operands
                if a in GPR32 and a not in self.regs:
                    saved.add(a)  # a callee-saved register, not an argument
                else:
                    try:
                        self.pushes.append(self.source(a, "dword"))
                    except Unsupported:
                        if not (a.startswith("0x") and self.image.is_code(int(a, 16))):
                            raise
                        # a code address: fine only as the destructor _atexit takes
                        self.pushes.append(Value("code", const=int(a, 16)))
            elif op == "pop":
                (a,) = operands
                if a in saved:
                    self.regs.pop(a, None)
                elif a != "ecx" or self.pushes:  # pop ecx: the _atexit argument
                    raise Unsupported(f"pop {a}")
            elif op == "add" and operands[0] == "esp":
                pass  # the _atexit argument
            elif op in ("call", "jmp"):
                target = int(operands[0], 16)
                if op == "jmp" and index != len(instructions) - 1:
                    raise Unsupported("jmp within the function")
                self.call(target)
                if op == "jmp":
                    return self.lines
            elif op == "mov" and operands[0] in GPR32 + list(SUB):
                dest = operands[0]
                width = "dword" if dest in GPR32 else ("byte" if dest.endswith("l") else "word")
                self.set_reg(SUB.get(dest, dest), self.source(operands[1], width))
            elif op == "mov":
                width, _ = self.mem(operands[0])
                self.store(operands[0], self.source(operands[1], width))
            elif op == "xor" and operands[0] == operands[1] and operands[0] in GPR32:
                self.set_reg(operands[0], Value("int", const=0))
            elif op == "or" and operands[0] in GPR32 and operands[1] == "-0x1":
                self.set_reg(operands[0], Value("int", const=0xffffffff))
            elif op == "or":
                width, address = self.mem(operands[0])
                ctype, mask = WIDTH[width]
                self.lines.append(f"\t*({ctype} *)0x{address:x} |= {self.source(operands[1], width).int_text(mask)};")
            elif op == "inc" and operands[0] in GPR32:
                value = self.reg_value(operands[0])
                if value.const is not None:
                    self.set_reg(operands[0], Value("int", const=(value.const + 1) & 0xffffffff))
                else:
                    self.set_reg(operands[0], Value("int", value.text, offset=value.offset + 1))
            elif op == "movss":
                if operands[0].startswith("xmm"):
                    self.xmm[operands[0]] = self.xmm_source(operands[1])
                else:
                    self.store(operands[0], self.xmm[operands[1]])
            elif op == "movaps" and operands[1].startswith("xmm"):
                self.xmm[operands[0]] = self.xmm[operands[1]]
            elif op == "xorps" and operands[0] == operands[1]:
                self.xmm[operands[0]] = Value("float", "0.0f")
            elif op in FLOAT_OPS:
                left, right = self.xmm[operands[0]], self.xmm_source(operands[1])
                self.xmm[operands[0]] = Value("float", f"({left.text} {FLOAT_OPS[op]} {right.text})")
            elif op == "fld":
                width, address = self.mem(operands[0])
                if width != "qword" or self.image.writable(address):
                    raise Unsupported("fld")
                self.x87.append(f"{double_literal(self.image.raw(address, 8))} /* [0x{address:x}] */")
            elif op == "fcos":
                self.x87[-1] = f"__builtin_cos({self.x87[-1]})"
            elif op == "fstp":
                width, address = self.mem(operands[0])
                if width != "dword":
                    raise Unsupported("fstp")
                self.lines.append(f"\t*(float *)0x{address:x} = (float){self.x87.pop()};")
            else:
                raise Unsupported(f"instruction {text}")
        raise Unsupported("no ret")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--xbe", type=Path, default=ROOT / "orig/game/default.xbe")
    args = parser.parse_args()
    image = Image(args.xbe)
    table = [image.dword(a) for a in range(TABLE_START, TABLE_END, 4)]
    if 0 in table or image.dword(TABLE_END) != 0 or image.dword(TABLE_START - 4) != 0:
        sys.exit("the initialiser table is not where it was expected")

    calls = {}
    bodies = []
    runner = []
    counts = {"generated": 0, "atexit only": 0, "existing": 0, "hand": 0, "skipped": 0}
    problems = []
    for address in table:
        if address in SKIP:
            runner.append(f"\t// 0x{address:x} skipped: {SKIP[address]}")
            counts["skipped"] += 1
            continue
        if address in EXISTING:
            runner.append(f"\th2_retail_{address:x}(); // {EXISTING[address]}")
            calls.setdefault(address, "void")
            counts["existing"] += 1
            continue
        if address in HAND:
            runner.append(f"\tretail_initializer_{address:x}(); // {HAND[address]}")
            counts["hand"] += 1
            continue
        try:
            lines = Interpreter(image, address, calls).run()
        except Unsupported as error:
            problems.append(f"0x{address:x}: {error}")
            continue
        if all(line.lstrip().startswith("//") for line in lines):
            runner.append(f"\t// 0x{address:x}: only registers a destructor with _atexit")
            counts["atexit only"] += 1
            continue
        counts["generated"] += 1
        bodies.append(f"// @retail 0x{address:x}\nextern \"C\" void retail_initializer_{address:x}(void)\n{{\n"
                      + "\n".join(lines) + "\n}\n")
        runner.append(f"\tretail_initializer_{address:x}();")
    if problems:
        sys.exit("initialisers the interpreter cannot handle (add them to HAND or SKIP):\n  "
                 + "\n  ".join(problems))

    out = ["// @flags /O2",
           "// Generated by port/tools/gen_initializer_recoveries.py: the retail C++",
           "// dynamic initialisers (the CRT's __xc table, 0x461194-0x46180c) and the",
           "// runner that calls them in table order before main(). Do not edit; rerun",
           "// the script. The ones it cannot interpret are hand-written in",
           "// retail_initializers.cpp.",
           "//",
           "// Writes go through the retail address (each global lives in the arena at",
           "// its retail address); a literal's comment names the read-only retail data",
           "// it was read from.",
           "",
           "static inline float float_bits(unsigned int bits)",
           "{",
           "\tfloat value;",
           "\t__builtin_memcpy(&value, &bits, sizeof(value));",
           "\treturn value;",
           "}",
           ""]
    for target, signature in sorted(calls.items()):
        out.append(f"extern \"C\" void h2_retail_{target:x}({signature});")
    out.append("")
    for address in sorted(HAND):
        out.append(f"extern \"C\" void retail_initializer_{address:x}(void);")
    out.append("")
    out += bodies
    out += ["// retail runs these from _cinit before main(); h2_whole_game_main",
            "// (port/runtime/whole_image_start.s) calls this after the image's own",
            "// static constructors",
            "extern \"C\" void h2_run_retail_initializers(void)",
            "{"] + runner + ["}", ""]
    OUTPUT.write_text("\n".join(out))
    print(f"{OUTPUT.relative_to(ROOT)}: {len(table)} entries; "
          + ", ".join(f"{name} {count}" for name, count in counts.items()))


if __name__ == "__main__":
    main()
