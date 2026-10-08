#!/usr/bin/env python3
"""Classify unresolved guest imports by available recovered implementation."""

import argparse
import json
from pathlib import Path
import re

ADDRESS_SYMBOL = re.compile(r"^\?function_([0-9a-f]+)@@")
RETAIL_MARKER = re.compile(r"@retail 0x([0-9a-f]+)\b", re.IGNORECASE)
PLATFORM_SERVICES = {"GetTickCount", "Sleep", "cos"}


def recovered_addresses(source_root):
    providers = {}
    for path in sorted(source_root.glob("*.cpp")):
        text = path.read_text(errors="replace")
        for match in RETAIL_MARKER.finditer(text):
            address = match.group(1).lower().lstrip("0") or "0"
            providers.setdefault(address, []).append(path.name)
    return providers


def build_plan(closure, source_root):
    providers = recovered_addresses(source_root)
    imports = []
    counts = {
        "recovered_provider": 0,
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
            category = "recovered_provider" if item["providers"] else "unrecovered_address"
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
