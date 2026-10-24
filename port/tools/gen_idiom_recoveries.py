#!/usr/bin/env python3
"""Write port/recovered/idiom_functions_generated.cpp: C++ for the
compiler-generated retail functions (deleting destructors, destructor thunks,
this-adjusting thunks) that follow a fixed shape.

Upstream recovers these as tags only (`@retail 0x31a710 deleting class_x`),
which is enough for its per-function byte matching but leaves the whole image
without code at those addresses when nothing instantiates the class: retail
vtables that point at them then reach unrecovered x86. Each shape here is a
handful of calls, so the generated function calls the retail functions it
names by address (h2_retail_<address>, which the image build resolves to the
native function the code map holds for that address).

Shapes (whole function bytes, call targets masked):
  deleting destructor   push esi/edi; mov this, ecx; [mov eax, this;]
                        call D; test byte [esp+8], 1; je; push this; call F;
                        pop ecx; mov eax, this; pop; ret 4
  Havok deleting        ... call D; test byte [esp+8], 1; je; movzx edx,
                        word [this+4]; (g_480118)->release_block(this, edx, TAG)
  empty-dtor deleting   mov [this], base table; if (flags & 1) free(this)
  Havok sized deleting  ... call D; if (flags & 1) release_block(this, SIZE, TAG)
  thunk                 jmp T
  adjustor              sub ecx, N; jmp T
A call to the C library's free (0x321379, 0x324c91) becomes free().

usage: gen_idiom_recoveries.py [--xbe orig/game/default.xbe]"""
import argparse
import csv
import re
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FREE = {0x321379, 0x324c91}

DELETING = re.compile(rb"(?:\x56\x8b\xf1(?:\x8b\xc6)?|\x57\x8b\xf9(?:\x8b\xc7)?)\xe8(....)"
                      rb"\xf6\x44\x24\x08\x01\x74\x07[\x56\x57]\xe8(....)\x59\x8b[\xc6\xc7][\x5e\x5f]"
                      rb"\xc2\x04\x00", re.S)
HAVOK_DELETING = re.compile(rb"\x56\x8b\xf1\xe8(....)\xf6\x44\x24\x08\x01\x74\x13\x0f\xb7\x56\x04"
                            rb"\x8b\x0d\x18\x01\x48\x00\x8b\x01\x6a(.)\x52\x56\xff\x50\x14\x8b\xc6\x5e"
                            rb"\xc2\x04\x00", re.S)
# an empty destructor inlined: the base's table goes back, then the block
INLINE_DELETING = re.compile(rb"\xf6\x44\x24\x04\x01\x56\x8b\xf1\xc7\x06(....)\x74\x09\x56\xe8(....)"
                             rb"\x83\xc4\x04\x8b\xc6\x5e\xc2\x04\x00", re.S)
# a Havok deleting destructor of a fixed-size object
HAVOK_SIZED_DELETING = re.compile(rb"\x56\x8b\xf1\xe8(....)\xf6\x44\x24\x08\x01\x74\x10\x8b\x0d\x18\x01"
                                  rb"\x48\x00\x8b\x01\x6a(.)(?:\x6a(.)|\x68(....))\x56\xff\x50\x14\x8b\xc6\x5e"
                                  rb"\xc2\x04\x00", re.S)
