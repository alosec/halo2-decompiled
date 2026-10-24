#!/usr/bin/env python3
"""Build one guest image from every recovered translation unit.

The curated main-menu image extracts hand-listed symbols from selected units.
This tool instead compiles every unit that the native frontend accepts, then:

* resolves duplicate strong definitions by priority (port runtime, then
  ``*_native.cpp`` files, then the source ``config/functions.csv`` names for
  the function, then other recovered units, then stubs), deleting the losers;
* binds globals whose names carry a retail address (``g_4e3b44``,
  ``g_4cf790_storage``) to that address unless their initializer holds code
  pointers, so retail data stays where retail code expects it;
* routes ``host_*`` calls through patchable host ABI trampolines;
* gives every function that no unit defines a trap stub that reports its name
  to the host and returns zero.

It writes ``whole-image.elf`` plus ``whole-image-report.json`` describing what
was compiled, skipped, overridden, bound and stubbed.
"""

import argparse
import concurrent.futures
import csv
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import sys

TOOLS = Path(__file__).resolve().parent

MS_DATALAYOUT = ('target datalayout = "e-m:o-p:32:32-p270:32:32-p271:32:32-'
                 'p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"')
GUEST_TRIPLE = 'target triple = "arm64_32-apple-watchos2.0.0"'
GUEST_BIAS = 0x10000000000

DEFINE = re.compile(r'^define ([^@]*)@("[^"]+"|[^ (]+)\(', re.M)
DECLARE = re.compile(r'^declare [^@]*@("[^"]+"|[^ (]+)\(', re.M)
GLOBAL = re.compile(r'^@("[^"]+"|[^ ]+) = ([^"\n]*?)\b(global|constant) (.*)$', re.M)
# the retail .text section; a g_<hex> name inside it is a function address the
# decompiler borrowed, not where retail keeps the global
RETAIL_TEXT = range(0x12000, 0x379c4c)
ADDRESS_NAME = re.compile(r'(?:^|[?@])g_(?:unknown_)?([0-9a-f]{5,6})(?:_[A-Za-z0-9_]+)?(?:@|$)')


def elf_name(symbol):
    """ELF linkers read '@' in a symbol name as a version tag, which merges
    overloads such as ?f@@YAXJ@Z and ?f@@YAX_N@Z. MS mangling never uses '%'."""
    return symbol.replace("@", "%")


def escape_symbols(assembly):
    out = []
    for line in assembly.split("\n"):
        stripped = line.lstrip()
        if stripped.startswith((".asciz", ".ascii", ".string")):
            out.append(line)
            continue
        out.append(re.sub(r'"([^"]*@[^"]*)"', lambda m: '"' + elf_name(m.group(1)) + '"', line))
    return "\n".join(out)


def run(args, **kwargs):
    return subprocess.run([str(value) for value in args], check=True, **kwargs)


def unquote(name):
    return name[1:-1] if name.startswith('"') else name


def linkage(prefix):
    if re.search(r'\b(internal|private)\b', prefix):
        return "internal"
    if re.search(r'\b(linkonce|linkonce_odr|weak|weak_odr|available_externally)\b', prefix):
        return "weak"
    if re.search(r'\bextern_weak\b|\bexternal\b', prefix):
        return "external"
    return "strong"


def retarget(text):
    text = re.sub(r'^target datalayout = ".*"$', MS_DATALAYOUT, text, flags=re.M)
    text = re.sub(r'^target triple = ".*"$', GUEST_TRIPLE, text, flags=re.M)
    text = text.replace("arm_aapcs_vfpcc ", "")
    text = re.sub(r'^\$.* = comdat .*\n', '', text, flags=re.M)
    text = re.sub(r',\s*comdat(?:\(\$[^\)]+\))?', '', text)
    text = re.sub(r'\s+comdat(?:\(\$[^\)]+\))?', '', text)
    text = re.sub(r' "target-cpu"="[^"]*"', '', text)
    text = re.sub(r' "target-features"="[^"]*"', '', text)
    return text


OPERAND = r'(?:(?:dword|word|qword|tbyte)\s+ptr\s+)?([A-Za-z_][\w.\[\]>-]*)'
SEP = r'\s*(?:;[^\n]*)?\s*'


def asm_block(*instructions):
    """A single __asm { ... } block holding exactly these instructions."""
    return re.compile(r'__asm\s*\{' + SEP + SEP.join(instructions) + SEP + r'\}')


def asm_blocks(*instructions):
    """Consecutive one-instruction __asm statements (braced or not, possibly
    continued with backslashes inside a macro)."""
    pieces = [r'__asm\s*(?:\{\s*' + i + r'\s*\}|' + i + r')' for i in instructions]
    return re.compile(r'(?:[ \t]*\\?\s*)'.join(pieces) + r'[ \t]*;?')


def operands(match):
    """The captured operands in order (alternative branches leave gaps)."""
    return [g for g in match.groups() if g is not None]


def fld_value(operand):
    """What fld loads from a variable: its storage read as float or double,
    whatever C type the variable has (MSVC asm ignores it)."""
    return (f"(sizeof({operand}) == 8 ? *(double *)&({operand}) : "
            f"(double)*(float *)&({operand}))")


def fistp_store(operand, value):
    """fistp into a variable of any C type."""
    return f"{operand} = (__typeof__({operand}))(long)({value});"


