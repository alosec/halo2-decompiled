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

    primary_path = args.root / "port/runtime/havok_primary_listener_vtables.json"
    primary = json.loads(primary_path.read_text())
    expected_primary_tables = [
        ("embedded_reference_callback", 0x1c, 0x0045A738,
         "??_7c_callback_277820@@6B@", [0x000A9EF0, 0x00277820]),
        ("contact_events", 0x04, 0x0045A74C,
         "??_7c_interface_277890@@6Bc_interface_277840@@@",
         [0x00277A00, 0x00278030, 0x00278100]),
        ("entity_listener", 0x00, 0x0045A758,
         "??_7c_interface_277890@@6Bc_interface_2777d0@@@",
         [0x00277890, 0x002778E0, 0x00277990, 0x00278200,
          0x002789F0, 0x00278A70, 0x00278B30, 0x00278AE0,
          0x002789B0, 0x00277800, 0x0032131E, 0x0032131E,
          0x0032131E]),
    ]
    actual_primary_tables = [
        (entry.get("name"), int(entry.get("object_offset", "-1"), 0),
         int(entry.get("retail_address", "0"), 0), entry.get("native_symbol"),
         [int(method, 0) for method in entry.get("methods", [])])
        for entry in primary.get("tables", [])
    ]
    if (primary.get("version") != 1 or
            int(primary.get("object_size", "0"), 0) != 0x28 or
            actual_primary_tables != expected_primary_tables):
        raise RuntimeError("primary Havok listener table inventory changed")
    primary_methods = [method for table in actual_primary_tables for method in table[4]]
    if len(primary_methods) != 18 or len(set(primary_methods)) != 16:
        raise RuntimeError("primary Havok listener method coverage changed")
    primary_unique = set(primary_methods)
    if primary_unique - functions.keys():
        raise RuntimeError("primary Havok listener method is absent from functions.csv")
    primary_source_owned = {
        method for method in primary_unique if functions[method]["source"]
    }
    if len(primary_source_owned) != 12:
        raise RuntimeError("primary Havok listener source coverage changed")
    excluded_primary = primary.get("excluded_adjacent_tables")
    if ([int(entry.get("retail_address", "0"), 0) for entry in excluded_primary]
            != [0x0045A740, 0x0045A78C]):
        raise RuntimeError("primary Havok listener table boundaries changed")

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
        "g_468630_owned_object_type_definitions",
        "g_4e0350_owned_scenario",
        "g_4de2e0_owned_object_partition",
        "g_4f55d0_owned_ai_globals",
        "g_4f55f0_owned_actor_pool",
        "g_51e9d8_owned_squad_pool",
        "g_5044c8_owned_perception_pool",
        "g_502420_owned_clump_pool",
        "g_46eeb8_native_slot_callbacks",
        "g_557c6c_owned_reference_table",
        "g_557c6c_native_hash_callbacks",
        "g_502414_owned_tracking_pool",
        "g_502418_owned_prop_reference_pool",
        "g_50241c_owned_prop_pool",
        "g_51eca4_owned_location_record_pool",
        "g_4e0340_owned_collision_bsp",
        "g_4e0348_owned_match_globals",
        "g_4e7c20_owned_collision_scratch",
        "g_4ed280_owned_bsp_bit_vectors",
        "g_479888_owned_havok_allocator_block",
        "g_479898_owned_havok_pool_allocator",
        "g_51eca8_owned_primary_listener_storage",
        "g_51ecac_owned_secondary_listener",
        "native_havok_transition_tables_and_callback_owned",
        "g_510c44_owned_ai_scratch_buffer",
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
    if retained_globals != {
        "??_C@_0BH@MAPMFIGJ@placing?5objects?5on?5bsp?$AA@",
        "g_h2_native_fixed_vtable",
        "g_h2_native_pool_vtable",
        "g_h2_native_primary_listener_storage",
        "g_h2_native_secondary_listener_vtable",
        "g_h2_native_secondary_listener_storage",
        "g_h2_native_transition_pending_table",
        "g_h2_native_transition_complete_table",
        "g_h2_native_transition_context",
    }:
        raise RuntimeError("structure-BSP retained data globals changed")
    required_light_symbols = {
        "?function_c1720@@YAXJ_N0@Z",
        "?function_c2d00@@YAXJ@Z",
    }
    if not required_light_symbols.issubset(closure_symbols):
        raise RuntimeError("object-attached light refresh left outside BSP closure")
    required_actor_reference_symbols = {
        "?function_13e2d0@@YAPAUhash_node@@PAUhash_table@@PAX@Z",
        "?hash_table_remove@@YA_NPAUhash_table@@PAX@Z",
        "?function_1e1500@@YA_NJ@Z",
        "?function_1e22d0@@YAXJ_N@Z",
        "?function_1f86a0@@YAXJ@Z",
        "?function_25d690@@YAPAUs_type_5cfb45@@PAUs_prop_datum@@@Z",
        "?function_2628f0@@YAXJUs_reference@@@Z",
        "?function_26dc20@@YAXJ@Z",
        "?function_28fdf0@@YAXJ@Z",
    }
    if not required_actor_reference_symbols.issubset(closure_symbols):
        raise RuntimeError("actor-reference refresh left outside BSP closure")
    required_geometry_symbols = {
        "?function_16a8e0@@YA_NJPBTpoint3f@@MJJPAT1@PAM@Z",
        "?function_1f0230@@YA_NPAUs_lookup@@PBTpoint3f@@MMMPAUs_collection_245270@@@Z",
        "?function_244ca0@@YAXMMPAUs_line_list@@JJJEEFPBTpoint3f@@@Z",
        "?function_245aa0@@YAXPBUs_245aa0@@PBUs_source_245400@@PBUtransform4x3f@@MMJJPAUs_collection_245270@@@Z",
        "?function_2469b0@@YA_NPBUs_shapes@@PBTpoint3f@@PBTvector3f@@PAUs_ray_shape_result@@@Z",
    }
    if not required_geometry_symbols.issubset(closure_symbols):
        raise RuntimeError("collision correction left outside BSP closure")
    required_fixed_allocator_symbols = {
        "?function_146a20@@YAXXZ",
        "?function_146b80@@YAXXZ",
        "h2_native_havok_fixed_memory_construct",
        "h2_native_havok_pool_memory_construct",
        "h2_native_pool_allocate_aligned",
        "h2_native_pool_deallocate_aligned",
        "?allocate_aligned@c_havok_fixed_memory@@UAAPAXJJJ@Z",
        "?h2_native_fixed_allocate_aligned@@YAPAXPAVc_havok_fixed_memory@@JJJ@Z",
        "?h2_native_fixed_deallocate_aligned@@YAXPAVc_havok_fixed_memory@@PAX@Z",
    }
    if not required_fixed_allocator_symbols.issubset(closure_symbols):
        raise RuntimeError("Havok scratch allocator left outside BSP closure")
    required_secondary_listener_symbols = {
        "?function_278370@c_278370@@UAAXPAUs_278371@@@Z",
        "?function_3153c0@@YAXPAX0PBX@Z",
        "h2_native_havok_primary_listener_construct",
        "h2_native_havok_secondary_listener_construct",
        "h2_native_secondary_listener_event",
        "h2_native_secondary_listener_idle",
    }
    if not required_secondary_listener_symbols.issubset(closure_symbols):
        raise RuntimeError("secondary Havok listener left outside BSP closure")

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
    if len(undefined) != 22:
        raise RuntimeError(f"structure-BSP bound closure changed: {len(undefined)} imports, expected 22")
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
    if len(production_undefined) != 16:
        raise RuntimeError(
            f"structure-BSP production closure changed: {len(production_undefined)} imports, expected 16")

    frontier_path = args.root / "port/runtime/structure_bsp_remaining_frontier.json"
    frontier_manifest = json.loads(frontier_path.read_text())
    if frontier_manifest.get("version") != 1:
        raise RuntimeError("unsupported structure-BSP frontier manifest")
    frontier_entries = frontier_manifest.get("imports")
    if not isinstance(frontier_entries, list) or len(frontier_entries) != 16:
        raise RuntimeError("structure-BSP frontier inventory changed")
    frontier_symbols = set()
    allowed_subsystems = {"ai", "geometry_query", "havok_lifecycle", "object_placement"}
    for entry in frontier_entries:
        if (not isinstance(entry, dict) or not isinstance(entry.get("symbol"), str) or
                entry.get("subsystem") not in allowed_subsystems or
                not isinstance(entry.get("blocker"), str) or not entry["blocker"].strip()):
            raise RuntimeError("invalid structure-BSP frontier entry")
        if "stub" in entry["blocker"].lower():
            raise RuntimeError("structure-BSP frontier must describe real ownership, not stubs")
        frontier_symbols.add(entry["symbol"])
    if len(frontier_symbols) != len(frontier_entries):
        raise RuntimeError("structure-BSP frontier contains duplicate symbols")
    if frontier_symbols != production_undefined:
        raise RuntimeError("structure-BSP frontier manifest differs from production imports: " +
                           ", ".join(sorted(frontier_symbols ^ production_undefined)))

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
    absolute_lines = [line for line in data_relocations.splitlines()
                      if "R_AARCH64_ABS32" in line]
    callback_lines = [line for line in absolute_lines
                      if int(line.split()[0], 16) < 0x110]
    allocator_lines = [line for line in absolute_lines
                       if int(line.split()[0], 16) >= 0x110]
    if len(callback_lines) != 21:
        raise RuntimeError("structure-BSP callback table must retain exactly 21 providers")
    if sum("h2_native_structure_bsp_idle" in line for line in callback_lines) != 5:
        raise RuntimeError("structure-BSP callback table must retain five idle leaves")
    fixed_allocator_lines = [line for line in allocator_lines
                             if "h2_native_fixed_" in line]
    pool_allocator_lines = [line for line in allocator_lines
                            if "h2_native_pool_" in line]
    secondary_listener_lines = [line for line in allocator_lines
                                if "h2_native_secondary_listener_" in line]
    if len(fixed_allocator_lines) != 11:
        raise RuntimeError("native fixed allocator table must retain exactly 11 methods")
    if sum("h2_native_fixed_idle" in line for line in fixed_allocator_lines) != 4:
        raise RuntimeError("native fixed allocator table must retain four idle methods")
    if len(pool_allocator_lines) != 11:
        raise RuntimeError("native pool allocator table must retain exactly 11 methods")
    if sum("h2_native_pool_idle" in line for line in pool_allocator_lines) != 4:
        raise RuntimeError("native pool allocator table must retain four idle methods")
    if len(secondary_listener_lines) != 3:
        raise RuntimeError("native secondary listener table must retain exactly three methods")
    if sum("h2_native_secondary_listener_idle" in line for line in secondary_listener_lines) != 2:
        raise RuntimeError("native secondary listener table must retain two idle methods")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