THUNK = re.compile(rb"\xe9(....)", re.S)
ADJUSTOR = re.compile(rb"\x83([\xe9\xc1])(.)\xe9(....)", re.S)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--xbe", type=Path, default=ROOT / "orig/game/default.xbe")
    args = parser.parse_args()
    xbe = args.xbe.read_bytes()
    base = struct.unpack_from("<I", xbe, 0x104)[0]
    count = struct.unpack_from("<I", xbe, 0x11c)[0]
    headers = struct.unpack_from("<I", xbe, 0x120)[0] - base
    sections = []
    for index in range(count):
        _, va, vsize, raw, rsize, name = struct.unpack_from("<IIIIII", xbe, headers + index * 56)
        offset = name - base
        sections.append((xbe[offset:xbe.index(b"\0", offset)], va, vsize, raw, rsize))
    text_va, text_raw, text_size = next((va, raw, rsize) for n, va, _, raw, rsize in sections if n == b".text")

    rows = {}
    with (ROOT / "config/functions.csv").open(newline="") as handle:
        for row in csv.DictReader(handle):
            rows[int(row["va"], 16)] = row

    # candidates: what the source tags as compiler-generated, and every
    # function retail data points at that no source recovers
    candidates = set()
    for source in (ROOT / "src").rglob("*.cpp"):
        candidates.update(int(m, 16) for m in re.findall(
            r"@retail 0x([0-9a-f]+) (?:deleting|destructor|adjustor)", source.read_text(errors="replace")))
    for name, va, _, raw, rsize in sections:
        if name not in (b".rdata", b".data"):
            continue
        for at in range(0, rsize - 3, 4):
            word = struct.unpack_from("<I", xbe, raw + at)[0]
            if word in rows and not rows[word]["source"]:
                candidates.add(word)

    def target(address, at, field):
        return address + at + 4 + struct.unpack("<i", field)[0]

    def call(callee, arguments):
        if callee in FREE:
            return f"free({arguments.split(',')[0]})"
        used.add(callee)
        return f"h2_retail_{callee:x}({arguments})"

    used = set()
    bodies = []
    for address in sorted(candidates):
        row = rows.get(address)
        if not row or not text_va <= address < text_va + text_size:
            continue
        code = xbe[text_raw + address - text_va:text_raw + address - text_va + int(row["size"])]
        m = DELETING.fullmatch(code)
        if m:
            destructor = target(address, m.start(1), m.group(1))
            release = target(address, m.start(2), m.group(2))
            bodies.append((address, "deleting destructor",
                           f"\t{call(destructor, 'self, 0, 0, 0')};\n"
                           f"\tif (flags & 1)\n\t\t{call(release, 'self, 0, 0, 0')};\n\treturn self;"))
            continue
        m = HAVOK_DELETING.fullmatch(code)
        if m:
            destructor = target(address, m.start(1), m.group(1))
            tag = m.group(2)[0]
            bodies.append((address, f"Havok deleting destructor (tag 0x{tag:x})",
                           f"\t{call(destructor, 'self, 0, 0, 0')};\n"
                           f"\tif (flags & 1)\n\t\tg_memory_2d88a0->release_block(self, "
                           f"*(unsigned short *)((char *)self + 4), 0x{tag:x});\n\treturn self;"))
            continue
        m = INLINE_DELETING.fullmatch(code)
        if m:
            table = struct.unpack("<I", m.group(1))[0]
            release = target(address, m.start(2), m.group(2))
            bodies.append((address, "deleting destructor of an empty destructor",
                           f"\t*(unsigned int *)self = 0x{table:x};\n"
                           f"\tif (flags & 1)\n\t\t{call(release, 'self, 0, 0, 0')};\n\treturn self;"))
            continue
        m = HAVOK_SIZED_DELETING.fullmatch(code)
        if m:
            destructor = target(address, m.start(1), m.group(1))
            tag = m.group(2)[0]
            size = m.group(3)[0] if m.group(3) is not None else struct.unpack("<I", m.group(4))[0]
            bodies.append((address, f"Havok deleting destructor (0x{size:x} bytes, tag 0x{tag:x})",
                           f"\t{call(destructor, 'self, 0, 0, 0')};\n"
                           f"\tif (flags & 1)\n\t\tg_memory_2d88a0->release_block(self, 0x{size:x}, 0x{tag:x});\n"
                           f"\treturn self;"))
            continue
        m = THUNK.fullmatch(code)
        if m:
            callee = target(address, m.start(1), m.group(1))
            bodies.append((address, "thunk", f"\treturn {call(callee, 'self, flags, c, d')};"))
            continue
        m = ADJUSTOR.fullmatch(code)
        if m:
            amount = m.group(2)[0]
            sign = "-" if m.group(1) == b"\xe9" else "+"
            callee = target(address, m.start(3), m.group(3))
            bodies.append((address, f"this {sign}= 0x{amount:x}, then a thunk",
                           f"\treturn {call(callee, f'(char *)self {sign} 0x{amount:x}, flags, c, d')};"))

    lines = [
        "// @flags /O2",
        "// Generated by port/tools/gen_idiom_recoveries.py: compiler-generated retail",
        "// functions of a fixed shape (deleting destructors, destructor thunks,",
        "// this-adjusting thunks), calling the retail functions they name by",
        "// address. Do not edit; rerun the script.",
        "",
        "#include \"reference_counted_2d88a0.h\"",
        "",
        "extern \"C\" void free(void *block);",
        "",
    ]
    for callee in sorted(used):
        lines.append(f"extern \"C\" void *h2_retail_{callee:x}(void *self, unsigned long flags, void *c, void *d);")
    lines.append("")
    for address, what, body in bodies:
        lines += [f"/* {what} */", f"// @retail 0x{address:x}",
                  f"extern \"C\" void *idiom_{address:x}(void *self, unsigned long flags, void *c, void *d)",
                  "{", "\t(void)flags;", "\t(void)c;", "\t(void)d;", body, "}", ""]
    out = ROOT / "port/recovered/idiom_functions_generated.cpp"
    out.write_text("\n".join(lines))
    print(f"{out.relative_to(ROOT)}: {len(bodies)} functions")


if __name__ == "__main__":
    main()
