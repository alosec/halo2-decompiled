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
    parser.add_argument("--primary-listener-table-object", type=Path, required=True)
    parser.add_argument("--primary-listener-provider-cluster-object", type=Path, required=True)
    parser.add_argument("--particle-provider-table-object", type=Path, required=True)
    parser.add_argument("--particle-provider-unbound-object", type=Path, required=True)
    parser.add_argument("--particle-provider-cluster-object", type=Path, required=True)
    parser.add_argument("--contact-processing-candidate-object", type=Path, required=True)
    parser.add_argument("--impact-allocation-candidate-object", type=Path, required=True)
    parser.add_argument("--effect-lifecycle-candidate-object", type=Path, required=True)
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

    particle_provider_path = (
        args.root / "port/runtime/effect_particle_provider_vtables.json")
    particle_providers = json.loads(particle_provider_path.read_text())
    expected_particle_objects = [
        ("prt3", 0x00479868, 0x004535F0, "c_interface_b",
         [0x224D20, 0x224D50, 0x224D60, 0x224D30, 0x224240, 0x224240,
          0x224250, 0x224260, 0x224280, 0x224290, 0x2242A0, 0x2242B0,
          0x2242E0, 0x2242F0, 0x224300, 0x224310, 0x224320, 0x224330,
          0x224340, 0x224350, 0x2243A0]),
        ("PRTM", 0x00479874, 0x00453598, "c_interface_a",
         [0x224CF0, 0x224D00, 0x224D10, 0x224D20, 0x224240, 0x224240,
          0x224D30, 0x224D40, 0x224D50, 0x224D60, 0x224D70, 0x224D90,
          0x2242E0, 0x2242F0, 0x224300, 0x224310, 0x224320, 0x224330,
          0x224340, 0x224DA0, 0x2243A0]),
    ]
    actual_particle_objects = []
    particle_methods = []
    particle_provider_symbols = []
    for entry in particle_providers.get("objects", []):
        slots = entry.get("slots", [])
        if [slot.get("slot") for slot in slots] != list(range(21)):
            raise RuntimeError("particle provider slot order changed")
        methods = [int(slot.get("retail_method", "0"), 0) for slot in slots]
        class_name = entry.get("class")
        expected_symbols = []
        for slot_index in range(21):
            if slot_index <= 6 or 8 <= slot_index <= 10:
                signature = "UAAJXZ"
            elif slot_index == 7:
                signature = "UAAPAXJ@Z"
            elif slot_index == 11:
                signature = "UAAXJJJJ@Z"
            elif 12 <= slot_index <= 18:
                signature = "UAA_NXZ"
            elif slot_index == 19:
                signature = "UAAMXZ"
            else:
                signature = "UAAXJ@Z"
            expected_symbols.append(
                f"?v{slot_index}@{class_name}@@{signature}")
        if [slot.get("provider") for slot in slots] != expected_symbols:
            raise RuntimeError("particle provider symbol mapping changed")
        particle_provider_symbols.extend(expected_symbols)
        particle_methods.extend(methods)
        actual_particle_objects.append((
            entry.get("group"), int(entry.get("object_address", "0"), 0),
            int(entry.get("table_address", "0"), 0), class_name, methods))
    if (particle_providers.get("version") != 1 or
            particle_providers.get("object_size") != 12 or
            actual_particle_objects != expected_particle_objects):
        raise RuntimeError("particle provider object/table inventory changed")
    if len(particle_methods) != 42 or len(set(particle_methods)) != 27:
        raise RuntimeError("particle provider method coverage changed")
    particle_unique = set(particle_methods)
    if particle_unique - functions.keys():
        raise RuntimeError("particle provider method is absent from functions.csv")
    if any(functions[method]["source"] != "src/unknown_224240.cpp"
           for method in particle_unique):
        raise RuntimeError("particle provider source coverage changed")

    primary_path = args.root / "port/runtime/havok_primary_listener_vtables.json"
    primary = json.loads(primary_path.read_text())
    expected_primary_tables = [
        ("embedded_reference_callback", 0x1c, 0x0045A738,
         "h2_native_havok_primary_listener_embedded_table", [0x000A9EF0, 0x00277820]),
        ("contact_events", 0x04, 0x0045A74C,
         "h2_native_havok_primary_listener_contact_table",
         [0x00277A00, 0x00278030, 0x00278100]),
        ("entity_listener", 0x00, 0x0045A758,
         "h2_native_havok_primary_listener_entity_table",
         [0x00277890, 0x002778E0, 0x00277990, 0x00278200,
          0x002789F0, 0x00278A70, 0x00278B30, 0x00278AE0,
          0x002789B0]),
    ]
    actual_primary_tables = [
        (entry.get("name"), int(entry.get("object_offset", "-1"), 0),
         int(entry.get("retail_address", "0"), 0), entry.get("native_symbol"),
         [int(method, 0) for method in entry.get("methods", [])])
        for entry in primary.get("tables", [])
    ]
    if (primary.get("version") != 2 or
            int(primary.get("object_size", "0"), 0) != 0x28 or
            actual_primary_tables != expected_primary_tables):
        raise RuntimeError("primary Havok listener table inventory changed")
    primary_methods = [method for table in actual_primary_tables for method in table[4]]
    if len(primary_methods) != 14 or len(set(primary_methods)) != 14:
        raise RuntimeError("primary Havok listener method coverage changed")
    primary_unique = set(primary_methods)
    if primary_unique - functions.keys():
        raise RuntimeError("primary Havok listener method is absent from functions.csv")
    primary_source_owned = {
        method for method in primary_unique if functions[method]["source"]
    }
    if primary_source_owned != primary_unique:
        raise RuntimeError("primary Havok listener source coverage changed")
    excluded_primary = primary.get("excluded_adjacent_tables")
    if ([int(entry.get("retail_address", "0"), 0) for entry in excluded_primary]
            != [0x0045A740, 0x0045A77C, 0x0045A78C]):
        raise RuntimeError("primary Havok listener table boundaries changed")

    primary_provider_path = args.root / "port/runtime/havok_primary_listener_native_providers.json"
    primary_provider_manifest = json.loads(primary_provider_path.read_text())
    expected_primary_providers = {
        "0x000a9ef0": "h2_native_primary_listener_embedded_delete",
        "0x00277820": "?test@c_callback_277820@@UAA?AVhkBool@@PAX0@Z",
        "0x00277a00": "?event0@c_interface_277890@@UAAXPAX@Z",
        "0x00278030": "?event1@c_interface_277890@@UAAXPAX@Z",
        "0x00278100": "?event2@c_interface_277890@@UAAXPAX@Z",
        "0x00277890": "h2_native_primary_listener_delete",
        "0x002778e0": "?slot1@c_interface_277890@@UAAXPAVhkEntity@@@Z",
        "0x00277990": "?slot2@c_interface_277890@@UAAXPAVhkEntity@@@Z",
        "0x00278200": "?test@c_interface_277890@@UAA_NPAUs_278a70_body@@0@Z",
        "0x002789f0": "?function_2789f0@c_2789f0@@UAAXPAUs_2789f1@@@Z",
        "0x00278a70": "?slot2@c_callback_2789b0@@UAAXPAUs_278a70_event@@@Z",
        "0x00278b30": "h2_native_primary_listener_callback_adjusted_delete",
        "0x00278ae0": "?slot4@c_callback_2789b0@@UAAXPAX@Z",
        "0x002789b0": "h2_native_primary_listener_callback_delete",
    }
    if (primary_provider_manifest.get("version") != 1 or
            primary_provider_manifest.get("providers") != expected_primary_providers):
        raise RuntimeError("primary Havok listener provider map changed")
    primary_table_defined = subprocess.check_output(
        [args.llvm_nm, "--defined-only", "--format=posix",
         args.primary_listener_table_object], text=True)
    for table in expected_primary_tables:
        if table[3] not in primary_table_defined:
            raise RuntimeError(f"missing native primary listener table: {table[3]}")
    if "h2_native_havok_primary_listener_vtables" not in primary_table_defined:
        raise RuntimeError("missing native primary listener table lookup")
    primary_table_undefined = {
        line.strip().split()[-1]
        for line in subprocess.check_output(
            [args.llvm_nm, "--undefined-only", args.primary_listener_table_object],
            text=True).splitlines() if line.strip()
    }
    if primary_table_undefined != set(expected_primary_providers.values()):
        raise RuntimeError("native primary listener table providers changed")
    primary_table_relocations = subprocess.check_output(
        [args.llvm_readelf, "-r", args.primary_listener_table_object], text=True)
    if primary_table_relocations.count("R_AARCH64_ABS32") != 17:
        raise RuntimeError("native primary listener table relocations changed")

    particle_table_defined = subprocess.check_output(
        [args.llvm_nm, "--defined-only", "--format=posix",
         args.particle_provider_table_object], text=True)
    particle_labels = {
        "h2_native_effect_particle_provider_prt3_table",
        "h2_native_effect_particle_provider_prtm_table",
        "h2_native_effect_particle_provider_prt3_object",
        "h2_native_effect_particle_provider_prtm_object",
    }
    if any(label not in particle_table_defined for label in particle_labels):
        raise RuntimeError("native particle provider table/object is missing")
    particle_table_undefined = {
        line.strip().split()[-1]
        for line in subprocess.check_output(
            [args.llvm_nm, "--undefined-only", args.particle_provider_table_object],
            text=True).splitlines() if line.strip()
    }
    if particle_table_undefined != set(particle_provider_symbols):
        raise RuntimeError("native particle provider table methods changed")
    particle_table_relocations = subprocess.check_output(
        [args.llvm_readelf, "-r", args.particle_provider_table_object], text=True)
    if particle_table_relocations.count("R_AARCH64_ABS32") != 44:
        raise RuntimeError("native particle provider relocations changed")
    particle_unbound_undefined = {
        line.strip().split()[-1]
        for line in subprocess.check_output(
            [args.llvm_nm, "--undefined-only", args.particle_provider_unbound_object],
            text=True).splitlines() if line.strip()
    }
    expected_particle_unbound = {
        "?function_2243c0@@YAXPAUs_interface_definition@@JJJJJ@Z",
        "?function_224db0@@YAXPAUs_interface_definition@@JJJJ@Z",
        "?g_4e3b44@@3PAUs_tag_instance@@A",
    }
    if particle_unbound_undefined != expected_particle_unbound:
        raise RuntimeError("unbound particle provider cluster frontier changed")
    particle_cluster_undefined = {
        line.strip().split()[-1]
        for line in subprocess.check_output(
            [args.llvm_nm, "--undefined-only", args.particle_provider_cluster_object],
            text=True).splitlines() if line.strip()
    }
    if particle_cluster_undefined != expected_particle_unbound - {
            "?g_4e3b44@@3PAUs_tag_instance@@A"}:
        raise RuntimeError("bound particle provider cluster frontier changed")
    particle_cluster_defined = subprocess.check_output(
        [args.llvm_nm, "--defined-only", "--format=posix",
         args.particle_provider_cluster_object], text=True)
    if (any(label not in particle_cluster_defined for label in particle_labels) or
            any(symbol not in particle_cluster_defined
                for symbol in particle_provider_symbols)):
        raise RuntimeError("particle provider cluster definitions changed")

    production_defined_for_listener = subprocess.check_output(
        [args.llvm_nm, "--defined-only", "--format=posix", args.production_object],
        text=True)
    if ("h2_native_havok_primary_listener_vtables" in production_defined_for_listener or
            any(table[3] in production_defined_for_listener for table in expected_primary_tables)):
        raise RuntimeError("dormant primary listener tables entered production")
    if any(label in production_defined_for_listener for label in particle_labels):
        raise RuntimeError("dormant particle provider tables entered production")
    if "?function_277b80@@YAJPAX_NMPAVc_1cf520@@FPAUs_277ec0@@3PAVhkEntity@@4PAPAVc_277b80@@@Z" in production_defined_for_listener:
        raise RuntimeError("dormant contact-processing candidate entered production")
    if ("?impact_new@@YAJPBUs_impact_data@@J@Z" in production_defined_for_listener or
            "?function_2266a0@@YAXJ@Z" in production_defined_for_listener or
            "?function_226a60@@YAXJ@Z" in production_defined_for_listener or
            "?function_189650@@YAJPBUs_sound_position@@PBUs_sound_label_play@@@Z" in production_defined_for_listener or
            "?effect_new_from_parameters@@YAJPAUs_effect_parameters@@@Z" in production_defined_for_listener or
            "?function_177260@@YAXJ_N@Z" in production_defined_for_listener or
            "?function_3ebd0@@YA_NPBTvector3f@@PBUtransform4x3f@@PAU2@J@Z" in production_defined_for_listener or
            "?function_c0350@@YAXJJJPBTvector3f@@0PBTpoint3f@@M@Z" in production_defined_for_listener or
            "?function_175610@@YAXPAUs_particle_value_cache@@PAUs_particle_system_datum@@PAUs_particle_location_datum@@P6AXPAUs_particle_175610@@0PBTpoint3f@@PBTvector3f@@MPBUcolor4f@@PAX@Z7@Z" in production_defined_for_listener or
            "?function_2243c0@@YAXPAUs_interface_definition@@JJJJJ@Z" in production_defined_for_listener or
            "?function_224550@@YAXPBUs_draw_sprite_seed@@PAUs_draw_value_cache@@PATpoint3f@@2MPBUcolor4f@@PAUs_draw_sprite_context@@@Z" in production_defined_for_listener or
            "?function_1d05d0@@YAXPBVhkEntity@@0PAUs_havok_component@@PBF@Z" in production_defined_for_listener or
            "?function_1d0400@@YAJPAUs_havok_component@@FRBXJJJDPAVhkEntity@@2MMFF@Z" in production_defined_for_listener):
        raise RuntimeError("dormant impact lifetime candidate entered production")
    expected_primary_cluster_imports = {
        "?function_1d05d0@@YAXPBVhkEntity@@0PAUs_havok_component@@PBF@Z",
        "?function_226a60@@YAXJ@Z",
        "?function_277b80@@YAJPAX_NMPAVc_1cf520@@FPAUs_277ec0@@3PAVhkEntity@@4PAPAVc_277b80@@@Z",
        "?cache_file_globals@@3Us_cache_file_globals@@A",
        "?function_2d91d0@@YAXPAXJ@Z",
        "?function_a7670@@YA_NJ@Z",
        "?function_b8540@@YAXJ@Z",
        "?g_480118@@3PAVc_allocator@@A",
        "?g_4e0300@@3PAUs_record_pool@@A",
        "?g_4e3b44@@3PAUs_tag_instance@@A",
        "?g_510c54@@3PAUs_game_time_globals@@A",
        "?g_51e9b8@@3PAUs_record_pool@@A",
        "?g_4687a4@@3PATvector3f@@A",
        "h2_guest_memcpy",
        "sqrtf",
    }
    primary_cluster_undefined = {
        line.strip().split()[-1]
        for line in subprocess.check_output(
            [args.llvm_nm, "--undefined-only", args.primary_listener_provider_cluster_object],
            text=True).splitlines() if line.strip()
    }
    if primary_cluster_undefined != expected_primary_cluster_imports:
        raise RuntimeError("primary Havok listener provider closure changed")

    expected_contact_candidate_imports = {
        "?function_1cf520@@YAXPAUs_1cf520@@PAVc_1cf520@@_N2JJ@Z",
        "?function_1d0400@@YAJPAUs_havok_component@@FRBXJJJDPAVhkEntity@@2MMFF@Z",
        "?g_4687a4@@3PATvector3f@@A",
        "?g_4e0300@@3PAUs_record_pool@@A",
        "?g_4e0348@@3PAUs_match_globals@@A",
        "?g_4e034c@@3PAUs_tag_header_globals@@A",
        "?g_4e3b44@@3PAUs_tag_instance@@A",
        "?g_502138@@3JA",
        "?g_50213c@@3PAJA",
        "?g_51e9b8@@3PAUs_record_pool@@A",
        "?g_51ebfc@@3PAUs_record_pool@@A",
        "?g_51ec00@@3PAUs_record_pool@@A",
        "?g_54e898@@3FA",
        "?havok_cd_body_shape_key_get@@YAJPBUs_havok_cd_body@@@Z",
        "?function_2266a0@@YAXJ@Z",
        "?havok_entity_component_index_get@@YAJPBVhkEntity@@@Z",
        "?havok_entity_property_2002_get@@YAJPBVhkEntity@@@Z",
        "?record_pool_allocate@@YAJPAUs_record_pool@@@Z",
        "?g_4e8c24@@3PAUs_record_pool@@A",
        "?g_4e9bd4@@3PAUs_player_4e9bd4@@A",
        "?impact_new@@YAJPBUs_impact_data@@J@Z",
        "h2_guest_memcpy",
        "sqrt",
    }
    contact_candidate_undefined = {
        line.strip().split()[-1]
        for line in subprocess.check_output(
            [args.llvm_nm, "--undefined-only", args.contact_processing_candidate_object],
            text=True).splitlines() if line.strip()
    }
    if contact_candidate_undefined != expected_contact_candidate_imports:
        raise RuntimeError("Havok contact-processing candidate closure changed")
    contact_candidate_data = subprocess.check_output(
        [args.llvm_readelf, "-x", ".data", args.contact_processing_candidate_object],
        text=True)
    if "807b063f" not in contact_candidate_data:
        raise RuntimeError("impact match tolerance no longer equals binary32 cos(45.0)")

    primary_cluster_defined = subprocess.check_output(
        [args.llvm_nm, "--defined-only", "--format=posix",
         args.primary_listener_provider_cluster_object], text=True)
    for symbol in (set(expected_primary_providers.values()) |
                   {table[3] for table in expected_primary_tables} |
                   {"h2_native_havok_primary_listener_vtables",
                    "??0c_interface_277890@@QAA@XZ",
                    "?function_1d0770@@YAXPAUs_1d0770@@PAUs_havok_component@@JJJJ@Z",
                    "?function_1d8ec0@s_1d8e40@@QAAXABUhkVector4@@0@Z",
                    "?isActive@hkRigidBody@@QBA?AVhkBool@@XZ",
                    "?activate@hkRigidBody@@QAAXXZ",
                    "?removeProperty@hkEntity@@QAA?AVhkPropertyValue@@K@Z",
                    "?addProperty@hkEntity@@QAAXKVhkPropertyValue@@@Z",
                    "?havok_cd_body_shape_key_get@@YAJPBUs_havok_cd_body@@@Z",
                    "?havok_entity_component_index_get@@YAJPBVhkEntity@@@Z",
                    "?havok_entity_property_2002_get@@YAJPBVhkEntity@@@Z"}):
        if symbol not in primary_cluster_defined:
            raise RuntimeError(f"primary Havok listener cluster lacks {symbol}")

    contact_candidate_defined = subprocess.check_output(
        [args.llvm_nm, "--defined-only", "--format=posix",
         args.contact_processing_candidate_object], text=True)
    for symbol in {
            "?function_277b80@@YAJPAX_NMPAVc_1cf520@@FPAUs_277ec0@@3PAVhkEntity@@4PAPAVc_277b80@@@Z",
            "?function_277ec0@@YAXPAM0PAF1PAUs_277ec0@@2JJ@Z",
            "?function_182b90@@YAXPAVc_material_shape@@PBVhkEntity@@PAM2PAF@Z",
            "?function_1ee410@@YAXPBUs_lookup_source@@PAF@Z",
            "?function_ba1d0@@YAXJPATvector3f@@0@Z",
            "?impact_data_set@@YAXPAUs_impact_data@@_NJJVc_type_47f957@@JJ2PBTpoint3f@@PBTvector3f@@JPBUs_physics_model_shape_key@@@Z",
            "?havok_component_impact_add@@YAXPAUs_havok_component@@J@Z",
            "?impact_components_valid@@YA_NJJ@Z",
            "?impact_matches_data@@YA_NPAUs_impact@@PBUs_impact_data@@_N@Z",
            "?havok_component_impact_find@@YAJPAUs_havok_component@@PBUs_havok_impact_contact@@_N@Z",
            "?havok_component_impact_make_room@@YA_NJPAUs_havok_component@@M@Z",
            "?impacts_last_sorted@@YAJXZ",
            "?impact_distance_squared_to_nearest_player@@YAMPBTpoint3f@@J@Z",
            "?g_55c2d0@@3MA",
            "?havok_component_rigid_body_linear_velocity_get@@YAXJPAUs_havok_component@@PATvector3f@@@Z",
            "?havok_component_rigid_body_angular_velocity_get@@YAXJPAUs_havok_component@@PATvector3f@@@Z"}:
        if symbol not in contact_candidate_defined:
            raise RuntimeError(f"contact-processing candidate lacks {symbol}")

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
        "g_4687a4_owned_zero_vector",
        "g_47f048_owned_havok_listener",
        "g_480118_owned_havok_allocator",
        "g_461f08_owned_havok_state_flag",
        "g_51e9cc_owned_scenery_index_table",
        "g_51e9d0_owned_shape_owner",
        "g_468630_owned_object_type_definitions",
        "g_4e0300_owned_object_pool",
        "g_4e3b44_owned_tag_instances",
        "g_510c54_owned_game_time_globals",
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
        "native_havok_contact_transition_tables_and_callback_owned",
        "native_havok_listener_cache_attachment_owned",
        "native_havok_entity_property_arrays_owned",
        "native_havok_contact_shape_tree_owned",
        "native_havok_material_response_state_owned",
        "g_51ebfc_owned_impact_pool",
        "g_51ec00_owned_component_impact_lists",
        "g_502138_owned_impact_sort_state",
        "g_4e8c24_owned_players_and_local_state",
        "native_havok_impact_response_state_owned",
        "native_havok_effect_pools_owned",
        "native_light_record_pool_owned",
        "native_effect_runtime_state_owned",
        "native_effect_placement_state_owned",
        "native_first_person_state_owned",
        "native_particle_system_pool_owned",
        "native_particle_emitter_and_particle_pools_owned",
        "native_particle_interface_state_owned",
        "native_particle_frame_state_owned",
        "native_particle_sprite_render_state_owned",
        "native_impact_looping_sound_pool_owned",
        "native_havok_motion_basis_state_owned",
        "native_global_color_table_initialized",
        "native_tag_function_tables_initialized",
        "native_object_function_handler_tables_owned",
        "native_object_interpolated_values_owned",
        "native_ai_effect_event_queue_owned",
        "native_havok_rigid_body_island_state_owned",
        "native_havok_motion_point_impulse_owned",
        "native_havok_world_activation_owned",
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

    listener_additional_imports = {
        "?function_1d05d0@@YAXPBVhkEntity@@0PAUs_havok_component@@PBF@Z",
        "?function_226a60@@YAXJ@Z",
        "?function_277b80@@YAJPAX_NMPAVc_1cf520@@FPAUs_277ec0@@3PAVhkEntity@@4PAPAVc_277b80@@@Z",
    }
    expected_listener_production = (
        frontier_symbols - {"??0c_interface_277890@@QAA@XZ"}
    ) | listener_additional_imports
    with tempfile.TemporaryDirectory(prefix="h2-primary-listener-score-") as temporary:
        scored = Path(temporary) / "primary-listener-production.o"
        subprocess.run([
            args.lld, "-r", "--allow-multiple-definition", "-o", scored,
            args.primary_listener_provider_cluster_object,
            args.provider_cluster_object,
            args.production_object,
        ], check=True)
        actual_listener_production = {
            line.strip().split()[-1]
            for line in subprocess.check_output(
                [args.llvm_nm, "--undefined-only", scored], text=True
            ).splitlines() if line.strip()
        }
    if (actual_listener_production != expected_listener_production or
            len(actual_listener_production) != 18):
        raise RuntimeError("complete primary Havok listener production score changed")

    contact_processing_symbol = (
        "?function_277b80@@YAJPAX_NMPAVc_1cf520@@FPAUs_277ec0@@3PAVhkEntity@@4PAPAVc_277b80@@@Z"
    )
    contact_processing_new_imports = {
        "?function_1d0400@@YAJPAUs_havok_component@@FRBXJJJDPAVhkEntity@@2MMFF@Z",
        "?function_2266a0@@YAXJ@Z",
        "?impact_new@@YAJPBUs_impact_data@@J@Z",
    }
    expected_contact_processing_score = (
        expected_listener_production - {contact_processing_symbol}
    ) | contact_processing_new_imports
    with tempfile.TemporaryDirectory(prefix="h2-contact-processing-score-") as temporary:
        scored = Path(temporary) / "contact-processing-production.o"
        subprocess.run([
            args.lld, "-r", "--allow-multiple-definition", "-o", scored,
            args.contact_processing_candidate_object,
            args.primary_listener_provider_cluster_object,
            args.provider_cluster_object,
            args.production_object,
        ], check=True)
        actual_contact_processing_score = {
            line.strip().split()[-1]
            for line in subprocess.check_output(
                [args.llvm_nm, "--undefined-only", scored], text=True
            ).splitlines() if line.strip()
        }
    if (actual_contact_processing_score != expected_contact_processing_score or
            len(actual_contact_processing_score) != 20):
        raise RuntimeError("Havok contact-processing candidate score changed")

    contact_frontier_path = (
        args.root / "port/runtime/havok_contact_processing_remaining_frontier.json"
    )
    contact_frontier = json.loads(contact_frontier_path.read_text())
    if contact_frontier.get("version") != 1:
        raise RuntimeError("unsupported Havok contact-processing frontier manifest")
    contact_frontier_entries = contact_frontier.get("imports")
    if not isinstance(contact_frontier_entries, list):
        raise RuntimeError("Havok contact-processing frontier imports must be a list")
    contact_frontier_symbols = [entry.get("symbol") for entry in contact_frontier_entries]
    if len(contact_frontier_symbols) != len(set(contact_frontier_symbols)):
        raise RuntimeError("duplicate Havok contact-processing frontier symbol")
    if set(contact_frontier_symbols) != contact_processing_new_imports:
        raise RuntimeError("Havok contact-processing frontier manifest is stale")
    for entry in contact_frontier_entries:
        if not entry.get("subsystem") or not entry.get("blocker"):
            raise RuntimeError("incomplete Havok contact-processing frontier entry")

    impact_allocation_symbol = "?impact_new@@YAJPBUs_impact_data@@J@Z"
    impact_eviction_symbol = "?function_2266a0@@YAXJ@Z"
    impact_contact_creation_symbol = "?function_1d0400@@YAJPAUs_havok_component@@FRBXJJJDPAVhkEntity@@2MMFF@Z"
    impact_event_creation_symbol = "?function_226a60@@YAXJ@Z"
    impact_contact_removal_symbol = "?function_1d05d0@@YAXPBVhkEntity@@0PAUs_havok_component@@PBF@Z"
    impact_allocation_new_imports = {
        "?effect_new_from_parameters@@YAJPAUs_effect_parameters@@@Z",
        "?function_177260@@YAXJ_N@Z",
    }
    expected_impact_allocation_score = (
        expected_contact_processing_score - {
            impact_allocation_symbol, impact_eviction_symbol,
            impact_contact_creation_symbol, impact_event_creation_symbol,
            impact_contact_removal_symbol
        }
    ) | impact_allocation_new_imports
    with tempfile.TemporaryDirectory(prefix="h2-impact-allocation-score-") as temporary:
        scored = Path(temporary) / "impact-allocation-production.o"
        subprocess.run([
            args.lld, "-r", "--allow-multiple-definition", "-o", scored,
            args.impact_allocation_candidate_object,
            args.primary_listener_provider_cluster_object,
            args.provider_cluster_object,
            args.production_object,
        ], check=True)
        actual_impact_allocation_score = {
            line.strip().split()[-1]
            for line in subprocess.check_output(
                [args.llvm_nm, "--undefined-only", scored], text=True
            ).splitlines() if line.strip()
        }
    if (actual_impact_allocation_score != expected_impact_allocation_score or
            len(actual_impact_allocation_score) != 17):
        raise RuntimeError("Havok impact-allocation candidate score changed")
    impact_undefined = {
        line.strip().split()[-1]
        for line in subprocess.check_output(
            [args.llvm_nm, "--undefined-only", args.impact_allocation_candidate_object],
            text=True).splitlines() if line.strip()
    }
    if len(impact_undefined) != 49:
        raise RuntimeError("Havok impact-allocation raw closure changed")
    impact_defined = subprocess.check_output(
        [args.llvm_nm, "--defined-only", args.impact_allocation_candidate_object], text=True
    )
    if (impact_allocation_symbol not in impact_defined or
            "?function_146650@@YAJXZ" not in impact_defined or
            "?function_1201a0@@YAMPATvector3f@@PBT1@@Z" not in impact_defined or
            "?vector3d_compress@@YAKPBTvector3f@@@Z" not in impact_defined or
            "?havok_component_contact_properties_get@@YAXPBUs_havok_component@@JPAJ1@Z" not in impact_defined or
            "?havok_component_rigid_body_matrix_get@@YAXJPAUs_havok_component@@PAUtransform4x3f@@@Z" not in impact_defined or
            "?function_11c120@@YA_NPBUs_location@@PBTpoint3f@@PAF@Z" not in impact_defined or
            "?function_17b030@@YA_NJPBTvector3f@@MMPBUtransform4x3f@@PBM@Z" not in impact_defined or
            "?function_172750@@YA_NJPBTpoint3f@@M@Z" not in impact_defined or
            "?function_1883e0@@YAXJ_NPBTpoint3f@@FJJJPAJ22222@Z" not in impact_defined or
            any(symbol not in impact_defined for symbol in (
                "?function_18a5e0@@YAPAGJ@Z", "?function_18a6c0@@YAJJJ@Z",
                "?function_18a720@@YAXJ_N@Z", "?function_2266a0@@YAXJ@Z",
                "?function_1d0400@@YAJPAUs_havok_component@@FRBXJJJDPAVhkEntity@@2MMFF@Z",
                "?function_181db0@@YA_NJPATvector3f@@@Z",
                "?function_355e0@@YA_NJJPAM0@Z",
                "?function_226a60@@YAXJ@Z",
                "?function_1d05d0@@YAXPBVhkEntity@@0PAUs_havok_component@@PBF@Z",
                "?function_189650@@YAJPBUs_sound_position@@PBUs_sound_label_play@@@Z",
                "?havok_component_any_rigid_body_active@@YA_NPAUs_havok_component@@@Z",
                "?impact_release@@YAXJPAUs_impact@@@Z"))):
        raise RuntimeError("Havok impact-allocation candidate lost a required provider")
    if "?function_13b390@@YAMPBXMM@Z" in impact_defined:
        raise RuntimeError("general tag-function evaluator entered endpoint-only motion closure")
    impact_frontier = json.loads((
        args.root / "port/runtime/havok_impact_allocation_remaining_frontier.json"
    ).read_text())
    if impact_frontier.get("version") != 1:
        raise RuntimeError("unsupported Havok impact-allocation frontier manifest")
    impact_entries = impact_frontier.get("imports")
    if not isinstance(impact_entries, list):
        raise RuntimeError("Havok impact-allocation frontier imports must be a list")
    impact_symbols = [entry.get("symbol") for entry in impact_entries]
    if len(impact_symbols) != len(set(impact_symbols)):
        raise RuntimeError("duplicate Havok impact-allocation frontier symbol")
    if set(impact_symbols) != impact_allocation_new_imports:
        raise RuntimeError("Havok impact-allocation frontier manifest is stale")
    for entry in impact_entries:
        if not entry.get("subsystem") or not entry.get("blocker"):
            raise RuntimeError("incomplete Havok impact-allocation frontier entry")

    effect_frontier = json.loads((
        args.root / "port/runtime/havok_effect_lifecycle_remaining_frontier.json"
    ).read_text())
    if effect_frontier.get("version") != 1:
        raise RuntimeError("unsupported Havok effect-lifecycle frontier manifest")
    effect_entries = effect_frontier.get("imports")
    if not isinstance(effect_entries, list) or len(effect_entries) != 18:
        raise RuntimeError("Havok effect-lifecycle frontier inventory changed")
    effect_symbols = [entry.get("symbol") for entry in effect_entries]
    if len(effect_symbols) != len(set(effect_symbols)):
        raise RuntimeError("duplicate Havok effect-lifecycle frontier symbol")
    for entry in effect_entries:
        if (not isinstance(entry.get("subsystem"), str) or not entry["subsystem"] or
                not isinstance(entry.get("blocker"), str) or not entry["blocker"] or
                "stub" in entry["blocker"].lower() or
                "placeholder" in entry["blocker"].lower() or
                entry["blocker"] == "Requires complete recovered effect creation/deletion ownership for this transitive provider."):
            raise RuntimeError("incomplete Havok effect-lifecycle frontier entry")
    ai_insertion = json.loads((
        args.root / "port/runtime/ai_effect_event_insertion_frontier.json"
    ).read_text())
    if (ai_insertion.get("retail_function") != "0x002902b0" or
            ai_insertion.get("size") != 1298 or
            ai_insertion.get("queue_slots") != 5 or
            ai_insertion.get("execution_gate") != "native_ai_effect_event_queue_owned"):
        raise RuntimeError("AI effect-event insertion identity changed")
    ai_calls = ai_insertion.get("direct_calls")
    expected_ai_calls = {
        "0x00030bf0", "0x000a7b30", "0x000b7930", "0x000b7b40",
        "0x000b8540", "0x000b9c60", "0x000b9dd0", "0x000b9fc0",
        "0x000cbd50", "0x000e6900", "0x0011dfb0", "0x001e0160",
        "0x001e02f0", "0x001e04a0", "0x001e06b0", "0x001e11b0",
        "0x001e4990", "0x001e4a50", "0x001f34b0", "0x00210850",
        "0x0028dab0", "0x0028e200", "0x002907d0", "0x002909b0",
    }
    if (not isinstance(ai_calls, list) or len(ai_calls) != 24 or
            {entry.get("address") for entry in ai_calls} != expected_ai_calls or
            any(entry.get("status") not in {"matched", "done", "todo"} for entry in ai_calls)):
        raise RuntimeError("AI effect-event direct call frontier changed")
    expected_ai_data = {
        "0x0045dbb8", "0x0045dbcc", "0x0045dbd8", "0x0045dc50",
        "0x0045dc68", "0x0045dd30", "0x004e0300", "0x004e0350",
        "0x004e3b44", "0x004e7408", "0x004f55f0", "0x005044c4",
        "0x005044c8", "0x00510c54",
    }
    if set(ai_insertion.get("fixed_data_references", [])) != expected_ai_data:
        raise RuntimeError("AI effect-event fixed-data frontier changed")

    mesh_update = json.loads((
        args.root / "port/runtime/particle_mesh_update_frontier.json"
    ).read_text())
    expected_mesh_calls = {
        "0x0001bbf0", "0x0001bd50", "0x0001c590", "0x0001c620",
        "0x0001c710", "0x0001cdd0", "0x0002c3b0", "0x0012de70",
        "0x00137bd0", "0x00173ba0", "0x00175610",
    }
    expected_mesh_expansion = {
        "?function_12de70@@YA_NPAUs_geometry_block_info@@K@Z",
        "?function_1bbf0@@YAXJJJJJM@Z",
        "?function_1c590@@YAXPAUs_shader_cache@@JJ@Z",
        "?function_1c620@@YAXPAUs_shader_cache@@JJJPBE@Z",
        "?function_1c710@@YAXPAX@Z",
        "?function_1cdd0@@YAXPBME@Z",
        "?function_224fd0@@YAXPBUs_draw_mesh_seed@@PAUs_draw_value_cache@@PBTpoint3f@@PBTvector3f@@MPBUcolor4f@@PBUs_draw_mesh_context@@@Z",
        "?g_51f0f0@@3PAEA",
        "?g_55ed60@@3Us_interface_function_context@@A",
        "g_43f408",
    }
    if (mesh_update.get("version") != 1 or
            mesh_update.get("retail_function") != "0x00224db0" or
            mesh_update.get("size") != 544 or
            mesh_update.get("execution_gate") != "native_particle_mesh_render_state_owned" or
            set(mesh_update.get("direct_calls", [])) != expected_mesh_calls or
            mesh_update.get("production_score_before") != 33 or
            mesh_update.get("production_score_after") != 42 or
            set(mesh_update.get("newly_exposed", [])) != expected_mesh_expansion or
            not mesh_update.get("blocker")):
        raise RuntimeError("particle mesh update frontier changed")

    mesh_callback = json.loads((
        args.root / "port/runtime/particle_mesh_callback_frontier.json"
    ).read_text())
    expected_mesh_callback_calls = {
        "0x0001cf50", "0x00030bf0", "0x0011d000", "0x00142eb0",
        "0x00173ba0", "0x00246cd0", "0x003faae0", "0x003fafa0",
        "0x003fb2d0",
    }
    expected_mesh_callback_expansion = {
        "?function_1cf50@@YAXXZ", "?g_4687ac@@3PATvector3f@@A",
        "D3DDevice_DrawIndexedVertices",
    }
    if (mesh_callback.get("version") != 1 or
            mesh_callback.get("retail_function") != "0x00224fd0" or
            mesh_callback.get("size") != 1836 or
            mesh_callback.get("execution_gate") != "native_particle_mesh_render_state_owned" or
            set(mesh_callback.get("direct_calls", [])) != expected_mesh_callback_calls or
            mesh_callback.get("production_score_before") != 33 or
            mesh_callback.get("production_score_after") != 36 or
            mesh_callback.get("mesh_update_score_before") != 42 or
            mesh_callback.get("mesh_update_score_after") != 44 or
            set(mesh_callback.get("newly_exposed", [])) != expected_mesh_callback_expansion or
            not mesh_callback.get("blocker")):
        raise RuntimeError("particle mesh callback frontier changed")

    texture_state = json.loads((
        args.root / "port/runtime/particle_texture_state_frontier.json"
    ).read_text())
    expected_texture_functions = {
        ("0x0001cf50", 48, "?function_1cf50@@YAXXZ", True),
        ("0x0001cf80", 40, "?function_1cf80@@YAXXZ", False),
    }
    texture_functions = {
        (entry.get("address"), entry.get("size"), entry.get("symbol"),
         entry.get("changed_only"))
        for entry in texture_state.get("retail_functions", [])
    }
    if (texture_state.get("version") != 1 or
            texture_functions != expected_texture_functions or
            texture_state.get("execution_gate") != "native_particle_texture_state_owned" or
            texture_state.get("stage_count") != 4 or
            texture_state.get("direct_calls") != ["0x003f8320"] or
            set(texture_state.get("newly_exposed", [])) != {
                "?g_51f3c8@@3PAY03PAXA", "D3DDevice_SetTexture"} or
            texture_state.get("sprite_setup_score_before") != 46 or
            texture_state.get("sprite_setup_score_after") != 47 or
            texture_state.get("mesh_callback_score_before") != 36 or
            texture_state.get("mesh_callback_score_after") != 37 or
            texture_state.get("mesh_update_and_callback_score_before") != 44 or
            texture_state.get("mesh_update_and_callback_score_after") != 45 or
            not texture_state.get("blocker")):
        raise RuntimeError("particle texture state frontier changed")

    shader_state = json.loads((
        args.root / "port/runtime/particle_shader_cache_state_frontier.json"
    ).read_text())
    expected_shader_functions = {
        ("0x0001c4a0", 227, "?function_1c4a0@@YAXPAUs_shader_cache@@@Z"),
        ("0x0001c6b0", 87, "?function_1c6b0@@YAXPAX@Z"),
        ("0x0001ccb0", 55, "?function_1ccb0@@YAXJ@Z"),
    }
    shader_functions = {
        (entry.get("address"), entry.get("size"), entry.get("symbol"))
        for entry in shader_state.get("retail_functions", [])
    }
    descriptor_table = shader_state.get("owned_descriptor_table", {})
    cache_initializer = shader_state.get("owned_cache_initializer", {})
    if (shader_state.get("version") != 1 or
            shader_functions != expected_shader_functions or
            shader_state.get("execution_gate") != "native_particle_camera_and_shader_state_owned" or
            cache_initializer != {"size": 728, "binding_count": 8,
                                  "descriptor_count": 16, "stream_count": 16,
                                  "active_value": -1,
                                  "clears_current_descriptors": False} or
            descriptor_table != {"symbol": "g_43f408", "entries": 63,
                                 "bytes_per_entry": 21, "total_bytes": 1323} or
            shader_state.get("sprite_setup_score_before") != 46 or
            shader_state.get("sprite_setup_score_after") != 44 or
            set(shader_state.get("remaining_providers", [])) != {
                "?function_1c710@@YAXPAX@Z", "?g_51f0f0@@3PAEA"} or
            not shader_state.get("blocker")):
        raise RuntimeError("particle shader cache state frontier changed")

    shader_input = json.loads((
        args.root / "port/runtime/particle_shader_input_frontier.json"
    ).read_text())
    expected_shader_input_functions = {
        ("0x0001c330", 359, "?function_1c330@@YAXPAEPAUs_1c330_output@@@Z"),
        ("0x0001c710", 213, "?function_1c710@@YAXPAX@Z"),
        ("0x00035790", 188, "?function_35790@@YAJJ@Z"),
        ("0x00035850", 128, "?function_35850@@YAJJ@Z"),
    }
    shader_input_functions = {
        (entry.get("address"), entry.get("size"), entry.get("symbol"))
        for entry in shader_input.get("retail_functions", [])
    }
    if (shader_input.get("version") != 1 or
            shader_input_functions != expected_shader_input_functions or
            shader_input.get("execution_gate") != "native_particle_camera_and_shader_state_owned" or
            shader_input.get("sprite_setup_with_compact_state_score_before") != 44 or
            shader_input.get("sprite_setup_with_shader_input_score_after") != 45 or
            set(shader_input.get("newly_exposed", [])) != {
                "D3DDevice_SelectVertexShaderDirect",
                "D3DDevice_SetVertexShaderInputDirect"} or
            not shader_input.get("blocker")):
        raise RuntimeError("particle shader input frontier changed")

    shader_program = json.loads((
        args.root / "port/runtime/particle_shader_program_frontier.json"
    ).read_text())
    expected_shader_program_functions = {
        ("0x0001c2e0", 78, "?function_1c2e0@@YAPBKJJPAJ@Z"),
        ("0x0001c590", 144, "?function_1c590@@YAXPAUs_shader_cache@@JJ@Z"),
    }
    shader_program_functions = {
        (entry.get("address"), entry.get("size"), entry.get("symbol"))
        for entry in shader_program.get("retail_functions", [])
    }
    fallback_program = shader_program.get("owned_fallback_program", {})
    if (shader_program.get("version") != 1 or
            shader_program_functions != expected_shader_program_functions or
            shader_program.get("execution_gate") != "native_particle_camera_and_shader_state_owned" or
            fallback_program != {
                "program_symbol": "g_43f2b0", "program_words": 85,
                "table_symbol": "?g_4670ec@@3PAUs_shader_program@@A",
                "byte_count": 340} or
            shader_program.get("sprite_setup_with_shader_input_score_before") != 45 or
            shader_program.get("sprite_setup_with_shader_program_score_after") != 45 or
            shader_program.get("newly_exposed") != ["D3DDevice_LoadVertexShaderProgram"] or
            not shader_program.get("blocker")):
        raise RuntimeError("particle shader program frontier changed")

    shader_constants = json.loads((
        args.root / "port/runtime/particle_shader_constants_frontier.json"
    ).read_text())
    if (shader_constants.get("version") != 1 or
            shader_constants.get("retail_function") != "0x0023e460" or
            shader_constants.get("size") != 1562 or
            shader_constants.get("symbol") != "?function_23e460@@YAXXZ" or
            shader_constants.get("execution_gate") != "native_particle_camera_and_shader_state_owned" or
            shader_constants.get("constant_register") != 52 or
            shader_constants.get("constant_vectors") != 22 or
            shader_constants.get("ring_samples") != 16 or
            set(shader_constants.get("object_imports", [])) != {
                "?g_4856c4@@3PAMA", "?g_485af0@@3EA",
                "D3DDevice_SetVertexShaderConstant", "cos", "sin"} or
            set(shader_constants.get("newly_exposed", [])) != {
                "?g_4856c4@@3PAMA", "?g_485af0@@3EA"} or
            shader_constants.get("sprite_setup_with_shader_program_score_before") != 45 or
            shader_constants.get("sprite_setup_with_shader_constants_score_after") != 46 or
            not shader_constants.get("blocker")):
        raise RuntimeError("particle shader constants frontier changed")

    vertex_register = json.loads((
        args.root / "port/runtime/particle_vertex_register_state_frontier.json"
    ).read_text())
    if (vertex_register.get("version") != 1 or
            vertex_register.get("retail_function") != "0x0023ecd0" or
            vertex_register.get("size") != 81 or
            vertex_register.get("symbol") != "?function_23ecd0@@YAXXZ" or
            vertex_register.get("execution_gate") != "native_particle_vertex_register_state_owned" or
            vertex_register.get("initialized_registers") != list(range(1, 9)) or
            vertex_register.get("remaining_provider") != "?g_470a3c@@3PAJA" or
            vertex_register.get("sprite_setup_score_before") != 46 or
            vertex_register.get("sprite_setup_score_after") != 46 or
            vertex_register.get("sprite_setup_and_geometry_score_before") != 48 or
            vertex_register.get("sprite_setup_and_geometry_score_after") != 47 or
            vertex_register.get("sprite_setup_geometry_and_shader_state_score_after") != 45 or
            not vertex_register.get("blocker")):
        raise RuntimeError("particle vertex register state frontier changed")

    material_binding = json.loads((
        args.root / "port/runtime/particle_material_binding_frontier.json"
    ).read_text())
    expected_material_calls = {
        "0x00016f60", "0x00017000", "0x00017170", "0x00017420",
        "0x000174d0", "0x00018900", "0x000189a0", "0x00018a90",
        "0x00018d70", "0x00018e80", "0x00018ee0", "0x00019be0",
        "0x00019ca0", "0x0001a170", "0x0001ab50", "0x0001ae70",
        "0x0001b050", "0x0001b0a0", "0x0001ba00", "0x0001cf50",
        "0x00137bd0",
    }
    material_groups = material_binding.get("remaining_groups", {})
    grouped_material_calls = {
        call for calls in material_groups.values() for call in calls
    }
    if (material_binding.get("version") != 1 or
            material_binding.get("retail_function") != "0x00016b90" or
            material_binding.get("size") != 972 or
            material_binding.get("symbol") != "?function_16b90@@YAXPAEJJJJM@Z" or
            material_binding.get("execution_gate") != "native_particle_material_binding_state_owned" or
            set(material_binding.get("direct_calls", [])) != expected_material_calls or
            set(material_binding.get("already_owned_calls", [])) != {
                "0x00016f60", "0x00017000", "0x00017170", "0x00017420", "0x000174d0", "0x00018d70", "0x00018e80", "0x00018900", "0x00018a90", "0x00019be0", "0x00019ca0", "0x0001ae70", "0x0001b050", "0x0001b0a0", "0x0001ba00", "0x0001cf50", "0x00137bd0"} or
            grouped_material_calls != expected_material_calls - {
                "0x00016f60", "0x00017000", "0x00017170", "0x00017420", "0x000174d0", "0x00018d70", "0x00018e80", "0x00018900", "0x00018a90", "0x00019be0", "0x00019ca0", "0x0001ae70", "0x0001b050", "0x0001b0a0", "0x0001ba00", "0x0001cf50", "0x00137bd0"} or
            material_binding.get("owned_state_dependencies") != {
                "0x00016f60": ["?g_485a80@@3PAEA"],
                "0x00034060": [
                    "?g_4858c4@@3PAEA", "?g_485a2c@@3JA", "?g_485a64@@3JA",
                    "?g_485a68@@3JA", "?g_4ba014@@3KA"],
                "0x0001b0a0": [
                    "?g_4ba019@@3_NA", "?g_4e3b44@@3PAUs_tag_instance@@A"],
                "0x00018a90": [
                    "?g_485a60@@3JA", "?g_485a80@@3PAEA",
                    "?g_4e0344@@3PAUs_structure_bsp_globals@@A",
                    "?g_4e0348@@3PAUs_match_globals@@A",
                    "?g_4e3b44@@3PAUs_tag_instance@@A"],
                "0x000143c0": [
                    "?g_485a80@@3PAEA", "?g_485af4@@3PAJA", "?g_485b04@@3PAJA",
                    "?g_4e3b44@@3PAUs_tag_instance@@A", "?g_51f3c8@@3PAY03PAXA"],
                "0x00012310": ["?g_4e6488@@3JA"],
                "0x0012ccf0": [
                    "?g_468841@@3_NA", "?g_4e6454@@3PAUs_record_pool@@A",
                    "?g_4e6464@@3PAUs_physical_object@@A", "?g_4e647b@@3_NA",
                    "?g_4e647c@@3MA", "?g_4e6488@@3JA"],
                "0x0012c960": ["?g_4e6468@@3_NA"],
                "0x0012c820": [
                    "?g_450768@@3PAY0BI@JA", "?g_4e646c@@3JA"],
                "0x00137550": ["?g_4e3b44@@3PAUs_tag_instance@@A"],
                "0x00014390": [
                    "?g_485af4@@3PAJA", "?g_485b04@@3PAJA", "?g_51f3c8@@3PAY03PAXA"],
                "0x000144f0": [
                    "?g_485a80@@3PAEA", "?g_4e3b44@@3PAUs_tag_instance@@A"],
                "0x00025960": ["?g_4b4b58@@3PAUs_unknown_01dcc0@@A"],
                "0x0001dcc0": ["?g_4b4b58@@3PAUs_unknown_01dcc0@@A"],
                "0x0001dd60": ["?g_4b4b58@@3PAUs_unknown_01dcc0@@A"],
                "0x00014f60": [
                    "?g_485aa0@@3_JA", "?g_485af4@@3PAJA", "?g_485b04@@3PAJA",
                    "?g_509354@@3PAPAUD3DTexture@@A", "?g_509368@@3PAUD3DTexture@@A",
                    "?g_50936c@@3PAUD3DTexture@@A", "?g_509374@@3PAUD3DResource@@A",
                    "?g_50937c@@3PAUD3DResource@@A", "?g_50938c@@3PAUD3DTexture@@A",
                    "?g_51f3c8@@3PAY03PAUIDirect3DBaseTexture8@@A"],
                "0x00019530": [
                    "?g_485607@@3EA", "?g_4858b8@@3JA", "?g_4858c4@@3PAEA",
                    "?g_4858c8@@3FA", "?g_485a76@@3EA", "?g_485a77@@3EA",
                    "?g_485a78@@3PAJA", "?g_485a80@@3PAEA",
                    "?g_4b4b58@@3PAUs_unknown_01dcc0@@A",
                    "?g_4e3b44@@3PAUs_tag_instance@@A", "?g_4e6978@@3JA",
                    "?g_4e697c@@3JA", "?g_51f3c8@@3PAY03PAUIDirect3DBaseTexture8@@A",
                    "?g_55e6bd@@3EA"]} or
            material_binding.get("owned_zero_import_calls") != [
                "0x00017000", "0x00017170", "0x0001ba00", "0x0012ccb0",
                "0x000359e0", "0x0013e3a0", "0x0012cb10"] or
            material_binding.get("owned_immutable_data") != {
                "0x00467068": "8 exact dword channel masks retained with function_17170"} or
            material_binding.get("owned_transitive_dependencies") != {
                "0x000174d0": ["?function_17550@@YAXPAEPBG@Z"],
                "0x00017550": ["?function_1b230@@YAMPBUs_1b230_function@@JJM@Z"],
                "0x0001b230": [
                    "?function_13b390@@YAMPBXMM@Z", "?g_4858a0@@3NA",
                    "?g_485a58@@3PAUs_33a0b_view@@A", "fmod"],
                "0x00019be0": ["?function_1b2c0@@YA_NPBEJJJJPAM@Z"],
                "0x0001b2c0": ["sin", "cos"],
                "0x00017420": [
                    "?function_176a0@@YAXPAEPBG@Z",
                    "?function_17960@@YAXPAEPBG@Z",
                    "?function_17b60@@YAXPAEPBG@Z",
                    "?function_18560@@YAXPAEPBG@Z"],
                "0x0001ae70": [
                    "?function_336f0@@YAMJ@Z",
                    "?function_33980@@YAXJPAUvector3f@@@Z",
                    "?function_34060@@YAJJK@Z"],
                "0x0001b0a0": [
                    "?function_143c0@@YA_NJFFM@Z", "fabs", "fmod"],
                "0x00018a90": [
                    "?function_19530@@YAXJPAEFPAJ1@Z", "floor",
                    "h2_guest_memcpy", "log"],
                "0x00019ca0": [
                    "?function_1b2c0@@YA_NPBEJJJJPAM@Z", "?g_467068@@3QBKB",
                    "?pack_color4f@@YAKPBUcolor4f@@@Z", "h2_guest_memset"],
                "0x000143c0": [
                    "?function_12310@@YAPAUD3DTexture@@PAUs_bitmap_view@@M@Z",
                    "?function_137550@@YAPAUs_type_7ba8e9@@JF@Z"],
                "0x00012310": [
                    "?function_12ce00@@YAPAUD3DTexture@@PAUs_bitmap_data@@KM@Z",
                    "?texture_cache_bitmap_get_texture@@YAPAUD3DTexture@@PAUs_bitmap_data@@KM@Z"],
                "0x0012ccf0": [
                    "?texture_cache_bitmap_get_shared_texture@@YAPAUD3DTexture@@PAUs_bitmap_data@@@Z"],
                "0x0012c960": [
                    "?texture_cache_bitmap_build_texture@@YAXPAUs_bitmap_data@@@Z"],
                "0x0012c820": [
                    "?bitmap_get_mipmap_count@@YAFFFFF_NF@Z", "?log2_floor@@YAJK@Z"],
                "0x00014390": [
                    "?function_12310@@YAPAUD3DTexture@@PAUs_bitmap_view@@M@Z"],
                "0x000144f0": [
                    "?function_137550@@YAPAUs_type_7ba8e9@@JF@Z",
                    "?function_14390@@YA_NFPAUs_type_7ba8e9@@M@Z"],
                "0x00014f60": [
                    "?function_01dcc0@@YAPAXJ@Z", "?function_01dd60@@YA_NJPAJ0@Z"],
                "0x00019530": [
                    "?function_01dd60@@YA_NJPAJ0@Z",
                    "?function_137550@@YAPAUs_type_7ba8e9@@JF@Z",
                    "?function_14390@@YA_NFPAUs_type_7ba8e9@@M@Z",
                    "?function_143c0@@YA_NJFFM@Z", "?function_144f0@@YAXJFFFFM@Z",
                    "?function_14f60@@YAXFF@Z", "?function_25960@@YAJXZ"]} or
            material_binding.get("source_owned_transitive_calls") != [
                "0x00017550", "0x0001b230", "0x0001b2c0",
                "0x000176a0", "0x00017960", "0x00017b60", "0x00018560",
                "0x000336f0", "0x00033980", "0x00034060", "0x000143c0",
                "0x00012310", "0x0012ccb0", "0x0012ccf0", "0x0012c960",
                "0x0012c820", "0x000359e0", "0x0013e3a0", "0x00137550",
                "0x00014390", "0x000144f0", "0x00025960", "0x0001dd60",
                "0x0001dcc0", "0x00014f60", "0x00019530", "0x0012cb10"] or
            material_binding.get("owned_renderer_boundaries") != {
                "0x00018d70": ["D3DDevice_SetVertexData4f"],
                "0x00018e80": ["D3DDevice_SetVertexShaderConstantFast"],
                "0x0001b050": ["D3DDevice_SetPixelShaderProgram"]} or
            len(material_groups) != 4 or
            not material_binding.get("blocker")):
        raise RuntimeError("particle material binding frontier changed")

    sound_idle = json.loads((
        args.root / "port/runtime/texture_sound_idle_frontier.json"
    ).read_text())
    expected_sound_idle_calls = [
        ("0x00125a60", "?sound_system_available@@YAJXZ", True),
        ("0x001269f0", "?sound_system_update_time@@YAXXZ", True),
        ("0x001290d0", "?sound_voices_update_chunks@@YAXXZ", True),
        ("0x001293f0", "?function_1293f0@@YAXXZ", True),
        ("0x00218810", "?sound_cache_new_frame@@YAXXZ", True),
        ("0x0021ec00", "?function_21ec00@@YAXXZ", False),
        ("0x0021ee80", "?function_21ee80@@YAXXZ", False),
        ("0x003314b0", "GetTickCount", True),
    ]
    actual_sound_idle_calls = [
        (entry.get("address"), entry.get("symbol"), entry.get("source_owned"))
        for entry in sound_idle.get("direct_calls", [])
    ]
    if (
            sound_idle.get("version") != 1 or
            sound_idle.get("retail_function") != "0x00125d60" or
            sound_idle.get("size") != 143 or
            sound_idle.get("symbol") != "?function_125d60@@YAXXZ" or
            actual_sound_idle_calls != expected_sound_idle_calls or
            sound_idle.get("owned_direct_calls") != [
                "0x00125a60", "0x001269f0", "0x001290d0", "0x001293f0",
                "0x00218810", "0x003314b0"] or
            sound_idle.get("remaining_direct_calls") != [
                "0x0021ec00", "0x0021ee80"] or
            sound_idle.get("blocking_texture_requests_enabled") is not False or
            sound_idle.get("production_linked") is not False or
            sound_idle.get("source_owned") is not True or
            sound_idle.get("implementation") != {
                "source": "src/sound_idle_service_native.cpp",
                "symbol": "?function_125d60@@YAXXZ",
                "imports": [
                    "?function_1293f0@@YAXXZ",
                    "?function_21ec00@@YAXXZ",
                    "?function_21ee80@@YAXXZ",
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "?g_4e6948@@3PAUs_game_options_view@@A",
                    "?g_510c54@@3PAUs_game_time_globals@@A",
                    "?sound_cache_new_frame@@YAXXZ",
                    "?sound_system_available@@YAJXZ",
                    "?sound_system_update_time@@YAXXZ",
                    "?sound_voices_update_chunks@@YAXXZ",
                    "GetTickCount"],
                "semantics":
                    "exact timing gate, nested reentrancy flags, ordered sound "
                    "updates, and unconditional gated cache-frame advance"} or
            sound_idle.get("state") != {
                "sound_system_pointer": "?g_4e6380@@3PAUs_4e6380@@A",
                "sound_system_required_offsets": {
                    "initialized": "0x78", "hardware_available": "0x79",
                    "enabled": "0x7a", "last_tick": "0x80",
                    "idle_active": "0x7c", "changing_pause": "0x7d"},
                "game_options_pointer": "?g_4e6948@@3PAUs_game_options_view@@A",
                "game_options_active_flag_offset": "0x1120",
                "game_time_pointer": "?g_510c54@@3PAUs_game_time_globals@@A",
                "game_time_rate_offset": "0x4"} or
            sound_idle.get("owned_helpers") != [{
                "source": "src/sound_system_availability_native.cpp",
                "symbol": "?sound_system_available@@YAJXZ",
                "imports": ["?g_4e6380@@3PAUs_4e6380@@A"],
                "semantics":
                    "exact initialized_and_hardware_available_and_enabled query"}, {
                "source": "port/runtime/guest_host_runtime.cpp",
                "symbol": "GetTickCount",
                "imports": ["host_h2_get_milliseconds"],
                "semantics":
                    "monotonic host milliseconds through the production guest ABI"}, {
                "source": "src/sound_system_update_time_native.cpp",
                "symbol": "?sound_system_update_time@@YAXXZ",
                "imports": [
                    "?function_12be90@@YA_NXZ",
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "?g_4e6948@@3PAUs_game_options_view@@A",
                    "GetTickCount"],
                "semantics":
                    "exact wrap-safe sound clock, ambience fades, and master "
                    "pause fade policy"}, {
                "source": "src/sound_busy_state_native.cpp",
                "symbol": "?function_12be90@@YA_NXZ",
                "imports": [
                    "?g_4e6470@@3JA", "?g_4e64a0@@3JA",
                    "?g_4ed39d@@3_NA",
                    "?main_globals@@3Us_main_globals@@A"],
                "semantics":
                    "exact main-loop, async texture, and geometry-cache "
                    "busy-state query"}, {
                "source": "src/sound_cache_frame_native.cpp",
                "symbol": "?sound_cache_new_frame@@YAXXZ",
                "imports": [
                    "?g_50210c@@3PAXA",
                    "?physical_memory_reset_time@@YAXPAUs_physical_object@@@Z"],
                "semantics":
                    "exact physical-cache clock advance, wrap rebasing, and "
                    "eight per-type limit resets"}, {
                "source": "src/sound_voice_chunks_native.cpp",
                "symbol": "?sound_voices_update_chunks@@YAXXZ",
                "imports": [
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "?sound_voice_update_chunks@@YAXF@Z"],
                "semantics":
                    "exact signed-short traversal of every configured voice "
                    "queued-chunk update"}, {
                "source": "src/sound_voice_chunk_transition_native.cpp",
                "symbol": "?sound_voice_update_chunks@@YAXF@Z",
                "imports": [
                    "?function_218850@@YAKJPAUs_sound_chunk@@K@Z",
                    "?function_219290@@YAJFPAUs_permutation_group@@@Z",
                    "?function_21f650@@YAMJJ@Z",
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_4e6378@@3PAUs_sound_voice@@A",
                    "?g_4e637c@@3PAUs_record_pool@@A",
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A",
                    "?g_51ebe4@@3PAUs_bink_sound_settings@@A",
                    "?sound_reference_release@@YAXPAUs_sound_reference_holder@@@Z"],
                "semantics":
                    "exact queued-chunk promotion, reference release, "
                    "unloaded-chunk fallback, and 0x34-byte stream-time "
                    "advance"}, {
                "source": "src/sound_voice_chunk_helpers_native.cpp",
                "symbol":
                    "?sound_reference_release@@YAXPAUs_sound_reference_holder@@@Z",
                "imports": ["?g_502104@@3PAUs_record_pool@@A"],
                "semantics":
                    "exact salted-reference lookup and byte reference-count "
                    "decrement"}, {
                "source": "src/sound_voice_chunk_helpers_native.cpp",
                "symbol": "?function_219290@@YAJFPAUs_permutation_group@@@Z",
                "imports": [],
                "semantics":
                    "exact bounded next-chunk index or NONE selection"}, {
                "source": "src/sound_driver_playback_rate_native.cpp",
                "symbol": "?function_21f650@@YAMJJ@Z",
                "imports": [
                    "?g_51ebe4@@3PAUs_bink_sound_settings@@A", "exp"],
                "semantics":
                    "exact 0x34-byte driver-stream pitch-mode selection and "
                    "exponential cents-to-rate conversion"}, {
                "source": "src/sound_driver_channel_usage_native.cpp",
                "symbol":
                    "?function_21f360@@YAXPAUs_sound_driver_channel_usage@@@Z",
                "imports": [
                    "?g_51ebe4@@3PAUs_bink_sound_settings@@A",
                    "h2_guest_memset"],
                "semantics":
                    "exact 0x34-byte stream traversal, per-state channel "
                    "masks, free-voice mask, and three paired driver counts"}, {
                "source": "src/sound_attenuation_native.cpp",
                "symbol": "?sound_get_minimum_distance@@YAMPBUs_sound@@J@Z",
                "imports": [
                    "?function_221810@@YAPAUs_unknown_5c@@F@Z",
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A"],
                "semantics": "exact per-sound, promotion, and class "
                             "minimum-distance selection"}, {
                "source": "src/sound_attenuation_native.cpp",
                "symbol": "?sound_get_maximum_distance@@YAMPBUs_sound@@J@Z",
                "imports": [
                    "?function_221810@@YAPAUs_unknown_5c@@F@Z",
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A"],
                "semantics": "exact per-sound, promotion, and class "
                             "maximum-distance selection"}, {
                "source": "src/sound_attenuation_native.cpp",
                "symbol": "?function_125060@@YAJPBUs_sound@@J@Z",
                "imports": ["?g_4e3b44@@3PAUs_tag_instance@@A"],
                "semantics": "exact directional-cone override and "
                             "definition policy"}, {
                "source": "src/sound_attenuation_native.cpp",
                "symbol": "?sound_get_inner_cone_angle@@YAMPBUs_sound@@J@Z",
                "imports": [
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A"],
                "semantics": "exact compressed override or class "
                             "inner-cone angle"}, {
                "source": "src/sound_attenuation_native.cpp",
                "symbol": "?sound_get_outer_cone_angle@@YAMPBUs_sound@@J@Z",
                "imports": [
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A"],
                "semantics": "exact compressed override or class "
                             "outer-cone angle"}, {
                "source": "src/sound_attenuation_native.cpp",
                "symbol": "?sound_get_outer_cone_gain@@YAJPBUs_sound@@J@Z",
                "imports": [
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A"],
                "semantics": "exact per-sound override or class "
                             "outer-cone gain"}, {
                "source": "src/sound_attenuation_native.cpp",
                "symbol": "?sound_get_distance_gain@@YAMJPBUs_sound@@M@Z",
                "imports": [
                    "?function_12aff0@@YAMMMM_N@Z",
                    "?function_221810@@YAPAUs_unknown_5c@@F@Z",
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A"],
                "semantics": "exact minimum-distance plateau, "
                             "inverse-distance falloff, and maximum-distance "
                             "fade"}, {
                "source": "src/unknown_12a1b0.cpp",
                "symbol": "?function_21c400@@YAXFMPBUs_looping_channel_spatialization@@@Z",
                "imports": [
                    '?function_1251e0@@YAJPBXJM@Z',
                    '?function_125e60@@YAJPAUs_looping_track_sound@@@Z',
                    '?function_126960@@YAXPAUs_sound_playback@@@Z',
                    '?function_127320@@YAXJJ@Z',
                    '?function_128500@@YAXJPAUs_looping_voice_counts@@@Z',
                    '?function_128a60@@YAFJFPBF@Z',
                    '?function_12a420@@YAXF@Z',
                    '?function_12a9d0@@YAMJPBUs_sound_position@@@Z',
                    '?function_218f50@@YAFPAUs_looping_playback_definition@@FM@Z',
                    '?function_219110@@YAFPAUs_set_ref@@FPAKF1PA_N_N@Z',
                    '?function_219290@@YAJFPAUs_permutation_group@@@Z',
                    '?function_2192b0@@YAXPAUs_animation_state@@PAUs_animation_ref@@E@Z',
                    '?function_219370@@YAJPAUs_animation_state@@PAUs_animation_ref@@@Z',
                    '?function_2194c0@@YAKPAUs_packed_value@@PAUs_animation_ref@@@Z',
                    '?function_219560@@YAXPAUs_packed_value@@PAUs_animation_ref@@K@Z',
                    '?function_21f720@@YAXPBUs_looping_impulse_parameters@@@Z',
                    '?function_21f8a0@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@@Z',
                    '?function_21fa80@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@_N@Z',
                    '?function_221810@@YAPAUs_unknown_5c@@F@Z',
                    '?function_222770@@YAXJJPBUs_effect_inputs@@PAUs_sound_playback_effects@@@Z',
                    '?function_222930@@YAXJJPAUs_looping_impulse_parameters@@@Z',
                    '?g_4e034c@@3PAUs_tag_header_globals@@A',
                    '?g_4e3b44@@3PAUs_tag_instance@@A',
                    '?g_4e6378@@3PAUs_sound_voice@@A',
                    '?g_4e637c@@3PAUs_record_pool@@A',
                    '?g_4e6380@@3PAUs_4e6380@@A',
                    '?g_4e7408@@3PAUs_random_globals@@A',
                    '?g_502110@@3PAXA',
                    '?g_51ebd4@@3PAUs_sound_globals@@A',
                    '?g_51ebd8@@3PAXA',
                    '?g_51ebdc@@3PAXA',
                    '?g_51ebe4@@3PAUs_bink_sound_settings@@A',
                    '?sound_playback_set_chunk@@YAXJJDDF@Z',
                    '?sound_voice_queue_chunk@@YAXFPBUs_sound_permutation@@F@Z',
                    'g_44a150',
                    'g_44a16c',
                    'g_44a188',
                    'h2_guest_memcpy',
                    'h2_guest_memset',
                    'sqrt'
                ],
                "semantics":
                    "exact looping-track channel initialization, pitch-range/permutation "
                    "transitions, synchronization, gain/effect updates, chunk queueing, "
                    "and terminal playback cleanup"}, {
                "source": "src/sound_impulse_channel_native.cpp",
                "symbol":
                    "?function_128ca0@@YAXFJPBUs_looping_channel_"
                    "spatialization@@@Z",
                "imports": [
                    "?function_1251e0@@YAJPBXJM@Z",
                    "?function_125e60@@YAJPAUs_looping_track_sound@@@Z",
                    "?function_12a1b0@@YAXFPAUs_looping_track_sound@@PBUs_"
                    "looping_channel_spatialization@@PAUs_looping_channel_"
                    "properties@@@Z",
                    "?function_12a9d0@@YAMJPBUs_sound_position@@@Z",
                    "?function_21f720@@YAXPBUs_looping_impulse_parameters@@@Z",
                    "?function_21f8a0@@YAXJPBUs_looping_channel_properties@@PBUs_"
                    "looping_effect_playback@@@Z",
                    "?function_21fa80@@YAXJPBUs_looping_channel_properties@@PBUs_"
                    "looping_effect_playback@@_N@Z",
                    "?function_221810@@YAPAUs_unknown_5c@@F@Z",
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_4e6378@@3PAUs_sound_voice@@A",
                    "?g_4e637c@@3PAUs_record_pool@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A",
                    "?g_51ebd8@@3PAXA",
                    "?g_51ebe4@@3PAUs_bink_sound_settings@@A",
                    "?sound_playback_set_chunk@@YAXJJFFF@Z",
                    "?sound_voice_queue_chunk@@YAXFPBUs_sound_permutation@@F@Z",
                    "h2_guest_memset"],
                "semantics":
                    "exact impulse channel property assembly, Doppler pitch, "
                    "initial/update submission, controller synchronization, "
                    "chunk queueing, and terminal stream transition"}, {
                "source": "src/unknown_12a1b0.cpp",
                "symbol":
                    "?function_12a1b0@@YAXFPAUs_looping_track_sound@@PBUs_"
                    "looping_channel_spatialization@@PAUs_looping_channel_"
                    "properties@@@Z",
                "imports": [
                    "?function_222770@@YAXJJPBUs_effect_inputs@@PAUs_sound_"
                    "playback_effects@@@Z",
                    "?function_222930@@YAXJJPAUs_looping_impulse_parameters@@@Z",
                    "?g_4e034c@@3PAUs_tag_header_globals@@A",
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_4e6378@@3PAUs_sound_voice@@A",
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "h2_guest_memcpy", "sqrt"],
                "semantics":
                    "exact channel format, class flags, spatial fields, effect "
                    "inputs, impulse parameters, and attenuation-derived "
                    "runtime values"}, {
                "source": "src/sound_spatialization_native.cpp",
                "symbol":
                    "?function_129aa0@@YAXFPAJPAUs_looping_channel_"
                    "spatialization@@@Z",
                "imports": [
                    "?function_125060@@YAJPBUs_sound@@J@Z",
                    "?function_12a6d0@@YAJFMM@Z",
                    "?function_12ad30@@YAMPBTvector3f@@PBUs_sound@@J0@Z",
                    "?function_142700@@YAPATpoint3f@@PBUtransform4x3f@@PBT1@"
                    "PAT1@@Z",
                    "?function_1427f0@@YAPATvector3f@@PBUtransform4x3f@@PBT1@"
                    "PAT1@@Z",
                    "?function_2195f0@@YAMM@Z",
                    "?function_2197f0@@YAJM@Z",
                    "?function_221810@@YAPAUs_unknown_5c@@F@Z",
                    "?g_440c48@@3JA", "?g_440c4c@@3JA",
                    "?g_468788@@3PATpoint3f@@A",
                    "?g_4687a4@@3PATvector3f@@A",
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_4e6378@@3PAUs_sound_voice@@A",
                    "?g_4e637c@@3PAUs_record_pool@@A",
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "?sound_source_get_position@@YAXPBUs_sound_location_source@@"
                    "JPATpoint3f@@@Z",
                    "?vector3d_decompress@@YAPATvector3f@@KPAT1@@Z",
                    "h2_guest_memcpy"],
                "semantics":
                    "exact listener-space source transform, attenuation "
                    "routing, direct/spatial transition fades, promotion gain, "
                    "and source spatialization callback policy"}, {
                "source": "src/sound_source_position_native.cpp",
                "symbol":
                    "?function_1427f0@@YAPATvector3f@@PBUtransform4x3f@@PBT1@"
                    "PAT1@@Z",
                "imports": [],
                "semantics":
                    "exact inverse listener-frame vector transform with "
                    "recovered scale policy"}, {
                "source": "src/unknown_188dd0.cpp",
                "symbol": "?vector3d_decompress@@YAPATvector3f@@KPAT1@@Z",
                "imports": ["?function_30bf0@@YAMPATvector3f@@@Z"],
                "semantics":
                    "exact packed 11/11/10-bit direction decompression and "
                    "normalization"}, {
                "source": "src/sound_source_position_native.cpp",
                "symbol":
                    "?sound_source_get_position@@YAXPBUs_sound_location_source@@"
                    "JPATpoint3f@@@Z",
                "imports": [
                    "?g_4e6380@@3PAUs_4e6380@@A", "h2_guest_memcpy"],
                "semantics":
                    "exact world, ground-plane, and listener-height source "
                    "position selection"}, {
                "source": "src/sound_source_position_native.cpp",
                "symbol":
                    "?sound_source_get_listener_distance@@YAMPBUs_sound_location_"
                    "source@@J@Z",
                "imports": [
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "?magnitude3d@@YAMPBTvector3f@@@Z"],
                "semantics":
                    "exact unspatialized, world, bounded-height ground-plane, "
                    "and listener-relative distance policy"}, {
                "source": "src/sound_doppler_native.cpp",
                "symbol": "?function_12a9d0@@YAMJPBUs_sound_position@@@Z",
                "imports": [
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "fabs", "log", "sqrt"],
                "semantics":
                    "exact listener/source relative-velocity projection, "
                    "speed-of-sound ratio clamp, and cents conversion"}, {
                "source": "src/sound_cone_attenuation_native.cpp",
                "symbol":
                    "?function_12ad30@@YAMPBTvector3f@@PBUs_sound@@J0@Z",
                "imports": [
                    "?function_11cf50@@YAMPBTvector3f@@0@Z",
                    "?function_12aff0@@YAMMMM_N@Z",
                    "?function_2195f0@@YAMM@Z",
                    "?function_30bf0@@YAMPATvector3f@@@Z",
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?sound_get_distance_gain@@YAMJPBUs_sound@@M@Z",
                    "?sound_get_inner_cone_angle@@YAMPBUs_sound@@J@Z",
                    "?sound_get_outer_cone_angle@@YAMPBUs_sound@@J@Z",
                    "?sound_get_outer_cone_gain@@YAJPBUs_sound@@J@Z",
                    "fabs", "h2_guest_memcpy", "sqrt"],
                "semantics":
                    "exact distance/cone attenuation, direction reversal and "
                    "normalization, outer-cone interpolation, and inverse "
                    "policy"}, {
                "source": "src/sound_spatial_gain_native.cpp",
                "symbol": "?function_12a6d0@@YAJFMM@Z",
                "imports": [
                    "?function_2195f0@@YAMM@Z",
                    "?function_2197f0@@YAJM@Z",
                    "fabs", "sqrt"],
                "semantics":
                    "exact signed-range clamp, direction reversal, "
                    "linear/power interpolation, and decibel endpoint "
                    "conversion"}, {
                "source": "src/sound_fade_helpers_native.cpp",
                "symbol": "?function_12aff0@@YAMMMM_N@Z",
                "imports": ["fabs"],
                "semantics":
                    "exact bounded fade interpolation with recovered "
                    "near-equal endpoint policy"}, {
                "source": "src/unknown_2197f0.cpp",
                "symbol": "?function_2195f0@@YAMM@Z",
                "imports": ["exp"],
                "semantics":
                    "exact pinned decibel-to-linear-gain conversion"}, {
                "source": "src/sound_playback_fade_native.cpp",
                "symbol": "?function_12a810@@YAJJ@Z",
                "imports": [
                    "?function_12aff0@@YAMMMM_N@Z",
                    "?function_2195f0@@YAMM@Z",
                    "?function_2197f0@@YAJM@Z",
                    "?g_440c48@@3JA",
                    "?g_440c4c@@3JA",
                    "?g_4e637c@@3PAUs_record_pool@@A",
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "sqrt"],
                "semantics":
                    "exact fade-time anchoring, bounded progress, "
                    "linear/power gain interpolation, direction selection, "
                    "and negative-64-to-zero decibel pinning"}, {
                "source": "src/sound_playback_reference_native.cpp",
                "symbol":
                    "?function_126960@@YAXPAUs_sound_playback@@@Z",
                "imports": [
                    "?g_4e3b44@@3PAUs_tag_instance@@A",
                    "?g_502104@@3PAUs_record_pool@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A"],
                "semantics":
                    "exact playback-held reference lookup, reference-count "
                    "decrement, and ownership-bit clear"}, {
                "source":
                    "src/sound_looping_controller_release_native.cpp",
                "symbol": "?looping_sound_controller_release@@YAXJ@Z",
                "imports": [
                    "?g_51ebd8@@3PAXA",
                    "?record_pool_release@@YAXPAUs_record_pool@@J@Z"],
                "semantics":
                    "exact seven-bit controller reference decrement and "
                    "zero-reference salted record release"}, {
                "source": "src/sound_playback_delete_native.cpp",
                "symbol": "?sound_playback_delete@@YAXJ@Z",
                "imports": [
                    "?g_4e637c@@3PAUs_record_pool@@A",
                    "?g_51ebd8@@3PAXA",
                    "?looping_sound_controller_release@@YAXJ@Z",
                    "?record_pool_release@@YAXPAUs_record_pool@@J@Z"],
                "semantics":
                    "exact looping-controller playing-count adjustment and "
                    "release followed by salted playback-record release"}, {
                "source": "src/sound_voice_release_native.cpp",
                "symbol": "?sound_voice_release@@YAXJ@Z",
                "imports": [
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "?g_502114@@3PAUs_record_pool@@A",
                    "?record_pool_release@@YAXPAUs_record_pool@@J@Z"],
                "semantics":
                    "exact shared voice reference decrement, sound-to-voice "
                    "bit clear, and salted voice-record release"}, {
                "source": "src/sound_voice_free_native.cpp",
                "symbol": "?sound_voice_free@@YAXF@Z",
                "imports": [
                    "?g_4e6378@@3PAUs_sound_voice@@A",
                    "?g_4e6380@@3PAUs_4e6380@@A",
                    "?g_502104@@3PAUs_record_pool@@A",
                    "?g_51ebd4@@3PAUs_sound_globals@@A",
                    "?g_51ebe4@@3PAUs_bink_sound_settings@@A",
                    "?sound_stream_flush@@YAXPAUs_sound_stream@@@Z",
                    "?sound_voice_release@@YAXJ@Z"],
                "semantics":
                    "exact queued/current chunk unlock, 0x34-byte stream "
                    "flush, streaming-bit clear, driver-voice release, and "
                    "voice-state reset"}, {
                "source": "src/sound_playback_stop_native.cpp",
                "symbol": "?function_127320@@YAXJJ@Z",
                "imports": [
                    "?function_126960@@YAXPAUs_sound_playback@@@Z",
                    "?g_4e637c@@3PAUs_record_pool@@A",
                    "?sound_playback_delete@@YAXJ@Z",
                    "?sound_voice_free@@YAXF@Z"],
                "semantics":
                    "exact voice release, sound-reference release, source "
                    "stop callback, reason propagation, and playback "
                    "deletion ordering"}, {
                "source": "src/sound_master_gain_native.cpp",
                "symbol": "?function_2197f0@@YAJM@Z",
                "imports": ["log10"],
                "semantics":
                    "exact linear-gain clamp, base-10 decibel conversion, "
                    "and negative-64-to-zero pinning"}, {
                "source": "src/sound_cache_request_native.cpp",
                "symbol": "?function_218850@@YAKJPAUs_sound_chunk@@K@Z",
                "imports": [
                    "?function_218a10@@YAXPAUs_sound_chunk@@J@Z",
                    "?g_502104@@3PAUs_record_pool@@A",
                    "?g_50210c@@3PAUs_sound_cache_allocator@@A",
                    "SwitchToThread"],
                "semantics":
                    "exact load admission, page touch, cooperative blocking "
                    "wait, loaded/used/lock state, and result flags"}, {
                "source": "src/sound_cache_allocation_native.cpp",
                "symbol": "?function_218a10@@YAXPAUs_sound_chunk@@J@Z",
                "imports": [
                    "?datum_new_at_index_with_salt@@YAJPAUs_record_pool@@J@Z",
                    "?function_13d370@@YAJPAUs_physical_object@@JJ@Z",
                    "?function_213760@@YAJKJPAXPAKPA_NJJ@Z",
                    "?g_502104@@3PAUs_record_pool@@A",
                    "?g_502108@@3KA",
                    "?g_50210c@@3PAUs_sound_cache_allocator@@A"],
                "semantics":
                    "exact physical-page and salted-record allocation, "
                    "512-byte read rounding, and asynchronous loaded-byte "
                    "submission"}, {
                "source": "src/unknown_218850.cpp",
                "symbol": "?sound_cache_initialize@@YAXXZ",
                "imports": [
                    "?function_13d170@@YAXPAUs_physical_object@@PBDJJJP6AXJ@ZP6A_NJ@ZP6AEJ@ZPAVc_data_allocator@@@Z",
                    "?function_16b5f0@@YAXPAUs_record_pool@@PBDJJJPAVc_data_allocator@@@Z",
                    "?g_468758@@3PAVc_data_allocator@@A",
                    "?g_502104@@3PAUs_record_pool@@A",
                    "?g_502108@@3KA",
                    "?g_50210c@@3PAUs_sound_cache_allocator@@A",
                    "?g_global_f9ae07@@3Us_physical_memory_globals@@A",
                    "?sound_cache_entry_busy@@YA_NJ@Z",
                    "?sound_cache_entry_delete@@YAXJ@Z",
                    "XPhysicalProtect"],
                "semantics":
                    "exact record pool and physical cache construction, "
                    "callback installation, active state publication, and "
                    "fixed 3 MiB upload allocation"}, {
                "source": "src/unknown_218850.cpp",
                "symbol": "?function_2186f0@@YAXXZ",
                "imports": [
                    "?async_globals@@3Us_async_globals@@A",
                    "?block_delete@s_physical_object@@QAAXJ@Z",
                    "?function_120bf0@@YAJXZ",
                    "?g_502104@@3PAUs_record_pool@@A",
                    "?g_50210c@@3PAUs_sound_cache_allocator@@A",
                    "GetTickCount"],
                "semantics":
                    "exact bounded shutdown drain, pending-task "
                    "publication, physical block deletion, and record-pool "
                    "invalidation"}, {
                "source": "src/sound_cache_callbacks_native.cpp",
                "symbol": "?sound_cache_entry_busy@@YA_NJ@Z",
                "imports": ["?g_502104@@3PAUs_record_pool@@A"],
                "semantics":
                    "exact lock/reference/loading cache-entry eviction guard"}, {
                "source": "src/sound_cache_callbacks_native.cpp",
                "symbol": "?sound_cache_entry_delete@@YAXJ@Z",
                "imports": [
                    "?g_502104@@3PAUs_record_pool@@A",
                    "?record_pool_release@@YAXPAUs_record_pool@@J@Z"],
                "semantics":
                    "exact chunk residency clear and salted cache-record "
                    "release"}, {
                "source": "src/sound_cache_dispose.cpp",
                "symbol": "?function_2186b0@@YAXXZ",
                "imports": [
                    "?data_dispose@@YAXPAUs_record_pool@@@Z",
                    "?g_502104@@3PAUs_record_pool@@A",
                    "?g_502108@@3KA",
                    "?g_50210c@@3PAUs_sound_cache_allocator@@A"],
                "semantics":
                    "exact record-pool disposal, allocator-backed physical "
                    "cache release, and upload-base clear"}, {
                "source": "src/sound_playback_update_native.cpp",
                "symbol": "?function_1293f0@@YAXXZ",
                "imports": [
                    "?function_127320@@YAXJJ@Z",
                    "?function_128ca0@@YAXFJPBUs_looping_channel_spatialization@@@Z",
                    "?function_129aa0@@YAXFPAJPAUs_looping_channel_spatialization@@@Z",
                    "?function_12a810@@YAJJ@Z",
                    "?function_2197f0@@YAJM@Z",
                    "?function_21c400@@YAXFMPBUs_looping_channel_spatialization@@@Z",
                    "?function_21f360@@YAXPAUs_sound_driver_channel_usage@@@Z",
                    "?g_4e6378@@3PAUs_sound_voice@@A",
                    "?g_4e637c@@3PAUs_record_pool@@A",
                    "?g_4e6380@@3PAUs_4e6380@@A"],
                "semantics":
                    "exact master gain, fade-stop, spatialization, channel "
                    "submission, and source callback traversal"}] or
            sound_idle.get("execution_gate") != [
                "native_sound_system_storage_initialized",
                "native_main_loop_busy_state_initialized",
                "native_sound_voice_records_and_streams_initialized",
                "native_sound_cache_upload_buffer_initialized",
                "native_sound_cache_file_read_route_initialized",
                "native_sound_cache_allocator_initialized",
                "native_sound_cache_allocator_vtable_owned",
                "native_sound_cache_lifecycle_ordering_owned",
                "native_sound_reference_pool_initialized",
                "native_sound_driver_stream_state_initialized",
                "native_sound_definition_and_chunk_tables_initialized",
                "native_sound_driver_pre_and_post_update_owned",
                "native_sound_fade_endpoint_state_owned",
                "native_sound_playback_stop_lifetime_owned",
                "native_sound_gain_and_fade_lifecycle_owned",
                "native_sound_spatialization_and_channel_submission_owned",
                "native_sound_source_callback_lifetimes_owned",
                "native_sound_cache_physical_storage_initialized",
                "production_texture_requesters_linked"] or
            sound_idle.get("remaining_driver_frontier") !=
                "port/runtime/sound_driver_idle_frontier.json" or
            not sound_idle.get("blocker")):
        raise RuntimeError("texture sound-idle frontier changed")

    sound_driver_idle = json.loads((
        args.root / "port/runtime/sound_driver_idle_frontier.json"
    ).read_text())
    sound_stream_flush_source = (
        args.root / "src/sound_stream_flush.cpp").read_text()
    expected_stream_flush_fragments = [
        "stream->stream->GetStatus(&status)",
        "IDirectSoundStream_Pause(stream->stream, DSSTREAMPAUSE_RESUME)",
        "IDirectSoundStream_FlushEx(stream->stream, 0,",
        "DSSTREAMFLUSHEX_ASYNC | DSSTREAMFLUSHEX_ENVELOPE",
        "stream->codec->stop()",
        "stream->flushing = 1"]
    expected_sound_driver_providers = [{
        "address": "0x0021ec00",
        "symbol": "?function_21ec00@@YAXXZ",
        "phase": "pre_update",
        "source_owned": False,
        "source_level_calls": [
            "function_1915f0", "memset", "DirectSoundDoWork",
            "sound_driver_channel_get", "sound_driver_channel_flush"],
        "indirect_calls": ["IDirectSoundStream::GetStatus"],
        "state": ["g_51ebe4", "g_dwDirectSoundDeltaPanicCount"],
        "semantics":
            "audio-processor panic recovery, DirectSound work servicing, "
            "stopped-stream detection, and channel flush"}, {
        "address": "0x0021ee80",
        "symbol": "?function_21ee80@@YAXXZ",
        "phase": "post_update",
        "source_owned": False,
        "source_level_calls": [
            "function_21ece0", "sound_occlusion_overlap",
            "sound_submix_volume", "sound_stream_update", "function_191550"],
        "indirect_calls": [
            "c_sound_effects::set_i3dl2",
            "IDirectSoundBuffer::SetVolume",
            "IDirectSound::CommitDeferredSettings"],
        "state": ["g_47005c", "g_51ebe4", "g_44a250[2]"],
        "semantics":
            "deferred environment, reverb and occlusion application, submix "
            "gain updates, stream advancement, and deferred-settings commit"}]
    if (
            sound_driver_idle.get("version") != 1 or
            any(fragment not in sound_stream_flush_source
                for fragment in expected_stream_flush_fragments) or
            sound_driver_idle.get("source") != "src/unknown_21e330.cpp" or
            sound_driver_idle.get("providers") != expected_sound_driver_providers or
            sound_driver_idle.get("stream_flush") != {
                "address": "0x002ae4b0",
                "symbol": "?sound_stream_flush@@YAXPAUs_sound_stream@@@Z",
                "source": "src/sound_stream_flush.cpp",
                "source_owned": True,
                "stream_size": "0x34",
                "state_offsets": {
                    "state": "0x01 bits 0..2",
                    "flushing": "0x03 bit 2",
                    "direct_sound_stream": "0x24",
                    "codec": "0x30"},
                "indirect_calls": [
                    "IDirectSoundStream::GetStatus",
                    "IDirectSoundStream::Pause",
                    "IDirectSoundStream::FlushEx",
                    "c_sound_stream_codec::stop"],
                "constants": [
                    "DSSTREAMSTATUS_PAUSED", "DSSTREAMPAUSE_RESUME",
                    "DSSTREAMFLUSHEX_ASYNC", "DSSTREAMFLUSHEX_ENVELOPE"],
                "semantics":
                    "non-idle streams query status, resume paused playback, "
                    "request asynchronous envelope-aware flush, stop the codec, "
                    "and publish flushing state"} or
            sound_driver_idle.get("execution_gate") != [
                "native_audio_processor_recovery_owned",
                "native_direct_sound_work_service_owned",
                "native_sound_stream_objects_and_vtables_owned",
                "native_sound_effects_object_and_vtable_owned",
                "native_reverb_occlusion_and_submix_state_initialized",
                "native_deferred_sound_settings_commit_owned",
                "native_direct_sound_stream_status_owned",
                "native_direct_sound_stream_resume_owned",
                "native_direct_sound_async_envelope_flush_owned",
                "native_sound_stream_codec_stop_owned"] or
            sound_driver_idle.get("production_linked") is not False or
            sound_driver_idle.get("source_owned") is not False or
            not sound_driver_idle.get("blocker")):
        raise RuntimeError("sound driver idle frontier changed")

    texture_upload = json.loads((
        args.root / "port/runtime/particle_texture_upload_frontier.json"
    ).read_text())
    production_main_source = (args.root / "port/src/main.cpp").read_text()
    host_services_source = (args.root / "port/src/host_services.cpp").read_text()
    guest_cache_source = (
        args.root / "port/runtime/guest_cache_runtime.cpp").read_text()
    production_controller_fragments = [
        "start_async_texture_controller(",
        "stop_async_texture_controller(",
        'options.data_root / "maps" / "mainmenu.map"',
        'options.data_root / "maps" / "shared.map"',
        '"single_player_shared.map"',
    ]
    texture_upload_calls = texture_upload.get("direct_calls", [])
    if (
            any(fragment not in production_main_source
                for fragment in production_controller_fragments) or
            "switch_async_texture_map(" not in host_services_source or
            "controller_entries = context->controller_entries" not in
                host_services_source or
            "? g_55aca8 : NONE" not in guest_cache_source or
            production_main_source.index("h2_call_guest_int_0(entry, stack_top)") >
                production_main_source.index("start_async_texture_controller(") or
            texture_upload.get("version") != 1 or
            texture_upload.get("retail_function") != "0x0012ce00" or
            texture_upload.get("size") != 658 or
            texture_upload.get("symbol") !=
                "?function_12ce00@@YAPAUD3DTexture@@PAUs_bitmap_data@@KM@Z" or
            texture_upload.get("source_owned") is not True or
            texture_upload.get("implementation") !=
                "src/particle_texture_upload_request_native.cpp" or
            texture_upload.get("execution_gate") !=
                "native_particle_texture_upload_lifetime_owned" or
            texture_upload.get("native_platform_foundation") != {
                "source": "port/src/native_async_platform.cpp",
                "host_only": True,
                "guest_bridge_enabled": False,
                "opaque_handle_tokens": True,
                "host_pointer_exposure": False,
                "owned_services": [
                    "recursive_mutex_create_wait_release_close",
                    "bounded_semaphore_create_wait_release_close",
                    "finite_timeout_and_infinite_wait",
                    "stale_handle_generation_rejection",
                    "cooperative_thread_yield",
                    "read_only_regular_file_open_seek_read_close",
                    "short_read_and_64_bit_seek_semantics",
                    "symbolic_link_file_rejection",
                    "worker_thread_create_stack_wait_join_close",
                    "worker_thread_exit_code",
                    "worker_thread_precedence_priority_mapping",
                    "scheduler_lifecycle_contract",
                    "scheduler_admission_and_drain_state_gate",
                    "tracked_guest_page_read_write_validation",
                    "bounded_guest_sync_file_bridge",
                    "cache_file_slot_lifecycle_contract",
                    "validated_cache_file_slot_owner",
                    "bounded_guest_cache_slot_publication",
                    "cooperative_worker_drain_join_cleanup",
                    "guest_abi_v12_sync_file_and_admission_routing",
                    "validated_low_guest_stack_worker_entry",
                    "transactional_guest_queue_admission_and_wake",
                    "recovered_guest_queue_initialize_and_teardown",
                    "recovered_positioned_read_queue_round_trip"],
                "remaining_services": ["guest_path_file_open"],
                "guest_memory_validator": {
                    "source": "port/src/guest_arena.cpp",
                    "host_pointer_bounds_checked": True,
                    "cross_end_rejected": True,
                    "page_protections_tracked": True,
                    "write_requires_read_write": True,
                    "unmapped_pages_rejected": True},
                "guest_bridge": {
                    "source": "port/src/native_async_guest_bridge.cpp",
                    "status":
                        "abi_v12_routes_configured_for_production_controller",
                    "opaque_tokens_preserved": True,
                    "validate_outputs_before_side_effects": True,
                    "operations": [
                        "mutex_create_wait_release_close",
                        "semaphore_create_wait_release_close",
                        "host_selected_read_only_file_open",
                        "file_seek_and_read",
                        "worker_priority_wait_exit_code_close"],
                    "unowned_operations": ["guest_path_file_open"],
                    "guest_runtime_source":
                        "port/runtime/guest_async_runtime.cpp",
                    "host_routing_source": "port/src/host_services.cpp",
                    "host_abi_version": 12,
                    "production_configured": True}} or
            texture_upload.get("native_scheduler_lifecycle") != {
                "status":
                    "production_controller_started_after_map_bootstrap",
                "recovered_shutdown_provider": None,
                "states": ["stopped", "accepting", "stopping"],
                "initial_state": "stopped",
                "admission": {
                    "linearization_lock": "native_lifecycle_lock",
                    "allowed_state": "accepting",
                    "rejected_task_id": -1,
                    "rejection_side_effects":
                        "node_recycled_done_false_no_callback_or_task_id",
                    "stop_transition":
                        "accepting_to_stopping_before_any_handle_close",
                    "maximum_admitted_tasks": 150},
                "shutdown_policy": {
                    "mode": "drain_all_admitted_tasks",
                    "cancellation_supported": False,
                    "reason":
                        "the recovered callback and done ABIs have no cancellation result channel",
                    "queued_nodes":
                        "remain_queued_and_run_to_terminal_result_one",
                    "in_flight_node": "runs_to_terminal_result_one",
                    "retry_result": 0,
                    "completion_result": 1,
                    "unsupported_terminal_results":
                        "callbacks_that_can_return_other_nonzero_results_are_not_bridge_admissible",
                    "done_publication":
                        "accepted_tasks_publish_true_only_after_completion_result_one",
                    "timeout_behavior":
                        "remain_stopping_with_handles_and_queue_owned_for_retry"},
                "worker_exit": {
                    "wake_after_stop_transition": True,
                    "condition": "state_is_stopping_and_work_list_is_empty",
                    "check_lock": "work_list_mutex",
                    "exit_code": 0},
                "teardown_order": [
                    "stop_admission", "drain_admitted_tasks",
                    "wake_worker_for_cooperative_exit", "wait_worker_exit",
                    "join_and_close_host_worker",
                    "close_host_wake_semaphore", "transition_to_stopped",
                    "verify_empty_guest_work_list_and_150_free_nodes",
                    "close_guest_work_list_mutex",
                    "close_guest_free_list_mutex",
                    "clear_current_node_and_guest_queue_state"],
                "cache_teardown_dependency":
                    "close_cache_file_handles_and_release_cache_pools_only_after_scheduler_stopped",
                "restart_policy":
                    "only_stopped_may_initialize_a_fresh_150_node_queue_and_new_handles",
                "execution_enabled": True,
                "state_gate": {
                    "source": "port/src/native_async_lifecycle.cpp",
                    "owned_behavior": [
                        "stopped_accepting_stopping_transitions",
                        "accepting_only_admission",
                        "recovered_150_node_capacity_limit",
                        "admitted_task_accounting",
                        "finite_and_infinite_drain_wait",
                        "timeout_preserves_stopping_state",
                        "restart_only_after_finished_stop"]},
                "worker_controller": {
                    "source": "port/src/native_async_worker.cpp",
                    "status": "production_host_guest_step_routed",
                    "owned_behavior": [
                        "one_wake_per_accepted_admission",
                        "retry_result_resignals_without_completing_admission",
                        "completion_result_decrements_exactly_one_admission",
                        "stop_rejects_new_admission_before_drain",
                        "cooperative_exit_wake_only_after_drain",
                        "finite_timeout_budget_spans_drain_and_join",
                        "timeout_preserves_worker_and_wake_handles",
                        "joined_worker_then_wake_handle_close",
                        "restart_only_from_stopped_without_live_handles",
                        "reservation_before_guest_work_list_insertion",
                        "wake_publication_after_guest_node_visibility",
                        "reservation_abort_without_guest_execution",
                        "shutdown_drain_does_not_block_reservation_publish_or_abort"],
                    "guest_worker_entry_connected": True,
                    "execution_enabled": True,
                    "guest_worker_source": "port/src/native_guest_worker.cpp",
                    "guest_step_source":
                        "src/async.cpp:h2_native_async_worker_step",
                    "guest_stack": {
                        "default_address": "0x20100000",
                        "size": 65536,
                        "page_aligned": True,
                        "entry_requires_executable_guest_mapping": True,
                        "stack_requires_guest_read_write_mapping": True},
                    "production_configured": True,
                    "guest_read_round_trip": {
                        "location_router": "function_213760",
                        "request": "function_1a0f10",
                        "callback": "function_1a0e70",
                        "selector_slots": {
                            "0": "active_g_55aca8", "1": 2, "2": 0, "3": 1},
                        "tested_routes": [
                            "mainmenu_active_slot_2",
                            "shared_selector_2_slot_0",
                            "campaign_active_slot_4_after_stopped_switch",
                            "shared_slot_0_preserved_after_active_switch"],
                        "file_offset": 2048,
                        "read_bytes": 16,
                        "callback_chunk_bytes": 8,
                        "completion_and_150_node_recycle_required": True,
                        "production_configured": True},
                    "guest_queue_routing": {
                        "source": [
                            "src/async.cpp",
                            "src/particle_texture_cache_request_native.cpp"],
                        "host_abi_version": 12,
                        "reserve_before_insert": True,
                        "publish_wake_after_insert": True,
                        "rejected_or_failed_publish_recycles_node": True,
                        "production_configured": True},
                    "guest_queue_lifecycle": {
                        "source": "src/async.cpp",
                        "host_control_source": "port/src/host_services.cpp",
                        "initialize": "h2_native_async_queue_initialize",
                        "teardown": "h2_native_async_queue_teardown",
                        "initial_free_nodes": 150,
                        "native_omits_recovered_thread_and_work_semaphore": True,
                        "guest_mutexes_are_opaque_host_tokens": True,
                        "teardown_requires_empty_work_list_and_exact_free_list_restoration": True,
                        "failed_handle_close_is_retryable": True,
                        "reset_rejected_until_guest_queue_teardown": True,
                        "production_configured": True}}} or
            texture_upload.get("native_cache_file_slot_lifecycle") != {
                "status":
                    "production_controller_owns_active_and_shared_slots",
                "recovered_header_type_slots": {
                    "0": [4, 5], "1": [3], "2": [2], "3": [0], "4": [1]},
                "recovered_location_selector_slots": {
                    "0": "g_55aca8_current_slot", "1": 2, "2": 0, "3": 1},
                "known_retail_identities": {
                    "mainmenu.map": {"header_type": 2, "slot": 2},
                    "shared.map": {"header_type": 3, "slot": 0},
                    "single_player_shared.map": {"header_type": 4, "slot": 1}},
                "current_native_conflict":
                    "resolved_cache_file_find_returns_controller_published_g_55aca8",
                "required_initialization": [
                    "validate_each_host_selected_cache_header_before_open",
                    "install_only_opaque_native_file_tokens_in_guest_slots",
                    "copy_each_validated_0x800_header_into_its_matching_guest_slot",
                    "set_g_55aca8_to_the_active_maps_type_correct_slot",
                    "leave_absent_optional_slots_invalid"],
                "required_switch_order": [
                    "stop_scheduler_admission",
                    "drain_and_join_scheduler_worker",
                    "open_and_validate_new_active_map_token_and_header",
                    "install_and_publish_new_active_slot_in_host_owner",
                    "close_and_clear_old_dynamic_active_slot",
                    "restart_scheduler_only_after_all_referenced_shared_slots_are_ready"],
                "teardown_order": [
                    "scheduler_must_be_stopped",
                    "guest_queue_must_be_torn_down",
                    "close_active_and_shared_file_tokens",
                    "clear_all_six_guest_slot_handles_and_headers",
                    "set_g_55aca8_to_none"],
                "execution_enabled": True,
                "host_service_integration": {
                    "source": "port/src/host_services.cpp",
                    "context_owns_slot_lifecycle": True,
                    "install_publishes_before_worker_start": True,
                    "replacement_rejected_while_worker_live": True,
                    "clear_requires_worker_shutdown_and_guest_queue_teardown": True,
                    "reset_rejects_live_cache_handles": True,
                    "worker_start_and_reset_reject_unsynchronized_publication": True,
                    "recovered_positioned_read_round_trip_tested": True,
                    "read_round_trip_bytes": 16,
                    "read_callback_chunk_bytes": 8,
                    "stopped_dynamic_active_switch_read_tested": True,
                    "restart_after_complete_cleanup_tested": True,
                    "production_configured": True},
                "state_owner": {
                    "source": "port/src/native_cache_file_slots.cpp",
                    "owned_behavior": [
                        "header_magic_version_size_type_footer_build_and_name_validation",
                        "descriptor_backed_file_size_validation",
                        "type_correct_six_slot_assignment",
                        "campaign_slot_4_5_alternation",
                        "fixed_shared_slot_preservation_across_active_map_switch",
                        "stopped_only_install_and_teardown",
                        "opaque_token_close_and_slot_clear",
                        "exact_g_557c90_and_g_55aca8_guest_publication"]},
                "guest_publication": {
                    "slots_address": "0x00557c90",
                    "slot_count": 6,
                    "slot_size": 2052,
                    "active_slot_address": "0x0055aca8",
                    "requires_writable_guest_ranges": True,
                    "requires_scheduler_stopped": True,
                    "runtime_connected": True,
                    "production_configured": True}} or
            texture_upload.get("native_texture_pool_lifecycle") != {
                "status":
                    "production_controller_configured_upload_requesters_still_dormant",
                "recovered_sources": [
                    "src/unknown_13d170.cpp:function_13d370/physical_memory_flush",
                    "src/unknown_13d320.cpp:function_13d320",
                    "src/unknown_16b570.cpp:record_pool_allocate/datum_new_at_index_with_salt/record_pool_release",
                    "src/particle_texture_cache_request_native.cpp:function_12d5a0/function_12d6d0",
                    "src/particle_texture_mip_math_native.cpp:bitmap_get_mipmap_count/log2_floor",
                    "src/particle_bitmap_pitch_native.cpp:function_1365a0",
                    "src/particle_texture_upload_request_native.cpp:function_12ce00",
                    "src/particle_texture_cache_lookup_native.cpp:texture_cache_bitmap_build_texture/texture_cache_bitmap_level/texture_cache_format_scalable",
                    "src/particle_texture_cache_lifecycle_native.cpp:texture_cache_block_delete/texture_cache_entry_busy/texture_cache_entry_state/h2_native_texture_cache_flush",
                    "port/src/native_texture_pool_storage.cpp:host-owned guest pool and upload-buffer publication",
                    "port/src/host_services.cpp:ordered async texture map-switch controller"],
                "exact_layouts": {
                    "record_pool_bytes": 76,
                    "physical_block_bytes": 24,
                    "texture_entry_bytes": 40,
                    "page_shift": 12},
                "tested_behavior": [
                    "one_page_then_two_page_contiguous_allocation",
                    "salted_physical_handles_mirrored_into_texture_entries",
                    "doubly_linked_physical_block_order",
                    "texture_entry_release_and_bitmap_clear",
                    "complete_synthetic_reinitialization",
                    "bitmap_pitch_and_mip_policy_execute_with_exact_results",
                    "function_12d5a0_allocates_one_physical_page_and_matching_texture_entry",
                    "sixteen_source_bytes_round_to_one_512_byte_async_read",
                    "recovered_header_targets_the_allocated_guest_upload_address",
                    "worker_completion_sets_resident_and_restores_all_150_queue_nodes",
                    "function_12c990_nonblocking_request_returns_null_while_queuing_real_upload",
                    "function_12c990_returns_exact_resident_texture_after_worker_completion",
                    "nonblocking_request_never_invokes_function_125d60_idle_service",
                    "function_12ce00_applies_recovered_streaming_level_and_priority_policy",
                    "function_12ce00_returns_null_for_pending_nonblocking_upload",
                    "function_12ce00_returns_exact_resident_texture_after_completion",
                    "function_12ce00_returns_nonstreamed_bitmap_texture_directly",
                    "function_12ce00_returns_available_shared_hardware_texture",
                    "function_12ce00_routes_blocking_unavailable_request_to_type_fallback_without_idle_wait",
                    "host_publishes_exact_g_4e6454_g_4e6460_g_4e6464_and_g_4e6484_values",
                    "recovered_upload_allocates_from_host_owned_guest_pool_and_buffer_storage",
                    "active_cache_replacement_requires_queue_teardown_and_texture_pool_clear",
                    "texture_pool_clear_zeroes_fixed_globals_storage_and_upload_buffer",
                    "host_owned_texture_pool_lifecycle_restarts_cleanly",
                    "host_publishes_three_validated_executable_cache_callback_addresses",
                    "nonexecutable_cache_callback_publication_is_rejected_before_install",
                    "cache_state_callback_preserves_flag_and_pending_state_bits",
                    "cache_busy_callback_tracks_inflight_residency_without_fabricating_a_d3d_fence",
                    "pool_clear_rejects_live_texture_or_physical_records",
                    "cache_delete_callback_clears_bitmap_residency_and_releases_both_records",
                    "texture_pool_flush_requires_worker_stop_and_guest_queue_teardown",
                    "host_verifies_empty_pools_after_guest_flush_entry_returns_success",
                    "recovered_physical_memory_flush_evicts_every_resident_texture",
                    "ordered_flush_clears_old_map_source_bitmap_residency_before_pool_clear",
                    "single_controller_orders_worker_queue_flush_pool_cache_reinstall_and_restart",
                    "resident_old_map_texture_is_evicted_before_active_campaign_cache_replacement",
                    "campaign_and_preserved_shared_reads_execute_after_automatic_worker_restart",
                    "complete_controller_installs_active_and_shared_caches_pools_queue_and_worker",
                    "controller_requires_exactly_one_initial_active_cache_and_unique_cache_types",
                    "failed_pool_install_rolls_back_cache_handles_and_service_state",
                    "complete_controller_shutdown_stops_queue_flushes_pools_clears_caches_and_resets_services",
                    "production_guest_image_links_controller_queue_flush_and_callback_entries",
                    "production_guest_scheduler_storage_is_bound_to_exact_retail_addresses",
                    "production_guest_image_imports_only_standard_and_async_host_abi_services",
                    "production_controller_starts_after_recovered_mainmenu_cache_bootstrap",
                    "production_controller_installs_mainmenu_shared_and_single_player_shared_slots",
                    "production_host_map_selection_runs_ordered_texture_controller_switch_before_campaign_tag_load",
                    "production_cache_file_find_returns_controller_published_active_slot",
                    "authentic_campaign_handoff_succeeds_with_controller_restart",
                    "production_controller_shutdown_completes_after_headless_and_windowed_execution"],
                "guest_object_closure": {
                    "allocator_definitions": 1,
                    "choice_definitions": 1,
                    "record_pool_definitions": 4,
                    "imports_resolved_by_fixture": True,
                    "upload_allocation_definitions": 2,
                    "mip_math_definitions": 2,
                    "bitmap_pitch_definitions": 1,
                    "request_policy_chain_definitions": 3,
                    "upload_policy_definitions": 1,
                    "cache_lookup_policy_definitions": 3,
                    "physical_allocator_definitions": 3,
                    "cache_lifecycle_callback_definitions": 4},
                "remaining_ownership": [
                    "production_linkage_and_execution_of_texture_upload_requesters",
                    "genuine_function_125d60_main_thread_sound_idle_service_for_blocking_requests"],
                "execution_enabled": False,
                "production_configured": True,
                "recovered_upload_allocation_round_trip": {
                    "entry": "function_12ce00",
                    "header": "function_12d6d0",
                    "allocation": "function_12d5a0",
                    "request_policy": "function_12c990",
                    "blocking": False,
                    "initial_return": "null_pending_texture",
                    "completed_return": "exact_resident_texture_entry",
                    "sound_idle_calls": 0,
                    "source_location": 2048,
                    "source_bytes": 16,
                    "scheduled_bytes": 512,
                    "physical_pages": 1,
                    "active_cache_selector": 0,
                    "priority": 5,
                    "callback": "function_1a0e70",
                    "fixture_owned_bitmap_lifetime": True,
                    "exact_zero_padded_destination_verified": True,
                    "production_configured": False},
                "host_owned_guest_pool_storage": {
                    "source": "port/src/native_texture_pool_storage.cpp",
                    "fixed_globals": {
                        "texture_pool": "0x004e6454",
                        "upload_buffer": "0x004e6460",
                        "physical_object": "0x004e6464",
                        "upload_count": "0x004e6484"},
                    "guest_storage": {
                        "physical_object": "0x20400000",
                        "physical_pool": "0x20400100",
                        "physical_entries": "0x20400200",
                        "physical_bitmap": "0x20400800",
                        "texture_pool": "0x20400900",
                        "texture_entries": "0x20400a00",
                        "texture_bitmap": "0x20401400",
                        "upload_buffer": "0x20500000"},
                    "maximum_entries": 64,
                    "upload_pages": 16,
                    "page_shift": 12,
                    "install_requires_worker_and_queue_stopped": True,
                    "clear_requires_worker_stopped_and_queue_torn_down": True,
                    "active_cache_replacement_requires_pool_clear": True,
                    "cache_clear_requires_pool_clear": True,
                    "reset_rejects_installed_pools": True,
                    "production_configured": True,
                    "production_guest_linked": True,
                    "clear_requires_empty_physical_and_texture_pools": True,
                    "flush_requires_worker_stopped_and_queue_torn_down": True,
                    "flush_entry":
                        "h2_native_texture_cache_flush -> physical_memory_flush",
                    "flush_releases_source_bitmap_references_before_clear": True,
                    "ordered_map_switch_controller": "switch_async_texture_map",
                    "ordered_map_switch_sequence": [
                        "worker_shutdown", "queue_teardown", "texture_flush",
                        "texture_pool_clear", "active_cache_replacement",
                        "texture_pool_install", "queue_initialize",
                        "worker_restart"],
                    "partial_failure_policy":
                        "fail_closed_at_first_incomplete_stage",
                    "startup_controller": "start_async_texture_controller",
                    "startup_sequence": [
                        "service_configuration", "cache_installation",
                        "texture_pool_installation", "queue_initialization",
                        "worker_startup"],
                    "startup_failure_policy":
                        "rollback_completed_stages_and_fail_closed",
                    "shutdown_controller": "stop_async_texture_controller",
                    "shutdown_sequence": [
                        "worker_shutdown", "queue_teardown", "texture_flush",
                        "texture_pool_clear", "cache_clear", "service_reset"],
                    "production_guest_entries": [
                        "h2_native_async_worker_step",
                        "h2_native_async_queue_initialize",
                        "h2_native_async_queue_teardown",
                        "h2_native_texture_cache_flush",
                        "?texture_cache_block_delete@@YAXJ@Z",
                        "?texture_cache_entry_busy@@YA_NJ@Z",
                        "?texture_cache_entry_state@@YAEJ@Z",
                        "?physical_memory_flush@@YAXPAUs_physical_object@@@Z"],
                    "production_guest_bindings": {
                        "?async_globals@@3Us_async_globals@@A": "0x004e0354",
                        "?g_4e6454@@3PAUs_record_pool@@A": "0x004e6454",
                        "?g_4e6464@@3PAUs_physical_object@@A": "0x004e6464",
                        "?g_4e6479@@3_NA": "0x004e6479",
                        "?g_5020c8@@3Us_thread_stack@@A": "0x005020c8",
                        "?g_510810@@3PAUs_job_node@@A": "0x00510810"},
                    "production_start_after_mainmenu_bootstrap": True,
                    "production_campaign_switch_via_host_h2_map_select": True,
                    "production_shutdown_after_host_execution": True,
                    "production_transition_timeout_milliseconds": 5000,
                    "callbacks": {
                        "delete": "texture_cache_block_delete@0x0012d200",
                        "busy": "texture_cache_entry_busy@0x0012d1a0",
                        "state": "texture_cache_entry_state@0x0012d160",
                        "publication_requires_executable_guest_addresses": True,
                        "native_resident_entries_have_no_fabricated_d3d_fence": True}}} or
            [(call.get("address"), call.get("symbol"), call.get("source_owned"))
             for call in texture_upload_calls] != [
                ("0x0012c820", "?texture_cache_bitmap_build_texture@@YAXPAUs_bitmap_data@@@Z", True),
                ("0x0012c990", "?function_12c990@@YAPAUD3DTexture@@PAUs_bitmap_data@@J_N11JPA_N2@Z", True),
                ("0x0012cb10", "?texture_cache_bitmap_level@@YAJPBUs_bitmap_data@@M@Z", True),
                ("0x0012ccb0", "?texture_cache_format_scalable@@YA_NJ@Z", True)] or
            texture_upload.get("owned_direct_calls") != [
                "0x0012c820", "0x0012c990", "0x0012cb10", "0x0012ccb0"] or
            texture_upload.get("remaining_direct_calls") != [] or
            texture_upload.get("request_provider_dependencies") != [
                "?function_125d60@@YAXXZ", "SwitchToThread",
                "?async_globals@@3Us_async_globals@@A",
                "?function_1a0e70@@YAJPATs_async_task@@@Z", "ReleaseMutex",
                "WaitForSingleObject", "h2_guest_memset"] or
            texture_upload.get("blocking_wait_usage") != {
                "caller": "?function_12c990@@YAPAUD3DTexture@@PAUs_bitmap_data@@J_N11JPA_N2@Z",
                "wait_provider": "?function_120d50@@YAXPC_N_N@Z",
                "idle_argument": True,
                "idle_service": "?function_125d60@@YAXXZ",
                "native_bypass_allowed": False,
                "ownership_frontier":
                    "port/runtime/texture_sound_idle_frontier.json"} or
            texture_upload.get("request_provider_state") != [
                "?async_globals@@3Us_async_globals@@A",
                "?g_4e6454@@3PAUs_record_pool@@A",
                "?g_4e6464@@3PAUs_physical_object@@A", "?g_4e6484@@3JA"] or
            texture_upload.get("owned_lifecycle_helpers") != [{
                "address": "0x0012d5a0",
                "symbol": "?function_12d5a0@@YA_NPAUs_bitmap_data@@J_N1J@Z",
                "role": "physical block allocation, cache entry creation, upload request scheduling, and priority state"}, {
                "address": "0x0012d6d0",
                "symbol": "?function_12d6d0@@YAXPBUs_bitmap_texture_view@@JPAUD3DTexture@@@Z",
                "role": "level-specific Xbox hardware texture header construction"}, {
                "address": "0x00213760",
                "symbol": "?function_213760@@YAJKJPAXPAKPA_NJJ@Z",
                "role": "cache-location file selection and asynchronous read routing"}, {
                "address": "0x001a0f10",
                "symbol": "?function_1a0f10@@YAJUs_file_handle@@PAXKKJJPAKPC_N@Z",
                "role": "zero-initialized asynchronous file-read task construction and queue submission"}, {
                "address": "0x001a0e70",
                "symbol": "?function_1a0e70@@YAJPATs_async_task@@@Z",
                "role": "chunked positioned file-read worker and completion detection"}, {
                "address": "0x00120ba0",
                "symbol": "?function_120ba0@@YAJJPATs_async_task@@JP6AJ0@ZPC_N@Z",
                "role": "async node allocation, task capture, callback/category setup, and work-list insertion"}, {
                "address": "0x001208b0",
                "symbol": "?function_1208b0@@YAPAUs_job_node@@XZ",
                "role": "mutex-guarded free-node acquisition and cooperative retry"}, {
                "address": "0x00120900",
                "symbol": "?function_120900@@YAXPAUs_job_node@@@Z",
                "role": "priority and positioned-read work-list insertion"}, {
                "address": "0x001209c0",
                "symbol": "?async_task_queue@@YAJPAUs_job_node@@@Z",
                "role": "mutex-guarded task ID assignment and worker semaphore wakeup"}, {
                "address": "0x001a1720",
                "symbol": "?async_task_should_run_before@@YA_NJJPBTs_async_task@@0PAUs_async_insert_state@@P6AJPAT1@@Z3@Z",
                "role": "stable priority and positioned-read ordering policy"}, {
                "address": "0x00120ce0",
                "symbol": "?function_120ce0@@YA_NJJ@Z",
                "role": "mutex-guarded live-task reprioritization and ordered work-list reinsertion"}, {
                "address": "0x00120d50",
                "symbol": "?function_120d50@@YAXPC_N_N@Z",
                "role": "cooperative completion wait with optional main-thread idle servicing"}, {
                "address": "0x00120a30",
                "symbol": "?function_120a30@@YAXPAUs_job_node@@@Z",
                "role": "mutex-guarded completed-task removal from the work list"}, {
                "address": "0x00120c40",
                "symbol": "?async_work_loop@@YAXXZ",
                "role": "worker callback execution, retry signaling, completion publication, and free-list recycling"}, {
                "address": "0x00120c30",
                "symbol": "?async_thread_proc@@YAKPAX@Z",
                "role": "worker-thread entry into the non-returning async work loop"}, {
                "address": "0x00120a90",
                "symbol": "?function_120a90@@YAXXZ",
                "role": "150-node free-list, synchronization handles, worker stack, and thread initialization"}, {
                "address": "0x00120b50",
                "symbol": "?function_120b50@@YA_NJ@Z",
                "role": "mutex-guarded pending-category query"}, {
                "address": "0x00120bf0",
                "symbol": "?function_120bf0@@YAJXZ",
                "role": "mutex-guarded pending-task count"}, {
                "address": "0x0012d160",
                "symbol": "?texture_cache_entry_state@@YAEJ@Z",
                "role": "texture entry flag and pending-state classification"}, {
                "address": "0x0012d1a0",
                "symbol": "?texture_cache_entry_busy@@YA_NJ@Z",
                "role": "native async residency and cache pin busy policy without a fabricated D3D fence"}, {
                "address": "0x0012d200",
                "symbol": "?texture_cache_block_delete@@YAXJ@Z",
                "role": "completed texture eviction, bitmap residency cleanup, and texture-record release"}] or
            texture_upload.get("allocation_provider_imports") != [
                "?bitmap_get_mipmap_count@@YAFFFFF_NF@Z",
                "?datum_new_at_index_with_salt@@YAJPAUs_record_pool@@J@Z",
                "?function_1365a0@@YAJPBUs_type_7ba8e9@@J@Z",
                "?function_13d370@@YAJPAUs_physical_object@@JJ@Z",
                "?async_globals@@3Us_async_globals@@A", "ReleaseSemaphore",
                "?function_1a0e70@@YAJPATs_async_task@@@Z", "h2_guest_memset",
                "ReleaseMutex",
                "SwitchToThread", "WaitForSingleObject", "h2_guest_memcpy",
                "?g_450768@@3PAY0BI@JA", "?g_4e6454@@3PAUs_record_pool@@A",
                "?g_4e6460@@3KA", "?g_4e6464@@3PAUs_physical_object@@A",
                "?g_4e6484@@3JA", "?g_557c90@@3PAUs_texture_cache_file@@A",
                "?g_55aca8@@3JA", "?log2_floor@@YAJK@Z"] or
            texture_upload.get("async_read_provider_imports") != [
                "?async_globals@@3Us_async_globals@@A", "ReleaseSemaphore",
                "?function_1a0e70@@YAJPATs_async_task@@@Z", "h2_guest_memset",
                "ReleaseMutex",
                "SwitchToThread", "WaitForSingleObject", "h2_guest_memcpy",
                "?g_557c90@@3PAUs_texture_cache_file@@A", "?g_55aca8@@3JA"] or
            texture_upload.get("async_request_provider_imports") != [
                "?async_globals@@3Us_async_globals@@A", "ReleaseSemaphore",
                "?function_1a0e70@@YAJPATs_async_task@@@Z", "h2_guest_memset",
                "ReleaseMutex",
                "SwitchToThread", "WaitForSingleObject", "h2_guest_memcpy"] or
            texture_upload.get("async_worker_imports") != [
                "?g_46e488@@3KA", "ReadFile", "SetFilePointer"] or
            texture_upload.get("async_queue_provider_imports") != [
                "?async_globals@@3Us_async_globals@@A", "ReleaseSemaphore",
                "?function_1a0e70@@YAJPATs_async_task@@@Z", "h2_guest_memset",
                "ReleaseMutex", "SwitchToThread", "WaitForSingleObject", "h2_guest_memcpy"] or
            texture_upload.get("async_free_node_imports") != [
                "?async_globals@@3Us_async_globals@@A", "ReleaseMutex",
                "SwitchToThread", "WaitForSingleObject"] or
            texture_upload.get("async_work_queue_imports") != [
                "?async_globals@@3Us_async_globals@@A",
                "?function_1a0e70@@YAJPATs_async_task@@@Z", "ReleaseMutex",
                "ReleaseSemaphore", "WaitForSingleObject", "h2_guest_memset"] or
            texture_upload.get("async_ordering_imports") != [
                "?function_1a0e70@@YAJPATs_async_task@@@Z"] or
            texture_upload.get("async_reprioritize_imports") != [
                "?async_globals@@3Us_async_globals@@A",
                "?function_1a0e70@@YAJPATs_async_task@@@Z", "ReleaseMutex",
                "WaitForSingleObject", "h2_guest_memset"] or
            texture_upload.get("async_wait_imports") != [
                "?function_125d60@@YAXXZ", "SwitchToThread"] or
            texture_upload.get("async_worker_lifecycle_imports") != [
                "?async_globals@@3Us_async_globals@@A", "?g_510810@@3PAUs_job_node@@A",
                "ReleaseMutex", "ReleaseSemaphore", "WaitForSingleObject"] or
            texture_upload.get("async_work_removal_imports") != [
                "?async_globals@@3Us_async_globals@@A", "ReleaseMutex",
                "WaitForSingleObject"] or
            texture_upload.get("async_initialization_imports") != [
                "?async_globals@@3Us_async_globals@@A", "?async_thread_proc@@YAKPAX@Z",
                "?g_5020c8@@3Us_thread_stack@@A", "CreateMutexA", "CreateSemaphoreA",
                "CreateThread", "SetThreadPriority", "h2_guest_memset"] or
            texture_upload.get("async_query_imports") != [
                "?async_globals@@3Us_async_globals@@A", "ReleaseMutex",
                "WaitForSingleObject"] or
            texture_upload.get("helper_dependencies") != [
                "?bitmap_get_mipmap_count@@YAFFFFF_NF@Z",
                "?function_1365a0@@YAJPBUs_type_7ba8e9@@J@Z",
                "?g_450768@@3PAY0BI@JA", "?log2_floor@@YAJK@Z"] or
            texture_upload.get("lifecycle_requirements") != [
                "cache_request_allocation", "resident_block_ownership",
                "source_pixel_data_ownership", "initialized_cache_pools",
                "asynchronous_upload_completion", "fallback_texture_routing"] or
            not texture_upload.get("blocker")):
        raise RuntimeError("particle texture upload frontier changed")

    render_reset = json.loads((
        args.root / "port/runtime/particle_render_state_reset_frontier.json"
    ).read_text())
    expected_render_reset_functions = {
        ("0x00016b10", 117, "?function_16b10@@YAXPAUs_render_reset_state@@@Z"),
        ("0x00018900", 153, "?function_18900@@YAXPBUs_render_state_source@@PBG@Z"),
    }
    render_reset_functions = {
        (entry.get("address"), entry.get("size"), entry.get("symbol"))
        for entry in render_reset.get("retail_functions", [])
    }
    if (render_reset.get("version") != 1 or
            render_reset_functions != expected_render_reset_functions or
            render_reset.get("execution_gate") != "native_particle_render_state_owned" or
            render_reset.get("reset_state_size") != 5152 or
            render_reset.get("binding_slot_count") != 4 or
            render_reset.get("packed_entry_bytes") != 5 or
            render_reset.get("newly_exposed") != ["D3DDevice_SetRenderStateNotInline"] or
            not render_reset.get("blocker")):
        raise RuntimeError("particle render-state reset frontier changed")

    sprite_setup = json.loads((
        args.root / "port/runtime/particle_sprite_setup_frontier.json"
    ).read_text())
    expected_setup_calls = {
        "0x0001bbf0", "0x0001c590", "0x0001c6b0", "0x0001c710",
        "0x0001ccb0", "0x0001cf50", "0x00137bd0", "0x0023e460",
        "0x0023ecd0", "0x003f5cf0", "0x003f7330",
    }
    expected_setup_expansion = {
        "?function_1bbf0@@YAXJJJJJM@Z", "?function_1c590@@YAXPAUs_shader_cache@@JJ@Z",
        "?function_1c6b0@@YAXPAX@Z", "?function_1c710@@YAXPAX@Z",
        "?function_1ccb0@@YAXJ@Z", "?function_1cf50@@YAXXZ",
        "?function_23e460@@YAXXZ", "?function_23ecd0@@YAXXZ",
        "?g_470a38@@3EA", "?g_470a39@@3EA", "?g_4b8348@@3KA",
        "?g_51f0f0@@3PAEA", "D3DDevice_SetRenderState",
        "D3DDevice_SetVertexShaderConstant",
    }
    if (sprite_setup.get("version") != 1 or
            sprite_setup.get("retail_function") != "0x0023ea80" or
            sprite_setup.get("size") != 588 or
            sprite_setup.get("execution_gate") != "native_particle_camera_and_shader_state_owned" or
            set(sprite_setup.get("direct_calls", [])) != expected_setup_calls or
            sprite_setup.get("production_score_before") != 33 or
            sprite_setup.get("production_score_after") != 46 or
            set(sprite_setup.get("newly_exposed", [])) != expected_setup_expansion or
            not sprite_setup.get("blocker")):
        raise RuntimeError("particle sprite setup frontier changed")

    sprite_geometry = json.loads((
        args.root / "port/runtime/particle_sprite_geometry_frontier.json"
    ).read_text())
    expected_geometry_expansion = {
        "?function_015d00@@YA_NJPAPAK@Z", "?g_470a38@@3EA",
        "?g_470a3c@@3PAJA", "D3DDevice_EndPush",
    }
    packet = sprite_geometry.get("packet", {})
    if (sprite_geometry.get("version") != 1 or
            sprite_geometry.get("retail_function") != "0x0023ed30" or
            sprite_geometry.get("size") != 589 or
            sprite_geometry.get("execution_gate") != "native_particle_push_buffer_owned" or
            sprite_geometry.get("direct_calls") != ["0x00015d00"] or
            sprite_geometry.get("production_score_before") != 33 or
            sprite_geometry.get("production_score_after") != 36 or
            set(sprite_geometry.get("newly_exposed", [])) != expected_geometry_expansion or
            packet.get("reserved_dwords") != 46 or
            packet.get("vertex_attributes") != 8 or
            packet.get("primitive") != 8 or
            packet.get("inline_indices") != [0, 0, 1, 0, 65537, 0, 65536, 0] or
            not sprite_geometry.get("blocker")):
        raise RuntimeError("particle sprite geometry frontier changed")

    push_buffer = json.loads((
        args.root / "port/runtime/particle_push_buffer_frontier.json"
    ).read_text())
    if (push_buffer.get("version") != 1 or
            push_buffer.get("retail_function") != "0x00015d00" or
            push_buffer.get("size") != 104 or
            push_buffer.get("symbol") != "?function_015d00@@YA_NJPAPAK@Z" or
            push_buffer.get("execution_gate") != "native_particle_push_buffer_owned" or
            push_buffer.get("maximum_exclusive_dwords") != 2048 or
            push_buffer.get("sprite_packet_dwords") != 46 or
            push_buffer.get("sprite_geometry_score_before") != 36 or
            push_buffer.get("sprite_geometry_score_after") != 36 or
            push_buffer.get("newly_exposed") != ["D3DDevice_BeginPush"] or
            not push_buffer.get("blocker")):
        raise RuntimeError("particle push-buffer frontier changed")

    expected_effect_score = (
        actual_impact_allocation_score - impact_allocation_new_imports
    ) | set(effect_symbols)
    with tempfile.TemporaryDirectory(prefix="h2-effect-lifecycle-score-") as temporary:
        scored = Path(temporary) / "effect-lifecycle-production.o"
        subprocess.run([
            args.lld, "-r", "--allow-multiple-definition", "-o", scored,
            args.effect_lifecycle_candidate_object,
            args.primary_listener_provider_cluster_object,
            args.provider_cluster_object,
            args.production_object,
        ], check=True)
        actual_effect_score = {
            line.strip().split()[-1]
            for line in subprocess.check_output(
                [args.llvm_nm, "--undefined-only", scored], text=True
            ).splitlines() if line.strip()
        }
    if actual_effect_score != expected_effect_score or len(actual_effect_score) != 33:
        raise RuntimeError("Havok effect-lifecycle candidate score changed")
    effect_undefined = {
        line.strip().split()[-1]
        for line in subprocess.check_output(
            [args.llvm_nm, "--undefined-only", args.effect_lifecycle_candidate_object],
            text=True).splitlines() if line.strip()
    }
    if len(effect_undefined) != 120:
        raise RuntimeError("Havok effect-lifecycle raw closure changed")
    effect_defined = subprocess.check_output(
        [args.llvm_nm, "--defined-only", args.effect_lifecycle_candidate_object], text=True
    )
    if "?function_13b390@@YAMPBXMM@Z" in effect_defined:
        raise RuntimeError("general tag-function evaluator must remain outside dormant effect ownership")
    if ("?effect_new_from_parameters@@YAJPAUs_effect_parameters@@@Z" not in effect_defined or
            "?function_177260@@YAXJ_N@Z" not in effect_defined or
            "?function_259a0@@YAMPAK@Z" not in effect_defined or
            "?function_259d0@@YAMPAKPBDJMM@Z" not in effect_defined or
            "?random_vector_in_cone@@YAXPBTvector3f@@PAT1@PAKMM@Z" not in effect_defined or
            "?function_11c050@@YA_NPBUs_location@@@Z" not in effect_defined or
            "?function_11c080@@YA_NPBUs_location@@@Z" not in effect_defined or
            "?function_11d000@@YAPATvector3f@@PBT1@PAT1@@Z" not in effect_defined or
            "?function_3eb20@@YA_NJ@Z" not in effect_defined or
            "?function_1469f0@@YAJM@Z" not in effect_defined or
            "?function_163080@@YA_NXZ" not in effect_defined or
            "?function_16f3a0@@YAPAUs_player_state@@J@Z" not in effect_defined or
            "?function_189060@@YAJJFMPBTpoint3f@@PBTvector3f@@J@Z" not in effect_defined or
            "?function_1895f0@@YAJPBUs_sound_position@@MJ@Z" not in effect_defined or
            "?function_18a750@@YAJJJ@Z" not in effect_defined or
            "?first_person_weapon_get_markers_internal@@YAFJJPAUs_first_person_marker@@F@Z" not in effect_defined or
            "?particle_system_link@@YAXPAUs_particle_system_datum@@PAJ1@Z" not in effect_defined or
            "?function_1751d0@s_particle_system_datum@@QAAPAUs_effect_particle_system_definition@@XZ" not in effect_defined or
            "?function_175a80@@YAX_NKKPAUs_particle_system_datum@@0@Z" not in effect_defined or
            "?unpack_color3f@@YAPAUcolor3f@@KPAU1@@Z" not in effect_defined or
            "?function_a7640@@YA_NPAUs_effect_object_placement@@@Z" not in effect_defined or
            "?function_b5990@@YAJJ_N@Z" not in effect_defined or
            "?function_bad50@@YA_NJJPATpoint3f@@@Z" not in effect_defined or
            "?function_baf40@@YAJJ@Z" not in effect_defined or
            "?function_166244@@YAJJ@Z" not in effect_defined or
            "?function_166283@@YA_NJJ@Z" not in effect_defined or
            "?function_16658d@@YAJJJ@Z" not in effect_defined or
            "?function_d6660@@YAXPAUs_type_1e6529@@J@Z" not in effect_defined or
            "?g_47d8e0@@3FA" not in effect_defined or
            "?function_b7930@@YAXPAXJJPBUs_effect_owner@@@Z" not in effect_defined or
            "?function_badc0@@YAPAUs_object@@JK@Z" not in effect_defined or
            "?function_173fd0@@YAJPAUs_effect_particle_system_definition@@JJFJ@Z" not in effect_defined or
            "?function_178af0@@YA_NJ@Z" not in effect_defined or
            "?function_178b30@@YAXJJJ@Z" not in effect_defined or
            "?g_47ff88@@3JA" not in effect_defined or
            "?function_137bd0@@YAPAVc_type_4e7709@@J@Z" not in effect_defined or
            "?function_c0350@@YAXJJJPBTvector3f@@0PBTpoint3f@@M@Z" not in effect_defined or
            "?function_3ebd0@@YA_NPBTvector3f@@PBUtransform4x3f@@PAU2@J@Z" not in effect_defined or
            "?function_146de0@@YAXXZ" not in effect_defined or
            "?function_b7880@@YAXJJPBTpoint3f@@PBTvector3f@@1@Z" not in effect_defined or
            "?function_bba20@@YAXJ@Z" not in effect_defined or
            "?function_2c3b0@@YAXPAUs_interface_function_context@@@Z" not in effect_defined or
            "?function_173c30@@YAMPBUs_particle_system_datum@@J@Z" not in effect_defined or
            "?function_1318d0@@YAPAUhsv3f@@PBUcolor3f@@PAU1@@Z" not in effect_defined or
            "?function_131a00@@YAPAUcolor3f@@PBUhsv3f@@PAU1@@Z" not in effect_defined or
            "?function_131c20@@YAPAUcolor3f@@PBU1@0KMPAU1@@Z" not in effect_defined or
            "?function_131d60@@YAPAUcolor3f@@PBUcolor4f@@0KMPBU1@PAU1@@Z" not in effect_defined or
            "?object_or_parent_hidden@@YA_NJ@Z" not in effect_defined or
            "?function_b9d20@@YA_NJ@Z" not in effect_defined or
            "?function_baf80@@YAJJ@Z" not in effect_defined or
            "?function_bad50@@YA_NJJPATpoint3f@@@Z" not in effect_defined or
            "?function_17ca10@@YAMMF@Z" not in effect_defined or
            "?function_1e4990@@YAJJ@Z" not in effect_defined or
            "?function_ba8c0@@YA_NPBUs_object_function_ab@@JPAM@Z" not in effect_defined or
            "?function_bab40@@YA_NJJPAM@Z" not in effect_defined or
            "?function_108d90@@YA_NJJPAMPA_N@Z" not in effect_defined or
            "?function_10aac0@@YA_NJJPAMPAJ@Z" not in effect_defined or
            "?function_13bb90@@YAMPBUs_tag_data@@MM@Z" not in effect_defined or
            "?function_1ca290@@YAXJJJJMMPBUtransform4x3f@@@Z" not in effect_defined or
            "?function_290250@@YAXJJJJMMPBUtransform4x3f@@@Z" not in effect_defined or
            "?function_1bd50@@YAXPAX@Z" not in effect_defined or
            "?function_173ba0@@YAXKPAX0PBXPAM@Z" not in effect_defined or
            "?function_248df0@@YAMJPAX0PBX@Z" not in effect_defined or
            "?function_4efb0@@YAPATvector3f@@PAT1@@Z" not in effect_defined or
            "?function_142eb0@@YAPAUmatrix3x3@@PBU1@0PAU1@@Z" not in effect_defined or
            "?function_3eb70@@YAMXZ" not in effect_defined or
            "?function_175610@@YAXPAUs_particle_value_cache@@PAUs_particle_system_datum@@PAUs_particle_location_datum@@P6AXPAUs_particle_175610@@0PBTpoint3f@@PBTvector3f@@MPBUcolor4f@@PAX@Z7@Z" not in effect_defined or
            "?function_2243c0@@YAXPAUs_interface_definition@@JJJJJ@Z" not in effect_defined or
            "?function_224550@@YAXPBUs_draw_sprite_seed@@PAUs_draw_value_cache@@PATpoint3f@@2MPBUcolor4f@@PAUs_draw_sprite_context@@@Z" not in effect_defined or
            "?function_016ae0@@YAJM@Z" not in effect_defined or
            "?pack_color4f@@YAKPBUcolor4f@@@Z" not in effect_defined or
            "?function_246cd0@@YAMPBUs_particle_property@@PBM@Z" not in effect_defined or
            "?function_246d80@@YAXPBUs_particle_property@@PBMPAUcolor3f@@@Z" not in effect_defined or
            "?function_13bc00@@YAKPBUs_tag_data@@M@Z" not in effect_defined or
            "?function_17c900@@YAMFM@Z" not in effect_defined or
            "?function_13be80@@YAXPBUs_tag_data@@MPAUcolor3f@@@Z" not in effect_defined or
            "?function_248450@@YAXPAUmatrix3x3@@PBUs_particle_frame@@PATpoint3f@@PAPBU1@PAPBT3@_N@Z" not in effect_defined or
            "h2_native_effect_particle_provider_prt3_object" not in effect_defined or
            "h2_native_effect_particle_provider_prtm_object" not in effect_defined):
        raise RuntimeError("Havok effect-lifecycle candidate lost a required provider")

    listener_frontier_path = (
        args.root / "port/runtime/havok_primary_listener_remaining_frontier.json"
    )
    listener_frontier = json.loads(listener_frontier_path.read_text())
    if listener_frontier.get("version") != 1:
        raise RuntimeError("unsupported primary Havok listener frontier manifest")
    listener_frontier_entries = listener_frontier.get("imports")
    if not isinstance(listener_frontier_entries, list) or len(listener_frontier_entries) != 3:
        raise RuntimeError("primary Havok listener frontier inventory changed")
    allowed_listener_subsystems = {
        "contact_removal", "impact_creation", "contact_processing",
    }
    listener_frontier_symbols = set()
    for entry in listener_frontier_entries:
        if (not isinstance(entry, dict) or not isinstance(entry.get("symbol"), str) or
                entry.get("subsystem") not in allowed_listener_subsystems or
                not isinstance(entry.get("blocker"), str) or
                not entry["blocker"].strip()):
            raise RuntimeError("invalid primary Havok listener frontier entry")
        if "stub" in entry["blocker"].lower():
            raise RuntimeError("primary Havok listener frontier must describe real ownership")
        listener_frontier_symbols.add(entry["symbol"])
    if (len(listener_frontier_symbols) != len(listener_frontier_entries) or
            listener_frontier_symbols != listener_additional_imports):
        raise RuntimeError("primary Havok listener frontier differs from scored imports")

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
