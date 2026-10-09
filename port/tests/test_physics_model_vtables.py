#!/usr/bin/env python3
import argparse
import csv
import json
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
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
