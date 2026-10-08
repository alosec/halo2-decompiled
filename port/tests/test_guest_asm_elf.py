#!/usr/bin/env python3
"""Verify quoted Microsoft ABI aliases survive Darwin-to-ELF conversion."""

import argparse
from pathlib import Path
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--converter", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    args = parser.parse_args()

    with tempfile.TemporaryDirectory(prefix="h2-guest-asm-elf-") as temporary:
        root = Path(temporary)
        source = root / "alias.darwin.s"
        converted = root / "alias.elf.s"
        obj = root / "alias.o"
        source.write_text('''
.section __TEXT,__text,regular,pure_instructions
.globl "_?target@@YAXXZ"
"_?target@@YAXXZ":
  ret
.weak_reference "_??_Ewidget@@UAAPAXI@Z"
"_??_Ewidget@@UAAPAXI@Z" = "_?target@@YAXXZ"
.long "_??_Ewidget@@UAAPAXI@Z"
''')
        subprocess.run([sys.executable, args.converter, source, converted], check=True)
        text = converted.read_text()
        expected = '\t.set "??_Ewidget@@UAAPAXI@Z", "?target@@YAXXZ"'
        if expected not in text or '"_??_Ewidget' in text:
            raise RuntimeError("quoted Microsoft alias was not normalized")
        subprocess.run([
            args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj",
            converted, "-o", obj,
        ], check=True)
        undefined = subprocess.check_output([
            args.llvm / "llvm-nm", "--undefined-only", "--format=posix", obj,
        ], text=True)
        if undefined.strip():
            raise RuntimeError(f"converted alias remained undefined: {undefined.strip()}")
        print("quoted Microsoft ABI alias conversion passed")


if __name__ == "__main__":
    main()
