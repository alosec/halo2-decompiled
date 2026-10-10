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
        "native_impact_looping_sound_pool_owned",
        "native_havok_motion_basis_state_owned",
        "native_tag_function_tables_initialized",
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
    if not isinstance(effect_entries, list) or len(effect_entries) != 17:
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
    if actual_effect_score != expected_effect_score or len(actual_effect_score) != 32:
        raise RuntimeError("Havok effect-lifecycle candidate score changed")
    effect_undefined = {
        line.strip().split()[-1]
        for line in subprocess.check_output(
            [args.llvm_nm, "--undefined-only", args.effect_lifecycle_candidate_object],
            text=True).splitlines() if line.strip()
    }
    if len(effect_undefined) != 105:
        raise RuntimeError("Havok effect-lifecycle raw closure changed")
    effect_defined = subprocess.check_output(
        [args.llvm_nm, "--defined-only", args.effect_lifecycle_candidate_object], text=True
    )
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
