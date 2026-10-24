#!/usr/bin/env python3
"""Disassemble retail functions or dump retail data, straight from the XBE.

usage: retail_dis.py [--xbe orig/game/default.xbe] ADDRESS...
       retail_dis.py --data ADDRESS COUNT

Each ADDRESS is a function start from config/functions.csv; its size comes
from there. Call and jump targets that are other functions get their
functions.csv status and source appended, so it is clear which callees are
recovered. --data prints COUNT dwords from the retail image (the XBE's
initial contents, before the game runs), marking words that are function
starts."""
import argparse
import csv
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OBJDUMP = "/opt/homebrew/opt/llvm@22/bin/llvm-objdump"


def sections(xbe):
    base = struct.unpack_from("<I", xbe, 0x104)[0]
    count = struct.unpack_from("<I", xbe, 0x11c)[0]
    headers = struct.unpack_from("<I", xbe, 0x120)[0] - base
    result = []
    for index in range(count):
        _, va, vsize, raw, rsize, name = struct.unpack_from("<IIIIII", xbe, headers + index * 56)
        offset = name - base
        result.append((xbe[offset:xbe.index(b"\0", offset)].decode(), va, vsize, raw, rsize))
    return result


def read(xbe, secs, address, size):
    for _, va, vsize, raw, rsize in secs:
        if va <= address < va + vsize:
            offset = address - va
            data = xbe[raw + offset:raw + min(rsize, offset + size)]
            return data + bytes(size - len(data))
    return None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--xbe", type=Path, default=ROOT / "orig/game/default.xbe")
    parser.add_argument("--data", action="store_true")
    parser.add_argument("values", nargs="+", type=lambda text: int(text, 16))
    args = parser.parse_args()
    xbe = args.xbe.read_bytes()
    secs = sections(xbe)
    with (ROOT / "config/functions.csv").open(newline="") as handle:
        rows = {int(row["va"], 16): row for row in csv.DictReader(handle)}

    def describe(address):
        row = rows.get(address)
        if not row:
            return ""
        source = row["source"] or "no source"
        name = f" {row['name']}" if row["name"] else ""
        return f"  ; fn_{address:x}{name} [{row['status']}, {source}]"

    if args.data:
        address, count = args.values[0], args.values[1] if len(args.values) > 1 else 16
        data = read(xbe, secs, address, count * 4)
        if data is None:
            sys.exit(f"0x{address:x} is outside the image")
        for index in range(count):
            word = struct.unpack_from("<I", data, index * 4)[0]
            print(f"{address + index * 4:08x}: {word:08x}{describe(word)}")
        return

    for address in args.values:
        row = rows.get(address)
        if not row:
            sys.exit(f"0x{address:x} is not a function start in config/functions.csv")
        size = int(row["size"])
        code = read(xbe, secs, address, size)
        with tempfile.TemporaryDirectory() as temp:
            raw = Path(temp) / "code.bin"
            raw.write_bytes(code)
            elf = Path(temp) / "code.elf"
            subprocess.run([str(Path(OBJDUMP).with_name("llvm-objcopy")), "-I", "binary", "-O",
                            "elf32-i386", "--change-section-address", f".data=0x{address:x}",
                            raw, elf], check=True)
            out = subprocess.run([OBJDUMP, "-D", "-j", ".data", "--no-show-raw-insn", "-M", "intel", elf],
                                 capture_output=True, text=True, check=True).stdout
        print(f"; fn_{address:x} size {size}{describe(address)}")
        for line in out.splitlines():
            m = re.match(r"\s+([0-9a-f]+):\s+(.*)", line)
            if not m:
                continue
            text = re.sub(r"\s*<[^>]*>", "", m.group(2))
            target = re.search(r"\b(?:call|j\w+)\s+0x([0-9a-f]+)$", text)
            note = describe(int(target.group(1), 16)) if target else ""
            print(f"  {m.group(1)}: {text}{note}")
        print()


if __name__ == "__main__":
    main()
