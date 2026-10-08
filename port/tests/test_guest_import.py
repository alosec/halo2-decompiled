#!/usr/bin/env python3
"""Compile and execute a versioned guest-to-host call with a pointer argument."""

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

    with tempfile.TemporaryDirectory(prefix="h2-guest-import-") as temporary:
        output = Path(temporary)
        ir = output / "import.ll"
        rebased = output / "import.rebased.ll"
        assembly = output / "import.s"
        executable = output / "import-host"
        include = args.root / "port/include"

        run(args.llvm / "clang", "--target=arm64_32-apple-watchos", "-O2",
            "-ffreestanding", f"-I{include}", "-S", "-emit-llvm",
            args.root / "port/tests/guest_import_probe.c", "-o", ir)
        run(args.llvm / "opt", f"-load-pass-plugin={args.plugin}",
            "-passes=h2-rebase,verify", "-S", ir, "-o", rebased)
        transformed = rebased.read_text()
        assert "@host_h2_log(ptr addrspace(272)" in transformed
        assert "@host_h2_abi_version()" in transformed

        run(args.llvm / "llc", "-O2", "-mtriple=arm64_32-apple-watchos",
            rebased, "-o", assembly)
        run(args.host_cxx, "-std=c++20", "-arch", "arm64", f"-I{include}",
            args.root / "port/tests/guest_import_host.cpp", assembly,
            "-o", executable)
        run(executable)


if __name__ == "__main__":
    main()
