#!/usr/bin/env python3
"""Compile one real matching Halo 2 source unit through the ARM ILP32 pipeline."""

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

    source = args.root / "src/unknown_0259a0.cpp"
    with tempfile.TemporaryDirectory(prefix="h2-recovered-unit-") as temporary:
        root = Path(temporary)
        ir = root / "random.ll"
        rebased = root / "random.rebased.ll"
        assembly = root / "random.s"
        executable = root / "random-host"

        run(args.llvm / "clang++", "--target=arm64_32-apple-watchos", "-O2",
            "-ffreestanding", "-fno-exceptions", "-fno-rtti",
            f"-I{args.root / 'include'}", "-S", "-emit-llvm", source, "-o", ir)
        run(args.llvm / "opt", f"-load-pass-plugin={args.plugin}",
            "-passes=h2-rebase,verify", "-S", ir, "-o", rebased)
        run(args.llvm / "llc", "-O2", "-mtriple=arm64-apple-macosx",
            rebased, "-o", assembly)

        transformed = rebased.read_text()
        machine_code = assembly.read_text()
        assert "define noundef float @_Z14function_259a0Pm" in transformed
        assert "load i32, ptr addrspace(272)" in transformed
        assert "store i32" in transformed and "ptr addrspace(272)" in transformed
        assert "#0x10000000000" in machine_code
        assert "madd" in machine_code
        assert "ucvtf" in machine_code

        run(args.host_cxx, "-std=c++20", "-arch", "arm64",
            args.root / "port/tests/recovered_random_host.cpp", assembly,
            "-o", executable)
        run(executable)


if __name__ == "__main__":
    main()
