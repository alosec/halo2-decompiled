#!/usr/bin/env python3
import argparse
import csv
import json
import subprocess
import sys
import tempfile
from pathlib import Path

EXPECTED_TABLES = {
    0x00455500: ("0x58", 16, 0x002DF310),
    0x00455EF0: ("0x60-child", 16, 0x002FE260),
    0x00455498: ("0x60-parent", 10, 0x002DF4B0),
    0x00456008: ("0x70", 16, 0x002FD0A0),
    0x00455FC0: ("0x90", 17, 0x002FDE30),
    0x00455F30: ("0x108-parent", 10, 0x002FE0B0),
    0x00455F8C: ("0x108-child", 12, 0x002FDFA0),
}
COMMON_SLOTS = {1: 0x002D7210, 2: 0x002D7240, 4: 0x00072C70}
EXPECTED_COMPLETE_TABLES = set(EXPECTED_TABLES)
EXPECTED_NATIVE_PROVIDERS = {
    0x00072C70: "h2_native_physics_idle",
    0x00099810: "h2_native_physics_zero",
    0x0009A5E0: "h2_native_physics_kind_22",
    0x0009AC70: "h2_native_physics_kind_6",
    0x0009B910: "h2_native_physics_kind_7",
    0x0009BEC0: "h2_native_physics_kind_2",
    0x0009CD70: "h2_native_physics_kind_8",
    0x000A0250: "h2_native_physics_kind_10",
    0x001C2360: "h2_native_physics_delete",
    0x001C2380: "h2_native_physics_delete",
    0x001EA930: "h2_native_physics_delete",
    0x001EA970: "h2_native_physics_delete",
    0x001EA990: "h2_native_physics_delete",
    0x001EA9A0: "h2_native_physics_delete",
    0x001EE340: "h2_native_physics_first_vertex",
    0x00224240: "h2_native_physics_none",
    0x00244930: "h2_native_physics_delete",
}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm-readelf", type=Path)
    parser.add_argument("--table-object", type=Path)
    parser.add_argument("--leaf-object", type=Path)
    parser.add_argument("--initialize-object", type=Path)
    args = parser.parse_args()

    manifest_path = args.root / "port/runtime/physics_model_vtables.json"
    manifest = json.loads(manifest_path.read_text())
    if manifest.get("version") != 1:
        raise RuntimeError("unsupported physics-model vtable manifest version")

    tables = manifest.get("tables")
    if not isinstance(tables, list) or len(tables) != len(EXPECTED_TABLES):
        raise RuntimeError("physics-model vtable manifest must contain exactly seven tables")

    seen = set()
    slots_total = 0
    unique_methods = set()
    for table in tables:
        address = int(table["retail_address"], 0)
        if address in seen or address not in EXPECTED_TABLES:
            raise RuntimeError(f"unexpected or duplicate table address: 0x{address:08x}")
        seen.add(address)
        expected_family, expected_count, expected_type = EXPECTED_TABLES[address]
        slots = [int(value, 0) for value in table["slots"]]
        if table["family_offset"] != expected_family:
            raise RuntimeError(f"wrong family label for table 0x{address:08x}")
        if len(slots) != expected_count:
            raise RuntimeError(f"wrong slot count for table 0x{address:08x}")
        if slots[3] != expected_type:
            raise RuntimeError(f"wrong type-descriptor method for table 0x{address:08x}")
        for index, method in COMMON_SLOTS.items():
            if slots[index] != method:
                raise RuntimeError(f"wrong common slot {index} for table 0x{address:08x}")
        slots_total += len(slots)
        unique_methods.update(slots)

    if seen != set(EXPECTED_TABLES) or slots_total != 97 or len(unique_methods) != 72:
        raise RuntimeError("physics-model vtable coverage ratchet changed")

    with (args.root / "config/functions.csv").open(newline="") as source:
        functions = {int(row["va"], 16): row for row in csv.DictReader(source)}
    absent = sorted(unique_methods - functions.keys())
    if absent:
        raise RuntimeError("vtable methods absent from functions.csv: " + ", ".join(hex(v) for v in absent))

    recovered = {method for method in unique_methods if functions[method]["source"]}
    if len(recovered) != 72:
        raise RuntimeError(f"all active methods must remain recovered: {len(recovered)} != 72")
    complete_tables = {
        int(table["retail_address"], 0)
        for table in tables
        if all(int(method, 0) in recovered for method in table["slots"])
    }
    if not EXPECTED_COMPLETE_TABLES.issubset(complete_tables):
        missing = sorted(EXPECTED_COMPLETE_TABLES - complete_tables)
        raise RuntimeError("complete-table ratchet regressed: " + ", ".join(hex(v) for v in missing))

    provider_path = args.root / "port/runtime/physics_model_native_providers.json"
    provider_manifest = json.loads(provider_path.read_text())
    if provider_manifest.get("version") != 1:
        raise RuntimeError("unsupported native physics provider manifest version")
    raw_providers = provider_manifest.get("providers")
    if not isinstance(raw_providers, dict):
        raise RuntimeError("native physics provider map is missing")
    providers = {int(address, 0): value for address, value in raw_providers.items()}
    if set(providers) != unique_methods:
        missing = sorted(unique_methods - providers.keys())
        extra = sorted(providers.keys() - unique_methods)
        raise RuntimeError(f"native provider coverage mismatch: missing={missing}, extra={extra}")
    native_providers = {
        method: provider.get("symbol")
        for method, provider in providers.items()
        if provider.get("kind") == "native"
    }
    if native_providers != EXPECTED_NATIVE_PROVIDERS:
        raise RuntimeError("native-specialized provider inventory changed")
    for method, provider in providers.items():
        if provider.get("kind") not in {"native", "source"}:
            raise RuntimeError(f"invalid provider kind for {method:#x}")
        symbol = provider.get("symbol")
        if not isinstance(symbol, str) or not symbol:
            raise RuntimeError(f"missing provider symbol for {method:#x}")
        if provider["kind"] == "source" and not symbol.startswith("?"):
            raise RuntimeError(f"source provider lacks Microsoft symbol for {method:#x}")
    for table in tables:
        slots = [int(value, 0) for value in table["slots"]]
        if providers[slots[0]]["symbol"] != "h2_native_physics_delete":
            raise RuntimeError("active table does not use cache-owned deleting semantics")
        if providers[slots[4]]["symbol"] != "h2_native_physics_idle":
            raise RuntimeError("active table does not use the recovered idle slot")

    native_source = (args.root / "port/runtime/guest_physics_model_runtime.cpp").read_text()
    for symbol in EXPECTED_NATIVE_PROVIDERS.values():
        if symbol not in native_source:
            raise RuntimeError(f"native leaf provider is not defined: {symbol}")

    with tempfile.TemporaryDirectory() as temporary:
        assembly_path = Path(temporary) / "physics-model-vtables.s"
        subprocess.run([
            sys.executable,
            args.root / "port/tools/generate_physics_model_vtables.py",
            "--tables", manifest_path,
            "--providers", provider_path,
            "--output", assembly_path,
        ], check=True)
        assembly = assembly_path.read_text()
    if assembly.count("  .long ") != 104:
        raise RuntimeError("generated tables must contain 97 slots and seven lookup entries")
    for address, (_, slot_count, _) in EXPECTED_TABLES.items():
        label = f"h2_native_physics_model_vtable_{address:08x}:"
        if assembly.count(label) != 1:
            raise RuntimeError(f"generated native table is missing: {address:#x}")
        body = assembly.split(label, 1)[1].split(".size", 1)[0]
        if body.count("  .long ") != slot_count:
            raise RuntimeError(f"generated native table has wrong extent: {address:#x}")

    object_arguments = (
        args.llvm_readelf, args.table_object, args.leaf_object, args.initialize_object
    )
    if any(object_arguments) and not all(object_arguments):
        raise RuntimeError("all native table object arguments must be supplied together")
    if all(object_arguments):
        relocations = subprocess.check_output(
            [args.llvm_readelf, "-r", args.table_object], text=True
        )
        if relocations.count("R_AARCH64_ABS32") != 104:
            raise RuntimeError("native table object must contain exactly 104 ABS32 relocations")
        llvm_nm = args.llvm_readelf.with_name("llvm-nm")
        leaf_symbols = subprocess.check_output(
            [llvm_nm, "--defined-only", "--format=posix", args.leaf_object], text=True
        )
        for symbol in set(EXPECTED_NATIVE_PROVIDERS.values()):
            if f"{symbol} " not in leaf_symbols and f"_{symbol} " not in leaf_symbols:
                raise RuntimeError(f"compiled native leaf provider is absent: {symbol}")
        initialize_symbols = subprocess.check_output(
            [llvm_nm, "--defined-only", "--format=posix", args.initialize_object],
            text=True,
        )
        for symbol in (
            "h2_native_physics_model_initialize",
            "h2_native_physics_model_write_reset",
            "h2_native_physics_model_write_status",
        ):
            if f"{symbol} " not in initialize_symbols:
                raise RuntimeError(f"compiled active-family initializer is absent: {symbol}")
        initialize_undefined = subprocess.check_output(
            [llvm_nm, "--undefined-only", "--format=posix", args.initialize_object],
            text=True,
        )
        for address in EXPECTED_TABLES:
            symbol = f"h2_native_physics_model_vtable_{address:08x}"
            if f"{symbol} " not in initialize_undefined:
                raise RuntimeError(f"initializer does not reference complete table: {address:#x}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
