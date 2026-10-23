#!/usr/bin/env python3
"""Classify recovered translation units by their first native compile blocker."""

import argparse
from collections import Counter
import json
from pathlib import Path
import re
import subprocess


def category(stderr):
    patterns = (
        (r"fatal error: '([^']+)' file not found", "missing-header:{}"),
        (r"error: unknown type name '([^']+)'", "unknown-type:{}"),
        (r"error: use of undeclared identifier '([^']+)'", "undeclared:{}"),
        (r"error: no member named '([^']+)'", "missing-member:{}"),
        (r"error: (.+)", "compile-error:{}"),
    )
    for pattern, template in patterns:
        match = re.search(pattern, stderr)
        if match:
            return template.format(match.group(1).strip())
    return "compiler-failed"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--timeout", type=float, default=30)
    args = parser.parse_args()

    sources = sorted((args.root / "src").glob("*.cpp"))
    results = []
    for index, source in enumerate(sources, 1):
        command = [
            str(args.llvm / "clang++"), "--target=arm64_32-apple-watchos", "-O0",
            "-ffreestanding", "-fsyntax-only", "-ferror-limit=1", "-fms-extensions",
            "-DH2_NATIVE_PORT=1", "-Wno-ignored-attributes", "-Wno-c++11-narrowing", "-Wno-microsoft-anon-tag",
            f"-I{args.root / 'port/include'}", f"-I{args.root / 'port/include/crt'}",
            f"-I{args.root / 'port/include/xdk'}",
            f"-I{args.root / 'include'}", str(source),
        ]
        try:
            completed = subprocess.run(command, text=True, capture_output=True,
                                       timeout=args.timeout)
            status = "passed" if completed.returncode == 0 else category(completed.stderr)
            lines = completed.stderr.splitlines()
            diagnostic = "" if completed.returncode == 0 else (lines[-1] if lines else "compiler failed")
        except subprocess.TimeoutExpired:
            status = "timeout"
            diagnostic = "compiler timed out"
        results.append({"source": str(source.relative_to(args.root)),
                        "status": status, "diagnostic": diagnostic})
        if index % 100 == 0:
            print(f"checked {index}/{len(sources)}", flush=True)

    counts = Counter(result["status"] for result in results)
    report = {"source_count": len(sources), "counts": dict(counts.most_common()),
              "results": results}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({"source_count": len(sources), "counts": counts.most_common(20)}, indent=2))


if __name__ == "__main__":
    main()
