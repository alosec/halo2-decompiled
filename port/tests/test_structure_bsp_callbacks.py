#!/usr/bin/env python3
import argparse
import csv
import json
from pathlib import Path
import re
import subprocess
import tempfile

EXPECTED = {
    16: 0x00123D20,
    19: 0x00175F40,
    27: 0x000B6BB0,
    31: 0x000226A0,
    34: 0x00183EE0,
    36: 0x0016F120,
    37: 0x0017D350,
    39: 0x0017B940,
    40: 0x00175D70,
    41: 0x0003F2B0,
    44: 0x00125920,
    46: 0x00188980,
    57: 0x000D4E30,
    58: 0x001C7BB0,
    59: 0x001C2690,
    60: 0x0014B6A0,
    61: 0x00175F40,
    62: 0x00175F40,
    63: 0x00210DF0,
    65: 0x00175F40,
    67: 0x00175F40,
}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm-nm", type=Path, required=True)
    parser.add_argument("--llvm-readelf", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--provider-cluster-object", type=Path, required=True)
    parser.add_argument("--production-object", type=Path, required=True)
    args = parser.parse_args()

    path = args.root / "port/runtime/structure_bsp_switch_callbacks.json"
    manifest = json.loads(path.read_text())
    if manifest.get("version") != 1:
        raise RuntimeError("unsupported structure-BSP callback manifest")
    if (
        int(manifest.get("retail_table_base", "0"), 0) != 0x00440DE8
        or manifest.get("row_count") != 68
        or manifest.get("row_stride") != 0x24
        or manifest.get("callback_offset") != 0
    ):
        raise RuntimeError("structure-BSP callback table geometry changed")

    callbacks = manifest.get("callbacks")
    if not isinstance(callbacks, list):
        raise RuntimeError("structure-BSP callback inventory is missing")
    actual = {entry["row"]: int(entry["method"], 0) for entry in callbacks}
    if len(actual) != len(callbacks) or actual != EXPECTED:
        raise RuntimeError("structure-BSP callback inventory changed")
    unique = set(actual.values())
    if len(callbacks) != 21 or len(unique) != 17:
        raise RuntimeError("structure-BSP callback coverage ratchet changed")
    if sum(method == 0x00175F40 for method in actual.values()) != 5:
        raise RuntimeError("structure-BSP idle callback inventory changed")

    with (args.root / "config/functions.csv").open(newline="") as source:
        functions = {int(row["va"], 16): row for row in csv.DictReader(source)}
    absent = sorted(unique - functions.keys())
    if absent:
        raise RuntimeError("callbacks absent from functions.csv: " +
                           ", ".join(hex(value) for value in absent))
    recovered = {method for method in unique if functions[method]["source"]}
    if recovered != unique:
        raise RuntimeError("all structure-BSP callbacks must remain source-owned")

    providers_path = args.root / "port/runtime/structure_bsp_native_providers.json"
    providers_manifest = json.loads(providers_path.read_text())
    if providers_manifest.get("version") != 1:
        raise RuntimeError("unsupported structure-BSP provider manifest")
    providers = providers_manifest.get("providers")
    if not isinstance(providers, list) or len(providers) != 17:
        raise RuntimeError("structure-BSP provider inventory changed")
    provider_methods = {int(entry["method"], 0) for entry in providers}
    if provider_methods != unique:
        raise RuntimeError("structure-BSP providers do not cover every unique callback")
    if sum(bool(entry.get("portable")) for entry in providers) != 17:
        raise RuntimeError("all structure-BSP providers must remain portable")
    blocked = {int(entry["method"], 0) for entry in providers if not entry.get("portable")}
    if blocked:
        raise RuntimeError("structure-BSP portability frontier reopened")
    for entry in providers:
        if entry["source"] != "native" and not (args.root / entry["source"]).is_file():
            raise RuntimeError(f"missing structure-BSP provider source: {entry['source']}")

    execution_preconditions = providers_manifest.get("execution_preconditions")
    if execution_preconditions != [
        "g_51f400_owned_hash_table",
        "g_51f400_native_hash_callback",
        "g_4f55f8_native_ai_text_callbacks",
        "g_51ecb4_owned_flock_pool",
        "g_4e7408_owned_random_state",
        "g_4f93a0_owned_output_table",
        "g_47f048_owned_havok_listener",
        "g_480118_owned_havok_allocator",
        "g_461f08_owned_havok_state_flag",
        "g_51e9cc_owned_scenery_index_table",
        "g_51e9d0_owned_shape_owner",
        "native_offline_replication_unavailable",
    ]:
        raise RuntimeError("structure-BSP callback execution preconditions changed")

    closure = providers_manifest.get("closure")
    if not isinstance(closure, list) or not closure:
        raise RuntimeError("structure-BSP transitive closure inventory is missing")
    retained_globals = set()
    for provider in providers:
        entry_globals = provider.get("globals", [])
        if not isinstance(entry_globals, list):
            raise RuntimeError("invalid structure-BSP provider globals")
        retained_globals.update(entry_globals)
    closure_symbols = set()
    for entry in closure:
        source = entry.get("source")
        entry_symbols = entry.get("symbols")
        entry_globals = entry.get("globals", [])
        if (not source or not isinstance(entry_symbols, list) or not entry_symbols or
                not isinstance(entry_globals, list)):
            raise RuntimeError("invalid structure-BSP transitive closure entry")
        if not (args.root / source).is_file():
            raise RuntimeError(f"missing structure-BSP closure source: {source}")
        closure_symbols.update(entry_symbols)
        retained_globals.update(entry_globals)
    if retained_globals != {"??_C@_0BH@MAPMFIGJ@placing?5objects?5on?5bsp?$AA@"}:
        raise RuntimeError("structure-BSP retained data globals changed")
    required_light_symbols = {
        "?function_c1720@@YAXJ_N0@Z",
        "?function_c2d00@@YAXJ@Z",
    }
    if not required_light_symbols.issubset(closure_symbols):
        raise RuntimeError("object-attached light refresh left outside BSP closure")

    symbols = subprocess.check_output(
        [args.llvm_nm, "--defined-only", "--format=posix", args.provider_cluster_object],
        text=True,
    )
    definitions = {line.split()[0] for line in symbols.splitlines() if line.strip()}
    undefined_text = subprocess.check_output(
        [args.llvm_nm, "--undefined-only", "--format=posix", args.provider_cluster_object],
        text=True,
    )
    undefined = {line.split()[0] for line in undefined_text.splitlines() if line.strip()}
    if len(undefined) != 31:
        raise RuntimeError(f"structure-BSP bound closure changed: {len(undefined)} imports, expected 31")
    leaked_runtime = {"h2_guest_memcpy", "h2_guest_memset"} & undefined
    if leaked_runtime:
        raise RuntimeError("structure-BSP bound cluster leaked guest CRT imports")
    encoded_globals = sorted(symbol for symbol in undefined
                             if re.match(r"\?g_[0-9a-fA-F]+@@", symbol))
    if encoded_globals:
        raise RuntimeError("structure-BSP bound cluster left encoded globals unbound: " +
                           ", ".join(encoded_globals))

    production_owned = {
        "?data_datum_iterator_next@@YA_NPAUs_data_datum_iterator@@@Z",
        "?function_16bc00@@YAJPAUs_record_pool@@J@Z",
        "?loop_compact@@YAXPAUs_loop_allocator@@@Z",
        "?record_pool_allocate@@YAJPAUs_record_pool@@@Z",
        "?record_pool_release@@YAXPAUs_record_pool@@J@Z",
        "?record_pool_release_all@@YAXPAUs_record_pool@@@Z",
    }
    with tempfile.TemporaryDirectory(prefix="h2-structure-bsp-production-") as temporary:
        combined = Path(temporary) / "cluster.o"
        subprocess.run([
            args.lld, "-r", "--allow-multiple-definition", "-o", combined,
            args.provider_cluster_object, args.production_object,
        ], check=True)
        production_text = subprocess.check_output(
            [args.llvm_nm, "--undefined-only", "--format=posix", combined], text=True,
        )
    production_undefined = {
        line.split()[0] for line in production_text.splitlines() if line.strip()
    }
    if undefined - production_undefined != production_owned:
        raise RuntimeError("structure-BSP production ownership changed: " +
                           ", ".join(sorted(undefined - production_undefined)))
    if production_undefined - undefined:
        raise RuntimeError("structure-BSP production link added imports: " +
                           ", ".join(sorted(production_undefined - undefined)))
    if len(production_undefined) != 25:
        raise RuntimeError(
            f"structure-BSP production closure changed: {len(production_undefined)} imports, expected 25")

    expected_symbols = ({entry["symbol"] for entry in providers} |
                        closure_symbols | retained_globals)
    expected_symbols.add("h2_native_structure_bsp_switch_callbacks")
    missing_symbols = sorted(expected_symbols - definitions)
    if missing_symbols:
        raise RuntimeError("structure-BSP provider cluster omitted: " +
                           ", ".join(missing_symbols))

    relocations = subprocess.check_output(
        [args.llvm_readelf, "-r", args.provider_cluster_object], text=True,
    )
    data_heading = "Relocation section '.rela.data'"
    if data_heading not in relocations:
        raise RuntimeError("structure-BSP callback table relocation section is missing")
    data_relocations = relocations.split(data_heading, 1)[1].split("Relocation section ", 1)[0]
    if data_relocations.count("R_AARCH64_ABS32") != 21:
        raise RuntimeError("structure-BSP callback table must retain exactly 21 providers")
    if data_relocations.count("h2_native_structure_bsp_idle") != 5:
        raise RuntimeError("structure-BSP callback table must retain five idle leaves")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
