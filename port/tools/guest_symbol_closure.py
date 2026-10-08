#!/usr/bin/env python3
"""Report symbol closure for a set of recovered guest objects."""

import argparse
import json
from pathlib import Path
import subprocess
import sys


def read_symbols(nm: Path, obj: Path) -> list[tuple[str, str]]:
    result = subprocess.run(
        [str(nm), "--format=posix", str(obj)],
        check=True,
        text=True,
        stdout=subprocess.PIPE,
    )
    symbols: list[tuple[str, str]] = []
    for line in result.stdout.splitlines():
        fields = line.rsplit(maxsplit=3)
        if len(fields) < 2:
            continue
        name, kind = fields[0], fields[1]
        if len(kind) != 1:
            continue
        symbols.append((name, kind))
    return symbols


def build_report(nm: Path, objects: list[Path]) -> dict:
    definitions: dict[str, list[str]] = {}
    references: dict[str, list[str]] = {}

    for obj in objects:
        if not obj.is_file():
            raise FileNotFoundError(f"guest object does not exist: {obj}")
        owner = obj.name
        for name, kind in read_symbols(nm, obj):
            table = references if kind.upper() == "U" else definitions
            table.setdefault(name, []).append(owner)

    unresolved = sorted(set(references) - set(definitions))
    resolved = sorted(set(references) & set(definitions))
    duplicate_definitions = sorted(
        name for name, owners in definitions.items() if len(set(owners)) > 1
    )
    return {
        "format": 1,
        "objects": [str(obj) for obj in objects],
        "counts": {
            "defined": len(definitions),
            "referenced": len(references),
            "resolved_in_set": len(resolved),
            "unresolved": len(unresolved),
            "duplicate_definitions": len(duplicate_definitions),
        },
        "unresolved": [
            {"symbol": name, "referenced_by": sorted(set(references[name]))}
            for name in unresolved
        ],
        "resolved_in_set": [
            {
                "symbol": name,
                "defined_by": sorted(set(definitions[name])),
                "referenced_by": sorted(set(references[name])),
            }
            for name in resolved
        ],
        "duplicate_definitions": [
            {"symbol": name, "defined_by": sorted(set(definitions[name]))}
            for name in duplicate_definitions
        ],
    }


def render_text(report: dict) -> str:
    counts = report["counts"]
    lines = [
        "Halo 2 recovered guest symbol closure",
        f"objects: {len(report['objects'])}",
        f"defined: {counts['defined']}",
        f"referenced: {counts['referenced']}",
        f"resolved in set: {counts['resolved_in_set']}",
        f"unresolved: {counts['unresolved']}",
        f"duplicate definitions: {counts['duplicate_definitions']}",
        "",
        "Unresolved symbols:",
    ]
    for entry in report["unresolved"]:
        lines.append(f"{entry['symbol']}\t{','.join(entry['referenced_by'])}")
    return "\n".join(lines) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nm", type=Path, required=True, help="llvm-nm executable")
    parser.add_argument("--output", type=Path, help="write the report instead of stdout")
    parser.add_argument("--format", choices=("json", "text"), default="text")
    parser.add_argument("--expect-unresolved", type=int)
    parser.add_argument("objects", type=Path, nargs="+")
    args = parser.parse_args()

    report = build_report(args.nm, args.objects)
    if args.format == "json":
        rendered = json.dumps(report, indent=2, sort_keys=True) + "\n"
    else:
        rendered = render_text(report)

    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(rendered)
    else:
        sys.stdout.write(rendered)

    actual = report["counts"]["unresolved"]
    if args.expect_unresolved is not None and actual != args.expect_unresolved:
        print(
            f"expected {args.expect_unresolved} unresolved symbols, found {actual}",
            file=sys.stderr,
        )
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
