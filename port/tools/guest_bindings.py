#!/usr/bin/env python3
"""Generate ELF assembly for fixed-address guest symbol bindings."""

import argparse
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("manifest", type=Path)
    args = parser.parse_args()

    document = json.loads(args.manifest.read_text())
    if document.get("format") != 1 or not isinstance(document.get("symbols"), list):
        raise ValueError("unsupported guest binding manifest")
    seen: set[str] = set()
    lines = ["// Generated from clean fixed-address guest bindings."]
    for entry in document["symbols"]:
        symbol = entry["symbol"]
        address = int(entry["address"], 0)
        if symbol in seen:
            raise ValueError(f"duplicate guest binding: {symbol}")
        if not 0 <= address <= 0xFFFFFFFF:
            raise ValueError(f"guest binding is outside ILP32: {symbol}")
        seen.add(symbol)
        escaped = symbol.replace('"', '\\"')
        lines.append(f'.globl "{escaped}"')
        lines.append(f'.set "{escaped}", 0x{address:08x}')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines) + "\n")


if __name__ == "__main__":
    main()
