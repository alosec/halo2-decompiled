#!/usr/bin/env python3
import argparse
import subprocess
import tempfile
from pathlib import Path


def run(command, cwd=None):
    subprocess.run([str(value) for value in command], check=True, cwd=cwd)


def compile_slice(args, source, output, symbols, globals=()):
    command = [
        args.python, args.root / "port/tools/guest_elf_cc.py",
        "--llvm", args.llvm,
        "--plugin", args.plugin,
        "--include", args.root / "port/include",
        "--include", args.root / "port/include/crt",
        "--include", args.root / "port/include/xdk",
        "--include", args.root / "include",
        "--ms-abi",
    ]
    for symbol in symbols:
        command.extend(("--extract-symbol", symbol))
    for global_name in globals:
        command.extend(("--extract-global", global_name))
    source_argument = source.relative_to(args.root)
    command.extend(("--output", output, source_argument))
    run(command, cwd=args.root)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--python", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--impact-output", type=Path, required=True)
    parser.add_argument("--effect-output", type=Path, required=True)
    args = parser.parse_args()

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.impact_output.parent.mkdir(parents=True, exist_ok=True)
    args.effect_output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="h2-contact-material-") as temporary:
        temporary = Path(temporary)
        lookup = temporary / "lookup.o"
        properties = temporary / "properties.o"
        velocity = temporary / "velocity.o"
        rigid_velocity = temporary / "rigid-velocity.o"
        combine = temporary / "combine.o"
        impact_data = temporary / "impact-data.o"
        impact_validity = temporary / "impact-validity.o"
        impact_match = temporary / "impact-match.o"
        impact_find = temporary / "impact-find.o"
        impact_tolerance = temporary / "impact-tolerance.o"
        impact_allocation = temporary / "impact-allocation.o"
        game_time = temporary / "game-time.o"
        vector_normalize = temporary / "vector-normalize.o"
        vector_compress = temporary / "vector-compress.o"
        contact_properties = temporary / "contact-properties.o"
        rigid_body_matrix = temporary / "rigid-body-matrix.o"
        structure_location = temporary / "structure-location.o"
        effect_state = temporary / "effect-state.o"
        effect_distance = temporary / "effect-distance.o"
        effect_selection = temporary / "effect-selection.o"
        looping_sound_state = temporary / "looping-sound-state.o"
        contact_creation = temporary / "contact-creation.o"
        motion_basis = temporary / "motion-basis.o"
        active_body = temporary / "active-body.o"
        contact_removal = temporary / "contact-removal.o"
        audio_boundary = temporary / "audio-boundary.o"
        effect_audio = temporary / "effect-audio.o"
        effect_lifecycle = temporary / "effect-lifecycle.o"
        random_unit = temporary / "random-unit.o"
        random_range = temporary / "random-range.o"
        random_cone = temporary / "random-cone.o"
        location_bits = temporary / "location-bits.o"
        perpendicular = temporary / "perpendicular.o"
        current_cluster = temporary / "current-cluster.o"
        seconds_to_ticks = temporary / "seconds-to-ticks.o"
        game_state = temporary / "game-state.o"
        player_state = temporary / "player-state.o"
        first_person = temporary / "first-person.o"
        particle_helpers = temporary / "particle-helpers.o"
        particle_color = temporary / "particle-color.o"
        unpack_color = temporary / "unpack-color.o"
        object_placement = temporary / "object-placement.o"
        object_type = temporary / "object-type.o"
        object_geometry = temporary / "object-geometry.o"
        candidate = temporary / "candidate.o"
        compile_slice(args, args.root / "src/unknown_1ee410_native.cpp", lookup,
                      ["?function_1ee410@@YAXPBUs_lookup_source@@PAF@Z"])
        compile_slice(args, args.root / "src/unknown_182020.cpp", properties,
                      ["?function_182b90@@YAXPAVc_material_shape@@PBVhkEntity@@PAM2PAF@Z"])
        compile_slice(args, args.root / "src/unknown_0b8bd0.cpp", velocity,
                      ["?function_ba1d0@@YAXJPATvector3f@@0@Z"])
        compile_slice(
            args, args.root / "src/unknown_1cec30.cpp", rigid_velocity,
            ["?havok_component_rigid_body_linear_velocity_get@@YAXJPAUs_havok_component@@PATvector3f@@@Z",
             "?havok_component_rigid_body_angular_velocity_get@@YAXJPAUs_havok_component@@PATvector3f@@@Z"])
        compile_slice(args, args.root / "src/unknown_277ec0.cpp", combine,
                      ["?function_277ec0@@YAXPAM0PAF1PAUs_277ec0@@2JJ@Z"])
        compile_slice(
            args, args.root / "src/unknown_2055f0.cpp", impact_data,
            ["?impact_data_set@@YAXPAUs_impact_data@@_NJJVc_type_47f957@@JJ2PBTpoint3f@@PBTvector3f@@JPBUs_physics_model_shape_key@@@Z",
             "?havok_component_impact_add@@YAXPAUs_havok_component@@J@Z"])
        compile_slice(args, args.root / "src/impacts.cpp", impact_validity,
                      ["?impact_components_valid@@YA_NJJ@Z"])
        compile_slice(args, args.root / "src/impacts.cpp", impact_match,
                      ["?impact_matches_data@@YA_NPAUs_impact@@PBUs_impact_data@@_N@Z",
                       "?impacts_last_sorted@@YAJXZ",
                       "?impact_distance_squared_to_nearest_player@@YAMPBTpoint3f@@J@Z"])
        compile_slice(
            args, args.root / "src/unknown_1d5460.cpp", impact_find,
            ["?havok_component_impact_find@@YAJPAUs_havok_component@@PBUs_havok_impact_contact@@_N@Z",
             "?havok_component_impact_make_room@@YA_NJPAUs_havok_component@@M@Z"])
        compile_slice(args, args.root / "src/impact_match_tolerance_native.cpp",
                      impact_tolerance, [], ["?g_55c2d0@@3MA"])
        compile_slice(
            args, args.root / "src/unknown_277b80.cpp", candidate,
            ["?function_277b80@@YAJPAX_NMPAVc_1cf520@@FPAUs_277ec0@@3PAVhkEntity@@4PAPAVc_277b80@@@Z"])
        run([args.lld, "-r", "--allow-multiple-definition", "-o", args.output,
             lookup, properties, velocity, rigid_velocity, combine, impact_data,
             impact_validity, impact_match, impact_find, impact_tolerance,
             candidate])
        compile_slice(
            args, args.root / "src/impacts.cpp", impact_allocation,
            ["?impact_new@@YAJPBUs_impact_data@@J@Z",
             "?impact_component_b_is_faster@@YA_NJJ@Z",
             "?impact_local_positions_update@@YAXPAUs_impact@@JJ@Z",
             "?function_2266a0@@YAXJ@Z",
             "?function_226a60@@YAXJ@Z"])
        compile_slice(args, args.root / "src/unknown_146240.cpp", game_time,
                      ["?function_146650@@YAJXZ"])
        compile_slice(args, args.root / "src/unknown_11fc80.cpp", vector_normalize,
                      ["?function_1201a0@@YAMPATvector3f@@PBT1@@Z"])
        compile_slice(args, args.root / "src/unknown_188dd0.cpp", vector_compress,
                      ["?vector3d_compress@@YAKPBTvector3f@@@Z"])
        compile_slice(args, args.root / "src/unknown_0dc3a0.cpp", contact_properties,
                      ["?havok_component_contact_properties_get@@YAXPBUs_havok_component@@JPAJ1@Z"])
        compile_slice(args, args.root / "src/unknown_1cec30.cpp", rigid_body_matrix,
                      ["?havok_component_rigid_body_matrix_get@@YAXJPAUs_havok_component@@PAUtransform4x3f@@@Z"])
        compile_slice(args, args.root / "src/unknown_11c050.cpp", structure_location,
                      ["?function_11c120@@YA_NPBUs_location@@PBTpoint3f@@PAF@Z"])
        compile_slice(args, args.root / "src/unknown_175bd0.cpp", effect_state,
                      ["?function_17b030@@YA_NJPBTvector3f@@MMPBUtransform4x3f@@PBM@Z"])
        compile_slice(args, args.root / "src/unknown_170d70.cpp", effect_distance,
                      ["?function_172750@@YA_NJPBTpoint3f@@M@Z"])
        compile_slice(
            args, args.root / "src/unknown_187ec0.cpp", effect_selection,
            ["?function_1883e0@@YAXJ_NPBTpoint3f@@FJJJPAJ22222@Z"],
            ["switch.table.?function_1883e0@@YAXJ_NPBTpoint3f@@FJJJPAJ22222@Z",
             "switch.table.?function_1883e0@@YAXJ_NPBTpoint3f@@FJJJPAJ22222@Z.2"])
        compile_slice(
            args, args.root / "src/unknown_18a2f0.cpp", looping_sound_state,
            ["?function_18a5e0@@YAPAGJ@Z", "?function_18a6c0@@YAJJJ@Z",
             "?function_18a720@@YAXJ_N@Z"])
        compile_slice(
            args, args.root / "src/unknown_1cec30.cpp", contact_creation,
            ["?function_1d0400@@YAJPAUs_havok_component@@FRBXJJJDPAVhkEntity@@2MMFF@Z"])
        compile_slice(
            args, args.root / "src/havok_motion_basis_native.cpp", motion_basis,
            ["?function_181db0@@YA_NJPATvector3f@@@Z",
             "?function_355e0@@YA_NJJPAM0@Z"])
        compile_slice(
            args, args.root / "src/unknown_1cec30.cpp", active_body,
            ["?havok_component_any_rigid_body_active@@YA_NPAUs_havok_component@@@Z"])
        compile_slice(
            args, args.root / "src/unknown_1cec30.cpp", contact_removal,
            ["?function_1d05d0@@YAXPBVhkEntity@@0PAUs_havok_component@@PBF@Z"])
        compile_slice(
            args, args.root / "src/impact_audio_boundary_native.cpp", audio_boundary,
            ["?function_189650@@YAJPBUs_sound_position@@PBUs_sound_label_play@@@Z"])
        run([args.lld, "-r", "--allow-multiple-definition", "-o",
             args.impact_output, args.output, impact_allocation, game_time,
             vector_normalize, vector_compress, contact_properties,
             rigid_body_matrix, structure_location, effect_state,
             effect_distance, effect_selection, looping_sound_state,
             contact_creation, motion_basis, active_body, contact_removal,
             audio_boundary])
        compile_slice(
            args, args.root / "src/unknown_175bd0.cpp", effect_lifecycle,
            ["?effect_new_from_parameters@@YAJPAUs_effect_parameters@@@Z",
             "?function_177260@@YAXJ_N@Z"],
            ["??_C@_0BH@MAKJFDPB@src?1unknown_175bd0?4cpp?$AA@"])
        compile_slice(
            args, args.root / "src/impact_audio_boundary_native.cpp", effect_audio,
            ["?function_189060@@YAJJFMPBTpoint3f@@PBTvector3f@@J@Z",
             "?function_1895f0@@YAJPBUs_sound_position@@MJ@Z",
             "?function_18a750@@YAJJJ@Z"])
        compile_slice(args, args.root / "src/unknown_0259a0.cpp", random_unit,
                      ["?function_259a0@@YAMPAK@Z"])
        compile_slice(args, args.root / "src/unknown_0259d0.cpp", random_range,
                      ["?function_259d0@@YAMPAKPBDJMM@Z"])
        compile_slice(
            args, args.root / "src/unknown_146240.cpp", random_cone,
            ["?random_vector_in_cone@@YAXPBTvector3f@@PAT1@PAKMM@Z"],
            ["??_C@_0BH@GEOAMOEG@src?1unknown_146240?4cpp?$AA@"])
        compile_slice(
            args, args.root / "src/unknown_11c050.cpp", location_bits,
            ["?function_11c050@@YA_NPBUs_location@@@Z",
             "?function_11c080@@YA_NPBUs_location@@@Z"])
        compile_slice(
            args, args.root / "src/unknown_11cc90.cpp", perpendicular,
            ["?function_11d000@@YAPATvector3f@@PBT1@PAT1@@Z"])
        compile_slice(args, args.root / "src/unknown_03eb20.cpp", current_cluster,
                      ["?function_3eb20@@YA_NJ@Z"])
        compile_slice(args, args.root / "src/unknown_1469f0.cpp", seconds_to_ticks,
                      ["?function_1469f0@@YAJM@Z"])
        compile_slice(args, args.root / "src/unknown_163080.cpp", game_state,
                      ["?function_163080@@YA_NXZ"])
        compile_slice(args, args.root / "src/unknown_16f3a0.cpp", player_state,
                      ["?function_16f3a0@@YAPAUs_player_state@@J@Z"])
        compile_slice(
            args, args.root / "src/first_person_marker_native.cpp", first_person,
            ["?first_person_weapon_get_markers_internal@@YAFJJPAUs_first_person_marker@@F@Z"])
        compile_slice(
            args, args.root / "src/unknown_173b90.cpp", particle_helpers,
            ["?particle_system_link@@YAXPAUs_particle_system_datum@@PAJ1@Z",
             "?function_1751d0@s_particle_system_datum@@QAAPAUs_effect_particle_system_definition@@XZ"])
        compile_slice(args, args.root / "src/unknown_173b90.cpp", particle_color,
                      ["?function_175a80@@YAX_NKKPAUs_particle_system_datum@@0@Z"])
        compile_slice(args, args.root / "src/unknown_1317a0.cpp", unpack_color,
                      ["?unpack_color3f@@YAPAUcolor3f@@KPAU1@@Z"])
        compile_slice(
            args, args.root / "src/unknown_0a58d0.cpp", object_placement,
            ["?function_a7640@@YA_NPAUs_effect_object_placement@@@Z"])
        compile_slice(args, args.root / "src/unknown_0b58c0.cpp", object_type,
                      ["?function_b5990@@YAJJ_N@Z"])
        compile_slice(
            args, args.root / "src/unknown_0bad50.cpp", object_geometry,
            ["?function_bad50@@YA_NJJPATpoint3f@@@Z",
             "?function_baf40@@YAJJ@Z"])
        run([args.lld, "-r", "--allow-multiple-definition", "-o",
             args.effect_output, args.impact_output, effect_lifecycle,
             effect_audio, random_unit, random_range, random_cone, location_bits,
             perpendicular, current_cluster, seconds_to_ticks, game_state,
             player_state, first_person, particle_helpers, particle_color,
             unpack_color, object_placement, object_type, object_geometry])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
