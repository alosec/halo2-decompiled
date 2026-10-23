#!/usr/bin/env python3
"""Build one guest image from every recovered translation unit.

The curated main-menu image extracts hand-listed symbols from selected units.
This tool instead compiles every unit that the native frontend accepts, then:

* resolves duplicate strong definitions by priority (port runtime, then
  ``*_native.cpp`` files, then the source ``config/functions.csv`` names for
  the function, then other recovered units, then stubs), deleting the losers;
* binds globals whose names carry a retail address (``g_4e3b44``,
  ``g_4cf790_storage``) to that address unless their initializer holds code
  pointers, so retail data stays where retail code expects it;
* routes ``host_*`` calls through patchable host ABI trampolines;
* gives every function that no unit defines a trap stub that reports its name
  to the host and returns zero.

It writes ``whole-image.elf`` plus ``whole-image-report.json`` describing what
was compiled, skipped, overridden, bound and stubbed.
"""

import argparse
import concurrent.futures
import csv
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

TOOLS = Path(__file__).resolve().parent

MS_DATALAYOUT = ('target datalayout = "e-m:o-p:32:32-p270:32:32-p271:32:32-'
                 'p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"')
GUEST_TRIPLE = 'target triple = "arm64_32-apple-watchos2.0.0"'
GUEST_BIAS = 0x10000000000

DEFINE = re.compile(r'^define ([^@]*)@("[^"]+"|[^ (]+)\(', re.M)
DECLARE = re.compile(r'^declare [^@]*@("[^"]+"|[^ (]+)\(', re.M)
GLOBAL = re.compile(r'^@("[^"]+"|[^ ]+) = ([^"\n]*?)\b(global|constant) (.*)$', re.M)
ADDRESS_NAME = re.compile(r'(?:^|[?@])g_(?:unknown_)?([0-9a-f]{5,6})(?:_[A-Za-z0-9_]+)?(?:@|$)')


def elf_name(symbol):
    """ELF linkers read '@' in a symbol name as a version tag, which merges
    overloads such as ?f@@YAXJ@Z and ?f@@YAX_N@Z. MS mangling never uses '%'."""
    return symbol.replace("@", "%")


def escape_symbols(assembly):
    out = []
    for line in assembly.split("\n"):
        stripped = line.lstrip()
        if stripped.startswith((".asciz", ".ascii", ".string")):
            out.append(line)
            continue
        out.append(re.sub(r'"([^"]*@[^"]*)"', lambda m: '"' + elf_name(m.group(1)) + '"', line))
    return "\n".join(out)


def run(args, **kwargs):
    return subprocess.run([str(value) for value in args], check=True, **kwargs)


def unquote(name):
    return name[1:-1] if name.startswith('"') else name


def linkage(prefix):
    if re.search(r'\b(internal|private)\b', prefix):
        return "internal"
    if re.search(r'\b(linkonce|linkonce_odr|weak|weak_odr|available_externally)\b', prefix):
        return "weak"
    if re.search(r'\bextern_weak\b|\bexternal\b', prefix):
        return "external"
    return "strong"


def retarget(text):
    text = re.sub(r'^target datalayout = ".*"$', MS_DATALAYOUT, text, flags=re.M)
    text = re.sub(r'^target triple = ".*"$', GUEST_TRIPLE, text, flags=re.M)
    text = text.replace("arm_aapcs_vfpcc ", "")
    text = re.sub(r'^\$.* = comdat .*\n', '', text, flags=re.M)
    text = re.sub(r',\s*comdat(?:\(\$[^\)]+\))?', '', text)
    text = re.sub(r'\s+comdat(?:\(\$[^\)]+\))?', '', text)
    text = re.sub(r' "target-cpu"="[^"]*"', '', text)
    text = re.sub(r' "target-features"="[^"]*"', '', text)
    return text


class Unit:
    def __init__(self, source, root, work):
        self.source = source
        self.relative = str(source.relative_to(root))
        stem = re.sub(r'[^A-Za-z0-9_]', '_', self.relative)
        self.ir = work / "ir" / f"{stem}.ll"
        self.object = work / "obj" / f"{stem}.o"
        self.stamp = work / "ir" / f"{stem}.stamp"