ROUND = lambda m: fistp_store(operands(m)[1], f"__builtin_rint({fld_value(operands(m)[0])})")
REWRITES = [
    # fld/fistp under the default x87 control word: round to nearest, ties to even.
    (asm_block(r'fld\s+' + OPERAND, r'fistp\s+' + OPERAND), ROUND),
    (asm_blocks(r'fld\s+' + OPERAND, r'fistp\s+' + OPERAND), ROUND),
    # rsqrtss is an estimate; the native form is exact.
    (asm_block(r'rsqrtss\s+xmm0\s*,\s*' + OPERAND, r'movss\s+' + OPERAND + r'\s*,\s*xmm0'),
     lambda m: f"{operands(m)[1]} = 1.0f / __builtin_sqrtf({operands(m)[0]});"),
    # bsr: index of the highest set bit (the operand is known to be nonzero).
    (asm_blocks(r'bsr\s+eax\s*,\s*' + OPERAND, r'mov\s+' + OPERAND + r'\s*,\s*eax'),
     lambda m: f"{operands(m)[1]} = 31 - __builtin_clz((unsigned)({operands(m)[0]}));"),
    # bsf: index of the lowest set bit (the operand is known to be nonzero).
    (asm_block(r'bsf\s+(?:e[a-d]x)\s*,\s*' + OPERAND, r'mov\s+' + OPERAND + r'\s*,\s*e[a-d]x'),
     lambda m: f"{operands(m)[1]} = __builtin_ctz((unsigned)({operands(m)[0]}));"),
    (asm_blocks(r'bsf\s+(?:e[a-d]x)\s*,\s*' + OPERAND, r'mov\s+' + OPERAND + r'\s*,\s*e[a-d]x'),
     lambda m: f"{operands(m)[1]} = __builtin_ctz((unsigned)({operands(m)[0]}));"),
    # fld a / fld b / fmulp / fistp: the rounded product.
    (asm_blocks(r'fld\s+' + OPERAND, r'fld\s+' + OPERAND, r'fmulp\s+st\(1\)\s*,\s*st\(0\)',
                r'fistp\s+' + OPERAND),
     lambda m: f"{operands(m)[2]} = (long)__builtin_rint((double)({operands(m)[0]}) * (double)({operands(m)[1]}));"),
    # ... and the same rounded product shifted into the top byte.
    (asm_block(r'fld\s+' + OPERAND, r'fld\s+' + OPERAND, r'fmulp\s+st\(1\)\s*,\s*st(?:\(0\))?',
               r'fistp\s+' + OPERAND, r'shl\s+' + OPERAND + r'\s*,\s*(\d+)'),
     lambda m: f"{operands(m)[2]} = (long)((unsigned long)(long)__builtin_rint((double)({operands(m)[0]}) * "
               f"(double)({operands(m)[1]})) << {operands(m)[4]});"),
    # bsf leaves its destination alone for a zero source: -1 here.
    (asm_block(r'mov\s+(e[a-d]x)\s*,\s*-1', r'bsf\s+\1\s*,\s*' + OPERAND, r'mov\s+' + OPERAND + r'\s*,\s*\1'),
     lambda m: f"{operands(m)[2]} = ({operands(m)[1]}) ? (long)__builtin_ctz((unsigned)({operands(m)[1]})) : -1;"),
    (re.compile(r'__asm\s+wbinvd\b;?'), lambda m: "/* wbinvd: no cache maintenance natively */"),
    # truncate, keep the low 16 bits, leave it in eax
    (asm_block(r'movss\s+xmm0\s*,\s*' + OPERAND, r'cvttss2si\s+eax\s*,\s*xmm0', r'and\s+eax\s*,\s*0xffff'),
     lambda m: f"return (long)({operands(m)[0]}) & 0xffff;"),
    (asm_block(r'cvtsi2ss\s+(xmm\d)\s*,\s*' + OPERAND, r'movss\s+' + OPERAND + r'\s*,\s*\1'),
     lambda m: f"{operands(m)[2]} = (float)({operands(m)[1]});"),
    # Time-stamp reads.
    (asm_block(r'rdtsc', r'mov\s+' + OPERAND + r'\s*,\s*eax'),
     lambda m: f"{operands(m)[0]} = (unsigned long)h2_native_time_stamp();"),
    (asm_blocks(r'rdtsc', r'mov\s+' + OPERAND + r'\s*,\s*eax'),
     lambda m: f"{operands(m)[0]} = (unsigned long)h2_native_time_stamp();"),
    (re.compile(r'volatile\s+(?:unsigned\s+)?__int64\s+\w+\s*=\s*0\s*;\s*__asm\s+rdtsc\b'),
     lambda m: "return h2_native_time_stamp();"),
    # fld a / fmul b leaving the product in st(0) as the implicit result.
    (asm_block(r'fld\s+' + OPERAND, r'fmul\s+' + OPERAND),
     lambda m: f"return (double)({operands(m)[0]}) * (double)({operands(m)[1]});"),
    (re.compile(r'__asm\s*\{\s*int\s+3\s*\}|__asm\s+int\s+3\b;?'), lambda m: "__builtin_trap();"),
]
# Widen four signed words (a quantized quaternion) and normalize them.
QUATERNION_LOAD = re.compile(
    r'__asm\s*\{\s*mov\s+ecx\s*,\s*(\w+)\s+mov\s+eax\s*,\s*(\w+)\s+movq\s+mm3\s*,\s*\[ecx\]\s+'
    r'punpcklwd\s+mm1\s*,\s*mm3\s+punpckhwd\s+mm2\s*,\s*mm3\s+psrad\s+mm1\s*,\s*0x10\s+'
    r'psrad\s+mm2\s*,\s*0x10\s+cvtpi2ps\s+xmm1\s*,\s*mm1\s+cvtpi2ps\s+xmm2\s*,\s*mm2\s+emms\s+'
    r'movlhps\s+xmm1\s*,\s*xmm2\s+movaps\s+xmm0\s*,\s*xmm1\s+mulps\s+xmm0\s*,\s*xmm1\s+'
    r'movaps\s+xmm3\s*,\s*xmm0\s+shufps\s+xmm3\s*,\s*xmm3\s*,\s*0x4e\s+addps\s+xmm0\s*,\s*xmm3\s+'
    r'movaps\s+xmm4\s*,\s*xmm0\s+shufps\s+xmm4\s*,\s*xmm4\s*,\s*0x11\s+addps\s+xmm0\s*,\s*xmm4\s+'
    r'rsqrtps\s+xmm0\s*,\s*xmm0\s+mulps\s+xmm1\s*,\s*xmm0\s+movaps\s+\[eax\]\s*,\s*xmm1\s*\}')
QUATERNION_LOAD_NATIVE = (
    "{{ short const *h2_q = (short const *)({0}); float h2_v[4], h2_d = 0.0f; "
    "for (int h2_i = 0; h2_i < 4; h2_i++) {{ h2_v[h2_i] = (float)h2_q[h2_i]; h2_d += h2_v[h2_i] * h2_v[h2_i]; }} "
    "float h2_s = 1.0f / __builtin_sqrtf(h2_d); "
    "for (int h2_i = 0; h2_i < 4; h2_i++) ((float *)({1}))[h2_i] = h2_v[h2_i] * h2_s; }}")
# The nine-real dot product macro (unknown_023540.cpp).
DOT_PRODUCT9 = re.compile(r'#define DOT_PRODUCT9\(result, a, b\) \\\n(?:\s*__asm[^\n]*\\\n)*\s*__asm[^\n]*\n')
DOT_PRODUCT9_NATIVE = ("#define DOT_PRODUCT9(result, a, b) do { float const *h2_a = (float const *)(a); "
                       "float const *h2_b = (float const *)(b); float h2_r = 0.0f; "
                       "for (int h2_i = 0; h2_i < 9; h2_i++) h2_r += h2_a[h2_i] * h2_b[h2_i]; "
                       "(result) = h2_r; } while (0)\n")
# Truncate-then-correct sequences that leave floor(value) in eax.
FLOOR_BLOCK = re.compile(r'__asm\s*\{\s*movss\s+xmm0\s*,\s*' + OPERAND +
                         r'\s*\n\s*cvttss2si\s+eax\s*,\s*xmm0[^}]*?cmpneqss[^}]*?movmskps[^}]*\}')


REWRITE_VERSION = "8"


def native_source(text):
    """Rewrite mechanical x86 inline-assembly idioms for the ARM guest.

    Rewriting a block that a native guard already hides is harmless. Returns
    None when nothing changed."""
    if "__asm" not in text:
        return None
    original = text
    for pattern, replacement in REWRITES:
        text = pattern.sub(replacement, text)
    text = DOT_PRODUCT9.sub(lambda m: DOT_PRODUCT9_NATIVE, text)
    text = QUATERNION_LOAD.sub(lambda m: QUATERNION_LOAD_NATIVE.format(m.group(1), m.group(2)), text)
    text = FLOOR_BLOCK.sub(lambda m: f"return (long)__builtin_floorf({m.group(1)});", text)
    if text == original:
        return None
    if "h2_native_time_stamp" in text and "h2_native_time_stamp" not in original:
        text = 'extern "C" __int64 h2_native_time_stamp(void);\n' + text
    return text


def header_native(text):
    """A header with its H2_NATIVE_PORT branches forced on and the remaining
    assembly idioms rewritten; None when it needs neither."""
    if "H2_NATIVE_PORT" not in text and "__asm" not in text:
        return None
    text = re.sub(r'^(\s*#\s*)ifdef\s+H2_NATIVE_PORT\b', r'\1if 1', text, flags=re.M)
    text = re.sub(r'^(\s*#\s*)ifndef\s+H2_NATIVE_PORT\b', r'\1if 0', text, flags=re.M)
    text = re.sub(r'\bdefined\s*\(\s*H2_NATIVE_PORT\s*\)', '1', text)
    text = re.sub(r'\bdefined\s+H2_NATIVE_PORT\b', '1', text)
    text = re.sub(r'(^\s*#\s*(?:if|elif)\b.*)\bH2_NATIVE_PORT\b', r'\g<1>1', text, flags=re.M)
    return native_source(text) or text


class Unit:
    def __init__(self, source, root, work):
        self.source = source
        self.relative = str(source.relative_to(root))
        stem = re.sub(r'[^A-Za-z0-9_]', '_', self.relative)
        self.ir = work / "ir" / f"{stem}.ll"
        self.object = work / "obj" / f"{stem}.o"
        self.stamp = work / "ir" / f"{stem}.stamp"


