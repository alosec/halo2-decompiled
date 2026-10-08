#!/usr/bin/env python3
"""Require Microsoft C++ symbols and vtables in the native main-menu guest."""

import argparse
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--object", type=Path, required=True)
    args = parser.parse_args()
    output = subprocess.check_output(["nm", "-g", str(args.object)], text=True)
    required = (
        "?function_14741b@@",
        "??_7c_main_menu_screen@@6B@",
        "??0c_main_menu_screen@@QAA@JJG@Z",
    )
    for symbol in required:
        if symbol not in output:
            raise SystemExit(f"missing Microsoft ABI main-menu symbol: {symbol}")
    if "__ZTV18c_main_menu_screen" in output:
        raise SystemExit("main-menu object incorrectly uses the Itanium C++ ABI")


if __name__ == "__main__":
    main()