def frontend(unit, args, header_stamp):
    """Compile one unit to retargeted IR; return (ok, diagnostic)."""
    is_c = unit.source.suffix == ".c"
    compiler = args.llvm / ("clang" if is_c else "clang++")
    command = [compiler, "--target=thumbv7-pc-windows-msvc", "-O2", "-ffreestanding",
               "-fno-stack-protector", "-DH2_NATIVE_PORT=1", "-DH2_WHOLE_IMAGE=1",
               "-fms-extensions", "-Wno-ignored-attributes", "-Wno-c++11-narrowing",
               "-DH2_MS_ABI_FRONTEND=1", "-Xclang", "-fno-wchar", "-w"]
    if not is_c:
        # Retail relies on LTCG to inline 'inline' definitions across files;
        # emit every one as a weak definition so other units can link it.
        command += ["-fno-exceptions", "-fno-rtti", "-Dinline=inline __attribute__((used))"]
    for include in args.include:
        command.append(f"-I{include}")
    command += ["-S", "-emit-llvm", unit.source, "-o", unit.ir]
    key = hashlib.sha256((header_stamp + str(unit.source.stat().st_mtime_ns) +
                          " ".join(map(str, command))).encode()).hexdigest()
    if unit.ir.exists() and unit.stamp.exists() and unit.stamp.read_text() == key:
        return True, ""
    unit.ir.parent.mkdir(parents=True, exist_ok=True)
    result = subprocess.run([str(c) for c in command], capture_output=True, text=True)
    if result.returncode:
        errors = [line for line in result.stderr.splitlines() if "error:" in line]
        unit.ir.unlink(missing_ok=True)
        return False, errors[0] if errors else "frontend failed"
    unit.ir.write_text(retarget(unit.ir.read_text()))
    unit.stamp.write_text(key)
    return True, ""


def scan(unit):
    text = unit.ir.read_text()
    functions = {}
    for m in DEFINE.finditer(text):
        functions[unquote(m.group(2))] = linkage(m.group(1))
    declarations = {unquote(m.group(1)) for m in DECLARE.finditer(text)}
    globals_ = {}
    for m in GLOBAL.finditer(text):
        name = unquote(m.group(1))
        if name.startswith("llvm."):
            continue
        prefix, rest = m.group(2), m.group(4)
        kind = linkage(prefix)
        if kind == "external":
            declarations.add(name)
            continue
        code_pointers = bool(re.search(r'@("[^"]+"|[\w.$?@]+)', rest)) and \
            "zeroinitializer" not in rest.split(",")[0]
        zero = rest.startswith(("zeroinitializer", "%")) and "zeroinitializer" in rest.split(", align")[0]
        globals_[name] = {"linkage": kind, "code_pointers": code_pointers, "zero": zero,
                          "constant": m.group(3) == "constant"}
    return {"functions": functions, "globals": globals_, "declarations": sorted(declarations)}


def split_type(text):
    """Return the leading IR type expression of an initializer and the rest."""
    depth = 0
    for index, char in enumerate(text):
        if char in "[{<(":
            depth += 1
        elif char in "]}>)":
            depth -= 1
        elif char == " " and depth == 0 and index:
            head = text[:index]
            if head.endswith((",", "x")):
                continue
            return head, text[index + 1:]
    return text, ""


def prune(text, delete_functions, delete_globals, promote=()):
    """Turn the named definitions into declarations without touching linkage
    of anything else (llvm-extract --delete externalizes local symbols), and
    give the named internal definitions external linkage."""
    lines = text.split("\n")
    out = []
    skipping = False
    for line in lines:
        if promote and line.startswith("define internal "):
            m = DEFINE.match(line)
            if m and unquote(m.group(2)) in promote:
                line = "define " + line[len("define internal "):]
        if skipping:
            if line == "}":
                skipping = False
            continue
        m = DEFINE.match(line)
        if m and unquote(m.group(2)) in delete_functions:
            header = line[len("define "):]
            header = re.sub(r'^(?:(?:internal|private|linkonce_odr|linkonce|weak_odr|weak|'
                            r'dso_local|local_unnamed_addr|unnamed_addr|hidden|protected) )*', '', header)
            close = header.rfind(")")
            out.append("declare " + header[:close + 1])
            if line.rstrip().endswith("{"):
                skipping = True
            continue
        g = GLOBAL.match(line)
        if g and unquote(g.group(1)) in delete_globals:
            kind = g.group(3)
            type_text, _ = split_type(g.group(4))
            out.append(f"@{g.group(1)} = external {kind} {type_text}")
            continue
        out.append(line)
    return "\n".join(out)


