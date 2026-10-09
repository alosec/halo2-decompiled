#!/usr/bin/env python3
import argparse
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
    parser.add_argument("--table", type=Path, required=True)
    parser.add_argument("--crt", type=Path)
    parser.add_argument("--bindings", type=Path)
    parser.add_argument("--providers-output", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    manifest_path = args.root / "port/runtime/structure_bsp_native_providers.json"
    manifest = json.loads(manifest_path.read_text())
    if manifest.get("version") != 1:
        raise RuntimeError("unsupported native structure-BSP provider manifest")
    providers = manifest.get("providers")
    if not isinstance(providers, list):
        raise RuntimeError("native structure-BSP provider list is missing")

    units = defaultdict(list)
    unit_globals = defaultdict(list)
    native_symbols = set()
    methods = set()
    for provider in providers:
        method = int(provider["method"], 0)
        symbol = provider.get("symbol")
        source = provider.get("source")
        if method in methods or not symbol or not source:
            raise RuntimeError("invalid or duplicate native structure-BSP provider")
        methods.add(method)
        if source == "native":
            native_symbols.add(symbol)
        else:
            units[source].append(symbol)
            globals_ = provider.get("globals", [])
            if not isinstance(globals_, list):
                raise RuntimeError("invalid native structure-BSP provider globals")
            unit_globals[source].extend(globals_)

    for closure in manifest.get("closure", []):
        source = closure.get("source")
        symbols = closure.get("symbols")
        globals_ = closure.get("globals", [])
        if (not source or not isinstance(symbols, list) or not symbols or
                not isinstance(globals_, list)):
            raise RuntimeError("invalid native structure-BSP closure entry")
        units[source].extend(symbols)
        unit_globals[source].extend(globals_)

    if native_symbols != {"h2_native_structure_bsp_idle"}:
        raise RuntimeError("unexpected native structure-BSP provider set")

    bound_inputs = (args.crt, args.bindings, args.providers_output)
    if any(bound_inputs) and not all(bound_inputs):
        raise RuntimeError("crt, bindings, and providers-output must be supplied together")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    if args.providers_output:
        args.providers_output.parent.mkdir(parents=True, exist_ok=True)
    tool = args.root / "port/tools/guest_elf_cc.py"
    includes = [
        args.root / "port/include",
        args.root / "port/include/crt",
        args.root / "port/include/xdk",
        args.root / "include",
    ]
    with tempfile.TemporaryDirectory(prefix="h2-structure-bsp-providers-") as temporary:
        temporary = Path(temporary)
        objects = []
        for index, (source, symbols) in enumerate(sorted(units.items())):
            source_path = args.root / source
            if not source_path.is_file():
                raise RuntimeError(f"missing native structure-BSP provider source: {source}")
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
            for symbol in dict.fromkeys(unit_globals[source]):
                command.extend(("--extract-global", symbol))
            command.append(source_path)
            run(command)
            objects.append(output)

        provider_output = args.providers_output or args.output
        run([args.lld, "-r", "-o", provider_output, args.table, *objects])
        if args.providers_output:
            run([
                args.lld, "-r", "-o", args.output,
                args.providers_output, args.crt, args.bindings,
            ])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