def frontend(unit, args, header_stamp):
    """Compile one unit to retargeted IR; return (ok, diagnostic).

    Recovered units compile as retail code first: the port's H2_NATIVE_PORT
    branches (portability fixes and the curated menu build's replacements)
    stay off, while the SDK shims (H2_PORT_SHIMS) and the assembly rewriter
    still apply. A unit that only builds with its native branches falls back
    to them. Native rewrites and the port runtime are always native."""
    native_only = (unit.relative.startswith("port/") and not unit.relative.startswith("port/recovered/")) \
        or unit.relative.endswith("_native.cpp")
    if not native_only:
        ok, diagnostic = frontend_mode(unit, args, header_stamp, native=False)
        if ok:
            unit.mode = "retail"
            return True, ""
    ok, diagnostic = frontend_mode(unit, args, header_stamp, native=True)
    unit.mode = "native"
    return ok, diagnostic


def frontend_mode(unit, args, header_stamp, native):
    is_c = unit.source.suffix == ".c"
    compiler = args.llvm / ("clang" if is_c else "clang++")
    command = [compiler, "--target=thumbv7-pc-windows-msvc", "-O2", "-ffreestanding",
               # x87 and SSE math never touched errno; without this clang
               # turns __builtin_sqrtf into a call to sqrtf, the guest
               # runtime's own sqrtf included
               "-fno-stack-protector", "-fno-math-errno", *(["-DH2_NATIVE_PORT=1"] if native else []),
               "-DH2_PORT_SHIMS=1", "-DH2_WHOLE_IMAGE=1",
               "-fms-extensions", "-Wno-ignored-attributes", "-Wno-c++11-narrowing",
               "-DH2_MS_ABI_FRONTEND=1", "-Xclang", "-fno-wchar", "-w"]
    if not is_c:
        # Retail relies on LTCG to inline 'inline' definitions across files;
        # emit every one as a weak definition so other units can link it.
        # VC 7.1 (the retail compiler) predates thread-safe local statics.
        command += ["-fno-exceptions", "-fno-rtti", "-fno-threadsafe-statics",
                    "-Dinline=inline __attribute__((used))",
                    "-D__forceinline=inline __attribute__((used))",
                    "-D__inline=inline __attribute__((used))",
                    # MSVC accepts these; the Xbox headers make SSE types and
                    # the memory intrinsics visible everywhere.
                    "-Wno-address-of-temporary", "-include", "xmmintrin.h", "-include", "string.h"]
    if not native and args.shadow_include:
        command.append(f"-I{args.shadow_include}")
    for include in args.include:
        command.append(f"-I{include}")
    source = unit.source
    rewritten = native_source(unit.source.read_text(errors="replace"))
    if rewritten is not None:
        source = unit.ir.with_suffix(unit.source.suffix)
        source.parent.mkdir(parents=True, exist_ok=True)
        source.write_text(f'#line 1 "{unit.source}"\n' + rewritten)
        command.append(f"-I{unit.source.parent}")
    command += ["-S", "-emit-llvm", source, "-o", unit.ir]
    key = hashlib.sha256((REWRITE_VERSION + header_stamp + str(unit.source.stat().st_mtime_ns) +
                          " ".join(map(str, command))).encode()).hexdigest()
    if unit.ir.exists() and unit.stamp.exists() and unit.stamp.read_text() == key:
        return True, ""
    unit.ir.parent.mkdir(parents=True, exist_ok=True)
    result = subprocess.run([str(c) for c in command], capture_output=True, text=True)
    if result.returncode:
        errors = [line for line in result.stderr.splitlines() if "error:" in line]
        unit.ir.unlink(missing_ok=True)
        return False, errors[0] if errors else "frontend failed"
    unit.ir.write_text(retarget(unit.ir.read_text()))
    unit.stamp.write_text(key)
    return True, ""


def scan(unit):
    text = unit.ir.read_text()
    functions = {}
    for m in DEFINE.finditer(text):
        functions[unquote(m.group(2))] = linkage(m.group(1))
    declarations = {unquote(m.group(1)) for m in DECLARE.finditer(text)}
    globals_ = {}
    for m in GLOBAL.finditer(text):
        name = unquote(m.group(1))
        if name.startswith("llvm."):
            continue
        prefix, rest = m.group(2), m.group(4)
        kind = linkage(prefix)
        if kind == "external":
            declarations.add(name)
            continue
        code_pointers = bool(re.search(r'@("[^"]+"|[\w.$?@]+)', rest)) and \
            "zeroinitializer" not in rest.split(",")[0]
        zero = rest.startswith(("zeroinitializer", "%")) and "zeroinitializer" in rest.split(", align")[0]
        globals_[name] = {"linkage": kind, "code_pointers": code_pointers, "zero": zero,
                          "constant": m.group(3) == "constant"}
    return {"functions": functions, "globals": globals_, "declarations": sorted(declarations)}


def split_type(text):
    """Return the leading IR type expression of an initializer and the rest."""
    depth = 0
    for index, char in enumerate(text):
        if char in "[{<(":
            depth += 1
        elif char in "]}>)":
            depth -= 1
        elif char == " " and depth == 0 and index:
            return text[:index], text[index + 1:]
    return text, ""


def prune(text, delete_functions, delete_globals, promote=()):
    """Turn the named definitions into declarations without touching linkage
    of anything else (llvm-extract --delete externalizes local symbols), and
    give the named internal definitions external linkage."""
    lines = text.split("\n")
    out = []
    skipping = False
    for line in lines:
        if promote and line.startswith("define internal "):
            m = DEFINE.match(line)
            if m and unquote(m.group(2)) in promote:
                line = "define " + line[len("define internal "):]
        if skipping:
            if line == "}":
                skipping = False
            continue
        m = DEFINE.match(line)
        if m and unquote(m.group(2)) in delete_functions:
            header = line[len("define "):]
            header = re.sub(r'^(?:(?:internal|private|linkonce_odr|linkonce|weak_odr|weak|'
                            r'dso_local|local_unnamed_addr|unnamed_addr|hidden|protected) )*', '', header)
            close = header.rfind(")")
            out.append("declare " + header[:close + 1])
            if line.rstrip().endswith("{"):
                skipping = True
            continue
        g = GLOBAL.match(line)
        if g and unquote(g.group(1)) in delete_globals:
            kind = g.group(3)
            type_text, _ = split_type(g.group(4))
            out.append(f"@{g.group(1)} = external {kind} {type_text}")
            continue
        out.append(line)
    return "\n".join(out)


def backend(unit, args, delete_functions, delete_globals, promote=()):
    """Delete losing definitions, rebase, and emit an ELF object."""
    unit.object.parent.mkdir(parents=True, exist_ok=True)
    work = unit.object.with_suffix("")
    ir = unit.ir
    if delete_functions or delete_globals or promote:
        pruned = work.with_suffix(".pruned.ll")
        pruned.write_text(prune(ir.read_text(), delete_functions, delete_globals, promote))
        ir = pruned
    rebased = work.with_suffix(".rebased.ll")
    darwin = work.with_suffix(".darwin.s")
    elf = work.with_suffix(".elf.s")
    run([args.llvm / "opt", f"-load-pass-plugin={args.plugin}", "-passes=h2-rebase,verify",
         "-S", ir, "-o", rebased], capture_output=True)
    run([args.llvm / "llc", "-O2", "-mtriple=arm64_32-apple-watchos", rebased, "-o", darwin],
        capture_output=True)
    run([sys.executable, TOOLS / "guest_asm_elf.py", darwin, elf], capture_output=True)
    elf.write_text(escape_symbols(elf.read_text()))
    run([args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj", elf,
         "-o", unit.object], capture_output=True)
    for path in (rebased, darwin, elf):
        path.unlink(missing_ok=True)


PREFERRED = set()
# native rewrites that call helpers no unit of this image defines (they lived
# in the native halves of units the whole image compiles in retail mode)
INCOMPLETE_NATIVE = set()


