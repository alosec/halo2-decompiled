#!/usr/bin/env python3
"""Run the host under lldb with breakpoints on guest functions and log each hit.

usage: /usr/bin/python3 port/tools/guest_breaks.py [--hits N] [--seconds S]
           --break SYMBOL[:FORMAT] ... -- HOST ARGS...

SYMBOL is a guest symbol or the start of one ('?map_location_get' matches the
full mangled name). FORMAT per argument register x0..x3, e.g. 'sx' prints x0
as a guest C string and x1 as hex: s string, w wide string, x hex, d decimal,
f float (from s0..s3). --watch ADDRESS prints the guest dword there at
every hit. Needs the system Python, which Xcode's lldb module is
built for."""
import argparse
import subprocess
import sys
import time

sys.path.insert(0, subprocess.run(["lldb", "-P"], capture_output=True, text=True).stdout.strip())
import lldb  # noqa: E402

NM = "/opt/homebrew/opt/llvm@22/bin/llvm-nm"
BIAS = 0x10000000000


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
    parser.add_argument("--hits", type=int, default=40)
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
    process = target.Launch(info, error)
    if not error.Success():
        sys.exit("launch failed: %s" % error)
    listener = debugger.GetListener()
    formats = {}
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
            print("%s(%s) from 0x%x" % (name[:50], ", ".join(values), caller & 0xffffffff), flush=True)
            hits += 1
            process.Continue()
            continue
        if reason in (lldb.eStopReasonException, lldb.eStopReasonSignal):
            print("stopped:", thread.GetStopDescription(200), "pc 0x%x" % pc)
            frame = thread.GetFrameAtIndex(0)
            print(" ".join("x%d=0x%x" % (index, frame.FindRegister("x%d" % index).GetValueAsUnsigned())
                           for index in range(29)))
            break
        process.Continue()
    process.Kill()


if __name__ == "__main__":
    main()
