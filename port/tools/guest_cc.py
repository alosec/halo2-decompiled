#!/usr/bin/env python3
"""Compile one recovered source file into a native ARM64 guest object."""

import argparse
from pathlib import Path
import re
import subprocess
import tempfile


def run(*args):
    subprocess.run([str(value) for value in args], check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--include", type=Path, action="append", default=[])
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("source", type=Path)
    args = parser.parse_args()

    args.output.parent.mkdir(parents=True, exist_ok=True)
    compiler = args.llvm / ("clang++" if args.source.suffix in {".cc", ".cpp", ".cxx"} else "clang")
    with tempfile.TemporaryDirectory(prefix="h2-guest-cc-") as temporary:
        root = Path(temporary)
        ir = root / "unit.ll"
        rebased = root / "unit.rebased.ll"
        assembly = root / "unit.s"
        language_flags = ["-fno-exceptions", "-fno-rtti"] if compiler.name == "clang++" else []
        include_flags = [f"-I{directory}" for directory in args.include]
        run(compiler, "--target=arm64_32-apple-watchos", "-O2", "-ffreestanding",
            "-DH2_NATIVE_PORT=1", "-fms-extensions", "-Wno-ignored-attributes",
            *language_flags, *include_flags,
            "-S", "-emit-llvm", args.source, "-o", ir)
        run(args.llvm / "opt", f"-load-pass-plugin={args.plugin}",
            "-passes=h2-rebase,verify", "-S", ir, "-o", rebased)
        run(args.llvm / "llc", "-O2", "-mtriple=arm64_32-apple-watchos",
            rebased, "-o", assembly)
        # arm64_32 encodes a GOT pointer load as a 32-bit LDR because its
        # native pointer is 32 bits. The object is linked into a normal ARM64
        # Mach-O host, whose GOT slots and relocation require an x-register.
        # Load the complete host symbol address; the following generated
        # truncation keeps the guest value ILP32 before the rebase pass uses it.
        text = assembly.read_text()
        text = re.sub(
            r"(^\s*ldr\s+)w(\d+)(\s*,\s*\[x\d+\s*,\s*[^\]]+@GOTPAGEOFF[^\]]*\])",
            r"\1x\2\3", text, flags=re.MULTILINE)
        assembly.write_text(text)
        run("clang", "-arch", "arm64", "-c", assembly, "-o", args.output)


if __name__ == "__main__":
    main()