def priority(unit, symbol, csv_sources, symbol_address):
    """Lower wins. Byte-matched recovered code beats every replacement written
    before it matched; native rewrites beat unmatched drafts; the curated
    menu runtime only fills gaps; stubs come last."""
    path = unit.relative
    if path in PREFERRED:
        return -1
    address = symbol_address.get(symbol)
    if address is not None:
        source, status = csv_sources.get(address, (None, None))
        if source == path and status == "matched":
            return 0
    if path.endswith("_native.cpp") and path not in INCOMPLETE_NATIVE:
        return 1
    if path.endswith("_generated.cpp"):
        # functions generated from retail idioms: anything recovered by hand
        # comes first
        return 3.9
    if path.startswith("port/runtime/") and re.match(r'\?function_[0-9a-f]+@', symbol):
        # the curated menu build's stand-ins for game functions: the
        # recovered source of the function is the real thing
        return 3.5
    if path.startswith("port/"):
        return 2
    if "/stubs/" in path:
        return 4
    return 3


def symbol_addresses(units, tag="retail"):
    """Map (unit, qualified name) and (unit, unique bare name) to the retail
    address of the nearest preceding @retail tag (or @stub tag, for the
    placeholders in src/stubs)."""
    result = {}
    bare = {}
    for unit in units:
        lines = unit.source.read_text(errors="replace").split("\n")
        pending = None
        for line in lines:
            adjustor = re.search(r'@retail 0x([0-9a-f]+) adjustor ([A-Za-z_]\w*) (0x[0-9a-f]+|\d+)', line)
            if adjustor:
                # the compiler-generated this-adjusting thunk of the class's
                # deleting destructor, for the base subobject at that offset
                result.setdefault((unit.relative, f"{adjustor.group(2)}::__adjustor_{int(adjustor.group(3), 0)}"),
                                  int(adjustor.group(1), 16))
                pending = None
                continue
            deleting = re.search(r'@retail 0x([0-9a-f]+) deleting ([A-Za-z_]\w*)', line)
            if deleting:
                # the compiler-generated deleting destructor of the class
                result.setdefault((unit.relative, f"{deleting.group(2)}::__deleting"),
                                  int(deleting.group(1), 16))
                pending = None
                continue
            found = re.search(r'@' + tag + r' 0x([0-9a-f]+)', line)
            if found:
                pending = int(found.group(1), 16)
                continue
            if pending is not None:
                m = re.search(r'((?:[A-Za-z_]\w*::)*~?[A-Za-z_]\w*)\s*\(', line)
                if m and not line.lstrip().startswith(("//", "/*", "*")):
                    qualified = m.group(1)
                    result.setdefault((unit.relative, qualified), pending)
                    key = (unit.relative, qualified.split("::")[-1])
                    bare.setdefault(key, set()).add(pending)
                    pending = None
    for key, addresses in bare.items():
        if len(addresses) == 1:
            result.setdefault(key, next(iter(addresses)))
    return result


def qualified_name(symbol):
    """'?v7@c_helper_b@@UAA_NXZ' -> 'c_helper_b::v7'; constructors and
    destructors as 'c_x::c_x' and 'c_x::~c_x'; None for anything else."""
    m = re.match(r'\?\?([01])([A-Za-z_]\w*)@((?:[A-Za-z_]\w*@)*)@', symbol)
    if m:
        scopes = [m.group(2)] + [s for s in m.group(3).split("@") if s]
        outer = "::".join(reversed(scopes))
        return f"{outer}::{'~' if m.group(1) == '1' else ''}{m.group(2)}"
    m = re.match(r'\?([A-Za-z_]\w*)@((?:[A-Za-z_]\w*@)*)@', symbol)
    if m:
        scopes = [s for s in m.group(2).split("@") if s]
        return "::".join(list(reversed(scopes)) + [m.group(1)])
    m = re.match(r'\?([A-Za-z_]\w*)@@', symbol)
    if m:
        return m.group(1)
    return symbol if re.match(r'^[A-Za-z_]\w*$', symbol) else None

STORAGE_MACRO = re.compile(
    r'^#define\s+g_(?:unknown_)?([0-9a-f]{5,6})\w*\s+\(\s*([A-Za-z_]\w*)((?:\.\w+|\[\d+\])+)\s*\)\s*$', re.M)


def storage_bindings(root, args, work):
    """Retail addresses of storage structs that recovered headers alias by
    address: '#define g_510800_pool_base (g_pool_storage.base)' places
    g_pool_storage at 0x510800 - offsetof(base). Returns {symbol: address}."""
    probes = []
    for header in sorted((root / "include").glob("*.h")):
        for m in STORAGE_MACRO.finditer(header.read_text(errors="replace")):
            probes.append((header, int(m.group(1), 16), m.group(2), m.group(3).lstrip(".")))
    if not probes:
        return {}
    lines = [f'#include "{header.name}"' for header in sorted({p[0] for p in probes})]
    for index, (_, _, name, path) in enumerate(probes):
        lines.append(f'extern "C" unsigned int h2_probe_offset_{index} = __builtin_offsetof(decltype({name}), {path});')
        lines.append(f'extern "C" void *h2_probe_symbol_{index} = (void *)&{name};')
    source = work / "storage_probe.cpp"
    source.write_text("\n".join(lines) + "\n")
    command = [args.llvm / "clang++", "--target=thumbv7-pc-windows-msvc", "-O0", "-ffreestanding",
               "-DH2_NATIVE_PORT=1", "-DH2_PORT_SHIMS=1", "-DH2_WHOLE_IMAGE=1", "-DH2_MS_ABI_FRONTEND=1", "-Xclang",
               "-fno-wchar", "-fms-extensions", "-fno-exceptions", "-fno-rtti", "-w",
               "-Wno-c++11-narrowing", "-include", "xmmintrin.h", "-include", "string.h",
               *[f"-I{i}" for i in args.include], "-S", "-emit-llvm", source, "-o", "-"]
    result = subprocess.run([str(c) for c in command], capture_output=True, text=True)
    if result.returncode:
        print("storage probe failed:", next((l for l in result.stderr.splitlines() if "error" in l), ""))
        return {}
    offsets = {int(m.group(1)): int(m.group(2)) for m in
               re.finditer(r'^@h2_probe_offset_(\d+) = \S+ (?:local_unnamed_addr )?global i32 (\d+)', result.stdout, re.M)}
    symbols = {int(m.group(1)): unquote(m.group(2).lstrip("@")) for m in
               re.finditer(r'^@h2_probe_symbol_(\d+) = .*? ptr (@"[^"]+"|@[\w.$?@]+)', result.stdout, re.M)}
    found = {}
    conflicts = set()
    for index, (_, address, name, _) in enumerate(probes):
        if index not in offsets or index not in symbols:
            continue
        symbol = symbols[index]
        base = address - offsets[index]
        if found.setdefault(symbol, base) != base:
            conflicts.add(symbol)
    for symbol in conflicts:
        print("storage binding conflict:", symbol)
        found.pop(symbol)
    return found


VTABLE = re.compile(r'^@("\?\?_7[^"]+@@6B[^"]*@") = [^\n]*?constant \{ \[(\d+) x ptr\] \} '
                    r'\{ \[\d+ x ptr\] \[(.*)\] \}$', re.M)


def xbe_data_words(path):
    """{word: [addresses]} for every aligned word of the XBE's data sections."""
    xbe = path.read_bytes()
    base = struct.unpack_from("<I", xbe, 0x104)[0]
    count = struct.unpack_from("<I", xbe, 0x11c)[0]
    headers = struct.unpack_from("<I", xbe, 0x120)[0] - base
    words = {}
    for index in range(count):
        _, va, _, raw, rsize, name = struct.unpack_from("<IIIIII", xbe, headers + index * 56)
        offset = name - base
        if xbe[offset:xbe.index(b"\0", offset)] not in (b".rdata", b".data", b".data1"):
            continue
        for at in range(0, rsize - 3, 4):
            words.setdefault(struct.unpack_from("<I", xbe, raw + at)[0], []).append(va + at)
    return words


