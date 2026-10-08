#!/usr/bin/env python3
"""Link, patch, and execute one generated AArch64 host import trampoline."""

import argparse
import ctypes
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

GUEST_BIAS = 0x10000000000
PAGE_SIZE = 0x4000
MAP_PRIVATE = 0x0002
MAP_ANON = 0x1000
PROT_READ = 1
PROT_WRITE = 2
PROT_EXEC = 4


def run(*args):
    subprocess.run([str(value) for value in args], check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--tool", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    args = parser.parse_args()

    with tempfile.TemporaryDirectory(prefix="h2-host-import-") as temporary:
        root = Path(temporary)
        plan = root / "plan.json"
        assembly = root / "imports.s"
        obj = root / "imports.o"
        script = root / "imports.ld"
        image = root / "imports.elf"
        plan.write_text(json.dumps({"imports": [
            {"symbol": "host_h2_probe", "category": "host_abi"}
        ]}))
        run(sys.executable, args.tool, "--output", assembly, plan)
        run(args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj",
            assembly, "-o", obj)
        script.write_text("""SECTIONS {
 . = 0x10000000;
 .text : { *(.text .text.*) }
 .data : { *(.data .data.*) }
}\n""")
        run(args.lld, "-static", "-T", script, "-z", "max-page-size=0x4000",
            "-o", image, obj)

        symbols = {}
        listing = subprocess.check_output(
            [args.llvm / "llvm-nm", "--format=posix", "--defined-only", image],
            text=True)
        for line in listing.splitlines():
            fields = line.rsplit(maxsplit=3)
            if len(fields) >= 3:
                symbols[fields[0]] = int(fields[2], 16)
        function = symbols["host_h2_probe"]
        slot = symbols["h2_host_slot_host_h2_probe"]

        data = image.read_bytes()
        program_offset = struct.unpack_from("<Q", data, 32)[0]
        record_size, record_count = struct.unpack_from("<HH", data, 54)
        segments = []
        for index in range(record_count):
            record = struct.unpack_from(
                "<IIQQQQQQ", data, program_offset + index * record_size)
            if record[0] == 1:
                segments.append((record[2], record[3], record[5], record[6]))
        low = min(item[1] for item in segments) & ~(PAGE_SIZE - 1)
        high = max(item[1] + item[3] for item in segments)
        high = (high + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)

        libc = ctypes.CDLL(None, use_errno=True)
        libc.mmap.restype = ctypes.c_void_p
        address = GUEST_BIAS + low
        mapped = libc.mmap(ctypes.c_void_p(address), high - low,
                           PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANON, -1, 0)
        if mapped != address:
            raise RuntimeError(f"could not map host import test at {address:#x}")
        callback_type = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_int)
        callback = callback_type(lambda value: value + 7)
        try:
            for file_offset, virtual_address, file_size, _ in segments:
                ctypes.memmove(GUEST_BIAS + virtual_address,
                               data[file_offset:file_offset + file_size], file_size)
            ctypes.c_uint64.from_address(GUEST_BIAS + slot).value = (
                ctypes.cast(callback, ctypes.c_void_p).value)
            if libc.mprotect(ctypes.c_void_p(address), high - low,
                             PROT_READ | PROT_EXEC) != 0:
                raise OSError(ctypes.get_errno(), "mprotect host import image")
            trampoline = callback_type(GUEST_BIAS + function)
            if trampoline(35) != 42:
                raise RuntimeError("patched host import trampoline returned wrong value")
            print("patched AArch64 host import trampoline passed")
        finally:
            libc.munmap(ctypes.c_void_p(address), high - low)


if __name__ == "__main__":
    main()
