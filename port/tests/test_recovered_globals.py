#!/usr/bin/env python3
"""Bind recovered globals to original guest addresses and execute their user."""

import argparse
from pathlib import Path
import subprocess
import tempfile


def run(*args):
    subprocess.run([str(value) for value in args], check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--host-cxx", type=Path, required=True)
    args = parser.parse_args()

    source = args.root / "src/unknown_07a9a0_2.cpp"
    with tempfile.TemporaryDirectory(prefix="h2-recovered-globals-") as temporary:
        root = Path(temporary)
        ir = root / "globals.ll"
        rebased = root / "globals.rebased.ll"
        assembly = root / "globals.s"
        symbols = root / "symbols.s"
        executable = root / "globals-host"

        run(args.llvm / "clang++", "--target=arm64_32-apple-watchos", "-O2",
            "-ffreestanding", "-fno-exceptions", "-fno-rtti",
            f"-I{args.root / 'include'}", "-S", "-emit-llvm", source, "-o", ir)
        run(args.llvm / "opt", f"-load-pass-plugin={args.plugin}",
            "-passes=h2-rebase,verify", "-S", ir, "-o", rebased)
        run(args.llvm / "llc", "-O2", "-mtriple=arm64-apple-macosx",
            rebased, "-o", assembly)

        symbols.write_text("""
.globl _g_4cf791
.set _g_4cf791, 0x4cf791
.globl _g_4cf792
.set _g_4cf792, 0x4cf792
""")
        run(args.host_cxx, "-std=c++20", "-arch", "arm64",
            args.root / "port/tests/recovered_globals_host.cpp", assembly, symbols,
            "-o", executable)
        run(executable)


if __name__ == "__main__":
    main()