def bind_retail_vtables(xbe_path, compiled, scans, tag_addresses, delete_functions):
    """{vtable symbol: retail address} for the vtables whose slots'
    retail addresses (from @retail tags; deleting destructors from their
    'deleting' tags) agree on one place in the retail data."""
    words = xbe_data_words(xbe_path)
    word_at = {address: word for word, addresses in words.items() for address in addresses}
    deleting = {q.split("::")[0]: a for (u, q), a in tag_addresses.items() if q.endswith("::__deleting")}
    # the retail address behind each defined function symbol
    retail_of = {}
    for unit in compiled:
        for name in scans[unit.relative]["functions"]:
            if name in delete_functions[unit.relative]:
                continue
            qualified = qualified_name(name)
            deleting_class = re.match(r'\?\?_[EG]([A-Za-z_]\w*)@@', name)
            if deleting_class:
                address = deleting.get(deleting_class.group(1))
            elif qualified is None:
                continue
            else:
                address = tag_addresses.get((unit.relative, qualified))
                if address is None and "::" in qualified:
                    address = tag_addresses.get((unit.relative, qualified.split("::")[-1]))
            if address is not None:
                retail_of.setdefault(name, address)
                if name.startswith("??_G"):
                    # vtables name the vector deleting destructor, an alias of
                    # the scalar one in this ABI
                    retail_of.setdefault("??_E" + name[4:], address)
    seen = {}
    for unit in compiled:
        for m in VTABLE.finditer(unit.ir.read_text()):
            symbol = unquote(m.group(1))
            if symbol not in seen:
                seen[symbol] = [unquote(s[1:]) if s != "null" else None
                                for s in re.findall(r'ptr (@"[^"]+"|@[^ ,\]]+|null)', m.group(3))]
    result = {}
    structors = structor_vtables(xbe_path, tag_addresses)
    mapped = set(retail_of.values())
    bind_retail_vtables.patches = []

    def bind(symbol, slots, start):
        """Bind the table, and overlay the native function on each slot whose
        retail function nobody recovered (a compiler-generated destructor
        without a tag, an SDK class's method the port's shim implements)."""
        result[symbol] = start
        owner = re.match(r'\?\?_7([A-Za-z_]\w*)@@6B', symbol)
        for index, slot in enumerate(slots):
            word = word_at.get(start + 4 * index)
            if not slot or word is None or not 0x12000 <= word < 0x379c4c or word in mapped:
                continue
            # only the class's own functions stand in: a base class's default
            # in a slot the class overrides would quietly do the wrong thing,
            # where the retail address traps with its name
            slot_class = qualified_name(slot)
            slot_class = slot_class.split("::")[-2] if slot_class and "::" in slot_class else None
            deleting = re.match(r'\?\?_[EG]([A-Za-z_]\w*)@@', slot)
            if deleting:
                slot_class = deleting.group(1)
            if owner and slot_class == owner.group(1):
                bind_retail_vtables.patches.append((start + 4 * index, slot))

    for symbol, slots in seen.items():
        own = re.match(r'\?\?_7([A-Za-z_]\w*)@@6B', symbol)
        primary = symbol.endswith("@@6B@")
        # The constructor and destructor name the table outright: the last
        # vtable a recovered constructor stores into the object (at the base
        # subobject's offset), or the first one its destructor does, is the
        # class's own.
        candidates = structors.get((own.group(1), vtable_offset(slots)), []) if own else []
        start = None
        if candidates:
            # several when a tag sits on the wrong function (a destructor tag
            # on the deleting destructor, whose first store is a base's):
            # the table holding most of the native slots' functions wins
            wanted = {retail_of[slot] for slot in slots if slot in retail_of}
            start = max(candidates, key=lambda entry: (len(wanted & {
                word_at.get(entry[0] + 4 * index) for index in range(max(len(slots), 16))}),
                *entry[1]))[0]
        if start is not None:
            bind(symbol, slots, start)
            continue
        # A class's own deleting destructor pins its table: the retail table
        # starts that many slots before one of the places the destructor's
        # address appears (identical destructors are shared between classes,
        # so there can be several), and the candidate holding most of the
        # native slots' functions wins. Native slot order may still be wrong
        # around virtuals a recovered header misses, so the score ignores it.
        if not primary:
            continue
        anchor = deleting.get(own.group(1)) if own else None
        anchor_slot = next((index for index, slot in enumerate(slots)
                            if slot and own and re.match(r'\?\?_[EG]' + own.group(1) + '@@', slot)), None)
        if anchor is not None and anchor_slot is not None and words.get(anchor):
            wanted = {retail_of[slot] for slot in slots if slot in retail_of}
            scored = []
            for position in words[anchor]:
                position -= 4 * anchor_slot
                window = {word_at.get(position + 4 * index) for index in range(max(len(slots), 16))}
                scored.append((len(wanted & window), position))
            scored.sort(reverse=True)
            if len(scored) == 1 or scored[0][0] > scored[1][0]:
                bind(symbol, slots, scored[0][1])
            continue
        votes = {}
        anchored = set()
        for index, slot in enumerate(slots):
            address = retail_of.get(slot) if slot else None
            if address is None:
                continue
            for position in words.get(address, ()):
                start = position - 4 * index
                votes[start] = votes.get(start, 0) + 1
                if slot.startswith(("??_E", "??_G")) and index == 0:
                    anchored.add(start)
        if not votes:
            continue
        best = max(votes.values())
        starts = [start for start, count in votes.items() if count == best]
        if len(starts) != 1:
            continue
        start = starts[0]
        if not (best >= 2 or start in anchored):
            continue
        bind(symbol, slots, start)
    return result


def structor_vtables(xbe_path, tag_addresses):
    """{class: retail vtable} from the retail code of recovered constructors
    (the last `mov dword ptr [reg], table` into the object) and destructors
    (the first), for tables in .rdata."""
    xbe = xbe_path.read_bytes()
    base = struct.unpack_from("<I", xbe, 0x104)[0]
    headers = struct.unpack_from("<I", xbe, 0x120)[0] - base
    _, text_va, _, text_raw, text_size = struct.unpack_from("<IIIII", xbe, headers)
    sizes = {}
    with (Path(__file__).resolve().parents[2] / "config/functions.csv").open(newline="") as handle:
        for row in csv.DictReader(handle):
            sizes[int(row["va"], 16)] = int(row["size"])
    # mov dword ptr [eax|ecx|edx|ebx|esi|edi (+ disp8 | disp32)], imm32
    # (a lookahead, so a 16-bit store's tail cannot hide the next store)
    store = re.compile(rb"(?=\xc7(?:([\x00\x01\x02\x03\x06\x07])|([\x40\x41\x42\x43\x46\x47]).|"
                       rb"([\x80\x81\x82\x83\x86\x87])....)(....))", re.S)
    result = {}
    for (unit, qualified), address in tag_addresses.items():
        parts = qualified.split("::")
        if len(parts) < 2 or parts[-1].lstrip("~") != parts[-2]:
            continue
        size = sizes.get(address)
        if not size or not text_va <= address < text_va + text_size:
            continue
        code = xbe[text_raw + address - text_va:text_raw + address - text_va + size]
        # the registers holding the object: ecx (thiscall) and whatever it is
        # copied to (mov esi|edi|ebx|ebp, ecx); eax only when nothing else
        # holds it, since a member's constructor returns its member there
        this_registers = {1}
        copies = {b"\x8b\xf1": 6, b"\x8b\xf9": 7, b"\x8b\xd9": 3, b"\x8b\xe9": 5}
        for pattern, register in copies.items():
            if pattern in code:
                this_registers.add(register)
        if len(this_registers) == 1 and b"\x8b\xc1" in code:
            this_registers.add(0)
        by_offset = {}
        for m in store.finditer(code):
            if m.start() and code[m.start() - 1] == 0x66:
                continue  # a 16-bit store
            # a store through another register may fill a member or a local
            # (or the object, when an optimised call passes it in eax)
            via_this = (code[m.start() + 1] & 7) in this_registers
            table = struct.unpack("<I", m.group(4))[0]
            if not 0x411520 <= table < 0x461180:
                continue
            if m.group(1) is not None:
                offset = 0
            elif m.group(2) is not None:
                offset = struct.unpack("<b", code[m.start() + 2:m.start() + 3])[0]
            else:
                offset = struct.unpack("<i", code[m.start() + 2:m.start() + 6])[0]
            by_offset.setdefault(offset, []).append((table, via_this))
        constructor = not parts[-1].startswith("~")
        for offset, tables in by_offset.items():
            candidates = result.setdefault((parts[-2], offset), [])
            for position, (table, via_this) in enumerate(tables):
                # a constructor's last store is its own table (bases come
                # first), a destructor's first; a destructor tag may also sit
                # on the deleting destructor, whose first store is a base's
                place = position if constructor else len(tables) - 1 - position
                candidates.append((table, (via_this, constructor, place)))
    return result


