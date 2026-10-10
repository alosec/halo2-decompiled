#!/usr/bin/env python3
import argparse
import json
from pathlib import Path


def quoted(symbol: str) -> str:
    return '"' + symbol.replace('\\', '\\\\').replace('"', '\\"') + '"'


def label(group: str, kind: str) -> str:
    if group not in ("prt3", "PRTM"):
        raise RuntimeError("invalid particle provider group")
    suffix = "prt3" if group == "prt3" else "prtm"
    return f"h2_native_effect_particle_provider_{suffix}_{kind}"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    manifest = json.loads(args.manifest.read_text())
    objects = manifest.get("objects")
    if (manifest.get("version") != 1 or manifest.get("object_size") != 12 or
            not isinstance(objects, list) or len(objects) != 2):
        raise RuntimeError("invalid particle provider table manifest")

    lines = [
        '.section .rodata.h2_effect_particle_provider_vtables,"a",@progbits',
        ".p2align 2",
    ]
    for entry in objects:
        slots = entry.get("slots")
        if (not isinstance(slots, list) or len(slots) != 21 or
                [slot.get("slot") for slot in slots] != list(range(21))):
            raise RuntimeError("invalid particle provider table shape")
        table_label = label(entry.get("group"), "table")
        lines.extend([
            f".global {table_label}",
            f".type {table_label}, @object",
            f"{table_label}:",
        ])
        for slot in slots:
            provider = slot.get("provider")
            if not isinstance(provider, str) or not provider:
                raise RuntimeError("invalid particle provider symbol")
            lines.append(f"  .long {quoted(provider)}")
        lines.extend([
            f".size {table_label}, . - {table_label}",
            ".p2align 2",
        ])

    lines.extend([
        '.section .data.h2_effect_particle_provider_objects,"aw",@progbits',
        ".p2align 2",
    ])
    for entry in objects:
        object_label = label(entry.get("group"), "object")
        table_label = label(entry.get("group"), "table")
        lines.extend([
            f".global {object_label}",
            f".type {object_label}, @object",
            f"{object_label}:",
            f"  .long {table_label}",
            "  .long 0",
            "  .long 0",
            f".size {object_label}, . - {object_label}",
            ".p2align 2",
        ])
    lines.extend(['.section .note.GNU-stack,"",@progbits', ""])
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
