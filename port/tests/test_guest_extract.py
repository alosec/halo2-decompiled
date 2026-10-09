#!/usr/bin/env python3
"""Verify function extraction retains referenced constant initializers."""

import argparse
from pathlib import Path
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    args = parser.parse_args()

    with tempfile.TemporaryDirectory(prefix="h2-guest-extract-") as temporary:
        output = Path(temporary) / "probe.o"
        command = [
            sys.executable, args.root / "port/tools/guest_cc.py",
            "--llvm", args.llvm,
            "--plugin", args.plugin,
        ]
        for directory in (args.root / "port/include", args.root / "port/include/crt",
                          args.root / "port/include/xdk", args.root / "include"):
            command.extend(("--include", directory))
        command.extend((
            "--ms-abi",
            "--extract-symbol", "h2_extract_selected",
            "--extract-global", "h2_extract_constant",
            "--extract-alias", "h2_extract_alias",
            "--output", output,
            args.root / "port/tests/guest_extract_probe.cpp",
        ))
        subprocess.run([str(value) for value in command], check=True)

        defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", output],
            text=True,
        )
        undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", output],
            text=True,
        )
        if "_h2_extract_selected" not in defined:
            raise RuntimeError("selected guest function was not retained")
        if "_h2_extract_constant" not in defined:
            raise RuntimeError("selected guest constant was not retained")
        if "_h2_extract_alias" not in defined:
            raise RuntimeError("selected guest alias was not retained")
        if "_h2_extract_unrelated" in defined:
            raise RuntimeError("unrelated guest function was retained")
        if undefined.strip():
            raise RuntimeError(f"extracted guest function has undefined constants: {undefined}")
        if b"retained recovered constant" not in output.read_bytes():
            raise RuntimeError("referenced guest constant initializer was discarded")

    print("guest function extraction retained referenced constants")


if __name__ == "__main__":
    main()
