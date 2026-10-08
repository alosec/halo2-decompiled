#!/usr/bin/env python3
"""Generate patchable AArch64 ELF trampolines for versioned host ABI calls."""

import argparse
import json
from pathlib import Path
import re

SAFE_SYMBOL = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def host_imports(plan):
    symbols = sorted(
        entry["symbol"] for entry in plan["imports"]
        if entry["category"] == "host_abi"
    )
    if not symbols:
        raise ValueError("import plan has no host ABI calls")
    for symbol in symbols:
        if not SAFE_SYMBOL.fullmatch(symbol):
            raise ValueError(f"host ABI symbol is not assembly-safe: {symbol!r}")
    return symbols


def render(symbols):
    lines = [
        '.section .text.h2_host_imports,"ax",@progbits',
        ".p2align 2",
    ]
    for symbol in symbols:
        slot = f"h2_host_slot_{symbol}"
        lines.extend((
            f".globl {symbol}",
            f".type {symbol}, @function",
            f"{symbol}:",
            f"    adrp x16, {slot}",
            f"    ldr x16, [x16, :lo12:{slot}]",
            "    br x16",
            f".size {symbol}, .-{symbol}",
            "",
        ))
    lines.extend((
        '.section .data.h2_host_imports,"aw",@progbits',
        ".p2align 3",
    ))
    for symbol in symbols:
        slot = f"h2_host_slot_{symbol}"
        lines.extend((
            f".globl {slot}",
            f".type {slot}, @object",
            f"{slot}:",
            "    .xword 0",
            f".size {slot}, 8",
        ))
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("plan", type=Path)
    args = parser.parse_args()
    symbols = host_imports(json.loads(args.plan.read_text()))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(render(symbols))


if __name__ == "__main__":
    main()
