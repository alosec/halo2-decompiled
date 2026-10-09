#!/usr/bin/env python3
import argparse
import json
from pathlib import Path


def quoted(symbol: str) -> str:
    return '"' + symbol.replace('\\', '\\\\').replace('"', '\\"') + '"'


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--tables", type=Path, required=True)
    parser.add_argument("--providers", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    table_manifest = json.loads(args.tables.read_text())
    provider_manifest = json.loads(args.providers.read_text())
    if table_manifest.get("version") != 2 or provider_manifest.get("version") != 1:
        raise RuntimeError("unsupported primary-listener manifest version")
    tables = table_manifest.get("tables")
    providers = provider_manifest.get("providers")
    if not isinstance(tables, list) or not isinstance(providers, dict):
        raise RuntimeError("invalid primary-listener manifests")

    methods = [method for table in tables for method in table.get("methods", [])]
    if len(methods) != 14 or len(set(methods)) != 14:
        raise RuntimeError("primary-listener table shape changed")
    if set(methods) != set(providers):
        raise RuntimeError("primary-listener providers do not exactly cover methods")

    lines = [
        '.section .rodata.h2_havok_primary_listener_vtables,"a",@progbits',
        ".p2align 2",
    ]
    labels = []
    for table in tables:
        label = table["native_symbol"]
        labels.append(label)
        lines.extend([
            f".global {quoted(label)}",
            f".type {quoted(label)}, @object",
            f"{quoted(label)}:",
        ])
        for method in table["methods"]:
            symbol = providers.get(method)
            if not isinstance(symbol, str) or not symbol:
                raise RuntimeError(f"invalid primary-listener provider for {method}")
            lines.append(f"  .long {quoted(symbol)}")
        lines.append(f".size {quoted(label)}, . - {quoted(label)}")
        lines.append(".p2align 2")

    lines.extend([
        ".global h2_native_havok_primary_listener_vtables",
        ".type h2_native_havok_primary_listener_vtables, @object",
        "h2_native_havok_primary_listener_vtables:",
    ])
    lines.extend(f"  .long {quoted(label)}" for label in labels)
    lines.extend([
        ".size h2_native_havok_primary_listener_vtables, . - h2_native_havok_primary_listener_vtables",
        '.section .note.GNU-stack,"",@progbits',
        "",
    ])
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
