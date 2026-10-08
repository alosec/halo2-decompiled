#!/usr/bin/env python3
"""Compile one recovered source file into a native ARM64 guest object."""

import argparse
from pathlib import Path
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
            "-Wno-ignored-attributes", *language_flags, *include_flags,
            "-S", "-emit-llvm", args.source, "-o", ir)
        run(args.llvm / "opt", f"-load-pass-plugin={args.plugin}",
            "-passes=h2-rebase,verify", "-S", ir, "-o", rebased)
        run(args.llvm / "llc", "-O2", "-mtriple=arm64-apple-macosx",
            rebased, "-o", assembly)
        run("clang", "-arch", "arm64", "-c", assembly, "-o", args.output)


if __name__ == "__main__":
    main()
