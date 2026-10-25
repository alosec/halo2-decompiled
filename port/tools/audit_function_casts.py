#!/usr/bin/env python3
"""Finds function casts that break on ARM64 but not on x86.

Usage: port/tools/audit_function_casts.py [--llvm DIR] [source...]
  (run from the repository root; default: every src/ and port/recovered/ unit)

A function cast to a pointer type whose parameters differ only in being
float or integer works on x86, where both travel on the stack in 4-byte
slots, and breaks on ARM64, where floats go in s registers and integers in
w/x registers: every later argument then arrives in the wrong place. (Firing
a weapon crashed this way: sound-event handlers declared (long, long, event *)
were called as (long, real, event *).) Pointer/integer and arity differences
are harmless here and are not reported.

Each unit is compiled as the whole image compiles it (MS-style assembly
rewritten by build_whole_image.native_source) with -fsyntax-only and only
-Wcast-function-type-mismatch enabled; casts whose float/int parameter or
return kinds differ are printed. Exit status 1 when any is found.
"""
import argparse
import importlib.util
import os
import re
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FLAGS = ["--target=thumbv7-pc-windows-msvc", "-fsyntax-only", "-ffreestanding",
         "-DH2_NATIVE_PORT=1", "-DH2_PORT_SHIMS=1", "-DH2_WHOLE_IMAGE=1", "-DH2_MS_ABI_FRONTEND=1",
         "-fms-extensions", "-Xclang", "-fno-wchar", "-fno-exceptions", "-fno-rtti",
         "-fno-threadsafe-statics", "-Dinline=inline __attribute__((used))",
         "-D__forceinline=inline __attribute__((used))", "-D__inline=inline __attribute__((used))",
         "-include", "xmmintrin.h", "-include", "string.h",
         "-Wno-everything", "-Wcast-function-type-mismatch"]
INCLUDES = ["port/include", "port/include/crt", "port/include/xdk", "include", "src", "port/recovered"]
CAST = re.compile(r"^(.*?): warning: cast from '([^']*)'(?: \(aka '([^']*)'\))? to '([^']*)'(?: \(aka '([^']*)'\))?")


def load_builder():
    spec = importlib.util.spec_from_file_location("build_whole_image", ROOT / "port/tools/build_whole_image.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def split_parameters(text):
    parts, depth, current = [], 0, ""
    for ch in text:
        depth += ch in "(<"
        depth -= ch in ")>"
        if ch == "," and depth == 0:
            parts.append(current.strip())
            current = ""
        else:
            current += ch
    if current.strip():
        parts.append(current.strip())
    return [] if parts == ["void"] else parts


def kinds(function_type):
    """(return kind, parameter kinds) of 'R (*)(P, ...)': F float, I other, V varargs"""
    text = re.sub(r"\s*__attribute__\(\(\w+\)\)", "", function_type).strip()
    match = re.match(r"(.*?)\(\*\)\((.*)\)$", text)
    if not match:
        return None
    floating = lambda t: re.fullmatch(r"(const )?(float|double)", t) is not None
    result = match.group(1).strip()
    parameters = ["V" if p == "..." else "F" if floating(p) else "I" for p in split_parameters(match.group(2))]
    return ("void" if result == "void" else "F" if floating(result) else "I"), parameters


def mismatches(source_type, target_type):
    a, b = kinds(source_type), kinds(target_type)
    if not a or not b:
        return None
    positions = [i for i in range(min(len(a[1]), len(b[1])))
                 if (a[1][i] == "F") != (b[1][i] == "F") and "V" not in (a[1][i], b[1][i])]
    result = (a[0] == "F") != (b[0] == "F") and "void" not in (a[0], b[0])
    return positions, result


def audit(source, compiler, builder, scratch):
    text = source.read_text(errors="replace")
    rewritten = builder.native_source(text)
    unit = source
    if rewritten is not None:
        unit = Path(scratch) / source.name
        unit.write_text(f'#line 1 "{source}"\n' + rewritten)
    command = [str(compiler), *FLAGS, *[f"-I{ROOT / i}" for i in INCLUDES], f"-I{source.parent}", str(unit)]
    result = subprocess.run(command, capture_output=True, text=True)
    errors = [line for line in result.stderr.splitlines() if "error:" in line]
    found = []
    for line in result.stderr.splitlines():
        match = CAST.match(line)
        if not match:
            continue
        source_type = match.group(3) or match.group(2)
        target_type = match.group(5) or match.group(4)
        verdict = mismatches(source_type, target_type)
        if verdict and (verdict[0] or verdict[1]):
            found.append(f"{match.group(1)}: {source_type} -> {target_type}: float/int differ at "
                         f"parameters {verdict[0]}{' and the return' if verdict[1] else ''}")
    return found, errors[:1]


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--llvm", type=Path, default=Path("/opt/homebrew/opt/llvm@22/bin"))
    parser.add_argument("sources", nargs="*", type=Path)
    args = parser.parse_args()
    sources = [s.resolve() for s in args.sources] or \
        sorted((ROOT / "src").glob("*.cpp")) + sorted((ROOT / "port/recovered").glob("*.cpp"))
    builder = load_builder()
    findings, failures = [], []
    with tempfile.TemporaryDirectory() as scratch:
        with ThreadPoolExecutor(max_workers=os.cpu_count() or 4) as pool:
            for source, (found, errors) in zip(sources, pool.map(
                    lambda s: audit(s, args.llvm / "clang++", builder, scratch), sources)):
                findings += found
                if errors:
                    failures.append(f"{source.relative_to(ROOT)}: {errors[0]}")
    for line in sorted(set(findings)):
        print(line)
    for line in failures:
        print("not checked:", line, file=sys.stderr)
    print(f"{len(sources)} units, {len(sources) - len(failures)} checked, {len(set(findings))} float/int cast "
          f"mismatches", file=sys.stderr)
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main())
