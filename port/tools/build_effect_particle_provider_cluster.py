#!/usr/bin/env python3
import argparse
import subprocess
import tempfile
from pathlib import Path


def run(command, cwd=None):
    subprocess.run([str(value) for value in command], check=True, cwd=cwd)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--python", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--tables", type=Path, required=True)
    parser.add_argument("--bindings", type=Path, required=True)
    parser.add_argument("--unbound-output", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    args.unbound_output.parent.mkdir(parents=True, exist_ok=True)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="h2-effect-particle-providers-") as temporary:
        methods = Path(temporary) / "methods.o"
        source = args.root / "src/visual_effects_particle_interfaces.cpp"
        run([
            args.python, args.root / "port/tools/guest_elf_cc.py",
            "--llvm", args.llvm,
            "--plugin", args.plugin,
            "--include", args.root / "port/include",
            "--include", args.root / "port/include/crt",
            "--include", args.root / "port/include/xdk",
            "--include", args.root / "include",
            "--ms-abi",
            "--output", methods,
            source.relative_to(args.root),
        ], cwd=args.root)
        run([
            args.lld, "-r", "--allow-multiple-definition", "-o",
            args.unbound_output, args.tables, methods,
        ])
        run([
            args.lld, "-r", "--allow-multiple-definition", "-o",
            args.output, args.unbound_output, args.bindings,
        ])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
