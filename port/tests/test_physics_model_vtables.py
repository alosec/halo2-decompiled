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
EXPECTED_COMPLETE_TABLES = {0x00455498, 0x00455EF0, 0x00455F30, 0x00455F8C}


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
    if len(recovered) < 62:
        raise RuntimeError(f"recovered-method ratchet regressed: {len(recovered)} < 62")
    complete_tables = {
        int(table["retail_address"], 0)
        for table in tables
        if all(int(method, 0) in recovered for method in table["slots"])
    }
    if not EXPECTED_COMPLETE_TABLES.issubset(complete_tables):
        missing = sorted(EXPECTED_COMPLETE_TABLES - complete_tables)
        raise RuntimeError("complete-table ratchet regressed: " + ", ".join(hex(v) for v in missing))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
