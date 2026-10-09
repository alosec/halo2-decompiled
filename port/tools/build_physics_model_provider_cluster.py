#!/usr/bin/env python3
import argparse
import csv
import json
from collections import defaultdict
from pathlib import Path
import subprocess
import sys
import tempfile


def run(command):
    subprocess.run([str(value) for value in command], check=True)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--tables", type=Path, required=True)
    parser.add_argument("--leaves", type=Path, required=True)
    parser.add_argument("--crt", type=Path, required=True)
    parser.add_argument("--bindings", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    manifest_path = args.root / "port/runtime/physics_model_native_providers.json"
    manifest = json.loads(manifest_path.read_text())
    if manifest.get("version") != 1:
        raise RuntimeError("unsupported native physics provider manifest")
    providers = manifest.get("providers")
    if not isinstance(providers, dict):
        raise RuntimeError("native physics provider map is missing")

    with (args.root / "config/functions.csv").open(newline="") as source:
        functions = {int(row["va"], 16): row for row in csv.DictReader(source)}
    units = defaultdict(list)
    for address, provider in providers.items():
        if provider.get("kind") != "source":
            continue
        method = int(address, 0)
        source = functions.get(method, {}).get("source")
        if not source:
            raise RuntimeError(f"recovered provider lacks source: {method:#x}")
        units[source].append(provider["symbol"])
    for closure in manifest.get("closure", []):
        source = closure.get("source")
        symbols = closure.get("symbols")
        if not source or not isinstance(symbols, list) or not symbols:
            raise RuntimeError("invalid native physics closure entry")
        units[source].extend(symbols)

    source_globals = manifest.get("source_globals", {})
    args.output.parent.mkdir(parents=True, exist_ok=True)
    tool = args.root / "port/tools/guest_elf_cc.py"
    includes = [
        args.root / "port/include",
        args.root / "port/include/crt",
        args.root / "port/include/xdk",
        args.root / "include",
    ]
    with tempfile.TemporaryDirectory(prefix="h2-physics-providers-") as temporary:
        temporary = Path(temporary)
        objects = []
        for index, (source, symbols) in enumerate(sorted(units.items())):
            output = temporary / f"provider-{index:02d}.o"
            command = [
                sys.executable, tool,
                "--llvm", args.llvm,
                "--plugin", args.plugin,
            ]
            for include in includes:
                command.extend(("--include", include))
            command.extend(("--ms-abi", "--output", output))
            for symbol in dict.fromkeys(symbols):
                command.extend(("--extract-symbol", symbol))
            for symbol in source_globals.get(source, []):
                command.extend(("--extract-global", symbol))
            command.append(args.root / source)
            run(command)
            objects.append(output)

        run([
            args.lld, "-r", "-o", args.output,
            args.tables, args.leaves, args.crt, args.bindings, *objects,
        ])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
