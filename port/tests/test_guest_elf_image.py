#!/usr/bin/env python3
"""Link, load, and execute a Microsoft-ABI ARM ILP32 guest image."""

import argparse
import ctypes
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


def map_exact(libc, address, size, protection):
    mapped = libc.mmap(ctypes.c_void_p(address), size, protection,
                       MAP_PRIVATE | MAP_ANON, -1, 0)
    if mapped != address:
        if mapped not in (-1, ctypes.c_void_p(-1).value):
            libc.munmap(ctypes.c_void_p(mapped), size)
        raise RuntimeError(f"could not map guest memory at {address:#x}")
    return mapped


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    args = parser.parse_args()

    with tempfile.TemporaryDirectory(prefix="h2-guest-elf-") as temporary:
        output = Path(temporary)
        darwin_assembly = output / "probe.darwin.s"
        elf_assembly = output / "probe.elf.s"
        elf_object = output / "probe.o"
        linker_script = output / "probe.ld"
        image = output / "probe.elf"
        include_args = []
        for directory in (args.root / "port/include", args.root / "port/include/crt",
                          args.root / "port/include/xdk", args.root / "include"):
            include_args.extend(("--include", directory))
        run(sys.executable, args.root / "port/tools/guest_cc.py",
            "--llvm", args.llvm, "--plugin", args.plugin, *include_args,
            "--ms-abi", "--emit-assembly", "--output", darwin_assembly,
            args.root / "port/tests/ms_abi_image_probe.cpp")
        run(sys.executable, args.root / "port/tools/guest_asm_elf.py",
            darwin_assembly, elf_assembly)
        run(args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj",
            elf_assembly, "-o", elf_object)
        linker_script.write_text("""SECTIONS {
 . = 0x10000000;
 .text : { *(.text .text.*) }
 .rodata : { *(.rodata .rodata.*) }
 .data : { *(.data .data.*) }
 .bss : { *(.bss .bss.* COMMON) }
}\n""")
        run(args.lld, "-static", "-T", linker_script, "-z", "max-page-size=0x4000",
            "-o", image, elf_object)

        symbols = {}
        listing = subprocess.check_output(
            [args.llvm / "llvm-nm", "--format=posix", "--defined-only", image], text=True)
        for line in listing.splitlines():
            fields = line.split()
            if len(fields) >= 3:
                symbols[fields[0]] = int(fields[2], 16)
        required = ("h2_image_probe_value", "h2_image_probe_construct",
                    "??_7c_image_probe", "?read", "?add")
        if any(name not in symbols for name in required):
            raise RuntimeError("linked guest image is missing Microsoft-ABI symbols")

        data = image.read_bytes()
        program_offset = struct.unpack_from("<Q", data, 32)[0]
        program_size, program_count = struct.unpack_from("<HH", data, 54)
        segments = []
        for index in range(program_count):
            record = struct.unpack_from("<IIQQQQQQ", data,
                                        program_offset + index * program_size)
            if record[0] == 1:
                segments.append((record[2], record[3], record[5], record[6]))
        low = min(segment[1] for segment in segments) & ~(PAGE_SIZE - 1)
        high = max(segment[1] + segment[3] for segment in segments)
        high = (high + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)

        libc = ctypes.CDLL(None, use_errno=True)
        libc.mmap.restype = ctypes.c_void_p
        image_mapping = GUEST_BIAS + low
        object_offset = 0x20000000
        object_mapping = GUEST_BIAS + object_offset
        map_exact(libc, image_mapping, high - low, PROT_READ | PROT_WRITE)
        map_exact(libc, object_mapping, PAGE_SIZE, PROT_READ | PROT_WRITE)
        try:
            for file_offset, virtual_address, file_size, _ in segments:
                ctypes.memmove(GUEST_BIAS + virtual_address,
                               data[file_offset:file_offset + file_size], file_size)
            if libc.mprotect(ctypes.c_void_p(image_mapping), high - low,
                             PROT_READ | PROT_EXEC) != 0:
                raise OSError(ctypes.get_errno(), "mprotect guest image")

            value = ctypes.CFUNCTYPE(ctypes.c_int)(
                GUEST_BIAS + symbols["h2_image_probe_value"])
            construct = ctypes.CFUNCTYPE(None, ctypes.c_uint32, ctypes.c_int)(
                GUEST_BIAS + symbols["h2_image_probe_construct"])
            if value() != 0x48493221:
                raise RuntimeError("guest image function returned the wrong value")
            construct(object_offset, 41)
            vtable = ctypes.c_uint32.from_address(object_mapping).value
            if vtable != symbols["??_7c_image_probe"]:
                raise RuntimeError("constructor installed the wrong 32-bit vtable")
            read_address = ctypes.c_uint32.from_address(GUEST_BIAS + vtable).value
            add_address = ctypes.c_uint32.from_address(GUEST_BIAS + vtable + 4).value
            if read_address != symbols["?read"] or add_address != symbols["?add"]:
                raise RuntimeError("linked vtable relocations are incorrect")
            read = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_uint32)(GUEST_BIAS + read_address)
            add = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_uint32, ctypes.c_int)(
                GUEST_BIAS + add_address)
            if read(object_offset) != 41 or add(object_offset, 5) != 46:
                raise RuntimeError("Microsoft-ABI virtual dispatch failed")
            print("linked guest ELF, 32-bit vtable relocation, and virtual dispatch passed")
        finally:
            libc.munmap(ctypes.c_void_p(object_mapping), PAGE_SIZE)
            libc.munmap(ctypes.c_void_p(image_mapping), high - low)


if __name__ == "__main__":
    main()
