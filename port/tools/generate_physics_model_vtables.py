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
    if table_manifest.get("version") != 1 or provider_manifest.get("version") != 1:
        raise RuntimeError("unsupported physics-model manifest version")
    tables = table_manifest.get("tables")
    providers = provider_manifest.get("providers")
    if not isinstance(tables, list) or not isinstance(providers, dict):
        raise RuntimeError("invalid physics-model manifests")

    methods = {method for table in tables for method in table.get("slots", [])}
    if methods != set(providers):
        raise RuntimeError("native provider map does not exactly cover table methods")

    lines = [
        ".section .rodata.h2_physics_model_vtables,\"a\",@progbits",
        ".p2align 2",
    ]
    labels = []
    slot_count = 0
    for table in tables:
        address = int(table["retail_address"], 0)
        label = f"h2_native_physics_model_vtable_{address:08x}"
        labels.append(label)
        lines.extend([f".global {label}", f".type {label}, @object", f"{label}:"])
        for method in table["slots"]:
            provider = providers[method]
            symbol = provider.get("symbol")
            if provider.get("kind") not in {"native", "source"} or not symbol:
                raise RuntimeError(f"invalid provider for {method}")
            lines.append(f"  .long {quoted(symbol)}")
            slot_count += 1
        lines.append(f".size {label}, . - {label}")
        lines.append(".p2align 2")

    lines.extend([
        ".global h2_native_physics_model_vtables",
        ".type h2_native_physics_model_vtables, @object",
        "h2_native_physics_model_vtables:",
    ])
    lines.extend(f"  .long {label}" for label in labels)
    lines.extend([
        ".size h2_native_physics_model_vtables, . - h2_native_physics_model_vtables",
        '.section .note.GNU-stack,"",@progbits',
        "",
    ])
    if len(tables) != 7 or slot_count != 97:
        raise RuntimeError("physics-model table shape changed")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
