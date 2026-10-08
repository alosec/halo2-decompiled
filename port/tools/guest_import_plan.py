#!/usr/bin/env python3
"""Classify unresolved guest imports by available recovered implementation."""

import argparse
import json
from pathlib import Path
import re

ADDRESS_SYMBOL = re.compile(r"^\?function_([0-9a-f]+)@@")
RETAIL_MARKER = re.compile(r"@retail 0x([0-9a-f]+)\b", re.IGNORECASE)
STUB_MARKER = re.compile(r"@stub 0x([0-9a-f]+)\b", re.IGNORECASE)
PLATFORM_SERVICES = {"GetTickCount", "Sleep", "cos"}


def address_providers(source_root, marker):
    providers = {}
    for path in sorted(source_root.rglob("*.cpp")):
        text = path.read_text(errors="replace")
        for match in marker.finditer(text):
            address = match.group(1).lower().lstrip("0") or "0"
            providers.setdefault(address, []).append(str(path.relative_to(source_root)))
    return providers


def build_plan(closure, source_root):
    providers = address_providers(source_root, RETAIL_MARKER)
    stubs = address_providers(source_root, STUB_MARKER)
    imports = []
    counts = {
        "recovered_provider": 0,
        "stubbed_external": 0,
        "platform_runtime": 0,
        "named_engine_service": 0,
        "unrecovered_address": 0,
    }
    for entry in closure["unresolved"]:
        symbol = entry["symbol"]
        address_match = ADDRESS_SYMBOL.match(symbol)
        item = {"symbol": symbol, "referenced_by": entry["referenced_by"]}
        if symbol in PLATFORM_SERVICES:
            category = "platform_runtime"
        elif address_match:
            address = address_match.group(1).lower().lstrip("0") or "0"
            item["retail_address"] = f"0x{int(address, 16):x}"
            item["providers"] = providers.get(address, [])
            if item["providers"]:
                category = "recovered_provider"
            else:
                item["providers"] = stubs.get(address, [])
                category = "stubbed_external" if item["providers"] else "unrecovered_address"
        else:
            category = "named_engine_service"
        item["category"] = category
        counts[category] += 1
        imports.append(item)
    return {
        "schema": 1,
        "unresolved_count": len(imports),
        "counts": counts,
        "imports": imports,
    }


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--closure", type=Path, required=True)
    parser.add_argument("--source-root", type=Path, required=True)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--expect-unresolved", type=int)
    args = parser.parse_args()

    plan = build_plan(json.loads(args.closure.read_text()), args.source_root)
    if args.expect_unresolved is not None and plan["unresolved_count"] != args.expect_unresolved:
        raise SystemExit(
            f"unresolved import count {plan['unresolved_count']} != expected {args.expect_unresolved}")
    rendered = json.dumps(plan, indent=2, sort_keys=True) + "\n"
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(rendered)
    else:
        print(rendered, end="")


if __name__ == "__main__":
    main()
