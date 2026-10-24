#!/usr/bin/env python3
"""Run the host under lldb with breakpoints on guest functions and log each hit.

usage: /usr/bin/python3 port/tools/guest_breaks.py [--hits N] [--seconds S]
           --break SYMBOL[:FORMAT] ... -- HOST ARGS...

SYMBOL is a guest symbol or the start of one ('?map_location_get' matches the
full mangled name). FORMAT per argument register x0..x3, e.g. 'sx' prints x0
as a guest C string and x1 as hex: s string, w wide string, x hex, d decimal,
f float (from s0..s3), r report the return value. --watch ADDRESS prints the guest dword there at
every hit. Needs the system Python, which Xcode's lldb module is
built for."""
import argparse
import os
import subprocess
import sys
import time

sys.path.insert(0, subprocess.run(["lldb", "-P"], capture_output=True, text=True).stdout.strip())
import lldb  # noqa: E402

NM = "/opt/homebrew/opt/llvm@22/bin/llvm-nm"
BIAS = 0x10000000000


def data_symbols(image):
    """sorted (address, name) of every symbol, for naming vtables"""
    out = subprocess.run([NM, "--defined-only", "-n", image], capture_output=True, text=True).stdout
    result = []
    for line in out.splitlines():
        parts = line.split(" ", 2)
        if len(parts) == 3:
            result.append((int(parts[0], 16), parts[2].replace("%", "@")))
    return result


def symbol_at(table, address):
    import bisect
    index = bisect.bisect_right(table, (address, chr(0x10ffff))) - 1
    if index < 0:
        return None
    base, name = table[index]
    return name if address - base < 0x400 else None


def guest_symbols(image):
    symbols = {}
    out = subprocess.run([NM, "--defined-only", image], capture_output=True, text=True).stdout
    for line in out.splitlines():
        parts = line.split(" ", 2)
        if len(parts) == 3 and parts[1] in "tT":
            symbols[parts[2].replace("%", "@")] = int(parts[0], 16)
    return symbols


def read_string(process, address, wide=False):
    error = lldb.SBError()
    data = process.ReadMemory(BIAS + address, 256, error)
    if not error.Success() or data is None:
        return "<unreadable 0x%x>" % address
    if wide:
        text = data.decode("utf-16-le", errors="replace")
    else:
        text = data.decode("latin-1")
    return repr(text.split("\0")[0])


