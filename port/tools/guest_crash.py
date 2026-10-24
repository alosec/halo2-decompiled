#!/usr/bin/env python3
"""Run the host under lldb and print the crashing thread with guest frames symbolized.

usage: guest_crash.py [host arguments...]   (run from the repository root)"""
import re, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
host = ROOT / "build/native/port/halo2-port"
args = sys.argv[1:]
image = args[args.index("--guest-image") + 1] if "--guest-image" in args else None
out = subprocess.run(["lldb", "-b", "-o", "run", "-k", "bt 40", "-k", "register read pc lr x0 x1 x8",
                      "--", str(host), *args], capture_output=True, text=True, timeout=600).stdout
lines = out.splitlines()
start = next((i for i, l in enumerate(lines) if "stop reason" in l), None)
for line in lines:
    if "unimplemented" in line or line.startswith("error"):
        print(line)
if start is None:
    print("no crash; exit:", next((l for l in lines if "exited with status" in l), "?"))
    sys.exit(0)
print(lines[start].strip())
addresses = []
for line in lines[start:]:
    m = re.search(r"frame #(\d+): (0x[0-9a-f]+)(.*)", line)
    if m:
        addresses.append((m.group(1), m.group(2), m.group(3).strip()))
    elif re.match(r"\s+(pc|lr|x0|x1|x8) =", line):
        print(line.strip())
guest = [a for _, a, rest in addresses if int(a, 16) >> 40 == 1]
names = {}
if guest and image:
    sym = subprocess.run([sys.executable, str(ROOT / "port/tools/guest_symbolize.py"), image, *guest],
                         capture_output=True, text=True).stdout.splitlines()
    names = dict(l.split(": ", 1) for l in sym if ": " in l)
for index, address, rest in addresses:
    print(f"#{index} {address} {names.get(address, rest)}")