def mangled_number(text):
    """An MSVC mangled number: '0'-'9' for 1-10, else hex digits as 'A'-'P'
    ending in '@'. Returns (value, rest)."""
    if text[:1].isdigit():
        return int(text[0]) + 1, text[1:]
    value = 0
    for index, char in enumerate(text):
        if char == "@":
            return value, text[index + 1:]
        value = value * 16 + (ord(char) - ord("A"))
    return value, ""


def vtable_offset(slots):
    """The offset of the base subobject a vtable serves: the this-adjustment
    of its thunks (`W<offset>` in their names), 0 for a primary table."""
    for slot in slots:
        m = re.match(r'\?[^@]*@[A-Za-z_]\w*@@W([0-9A-P][A-P@]*)', slot or "") or \
            re.match(r'\?\?_[EG][A-Za-z_]\w*@@W([0-9A-P][A-P@]*)', slot or "")
        if m:
            return mangled_number(m.group(1))[0]
    return 0




def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--bindings", type=Path, action="append", default=[])
    parser.add_argument("--start", type=Path, required=True,
                        help="assembly defining the guest entry points")
    parser.add_argument("--linker-script", type=Path, required=True)
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    parser.add_argument("--exclude", action="append", default=[],
                        help="relative source path to leave out")
    parser.add_argument("--xbe", type=Path,
                        help="the retail XBE (default orig/game/default.xbe), to find retail vtables")
    parser.add_argument("--check", type=Path,
                        help="only compile this source as the image would and print its diagnostics")
    parser.add_argument("--prefer", action="append", default=[],
                        help="relative source path whose definitions win every duplicate")
    args = parser.parse_args()
    PREFERRED.update(args.prefer)
    root = args.root.resolve()
    args.include = [root / "port/include", root / "port/include/crt",
                    root / "port/include/xdk", root / "include"]
    work = args.work.resolve()
    work.mkdir(parents=True, exist_ok=True)

    sources = sorted((root / "src").glob("*.cpp")) + sorted((root / "src/stubs").glob("*.cpp"))
    # the port's own recoveries of retail functions upstream has no source for
    sources += sorted((root / "port/recovered").glob("*.cpp"))
    sources += sorted((root / "port/runtime").glob("guest_*.cpp"))
    sources += sorted((root / "port/runtime").glob("guest_*.c"))
    excluded = set(args.exclude)
    units = [Unit(s, root, work) for s in sources if str(s.relative_to(root)) not in excluded]

    header_files = list((root / "include").glob("*.h")) + list((root / "port/include").rglob("*"))
    header_stamp = str(max(p.stat().st_mtime_ns for p in header_files if p.is_file()))

    # Retail-mode units see copies of the recovered headers whose native
    # branches are forced on: every H2_NATIVE_PORT branch in include/ is a
    # portability fix (assembly, SSE types, rounding), not a behaviour change.
    shadow = work / "shadow-include"
    shadow.mkdir(parents=True, exist_ok=True)
    # The whole tree lives there (unchanged headers as links), because a quoted
    # include searches the including header's own directory first.
    for header in sorted((root / "include").glob("*.h")):
        rewritten = header_native(header.read_text(errors="replace"))
        target = shadow / header.name
        if rewritten is not None:
            text = f'#line 1 "{header}"\n' + rewritten
            if target.is_symlink():
                target.unlink()
            if not target.exists() or target.read_text() != text:
                target.write_text(text)
        elif not target.is_symlink():
            target.unlink(missing_ok=True)
            target.symlink_to(header)
    args.shadow_include = shadow
    header_stamp += REWRITE_VERSION

    if args.check:
        unit = Unit(args.check.resolve(), root, work / "check")
        ok, diagnostic = frontend(unit, args, header_stamp)
        print(f"{unit.relative}: {'compiled in ' + unit.mode + ' mode' if ok else 'failed'}")
        if diagnostic:
            print(diagnostic)
        sys.exit(0 if ok else 1)

    report = {"skipped": {}, "overridden": {}, "bound": {}, "stubbed": [], "data_stubbed": [],
              "native_address_globals": []}
    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        results = list(pool.map(lambda u: frontend(u, args, header_stamp), units))
    compiled = []
    for unit, (ok, diagnostic) in zip(units, results):
        if ok:
            compiled.append(unit)
            report.setdefault("modes", {})[unit.relative] = unit.mode
        else:
            report["skipped"][unit.relative] = diagnostic
    print(f"frontend: {len(compiled)} compiled, {len(report['skipped'])} skipped", flush=True)

    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        scans = dict(zip((u.relative for u in compiled), pool.map(scan, compiled)))

    with (root / "config/functions.csv").open(newline="") as handle:
        csv_sources = {int(r["va"], 16): (r["source"], r["status"]) for r in csv.DictReader(handle)
                       if r["source"]}
    tag_addresses = symbol_addresses(compiled)

    def address_of(unit, symbol):
        qualified = qualified_name(symbol)
        if qualified is None:
            return None
        found = tag_addresses.get((unit.relative, qualified))
        if found is None and "::" in qualified:
            found = tag_addresses.get((unit.relative, qualified.split("::")[-1]))
        return found

    defined_anywhere = set()
    for unit in compiled:
        defined_anywhere.update(scans[unit.relative]["functions"])
        defined_anywhere.update(scans[unit.relative]["globals"])
    for unit in compiled:
        if unit.relative.endswith("_native.cpp"):
            missing = sorted(name for name in scans[unit.relative]["declarations"]
                             if name.startswith("h2_native_") and name not in defined_anywhere)
            if missing:
                INCOMPLETE_NATIVE.add(unit.relative)
                report.setdefault("incomplete_native", {})[unit.relative] = missing

    # Resolve strong function definitions.
    owners = {}
    for unit in compiled:
        for name, kind in scans[unit.relative]["functions"].items():
            if kind == "strong":
                owners.setdefault(name, []).append(unit)
    delete_functions = {u.relative: set() for u in compiled}
    for name, candidates in owners.items():
        if len(candidates) < 2:
            continue
        address = next((a for a in (address_of(c, name) for c in candidates) if a is not None), None)
        ranked = sorted(candidates, key=lambda c: (
            priority(c, name, csv_sources, {name: address}), c.relative))
        winner = ranked[0]
        report["overridden"][name] = {"winner": winner.relative,
                                      "losers": [c.relative for c in ranked[1:]]}
        for loser in ranked[1:]:
            delete_functions[loser.relative].add(name)

    # A placeholder (src/stubs, tagged `@stub 0x<address>`) whose retail
    # function another unit recovers, under another name or signature: the
    # placeholder's symbol becomes a branch to the recovery, so callers that
    # declare it the placeholder's way still reach real code.
    real_by_address = {}
    for unit in sorted(compiled, key=lambda u: (priority(u, "", csv_sources, {}), u.relative)):
        if "/stubs/" in unit.relative:
            continue
        for name, kind in scans[unit.relative]["functions"].items():
            if kind in ("strong", "weak") and name not in delete_functions[unit.relative]:
                address = address_of(unit, name)
                if address is not None:
                    real_by_address.setdefault(address, name)
    stub_tags = symbol_addresses([u for u in compiled if "/stubs/" in u.relative], tag="stub")
    placeholder_aliases = {}
    for unit in compiled:
        if "/stubs/" not in unit.relative:
            continue
        for name, kind in scans[unit.relative]["functions"].items():
            if kind != "strong" or name in delete_functions[unit.relative]:
                continue
            qualified = qualified_name(name)
            address = stub_tags.get((unit.relative, qualified)) if qualified else None
            if address is None and qualified and "::" in qualified:
                address = stub_tags.get((unit.relative, qualified.split("::")[-1]))
            real = real_by_address.get(address) if address is not None else None
            if real and real != name:
                delete_functions[unit.relative].add(name)
                placeholder_aliases[name] = real
    report["placeholder_aliases"] = len(placeholder_aliases)

    # Placeholder bodies from src/stubs that nothing better replaces: the game
    # runs on these, so list them for whoever recovers the real code.
    report["placeholders"] = sorted(
        name for name, candidates in owners.items()
        if name not in placeholder_aliases and "/stubs/" in sorted(candidates, key=lambda c: (
            priority(c, name, csv_sources, {}), c.relative))[0].relative)

    # Functions other units reference but that only exist as internal
    # (static inline) definitions: give one definition external linkage.
    referenced = set()
    for unit in compiled:
        referenced.update(scans[unit.relative]["declarations"])
    promote = {u.relative: set() for u in compiled}
    internal_owners = {}
    for unit in compiled:
        for name, kind in scans[unit.relative]["functions"].items():
            if kind == "internal" and name in referenced and name not in owners:
                internal_owners.setdefault(name, []).append(unit)
    for name, candidates in internal_owners.items():
        winner = sorted(candidates, key=lambda c: (priority(c, name, {}, {}), c.relative))[0]
        promote[winner.relative].add(name)
        report.setdefault("promoted", {})[name] = winner.relative

    # Bindings: explicit manifests first, then addresses carried by names.
    bindings = {}
    for manifest in args.bindings:
        for entry in json.loads(manifest.read_text())["symbols"]:
            bindings[entry["symbol"]] = int(entry["address"], 0)

    for symbol, address in storage_bindings(root, args, work).items():
        bindings.setdefault(symbol, address)
        report.setdefault("storage_bound", {})[symbol] = f"0x{address:08x}"

    xbe_words_at = {}
    xbe_file = args.xbe or root / "orig/game/default.xbe"
    if xbe_file.exists():
        for word, addresses in xbe_data_words(xbe_file).items():
            for at in addresses:
                xbe_words_at[at] = word

    def retail_word(address):
        return xbe_words_at.get(address)

    global_owners = {}
    delete_globals = {u.relative: set() for u in compiled}
    for unit in compiled:
        for name, info in scans[unit.relative]["globals"].items():
            if info["linkage"] in ("internal",):
                continue
            address = bindings.get(name)
            match = ADDRESS_NAME.search(name)
            if address is None and match and not name.startswith(("??_7", "??_C")):
                address = int(match.group(1), 16)
                if address in RETAIL_TEXT:
                    report.setdefault("code_address_globals", []).append(name)
                    address = None
            # A global whose initializer holds code pointers stays native,
            # unless retail initializes it itself: then the retail data
            # (vtables, strings, callbacks, all relocated by the code map) is
            # what the game expects there.
            retail_initialized = address is not None and info["code_pointers"] and \
                unit.relative.startswith("src/") and retail_word(address) not in (None, 0)
            if retail_initialized:
                report.setdefault("bound_retail_initialized", []).append(name)
            if address is not None and (not info["code_pointers"] or retail_initialized):
                delete_globals[unit.relative].add(name)
                bindings.setdefault(name, address)
                report["bound"][name] = f"0x{address:08x}"
                continue
            if address is not None:
                report["native_address_globals"].append(name)
            if info["linkage"] == "strong":
                global_owners.setdefault(name, []).append(unit)
    # Native vtables of classes whose declarations in the recovered headers
    # are incomplete come out short or in the wrong order. Bind each primary
    # vtable to the retail one instead: the host rewrites every retail code
    # address in the XBE's data to its native function, so a retail vtable
    # runs recovered code in every slot recovered under any class name, and a
    # slot nobody recovered faults with its retail address.
    xbe_path = args.xbe or root / "orig/game/default.xbe"
    retail_vtables = bind_retail_vtables(xbe_path, compiled, scans, tag_addresses,
                                         delete_functions) if xbe_path.exists() else {}
    vtable_patches = getattr(bind_retail_vtables, "patches", [])
    report["vtable_patches"] = len(vtable_patches)
    for symbol, address in retail_vtables.items():
        bindings[symbol] = address
        report.setdefault("vtables_bound", {})[symbol] = f"0x{address:08x}"
        for unit in compiled:
            if symbol in scans[unit.relative]["globals"]:
                delete_globals[unit.relative].add(symbol)

    for name, candidates in global_owners.items():
        if len(candidates) < 2:
            continue
        ranked = sorted(candidates, key=lambda c: (priority(c, name, {}, {}), c.relative))
        for loser in ranked[1:]:
            delete_globals[loser.relative].add(name)

    def build(unit):
        try:
            backend(unit, args, delete_functions[unit.relative], delete_globals[unit.relative],
                    promote[unit.relative])
            return None
        except subprocess.CalledProcessError as error:
            stderr = error.stderr.decode() if isinstance(error.stderr, bytes) else (error.stderr or "")
            lines = [l for l in stderr.splitlines() if l.strip()]
            line = next((l for l in lines if "error" in l.lower()), lines[-1] if lines else "failed")
            return f"backend: {line}"

    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        failures = list(pool.map(build, compiled))
    objects = []
    for unit, failure in zip(compiled, failures):
        if failure:
            report["skipped"][unit.relative] = failure
        else:
            objects.append(unit.object)
    print(f"backend: {len(objects)} objects", flush=True)

    # Partial link, then find what is still undefined.
    partial = work / "whole-image.partial.o"
    run([args.lld, "-r", "-o", partial, *objects])
    start = work / "start.o"
    run([args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj", args.start, "-o", start])
    nm = run([args.llvm / "llvm-nm", "--undefined-only", "--format=just-symbols", partial, start],
             capture_output=True, text=True).stdout.split()
    defined = set(run([args.llvm / "llvm-nm", "--defined-only", "--format=just-symbols", partial, start],
                      capture_output=True, text=True).stdout.split())
    undefined = sorted(symbol.replace("%", "@") for symbol in set(nm) - defined)

    declared_functions = set()
    for unit in compiled:
        text_decl = scans[unit.relative]["declarations"]
        declared_functions.update(text_decl)
    # The start assembly only calls functions.
    function_like = set(run([args.llvm / "llvm-nm", "--undefined-only", "--format=just-symbols", start],
                            capture_output=True, text=True).stdout.replace("%", "@").split())
    for unit in compiled:
        ir = unit.ir.read_text()
        function_like.update(unquote(m.group(1)) for m in DECLARE.finditer(ir))

    host = []
    stubs = []
    data = []
    lines = ["// Generated by build_whole_image.py", '.section .text.h2_whole,"ax",@progbits',
             ".p2align 2"]
    retail_calls = []
    for symbol in undefined:
        if symbol in bindings:
            continue
        if symbol in placeholder_aliases:
            continue
        if re.fullmatch(r'h2_retail_[0-9a-f]+', symbol):
            # a call by retail address (port/recovered/ code generated from
            # retail idioms): resolved through the code map below
            retail_calls.append(symbol)
            continue
        match = ADDRESS_NAME.search(symbol)
        if match and symbol not in function_like:
            bindings[symbol] = int(match.group(1), 16)
            report["bound"][symbol] = f"0x{bindings[symbol]:08x}"
            continue
        if symbol.startswith("host_"):
            host.append(symbol)
        elif symbol in function_like:
            stubs.append(symbol)
        else:
            data.append(symbol)
    services = (root / "port/src/host_services.cpp").read_text()
    patched = sorted(set(re.findall(r'\{"(host_h2_\w+)"', services)) | {"host_h2_unimplemented"})
    trampolines = list(host)
    host = sorted(set(host) | set(patched))
    for symbol in trampolines:
        slot = f"h2_host_slot_{symbol}"
        lines += [f'.globl "{symbol}"', f'.type "{symbol}", @function', f'"{symbol}":',
                  f"    adrp x16, {slot}", f"    ldr x16, [x16, :lo12:{slot}]", "    br x16"]
    for index, symbol in enumerate(stubs):
        lines += [f'.globl "{elf_name(symbol)}"', f'.type "{elf_name(symbol)}", @function', f'"{elf_name(symbol)}":',
                  # adrp is PC-relative, so this is already the host address.
                  f"    adrp x0, h2_stub_name_{index}", f"    add x0, x0, :lo12:h2_stub_name_{index}",
                  "    b host_h2_unimplemented"]
    if "host_h2_unimplemented" not in trampolines:
        slot = "h2_host_slot_host_h2_unimplemented"
        lines += [".globl host_h2_unimplemented", ".type host_h2_unimplemented, @function",
                  "host_h2_unimplemented:", f"    adrp x16, {slot}",
                  f"    ldr x16, [x16, :lo12:{slot}]", "    br x16"]
    lines += ['.section .rodata.h2_whole_names,"a",@progbits']
    for index, symbol in enumerate(stubs):
        escaped = symbol.replace("\\", "\\\\").replace('"', '\\"')
        lines += [f"h2_stub_name_{index}:", f'    .asciz "{escaped}"']
    lines += ['.section .data.h2_whole_host,"aw",@progbits', ".p2align 3"]
    for symbol in host:
        slot = f"h2_host_slot_{symbol}"
        lines += [f".globl {slot}", f"{slot}:", "    .xword 0"]
    lines += ['.section .bss.h2_whole_data,"aw",@nobits', ".p2align 4"]
    for symbol in data:
        lines += [f'.globl "{elf_name(symbol)}"', f'"{elf_name(symbol)}":', "    .zero 4096"]
    for symbol, address in sorted(bindings.items()):
        if symbol in undefined:
            escaped = elf_name(symbol).replace('"', '\\"')
            lines += [f'.globl "{escaped}"', f'.set "{escaped}", 0x{address:08x}']
    # Retail code addresses and the native definitions now behind them. The
    # host rewrites retail data words holding these addresses (vtables,
    # callback tables) so guest code never jumps to x86 code.
    retail_code = {}
    deleting = {q.split("::")[0]: a for (u, q), a in tag_addresses.items() if q.endswith("::__deleting")}
    # a definition without a tag of its own (a native rewrite) still stands
    # for the retail function another unit tags under the same name
    tagged_anywhere = {}
    for (u, q), a in tag_addresses.items():
        if "::" in q or not q.startswith("~"):
            tagged_anywhere.setdefault(q, set()).add(a)
    ranked_units = sorted(compiled, key=lambda u: (priority(u, "", csv_sources, {}), u.relative))
    for unit in ranked_units:
        for name, kind in scans[unit.relative]["functions"].items():
            if kind not in ("strong", "weak") or name in delete_functions[unit.relative]:
                continue
            address = address_of(unit, name)
            if address is None:
                qualified = qualified_name(name)
                candidates = tagged_anywhere.get(qualified) if qualified and "::" in qualified else None
                if candidates and len(candidates) == 1:
                    address = next(iter(candidates))
            # the deleting destructor (??_E aliases ??_G, which is the
            # definition the units carry)
            m = re.match(r'\?\?_[EG]([A-Za-z_]\w*)@@U', name)
            if address is None and m:
                address = deleting.get(m.group(1))
            if address is not None and address not in retail_code:
                retail_code[address] = name
    adjustors = {q: a for (u, q), a in tag_addresses.items() if "::__adjustor_" in q}
    if adjustors:
        for unit in ranked_units:
            for name, kind in scans[unit.relative]["functions"].items():
                m = re.match(r'\?\?_[EG]([A-Za-z_]\w*)@@W([0-9A-P][A-P@]*)', name)
                if not m or name in delete_functions[unit.relative]:
                    continue
                address = adjustors.get(f"{m.group(1)}::__adjustor_{mangled_number(m.group(2))[0]}")
                if address is not None and address not in retail_code:
                    retail_code[address] = name
    for symbol in stubs:
        m = re.match(r'\?+function_([0-9a-f]{5,6})@', symbol)
        if m and int(m.group(1), 16) not in retail_code:
            retail_code[int(m.group(1), 16)] = symbol
    lines += ['.section .rodata.h2_retail_code_map,"a",@progbits', ".p2align 2",
              ".globl __h2_retail_code_map_start", "__h2_retail_code_map_start:"]
    for address, symbol in sorted(retail_code.items()):
        lines += [f"    .long 0x{address:08x}", f'    .long "{elf_name(symbol)}"']
    lines += [".globl __h2_retail_code_map_end", "__h2_retail_code_map_end:"]
    # h2_retail_<address>: a branch to the native function behind that retail
    # address, or a trap naming it when nothing recovers it
    lines += [".text", ".p2align 2"]
    for symbol in retail_calls:
        address = int(symbol[len("h2_retail_"):], 16)
        target = retail_code.get(address)
        lines += [f".globl {symbol}", f".type {symbol}, @function", f"{symbol}:"]
        if target:
            lines += [f'    b "{elf_name(target)}"']
        else:
            lines += [f"    adrp x0, {symbol}_name", f"    add x0, x0, :lo12:{symbol}_name",
                      "    b host_h2_unimplemented"]
            report.setdefault("retail_calls_missing", []).append(f"0x{address:x}")
    for symbol, real in sorted(placeholder_aliases.items()):
        if symbol in undefined:
            lines += [f'.globl "{elf_name(symbol)}"', f'.type "{elf_name(symbol)}", @function',
                      f'"{elf_name(symbol)}":', f'    b "{elf_name(real)}"']
    lines += ['.section .rodata.h2_whole_names,"a",@progbits']
    for symbol in retail_calls:
        lines += [f"{symbol}_name:", f'    .asciz "retail function 0x{symbol[len("h2_retail_"):]}"']
    lines += ['.section .rodata.h2_vtable_patches,"a",@progbits', ".p2align 2",
              ".globl __h2_vtable_patches_start", "__h2_vtable_patches_start:"]
    for slot_address, symbol in vtable_patches:
        if symbol in stubs or symbol in undefined:
            continue
        lines += [f"    .long 0x{slot_address:08x}", f'    .long "{elf_name(symbol)}"']
    lines += [".globl __h2_vtable_patches_end", "__h2_vtable_patches_end:"]
    report["retail_code_map"] = len(retail_code)

    glue = work / "glue.s"
    glue.write_text("\n".join(lines) + "\n")
    glue_object = work / "glue.o"
    run([args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj", glue, "-o", glue_object])
    report["stubbed"] = stubs
    report["data_stubbed"] = data
    report["host_imports"] = host

    image = work / "whole-image.elf"
    link = subprocess.run([str(args.lld), "-static", "--no-undefined", "-T", str(args.linker_script),
                           "-z", "max-page-size=0x4000", "-o", str(image), str(partial),
                           str(start), str(glue_object)], capture_output=True, text=True)
    report["link"] = link.stderr.strip().splitlines()[:50]
    (work / "whole-image-report.json").write_text(json.dumps(report, indent=1, sort_keys=True))
    print(f"stubs: {len(stubs)} functions, {len(data)} data; host imports: {len(host)}; "
          f"bound globals: {len(report['bound'])}; overrides: {len(report['overridden'])}")
    if link.returncode:
        print(link.stderr[:4000])
        return 1
    print(f"image: {image}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