def main():
    if "--" not in sys.argv:
        sys.exit(__doc__)
    split = sys.argv.index("--")
    parser = argparse.ArgumentParser()
    parser.add_argument("--break", dest="breaks", action="append", default=[])
    parser.add_argument("--watch", action="append", default=[], type=lambda text: int(text, 0))
    parser.add_argument("--watchpoint", help="POINTER:OFFSET: at the first break hit, watch writes "
                        "to the dword at (the guest pointer stored at POINTER) + OFFSET")
    parser.add_argument("--hits", type=int, default=40)
    parser.add_argument("--skip", type=int, default=0, help="ignore the first N hits")
    parser.add_argument("--explain", action="store_true",
                        help="at each hit, name the vtables of objects in registers")
    parser.add_argument("--seconds", type=float, default=60)
    args = parser.parse_args(sys.argv[1:split])
    host = sys.argv[split + 1:]
    image = host[host.index("--guest-image") + 1]
    symbols = guest_symbols(image)

    debugger = lldb.SBDebugger.Create()
    debugger.SetAsync(True)
    target = debugger.CreateTarget(host[0])
    entry = target.BreakpointCreateByName("h2_call_guest_int_0")
    entry.SetOneShot(True)
    error = lldb.SBError()
    info = lldb.SBLaunchInfo(host[1:])
    info.SetWorkingDirectory(".")
    # the game reads its scripted input and traces from the environment
    info.SetEnvironmentEntries(["%s=%s" % item for item in os.environ.items()], True)
    process = target.Launch(info, error)
    if not error.Success():
        sys.exit("launch failed: %s" % error)
    listener = debugger.GetListener()
    formats = {}
    returns = {}
    watching = []
    table = []
    deadline = time.time() + args.seconds
    hits = 0
    event = lldb.SBEvent()
    while time.time() < deadline and hits < args.hits:
        if not listener.WaitForEvent(1, event):
            continue
        state = lldb.SBProcess.GetStateFromEvent(event)
        if state in (lldb.eStateExited, lldb.eStateCrashed, lldb.eStateDetached):
            print("process ended:", process.GetExitStatus())
            break
        if state != lldb.eStateStopped:
            continue
        thread = process.GetSelectedThread()
        reason = thread.GetStopReason()
        pc = thread.GetFrameAtIndex(0).GetPC()
        if not formats and reason == lldb.eStopReasonBreakpoint and pc < BIAS:
            for spec in args.breaks:
                name, _, fmt = spec.partition(":")
                match = [s for s in symbols if s == name or s.startswith(name + "@")]
                if not match:
                    sys.exit("no guest symbol " + name)
                breakpoint = target.BreakpointCreateByAddress(BIAS + symbols[match[0]])
                formats[BIAS + symbols[match[0]]] = (match[0], fmt or "x")
            process.Continue()
            continue
        if reason == lldb.eStopReasonBreakpoint and pc in formats and args.skip > 0:
            args.skip -= 1
            process.Continue()
            continue
        if reason == lldb.eStopReasonBreakpoint and pc in formats:
            name, fmt = formats[pc]
            frame = thread.GetFrameAtIndex(0)
            values = []
            for index, kind in enumerate(fmt[:4]):
                if kind == "f":
                    values.append(frame.FindRegister("s%d" % index).GetValue())
                    continue
                value = frame.FindRegister("x%d" % index).GetValueAsUnsigned() & 0xffffffff
                if kind == "s":
                    values.append(read_string(process, value))
                elif kind == "w":
                    values.append(read_string(process, value, wide=True))
                elif kind == "d":
                    values.append(str(value - (1 << 32) if value & 0x80000000 else value))
                else:
                    values.append("0x%x" % value)
            for address in args.watch:
                error = lldb.SBError()
                word = process.ReadUnsignedFromMemory(BIAS + address, 4, error)
                values.append("[0x%x]=%s" % (address, "0x%x" % word if error.Success() else "?"))
            caller = frame.FindRegister("lr").GetValueAsUnsigned()
            if "r" in fmt:
                # report the return value: a one-shot stop at the return address
                returning = target.BreakpointCreateByAddress(caller)
                returning.SetOneShot(True)
                returns[caller] = name
            print("%s(%s) from 0x%x" % (name[:50], ", ".join(values), caller & 0xffffffff), flush=True)
            if args.explain:
                if not table:
                    table.extend(data_symbols(image))
                explain(process, frame, table)
                for depth in range(1, 4):
                    caller_frame = thread.GetFrameAtIndex(depth)
                    if caller_frame.IsValid():
                        print("    frame %d pc 0x%x" % (depth, caller_frame.GetPC() & 0xffffffff))
            if args.watchpoint and not watching:
                # POINTER:OFFSET[:OFFSET...]: each offset but the last is
                # followed through the pointer stored there
                parts = [int(part, 0) for part in args.watchpoint.split(":")]
                error = lldb.SBError()
                guest = process.ReadUnsignedFromMemory(BIAS + parts[0], 4, error)
                for offset in parts[1:-1]:
                    guest = process.ReadUnsignedFromMemory(BIAS + guest + offset, 4, error)
                address = BIAS + guest + (parts[-1] if len(parts) > 1 else 0)
                address &= ~3
                watch = target.WatchAddress(address, 4, False, True, error)
                print("watching writes to 0x%x: %s" % (address & 0xffffffff, error.GetCString() or "ok"), flush=True)
                watching.append(watch)
            hits += 1
            process.Continue()
            continue
        if reason == lldb.eStopReasonWatchpoint:
            frame = thread.GetFrameAtIndex(0)
            print("write at pc 0x%x" % (frame.GetPC() & 0xffffffff), " <- ".join(
                "0x%x" % (thread.GetFrameAtIndex(d).GetPC() & 0xffffffff) for d in range(1, 5)
                if thread.GetFrameAtIndex(d).IsValid()), flush=True)
            hits += 1
            process.Continue()
            continue
        if reason == lldb.eStopReasonBreakpoint and pc in returns:
            print("  %s returned 0x%x" % (returns.pop(pc)[:50],
                  thread.GetFrameAtIndex(0).FindRegister("x0").GetValueAsUnsigned() & 0xffffffff), flush=True)
            process.Continue()
            continue
        if reason in (lldb.eStopReasonException, lldb.eStopReasonSignal):
            print("stopped:", thread.GetStopDescription(200), "pc 0x%x" % pc)
            frame = thread.GetFrameAtIndex(0)
            print(" ".join("x%d=0x%x" % (index, frame.FindRegister("x%d" % index).GetValueAsUnsigned())
                           for index in range(29)))
            explain(process, frame, data_symbols(image))
            break
        process.Continue()
    process.Kill()


def explain(process, frame, table):
    """name the vtables of the objects the registers point at"""
    for index in range(29):
        value = frame.FindRegister("x%d" % index).GetValueAsUnsigned() & 0xffffffff
        if not 0x1000 <= value < 0xf0000000:
            continue
        error = lldb.SBError()
        word = process.ReadUnsignedFromMemory(BIAS + value, 4, error)
        name = symbol_at(table, word) if error.Success() else None
        if name and name.startswith("??_7"):
            print("  x%d=0x%x -> vtable %s+0x%x" % (index, value, name, word - next(a for a, n in table if n == name)))
        elif error.Success() and 0x400000 <= word < 0x480000:
            print("  x%d=0x%x -> retail-table word 0x%x" % (index, value, word))


if __name__ == "__main__":
    main()
