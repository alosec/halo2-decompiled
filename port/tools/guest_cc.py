#!/usr/bin/env python3
"""Compile one recovered source file into a native ARM64 guest object."""

import argparse
from pathlib import Path
import re
import subprocess
import tempfile


def run(*args):
    subprocess.run([str(value) for value in args], check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--include", type=Path, action="append", default=[])
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--ms-abi", action="store_true",
                        help="lower C++ with the 32-bit Microsoft ABI before ARM64 retargeting")
    parser.add_argument("--emit-assembly", action="store_true",
                        help="write arm64_32 Darwin assembly instead of a Mach-O object")
    parser.add_argument("--extract-symbol", action="append", default=[],
                        help="retain only this IR function and its transitive callees")
    parser.add_argument("--extract-global", action="append", default=[],
                        help="retain this named IR global and its initializer")
    parser.add_argument("--extract-alias", action="append", default=[],
                        help="retain this exact IR alias and its target")
    parser.add_argument("source", type=Path)
    args = parser.parse_args()

    args.output.parent.mkdir(parents=True, exist_ok=True)
    compiler = args.llvm / ("clang++" if args.source.suffix in {".cc", ".cpp", ".cxx"} else "clang")
    with tempfile.TemporaryDirectory(prefix="h2-guest-cc-") as temporary:
        root = Path(temporary)
        ir = root / "unit.ll"
        rebased = root / "unit.rebased.ll"
        assembly = root / "unit.s"
        language_flags = ["-fno-exceptions", "-fno-rtti"] if compiler.name == "clang++" else []
        include_flags = [f"-I{directory}" for directory in args.include]
        frontend_target = "thumbv7-pc-windows-msvc" if args.ms_abi else "arm64_32-apple-watchos"
        abi_flags = ["-DH2_MS_ABI_FRONTEND=1", "-Xclang", "-fno-wchar"] if args.ms_abi else []
        run(compiler, f"--target={frontend_target}", "-O2", "-ffreestanding",
        "-fno-stack-protector",
            "-DH2_NATIVE_PORT=1", "-fms-extensions", "-Wno-ignored-attributes", "-Wno-c++11-narrowing",
            *abi_flags, *language_flags, *include_flags,
            "-S", "-emit-llvm", args.source, "-o", ir)
        if args.ms_abi:
            text = ir.read_text()
            text = re.sub(r'^target datalayout = ".*"$',
                          'target datalayout = "e-m:o-p:32:32-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"',
                          text, flags=re.MULTILINE)
            text = re.sub(r'^target triple = ".*"$',
                          'target triple = "arm64_32-apple-watchos2.0.0"',
                          text, flags=re.MULTILINE)
            text = text.replace("arm_aapcs_vfpcc ", "")
            text = re.sub(r'^\$.* = comdat .*\n', '', text, flags=re.MULTILINE)
            text = re.sub(r',\s*comdat(?:\(\$[^\)]+\))?', '', text)
            text = re.sub(r'\s+comdat(?:\(\$[^\)]+\))?', '', text)
            text = re.sub(r' "target-cpu"="[^"]*"', '', text)
            text = re.sub(r' "target-features"="[^"]*"', '', text)
            ir.write_text(text)
        if args.extract_symbol or args.extract_global or args.extract_alias:
            extracted = root / "unit.extracted.ll"
            extract_args = [value for symbol in args.extract_symbol
                            for value in ("--func", symbol)]
            extract_args.extend(value for symbol in args.extract_global
                                for value in ("--glob", symbol))
            extract_args.extend(value for symbol in args.extract_alias
                                for value in ("--ralias", f"^{re.escape(symbol)}$"))
            run(args.llvm / "llvm-extract", "--recursive", *extract_args,
                "-S", ir, "-o", extracted)
            ir = extracted
        run(args.llvm / "opt", f"-load-pass-plugin={args.plugin}",
            "-passes=h2-rebase,verify", "-S", ir, "-o", rebased)
        run(args.llvm / "llc", "-O2", "-mtriple=arm64_32-apple-watchos",
            rebased, "-o", assembly)
        if args.emit_assembly:
            args.output.write_text(assembly.read_text())
            return
        # arm64_32 encodes a GOT pointer load as a 32-bit LDR because its
        # native pointer is 32 bits. The object is linked into a normal ARM64
        # Mach-O host, whose GOT slots and relocation require an x-register.
        # Load the complete host symbol address; the following generated
        # truncation keeps the guest value ILP32 before the rebase pass uses it.
        text = assembly.read_text()
        text = re.sub(
            r"(^\s*ldr\s+)w(\d+)(\s*,\s*\[x\d+\s*,\s*[^\]]+@GOTPAGEOFF[^\]]*\])",
            r"\1x\2\3", text, flags=re.MULTILINE)
        assembly.write_text(text)
        run("clang", "-arch", "arm64", "-c", assembly, "-o", args.output)


if __name__ == "__main__":
    main()
