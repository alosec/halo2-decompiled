#!/usr/bin/env python3
"""Compile one recovered source file into an AArch64 ELF guest object."""

import argparse
from pathlib import Path
import subprocess
import sys
import tempfile


def run(*args):
    subprocess.run([str(value) for value in args], check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--include", type=Path, action="append", default=[])
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--ms-abi", action="store_true")
    parser.add_argument("--extract-symbol", action="append", default=[])
    parser.add_argument("source", type=Path)
    args = parser.parse_args()

    args.output.parent.mkdir(parents=True, exist_ok=True)
    tools = Path(__file__).resolve().parent
    with tempfile.TemporaryDirectory(prefix="h2-guest-elf-cc-") as temporary:
        root = Path(temporary)
        darwin = root / "unit.darwin.s"
        elf = root / "unit.elf.s"
        command = [
            sys.executable, tools / "guest_cc.py",
            "--llvm", args.llvm,
            "--plugin", args.plugin,
        ]
        for include in args.include:
            command.extend(("--include", include))
        if args.ms_abi:
            command.append("--ms-abi")
        for symbol in args.extract_symbol:
            command.extend(("--extract-symbol", symbol))
        command.extend(("--emit-assembly", "--output", darwin, args.source))
        run(*command)
        run(sys.executable, tools / "guest_asm_elf.py", darwin, elf)
        run(args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj",
            elf, "-o", args.output)


if __name__ == "__main__":
    main()
