#!/usr/bin/env python3
import argparse
import json
import subprocess
import tempfile
from pathlib import Path


def run(command):
    subprocess.run([str(value) for value in command], check=True)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--python", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--manifest", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    manifest = json.loads(args.manifest.read_text())
    if manifest.get("version") != 1:
        raise RuntimeError("unsupported sound-idle direct cluster manifest")
    providers = manifest.get("providers")
    if not isinstance(providers, list) or not providers:
        raise RuntimeError("sound-idle direct provider manifest is empty")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="h2-sound-idle-direct-") as temporary:
        temporary = Path(temporary)
        objects = []
        for index, provider in enumerate(providers):
            source = args.root / provider["source"]
            symbol = provider.get("symbol")
            global_symbol = provider.get("global")
            if (symbol is None) == (global_symbol is None):
                raise RuntimeError(
                    "sound-idle provider needs exactly one symbol or global")
            output = temporary / f"provider-{index}.o"
            command = [
                args.python,
                args.root / "port/tools/guest_elf_cc.py",
                "--llvm", args.llvm,
                "--plugin", args.plugin,
                "--include", args.root / "port/include",
                "--include", args.root / "port/include/crt",
                "--include", args.root / "port/include/xdk",
                "--include", args.root / "include",
                "--ms-abi",
                "--extract-symbol" if symbol is not None else "--extract-global",
                symbol if symbol is not None else global_symbol,
                "--output", output,
                source,
            ]
            run(command)
            objects.append(output)

        run([args.lld, "-r", "--allow-multiple-definition",
             "-o", args.output, *objects])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
