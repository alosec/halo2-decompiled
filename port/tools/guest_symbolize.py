#!/usr/bin/env python3
"""Map guest (or 1-TB-biased host) addresses to symbols in a guest ELF image."""
import bisect, subprocess, sys
from pathlib import Path

def main():
    image = sys.argv[1]
    nm = Path("/opt/homebrew/opt/llvm@22/bin/llvm-nm")
    out = subprocess.run([str(nm), "--defined-only", "-n", image], capture_output=True, text=True).stdout
    entries = []
    for line in out.splitlines():
        parts = line.split(" ", 2)
        if len(parts) == 3 and parts[1] in "tTwW":
            entries.append((int(parts[0], 16), parts[2].replace("%", "@")))
    keys = [e[0] for e in entries]
    for text in sys.argv[2:]:
        address = int(text, 16) & 0xFFFFFFFF
        index = bisect.bisect_right(keys, address) - 1
        if index < 0:
            print(f"{text}: ?")
            continue
        base, name = entries[index]
        demangled = subprocess.run(["/opt/homebrew/opt/llvm@22/bin/llvm-undname", name],
                                   capture_output=True, text=True).stdout.strip().splitlines()
        pretty = demangled[-1] if demangled and name.startswith("?") else name
        print(f"{text}: {name}+{address - base:#x}  [{pretty}]")

if __name__ == "__main__":
    main()