def backend(unit, args, delete_functions, delete_globals, promote=()):
    """Delete losing definitions, rebase, and emit an ELF object."""
    unit.object.parent.mkdir(parents=True, exist_ok=True)
    work = unit.object.with_suffix("")
    ir = unit.ir
    if delete_functions or delete_globals or promote:
        pruned = work.with_suffix(".pruned.ll")
        pruned.write_text(prune(ir.read_text(), delete_functions, delete_globals, promote))
        ir = pruned
    rebased = work.with_suffix(".rebased.ll")
    darwin = work.with_suffix(".darwin.s")
    elf = work.with_suffix(".elf.s")
    run([args.llvm / "opt", f"-load-pass-plugin={args.plugin}", "-passes=h2-rebase,verify",
         "-S", ir, "-o", rebased], capture_output=True)
    run([args.llvm / "llc", "-O2", "-mtriple=arm64_32-apple-watchos", rebased, "-o", darwin],
        capture_output=True)
    run([sys.executable, TOOLS / "guest_asm_elf.py", darwin, elf])
    elf.write_text(escape_symbols(elf.read_text()))
    run([args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj", elf,
         "-o", unit.object])
    for path in (rebased, darwin, elf):
        path.unlink(missing_ok=True)


def priority(unit, symbol, csv_sources, symbol_address):
    path = unit.relative
    if path.startswith("port/"):
        return 0
    if path.endswith("_native.cpp"):
        return 1
    address = symbol_address.get(symbol)
    if address is not None and csv_sources.get(address) == path:
        return 2
    if "/stubs/" in path:
        return 4
    return 3


