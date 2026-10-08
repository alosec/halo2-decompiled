#!/usr/bin/env python3
"""Compile a synthetic ILP32 guest unit and verify the Halo 2 rebase transform."""

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
    args = parser.parse_args()

    source_text = """
extern void host_probe(void *);
unsigned guest_load(unsigned *address) { return *address; }
void guest_host_call(void *address) { host_probe(address); }
"""
    with tempfile.TemporaryDirectory(prefix="h2-guest-rebase-") as temporary:
        root = Path(temporary)
        source = root / "probe.c"
        ir = root / "probe.ll"
        rebased = root / "probe.rebased.ll"
        assembly = root / "probe.s"
        source.write_text(source_text)

        run(args.llvm / "clang", "--target=arm64_32-apple-watchos", "-O1",
            "-ffreestanding", "-S", "-emit-llvm", source, "-o", ir)
        run(args.llvm / "opt", f"-load-pass-plugin={args.plugin}",
            "-passes=h2-rebase,verify", "-S", ir, "-o", rebased)
        transformed = rebased.read_text()
        assert "ptr addrspace(272)" in transformed
        assert "1099511627776" in transformed
        assert "@host_probe(ptr addrspace(272))" in transformed

        run(args.llvm / "llc", "-O2", "-mtriple=arm64_32-apple-watchos",
            rebased, "-o", assembly)
        machine_code = assembly.read_text()
        assert "#0x10000000000" in machine_code
        assert "_host_probe" in machine_code


if __name__ == "__main__":
    main()