def symbol_addresses(units):
    """Map symbols to the retail address of the nearest preceding @retail tag."""
    result = {}
    for unit in units:
        lines = unit.source.read_text(errors="replace").split("\n")
        pending = None
        for line in lines:
            tag = re.search(r'@retail 0x([0-9a-f]+)', line)
            if tag:
                pending = int(tag.group(1), 16)
                continue
            if pending is not None:
                m = re.search(r'([A-Za-z_~][A-Za-z0-9_:~]*)\s*\(', line)
                if m and not line.lstrip().startswith(("//", "/*", "*")):
                    result.setdefault((unit.relative, m.group(1).split("::")[-1]), pending)
                    pending = None
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--bindings", type=Path, action="append", default=[])
    parser.add_argument("--start", type=Path, required=True,
                        help="assembly defining the guest entry points")
    parser.add_argument("--linker-script", type=Path, required=True)
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    parser.add_argument("--exclude", action="append", default=[],
                        help="relative source path to leave out")
    args = parser.parse_args()
    root = args.root.resolve()
    args.include = [root / "port/include", root / "port/include/crt",
                    root / "port/include/xdk", root / "include"]
    work = args.work.resolve()
    work.mkdir(parents=True, exist_ok=True)

    sources = sorted((root / "src").glob("*.cpp")) + sorted((root / "src/stubs").glob("*.cpp"))
    sources += sorted((root / "port/runtime").glob("guest_*.cpp"))
    sources += [root / "port/runtime/guest_crt.c"]
    excluded = set(args.exclude)
    units = [Unit(s, root, work) for s in sources if str(s.relative_to(root)) not in excluded]

    header_files = list((root / "include").glob("*.h")) + list((root / "port/include").rglob("*"))
    header_stamp = str(max(p.stat().st_mtime_ns for p in header_files if p.is_file()))

    report = {"skipped": {}, "overridden": {}, "bound": {}, "stubbed": [], "data_stubbed": [],
              "native_address_globals": []}
    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        results = list(pool.map(lambda u: frontend(u, args, header_stamp), units))
    compiled = []
    for unit, (ok, diagnostic) in zip(units, results):
        if ok:
            compiled.append(unit)
        else:
            report["skipped"][unit.relative] = diagnostic
    print(f"frontend: {len(compiled)} compiled, {len(report['skipped'])} skipped", flush=True)

    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        scans = dict(zip((u.relative for u in compiled), pool.map(scan, compiled)))

    with (root / "config/functions.csv").open(newline="") as handle:
        csv_sources = {int(r["va"], 16): r["source"] for r in csv.DictReader(handle) if r["source"]}
    tag_addresses = symbol_addresses(compiled)

    def address_of(unit, symbol):
        base = re.match(r'\?+([A-Za-z_][A-Za-z0-9_]*)@', symbol)
        name = base.group(1) if base else symbol
        return tag_addresses.get((unit.relative, name))

    # Resolve strong function definitions.
    owners = {}
    for unit in compiled:
        for name, kind in scans[unit.relative]["functions"].items():
            if kind == "strong":
                owners.setdefault(name, []).append(unit)
    delete_functions = {u.relative: set() for u in compiled}
    for name, candidates in owners.items():
        if len(candidates) < 2:
            continue
        symbol_address = {name: address_of(c, name) for c in candidates}
        address = next((a for a in symbol_address.values() if a is not None), None)
        ranked = sorted(candidates, key=lambda c: (
            priority(c, name, csv_sources, {name: address}), c.relative))
        winner = ranked[0]
        report["overridden"][name] = {"winner": winner.relative,
                                      "losers": [c.relative for c in ranked[1:]]}
        for loser in ranked[1:]:
            delete_functions[loser.relative].add(name)

    # Functions other units reference but that only exist as internal
    # (static inline) definitions: give one definition external linkage.
    referenced = set()
    for unit in compiled:
        referenced.update(scans[unit.relative]["declarations"])
    promote = {u.relative: set() for u in compiled}
    internal_owners = {}
    for unit in compiled:
        for name, kind in scans[unit.relative]["functions"].items():
            if kind == "internal" and name in referenced and name not in owners:
                internal_owners.setdefault(name, []).append(unit)
    for name, candidates in internal_owners.items():
        winner = sorted(candidates, key=lambda c: (priority(c, name, {}, {}), c.relative))[0]
        promote[winner.relative].add(name)
        report.setdefault("promoted", {})[name] = winner.relative

    # Bindings: explicit manifests first, then addresses carried by names.
    bindings = {}
    for manifest in args.bindings:
        for entry in json.loads(manifest.read_text())["symbols"]:
            bindings[entry["symbol"]] = int(entry["address"], 0)

    global_owners = {}
    delete_globals = {u.relative: set() for u in compiled}
    for unit in compiled:
        for name, info in scans[unit.relative]["globals"].items():
            if info["linkage"] in ("internal",):
                continue
            address = bindings.get(name)
            match = ADDRESS_NAME.search(name)
            if address is None and match and not name.startswith(("??_7", "??_C")):
                address = int(match.group(1), 16)
            if address is not None and not info["code_pointers"]:
                delete_globals[unit.relative].add(name)
                bindings.setdefault(name, address)
                report["bound"][name] = f"0x{address:08x}"
                continue
            if address is not None:
                report["native_address_globals"].append(name)
            if info["linkage"] == "strong":
                global_owners.setdefault(name, []).append(unit)
    for name, candidates in global_owners.items():
        if len(candidates) < 2:
            continue
        ranked = sorted(candidates, key=lambda c: (priority(c, name, {}, {}), c.relative))
        for loser in ranked[1:]:
            delete_globals[loser.relative].add(name)

    def build(unit):
        try:
            backend(unit, args, delete_functions[unit.relative], delete_globals[unit.relative],
                    promote[unit.relative])
            return None
        except subprocess.CalledProcessError as error:
            stderr = error.stderr.decode() if isinstance(error.stderr, bytes) else (error.stderr or "")
            line = next((l for l in stderr.splitlines() if "error" in l.lower()), stderr[:200])
            return f"backend: {line}"

    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        failures = list(pool.map(build, compiled))
    objects = []
    for unit, failure in zip(compiled, failures):
        if failure:
            report["skipped"][unit.relative] = failure
        else:
            objects.append(unit.object)
    print(f"backend: {len(objects)} objects", flush=True)

    # Partial link, then find what is still undefined.
    partial = work / "whole-image.partial.o"
    run([args.lld, "-r", "-o", partial, *objects])
    start = work / "start.o"
    run([args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj", args.start, "-o", start])
    nm = run([args.llvm / "llvm-nm", "--undefined-only", "--format=just-symbols", partial, start],
             capture_output=True, text=True).stdout.split()
    defined = set(run([args.llvm / "llvm-nm", "--defined-only", "--format=just-symbols", partial, start],
                      capture_output=True, text=True).stdout.split())
    undefined = sorted(symbol.replace("%", "@") for symbol in set(nm) - defined)

    declared_functions = set()
    for unit in compiled:
        text_decl = scans[unit.relative]["declarations"]
        declared_functions.update(text_decl)
    function_like = set()
    for unit in compiled:
        ir = unit.ir.read_text()
        function_like.update(unquote(m.group(1)) for m in DECLARE.finditer(ir))

    host = []
    stubs = []
    data = []
    lines = ["// Generated by build_whole_image.py", '.section .text.h2_whole,"ax",@progbits',
             ".p2align 2"]
    for symbol in undefined:
        if symbol in bindings:
            continue
        match = ADDRESS_NAME.search(symbol)
        if match and symbol not in function_like:
            bindings[symbol] = int(match.group(1), 16)
            report["bound"][symbol] = f"0x{bindings[symbol]:08x}"
            continue
        if symbol.startswith("host_"):
            host.append(symbol)
        elif symbol in function_like:
            stubs.append(symbol)
        else:
            data.append(symbol)
    services = (root / "port/src/host_services.cpp").read_text()
    patched = sorted(set(re.findall(r'\{"(host_h2_\w+)"', services)) | {"host_h2_unimplemented"})
    trampolines = list(host)
    host = sorted(set(host) | set(patched))
    for symbol in trampolines:
        slot = f"h2_host_slot_{symbol}"
        lines += [f'.globl "{symbol}"', f'.type "{symbol}", @function', f'"{symbol}":',
                  f"    adrp x16, {slot}", f"    ldr x16, [x16, :lo12:{slot}]", "    br x16"]
    for index, symbol in enumerate(stubs):
        lines += [f'.globl "{elf_name(symbol)}"', f'.type "{elf_name(symbol)}", @function', f'"{elf_name(symbol)}":',
                  # adrp is PC-relative, so this is already the host address.
                  f"    adrp x0, h2_stub_name_{index}", f"    add x0, x0, :lo12:h2_stub_name_{index}",
                  "    b host_h2_unimplemented"]
    if "host_h2_unimplemented" not in trampolines:
        slot = "h2_host_slot_host_h2_unimplemented"
        lines += [".globl host_h2_unimplemented", ".type host_h2_unimplemented, @function",
                  "host_h2_unimplemented:", f"    adrp x16, {slot}",
                  f"    ldr x16, [x16, :lo12:{slot}]", "    br x16"]
    lines += ['.section .rodata.h2_whole_names,"a",@progbits']
    for index, symbol in enumerate(stubs):
        escaped = symbol.replace("\\", "\\\\").replace('"', '\\"')
        lines += [f"h2_stub_name_{index}:", f'    .asciz "{escaped}"']
    lines += ['.section .data.h2_whole_host,"aw",@progbits', ".p2align 3"]
    for symbol in host:
        slot = f"h2_host_slot_{symbol}"
        lines += [f".globl {slot}", f"{slot}:", "    .xword 0"]
    lines += ['.section .bss.h2_whole_data,"aw",@nobits', ".p2align 4"]
    for symbol in data:
        lines += [f'.globl "{elf_name(symbol)}"', f'"{elf_name(symbol)}":', "    .zero 4096"]
    for symbol, address in sorted(bindings.items()):
        if symbol in undefined:
            escaped = elf_name(symbol).replace('"', '\\"')
            lines += [f'.globl "{escaped}"', f'.set "{escaped}", 0x{address:08x}']
    glue = work / "glue.s"
    glue.write_text("\n".join(lines) + "\n")
    glue_object = work / "glue.o"
    run([args.llvm / "llvm-mc", "-triple=aarch64-linux-gnu", "-filetype=obj", glue, "-o", glue_object])
    report["stubbed"] = stubs
    report["data_stubbed"] = data
    report["host_imports"] = host

    image = work / "whole-image.elf"
    link = subprocess.run([str(args.lld), "-static", "--no-undefined", "-T", str(args.linker_script),
                           "-z", "max-page-size=0x4000", "-o", str(image), str(partial),
                           str(start), str(glue_object)], capture_output=True, text=True)
    report["link"] = link.stderr.strip().splitlines()[:50]
    (work / "whole-image-report.json").write_text(json.dumps(report, indent=1, sort_keys=True))
    print(f"stubs: {len(stubs)} functions, {len(data)} data; host imports: {len(host)}; "
          f"bound globals: {len(report['bound'])}; overrides: {len(report['overridden'])}")
    if link.returncode:
        print(link.stderr[:4000])
        return 1
    print(f"image: {image}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
