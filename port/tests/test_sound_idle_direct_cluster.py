#!/usr/bin/env python3
import argparse
import json
import subprocess
import tempfile
from pathlib import Path


def symbols(llvm_nm: Path, object_path: Path, option: str) -> set[str]:
    output = subprocess.check_output(
        [llvm_nm, option, "--extern-only", "--format=posix", object_path],
        text=True)
    return {
        line.split()[0].removeprefix("_")
        for line in output.splitlines() if line
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm-nm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--cluster-object", type=Path, required=True)
    parser.add_argument("--production-object", type=Path, required=True)
    args = parser.parse_args()

    manifest_path = args.root / "port/runtime/sound_idle_direct_cluster.json"
    manifest = json.loads(manifest_path.read_text())
    if manifest.get("version") != 1:
        raise RuntimeError("unsupported sound-idle direct cluster manifest")
    providers = manifest.get("providers")
    expected_providers = [
        ("src/sound_idle_service_native.cpp", "?function_125d60@@YAXXZ"),
        ("src/sound_system_availability_native.cpp", "?sound_system_available@@YAJXZ"),
        ("src/sound_system_update_time_native.cpp", "?sound_system_update_time@@YAXXZ"),
        ("src/sound_voice_chunks_native.cpp", "?sound_voices_update_chunks@@YAXXZ"),
        ("src/sound_playback_update_native.cpp", "?function_1293f0@@YAXXZ"),
        ("src/sound_cache_frame_native.cpp", "?sound_cache_new_frame@@YAXXZ"),
        ("src/sound_driver_pre_update_native.cpp", "?function_21ec00@@YAXXZ"),
        ("src/sound_driver_post_update_native.cpp", "?function_21ee80@@YAXXZ"),
        ("src/sound_driver_effect_refresh_native.cpp", "?function_191550@@YAXXZ"),
        ("src/sound_driver_effect_refresh_native.cpp", "?function_1915f0@@YAXXZ"),
        ("src/unknown_13d170.cpp", "?physical_memory_reset_time@@YAXPAUs_physical_object@@@Z"),
        ("src/sound_driver_channel_usage_native.cpp", "?function_21f360@@YAXPAUs_sound_driver_channel_usage@@@Z"),
        ("src/sound_master_gain_native.cpp", "?function_2197f0@@YAJM@Z"),
        ("src/sound_fade_helpers_native.cpp", "?function_12aff0@@YAMMMM_N@Z"),
        ("src/sound_playback_stop_native.cpp", "?function_127320@@YAXJJ@Z"),
        ("src/sound_playback_reference_native.cpp", "?function_126960@@YAXPAUs_sound_playback@@@Z"),
        ("src/sound_playback_delete_native.cpp", "?sound_playback_delete@@YAXJ@Z"),
        ("src/sound_voice_free_native.cpp", "?sound_voice_free@@YAXF@Z"),
        ("src/sound_voice_release_native.cpp", "?sound_voice_release@@YAXJ@Z"),
        ("src/sound_looping_controller_release_native.cpp", "?looping_sound_controller_release@@YAXJ@Z"),
        ("src/sound_playback_fade_native.cpp", "?function_12a810@@YAJJ@Z"),
        ("src/unknown_2197f0.cpp", "?function_2195f0@@YAMM@Z"),
        ("src/sound_stream_chunk_release_native.cpp", "?sound_stream_release_chunks@@YAXPAUs_sound_stream@@@Z"),
        ("src/sound_stream_update_native.cpp", "?sound_stream_update@@YAXPAUs_sound_stream@@@Z"),
        ("src/sound_voice_chunk_transition_native.cpp", "?sound_voice_update_chunks@@YAXF@Z"),
        ("src/sound_cache_request_native.cpp", "?function_218850@@YAKJPAUs_sound_chunk@@K@Z"),
        ("src/sound_voice_chunk_helpers_native.cpp", "?function_219290@@YAJFPAUs_permutation_group@@@Z"),
        ("src/sound_driver_playback_rate_native.cpp", "?function_21f650@@YAMJJ@Z"),
        ("src/sound_voice_chunk_helpers_native.cpp", "?sound_reference_release@@YAXPAUs_sound_reference_holder@@@Z"),
        ("src/sound_cache_allocation_native.cpp", "?function_218a10@@YAXPAUs_sound_chunk@@J@Z"),
        ("src/sound_cache_file_route_native.cpp", "?function_213760@@YAJKJPAXPAKPA_NJJ@Z"),
        ("src/sound_cache_async_read_native.cpp", "?function_1a0f10@@YAJUs_file_handle@@PAXKKJJPAKPC_N@Z"),
        ("src/sound_cache_async_read_native.cpp", "?function_1a0e70@@YAJPATs_async_task@@@Z"),
        ("src/particle_texture_cache_request_native.cpp", "?function_120ba0@@YAJJPATs_async_task@@JP6AJ0@ZPC_N@Z"),
        ("src/sound_cache_read_limit_native.cpp", "?g_46e488@@3KA"),
        ("src/sound_gain_defaults_native.cpp", "?g_440c48@@3JA"),
        ("src/sound_gain_defaults_native.cpp", "?g_440c4c@@3JA"),
        ("src/sound_gain_defaults_native.cpp", "?g_468834@@3MA"),
        ("src/sound_busy_state_native.cpp", "?function_12be90@@YA_NXZ"),
        ("src/sound_spatialization_native.cpp", "?function_129aa0@@YAXFPAJPAUs_looping_channel_spatialization@@@Z"),
        ("src/sound_spatial_gain_native.cpp", "?function_12a6d0@@YAJFMM@Z"),
        ("src/sound_attenuation_native.cpp", "?function_125060@@YAJPBUs_sound@@J@Z"),
        ("src/sound_cone_attenuation_native.cpp", "?function_12ad30@@YAMPBTvector3f@@PBUs_sound@@J0@Z"),
        ("src/sound_attenuation_native.cpp", "?sound_get_distance_gain@@YAMJPBUs_sound@@M@Z"),
        ("src/sound_attenuation_native.cpp", "?sound_get_inner_cone_angle@@YAMPBUs_sound@@J@Z"),
        ("src/sound_attenuation_native.cpp", "?sound_get_outer_cone_angle@@YAMPBUs_sound@@J@Z"),
        ("src/sound_attenuation_native.cpp", "?sound_get_outer_cone_gain@@YAJPBUs_sound@@J@Z"),
        ("src/unknown_11cc90.cpp", "?function_11cf50@@YAMPBTvector3f@@0@Z"),
        ("src/unknown_030290.cpp", "?function_30bf0@@YAMPATvector3f@@@Z"),
        ("src/unknown_141e10_native.cpp", "?function_142700@@YAPATpoint3f@@PBUtransform4x3f@@PBT1@PAT1@@Z"),
        ("src/sound_source_position_native.cpp", "?function_1427f0@@YAPATvector3f@@PBUtransform4x3f@@PBT1@PAT1@@Z"),
        ("src/sound_source_position_native.cpp", "?sound_source_get_position@@YAXPBUs_sound_location_source@@JPATpoint3f@@@Z"),
        ("src/unknown_188dd0.cpp", "?vector3d_decompress@@YAPATvector3f@@KPAT1@@Z"),
        ("src/sound_class_route_native.cpp", "?function_221810@@YAPAUs_unknown_5c@@F@Z"),
        ("src/sound_impulse_channel_native.cpp", "?function_128ca0@@YAXFJPBUs_looping_channel_spatialization@@@Z"),
        ("src/unknown_12a1b0.cpp", "?function_12a1b0@@YAXFPAUs_looping_track_sound@@PBUs_looping_channel_spatialization@@PAUs_looping_channel_properties@@@Z"),
        ("src/sound_doppler_native.cpp", "?function_12a9d0@@YAMJPBUs_sound_position@@@Z"),
        ("src/sound_effect_inputs_native.cpp", "?function_222770@@YAXJJPBUs_effect_inputs@@PAUs_sound_playback_effects@@@Z"),
        ("src/unknown_222930.cpp", "?function_222930@@YAXJJPAUs_looping_impulse_parameters@@@Z"),
        ("src/sound_effect_inputs_native.cpp", "?function_1914f0@@YAJJ@Z"),
        ("src/unknown_192b90.cpp", "?function_192b90@@YAMPBTpoint3f@@J_N@Z"),
        ("src/sound_get_class.cpp", "?sound_get_class@@YAPAUs_sound_class_definition@@J@Z"),
        ("src/unknown_18d090.cpp", "?function_18d090@@YAPAXJJ@Z"),
        ("src/unknown_219110.cpp", "?function_2194a0@@YAPAEPAUs_object_ref@@@Z"),
        ("src/sound_playback_reference_native.cpp", "?function_125e60@@YAJPAUs_looping_track_sound@@@Z"),
        ("src/sound_playback_gain_native.cpp", "?function_1251e0@@YAJPBXJM@Z"),
        ("src/tag_function_general_native.cpp", "?function_13b390@@YAMPBXMM@Z"),
        ("src/tag_function_periodic_native.cpp", "?function_17c900@@YAMFM@Z"),
        ("src/light_curve_lookup_native.cpp", "?function_17ca10@@YAMMF@Z"),
        ("src/tag_function_general_native.cpp", "g_44178c"),
        ("src/unknown_218ac0.cpp", "?function_218f50@@YAFPAUs_looping_playback_definition@@FM@Z"),
        ("src/unknown_219110.cpp", "?function_219110@@YAFPAUs_set_ref@@FPAKF1PA_N_N@Z"),
        ("src/unknown_219110.cpp", "?function_2192b0@@YAXPAUs_animation_state@@PAUs_animation_ref@@E@Z"),
        ("src/unknown_219110.cpp", "?function_219370@@YAJPAUs_animation_state@@PAUs_animation_ref@@@Z"),
        ("src/unknown_219110.cpp", "?function_2194c0@@YAKPAUs_packed_value@@PAUs_animation_ref@@@Z"),
        ("src/unknown_219110.cpp", "?function_219560@@YAXPAUs_packed_value@@PAUs_animation_ref@@K@Z"),
        ("src/sound_looping_voice_policy_native.cpp", "?function_12a420@@YAXF@Z"),
        ("src/sound_looping_voice_policy_native.cpp", "?function_128a60@@YAFJFPBF@Z"),
        ("src/sound_looping_voice_policy_native.cpp", "?function_128500@@YAXJPAUs_looping_voice_counts@@@Z"),
        ("src/sound_looping_chunk_policy_native.cpp", "?sound_playback_set_chunk@@YAXJJDDF@Z"),
        ("src/sound_looping_chunk_policy_native.cpp", "?sound_voice_queue_chunk@@YAXFPBUs_sound_permutation@@F@Z"),
        ("src/sound_stream_chunk_release_native.cpp", "?sound_stream_add_chunk@@YAXPAUs_sound_stream@@PAUs_sound_chunk@@@Z"),
        ("src/tag_function_range_native.cpp", "?function_13bb90@@YAMPBUs_tag_data@@MM@Z"),
        ("src/sound_looping_impulse_submit_native.cpp", "?function_21f720@@YAXPBUs_looping_impulse_parameters@@@Z"),
        ("src/sound_stream_envelope_native.cpp", "?sound_stream_set_envelope@@YAXPAUs_sound_stream@@MM@Z"),
        ("src/sound_looping_channel_start_native.cpp", "?function_21f8a0@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@@Z"),
        ("src/sound_driver_filter_helpers_native.cpp", "?function_21e330@@YAJM@Z"),
        ("src/sound_driver_filter_helpers_native.cpp", "?function_219880@@YAMPBUs_sound_random_parameter@@PAKM@Z"),
        ("src/sound_driver_filter_coefficients_native.cpp", "?sound_filter_frequency_coefficient@@YAJM@Z"),
        ("src/sound_driver_filter_coefficients_native.cpp", "?sound_filter_gain_coefficient@@YAKM@Z"),
        ("src/sound_channel_parameters_native.cpp", "?function_2201f0@@YAXJPBUs_sound_channel_parameters@@_N@Z"),
        ("src/sound_effect_data_submit_native.cpp", "?function_21f960@@YAXKPBK@Z"),
        ("src/sound_mixbin_submit_native.cpp", "?sound_mixbins_add@@YAXPAUs_mixbin_settings@@JM@Z"),
        ("src/sound_mixbin_submit_native.cpp", "?function_2215d0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundStream@@@Z"),
        ("src/sound_mixbin_submit_native.cpp", "?function_2216f0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundBuffer@@@Z"),
        ("src/sound_looping_channel_update_native.cpp", "?function_21fa80@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@_N@Z"),
        ("src/unknown_12a1b0.cpp", "?function_21c400@@YAXFMPBUs_looping_channel_spatialization@@@Z"),
        ("src/unknown_12a1b0.cpp", "g_44a150"),
        ("src/unknown_12a1b0.cpp", "g_44a16c"),
        ("src/unknown_12a1b0.cpp", "g_44a188"),
        ("src/sound_stream_flush_native.cpp", "?sound_stream_flush@@YAXPAUs_sound_stream@@@Z"),
        ("src/voice_xhv_dispose_native.cpp", "?voice_xhv_dispose@@YAXPAVc_voice_xhv@@@Z"),
        ("src/unknown_192b90.cpp", "g_444c84"),
    ]
    if [(entry.get("source"), entry.get("symbol", entry.get("global")))
            for entry in providers] != expected_providers:
        raise RuntimeError("sound-idle provider inventory changed")
    provider_symbols = [entry.get("symbol", entry.get("global"))
                        for entry in providers]
    if len(set(provider_symbols)) != 103:
        raise RuntimeError("sound-idle direct provider symbols are not unique")
    for entry in providers:
        if ("symbol" in entry) == ("global" in entry):
            raise RuntimeError("sound-idle provider kind is ambiguous")
        source = args.root / entry.get("source", "")
        if not source.is_file():
            raise RuntimeError(f"sound-idle provider source is absent: {source}")

    if manifest.get("compact_helpers") != [{
            "symbol": "?physical_memory_reset_time@@YAXPAUs_physical_object@@@Z",
            "semantics":
                "rebase every live physical-cache block timestamp after the "
                "clock reaches its maximum"}, {
            "symbol":
                "?function_21f360@@YAXPAUs_sound_driver_channel_usage@@@Z",
            "semantics":
                "snapshot per-state channel occupancy, free voices, and the "
                "three paired driver counts"}, {
            "symbol": "?function_2197f0@@YAJM@Z",
            "semantics":
                "clamp linear gain and encode the recovered -64 through 0 "
                "decibel range"}, {
            "symbol": "?function_12aff0@@YAMMMM_N@Z",
            "semantics":
                "bounded linear interpolation with optional reversed "
                "direction"}]:
        raise RuntimeError("sound-idle compact helper inventory changed")
    if manifest.get("playback_stop_helpers") != [{
            "symbol": "?function_127320@@YAXJJ@Z",
            "semantics":
                "stop a live playback in recovered voice, reference, callback, "
                "looping-controller, and record-release order"}, {
            "symbol": "?function_126960@@YAXPAUs_sound_playback@@@Z",
            "semantics":
                "release the playback-held sound cache reference selected "
                "through authentic tag permutation data"}, {
            "symbol": "?sound_playback_delete@@YAXJ@Z",
            "semantics":
                "adjust and release the optional looping controller before "
                "releasing the playback record"}, {
            "symbol": "?sound_voice_free@@YAXF@Z",
            "semantics":
                "unlock queued and current chunks, flush the live stream, "
                "clear ownership, and release the shared voice"}, {
            "symbol": "?sound_voice_release@@YAXJ@Z",
            "semantics":
                "clear sound-to-voice ownership and release the shared voice "
                "record at zero references"}, {
            "symbol": "?looping_sound_controller_release@@YAXJ@Z",
            "semantics":
                "preserve seven-bit looping-controller reference semantics "
                "and release at zero"}]:
        raise RuntimeError("sound-idle playback-stop inventory changed")
    if manifest.get("playback_fade_helpers") != [{
            "symbol": "?function_12a810@@YAJJ@Z",
            "semantics":
                "anchor NONE-relative fade times, evaluate linear or power "
                "gain curves, and return pinned decibel bits"}, {
            "symbol": "?function_2195f0@@YAMM@Z",
            "semantics":
                "pin decibels to the recovered -64 through 0 range and "
                "convert to linear gain"}]:
        raise RuntimeError("sound-idle playback-fade inventory changed")
    voice_transition_helpers = manifest.get("voice_transition_helpers", [])
    if [entry.get("symbol") for entry in voice_transition_helpers] != [
            "?sound_voice_update_chunks@@YAXF@Z",
            "?sound_stream_update@@YAXPAUs_sound_stream@@@Z",
            "?sound_stream_release_chunks@@YAXPAUs_sound_stream@@@Z",
            "?function_218850@@YAKJPAUs_sound_chunk@@K@Z",
            "?function_219290@@YAJFPAUs_permutation_group@@@Z",
            "?function_21f650@@YAMJJ@Z",
            "?sound_reference_release@@YAXPAUs_sound_reference_holder@@@Z"]:
        raise RuntimeError("sound-idle voice-transition inventory changed")
    if any(not entry.get("semantics") for entry in voice_transition_helpers):
        raise RuntimeError("sound-idle voice-transition semantics are missing")
    cache_helpers = manifest.get("sound_cache_allocation_helpers", [])
    if [entry.get("symbol") for entry in cache_helpers] != [
            "?function_218a10@@YAXPAUs_sound_chunk@@J@Z",
            "?function_213760@@YAJKJPAXPAKPA_NJJ@Z"]:
        raise RuntimeError("sound-idle cache-allocation inventory changed")
    if any(not entry.get("semantics") for entry in cache_helpers):
        raise RuntimeError("sound-idle cache-allocation semantics are missing")
    async_read_helpers = manifest.get("sound_cache_async_read_helpers", [])
    if [entry.get("symbol") for entry in async_read_helpers] != [
            "?function_1a0f10@@YAJUs_file_handle@@PAXKKJJPAKPC_N@Z",
            "?function_1a0e70@@YAJPATs_async_task@@@Z"]:
        raise RuntimeError("sound-idle async-read inventory changed")
    if any(not entry.get("semantics") for entry in async_read_helpers):
        raise RuntimeError("sound-idle async-read semantics are missing")
    queue_helpers = manifest.get("sound_cache_async_queue_helpers", [])
    if [entry.get("symbol") for entry in queue_helpers] != [
            "?function_120ba0@@YAJJPATs_async_task@@JP6AJ0@ZPC_N@Z",
            "?async_task_queue@@YAJPAUs_job_node@@@Z",
            "?function_120900@@YAXPAUs_job_node@@@Z"]:
        raise RuntimeError("sound-idle async-queue inventory changed")
    if any(not entry.get("semantics") for entry in queue_helpers):
        raise RuntimeError("sound-idle async-queue semantics are missing")
    read_limit = manifest.get("sound_cache_read_limit", {})
    if read_limit.get("symbol") != "?g_46e488@@3KA" or \
            read_limit.get("value") != "0x40000" or \
            not read_limit.get("semantics"):
        raise RuntimeError("sound-idle cache-read limit changed")
    cache_state_bindings = manifest.get("production_cache_state_bindings")
    expected_cache_state_bindings = {
        "?g_55aca8@@3JA": "0x0055aca8",
        "?g_557c90@@3PAUs_texture_cache_file@@A": "0x00557c90",
    }
    if not isinstance(cache_state_bindings, list) or \
            len(cache_state_bindings) != 2 or \
            any(not entry.get("semantics") for entry in cache_state_bindings) or \
            {entry.get("symbol"): entry.get("address")
             for entry in cache_state_bindings} != expected_cache_state_bindings:
        raise RuntimeError("sound-idle production cache-state bindings changed")
    main_menu_bindings = json.loads((
        args.root / "port/runtime/main_menu_bindings.json").read_text())
    binding_addresses = {
        entry.get("symbol"): entry.get("address")
        for entry in main_menu_bindings.get("symbols", [])}
    for symbol, address in expected_cache_state_bindings.items():
        if binding_addresses.get(symbol) != address:
            raise RuntimeError(
                f"sound-idle production cache-state binding is absent: {symbol}")
    gain_defaults = manifest.get("sound_gain_defaults")
    expected_gain_defaults = {
        "?g_440c48@@3JA": "0x00000000",
        "?g_440c4c@@3JA": "0xc2800000",
        "?g_468834@@3MA": "1.0",
    }
    if not isinstance(gain_defaults, list) or len(gain_defaults) != 3 or \
            any(not entry.get("semantics") for entry in gain_defaults) or \
            {entry.get("symbol"): entry.get("value")
             for entry in gain_defaults} != expected_gain_defaults:
        raise RuntimeError("sound-idle gain defaults changed")
    busy_state = manifest.get("busy_state_provider", {})
    if busy_state.get("symbol") != "?function_12be90@@YA_NXZ" or \
            not busy_state.get("semantics") or \
            busy_state.get("mutable_state") != [
                "?main_globals@@3Us_main_globals@@A",
                "?g_4ed39d@@3_NA",
                "?g_4e6470@@3JA",
                "?g_4e64a0@@3JA"]:
        raise RuntimeError("sound-idle busy-state ownership changed")
    spatialization = manifest.get("spatialization_provider", {})
    if spatialization.get("symbol") != \
            "?function_129aa0@@YAXFPAJPAUs_looping_channel_spatialization@@@Z" or \
            not spatialization.get("semantics") or \
            not spatialization.get("execution_gate"):
        raise RuntimeError("sound-idle spatialization ownership changed")
    spatial_gain = manifest.get("spatial_gain_provider", {})
    if spatial_gain.get("symbol") != "?function_12a6d0@@YAJFMM@Z" or \
            not spatial_gain.get("semantics") or \
            not spatial_gain.get("execution_gate"):
        raise RuntimeError("sound-idle spatial-gain ownership changed")
    directional_cone = manifest.get("directional_cone_provider", {})
    if directional_cone.get("symbol") != "?function_125060@@YAJPBUs_sound@@J@Z" or \
            not directional_cone.get("semantics") or \
            not directional_cone.get("execution_gate"):
        raise RuntimeError("sound-idle directional-cone ownership changed")
    cone_closure = manifest.get("cone_attenuation_closure", {})
    if cone_closure.get("root") != \
            "?function_12ad30@@YAMPBTvector3f@@PBUs_sound@@J0@Z" or \
            len(cone_closure.get("providers", [])) != 6 or \
            not cone_closure.get("semantics") or \
            not cone_closure.get("execution_gate"):
        raise RuntimeError("sound-idle cone-attenuation closure changed")
    source_transforms = manifest.get("source_transform_closure", {})
    if len(source_transforms.get("providers", [])) != 4 or \
            not source_transforms.get("semantics") or \
            not source_transforms.get("execution_gate"):
        raise RuntimeError("sound-idle source-transform closure changed")
    class_route = manifest.get("sound_class_route_provider", {})
    if class_route.get("symbol") != "?function_221810@@YAPAUs_unknown_5c@@F@Z" or \
            not class_route.get("semantics") or \
            not class_route.get("execution_gate"):
        raise RuntimeError("sound-idle sound-class routing changed")
    impulse_closure = manifest.get("impulse_channel_closure", {})
    if impulse_closure.get("root") != \
            "?function_128ca0@@YAXFJPBUs_looping_channel_spatialization@@@Z" or \
            len(impulse_closure.get("providers", [])) != 3 or \
            not impulse_closure.get("semantics") or \
            not impulse_closure.get("execution_gate"):
        raise RuntimeError("sound-idle impulse-channel closure changed")
    effect_routes = manifest.get("effect_routing_closure", {})
    if len(effect_routes.get("roots", [])) != 2 or \
            effect_routes.get("recursive_definitions") != [
                "?function_2222c0@@YAXPAUs_effect_data_block@@PBUs_effect_component@@PBUs_effect_component_overrides@@PBUs_effect_inputs@@J@Z",
                "?function_191660@@YAKJJ@Z"] or \
            not effect_routes.get("semantics") or \
            not effect_routes.get("execution_gate"):
        raise RuntimeError("sound-idle effect-routing closure changed")
    effect_lookups = manifest.get("effect_lookup_closure", {})
    if len(effect_lookups.get("providers", [])) != 5 or \
            effect_lookups.get("immutable_speaker_directions") != [
                [0.7071067690849304, 0.7071067690849304, 0.0],
                [0.7071067690849304, -0.7071067690849304, 0.0],
                [-0.7071067690849304, 0.7071067690849304, 0.0],
                [-0.7071067690849304, -0.7071067690849304, 0.0],
                [0.7071067690849304, 0.0, 0.0]
            ] or not effect_lookups.get("semantics") or \
            not effect_lookups.get("execution_gate"):
        raise RuntimeError("sound-idle effect-lookup closure changed")
    speaker_source = (args.root / "src/unknown_192b90.cpp").read_text()
    for fragment in ["static point3f const g_444c84[5]", "front left",
                     "front right", "back left", "back", "center"]:
        if fragment not in speaker_source:
            raise RuntimeError(
                f"sound-idle speaker table lost {fragment}")
    channel_scalars = manifest.get("channel_parameter_scalar_closure", {})
    if channel_scalars.get("providers") != [
            "?function_21e330@@YAJM@Z",
            "?function_219880@@YAMPBUs_sound_random_parameter@@PAKM@Z",
            "?sound_filter_frequency_coefficient@@YAJM@Z",
            "?sound_filter_gain_coefficient@@YAKM@Z"
            ] or not channel_scalars.get("semantics") or \
            not channel_scalars.get("execution_gate"):
        raise RuntimeError("sound-idle channel parameter scalar closure changed")
    channel_submission = manifest.get("channel_parameter_submission_closure", {})
    if channel_submission.get("provider") != \
            "?function_2201f0@@YAXJPBUs_sound_channel_parameters@@_N@Z" or \
            channel_submission.get("filter_modes") != {
                "bypass": 0,
                "dls2": 1,
                "parametric_equalizer": 2,
                "multiband": 3
            } or not channel_submission.get("semantics") or \
            len(channel_submission.get("execution_gates", [])) != 4:
        raise RuntimeError("sound-idle channel parameter submission closure changed")
    effect_data = manifest.get("effect_data_submission_closure", {})
    if effect_data.get("provider") != "?function_21f960@@YAXKPBK@Z" or \
            effect_data.get("packed_header_size") != 16 or \
            effect_data.get("effect_map_stride") != 32 or \
            not effect_data.get("semantics") or \
            len(effect_data.get("execution_gates", [])) != 4:
        raise RuntimeError("sound-idle effect data submission closure changed")
    mixbins = manifest.get("mixbin_submission_closure", {})
    if mixbins.get("providers") != [
            "?sound_mixbins_add@@YAXPAUs_mixbin_settings@@JM@Z",
            "?function_2215d0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundStream@@@Z",
            "?function_2216f0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundBuffer@@@Z"
            ] or mixbins.get("pair_capacity") != 8 or \
            not mixbins.get("semantics") or \
            len(mixbins.get("execution_gates", [])) != 3:
        raise RuntimeError("sound-idle mixbin submission closure changed")
    xhv_dispose = manifest.get("xhv_dispose_closure", {})
    if xhv_dispose.get("source") != "src/voice_xhv_dispose_native.cpp" or \
            xhv_dispose.get("provider") != \
            "?voice_xhv_dispose@@YAXPAVc_voice_xhv@@@Z" or \
            xhv_dispose.get("checked_layout") != {
                "xuid_size": 12,
                "initialized_offset": 4,
                "flags_offset": 12,
                "engine_offset": 16
            } or xhv_dispose.get("retail_verified_policy") != {
                "local_ports": 4,
                "remote_capacity": 30,
                "remote_enabled_mask": 4,
                "user_flags": 0,
                "local_unregister":
                    "?UnregisterLocalTalker@CXHVEngine@XHVNamespace@@QAGJK@Z",
                "remote_enumeration":
                    "?GetRemoteTalkers@CXHVEngine@XHVNamespace@@QAGJPAKPAU_XUID@@@Z",
                "remote_unregister":
                    "?UnregisterRemoteTalker@CXHVEngine@XHVNamespace@@QAGJU_XUID@@@Z",
                "engine_release":
                    "?Release@CXHVEngine@XHVNamespace@@QAGKXZ"
            } or not xhv_dispose.get("semantics") or \
            len(xhv_dispose.get("execution_gates", [])) != 3:
        raise RuntimeError("sound-idle XHV disposal closure changed")
    xhv_source = (args.root / xhv_dispose["source"]).read_text()
    for fragment in [
            "for (dword port = 0; port < 4; port++)",
            "_XUID talkers[30]",
            "xhv_get_remote_talkers",
            "xhv_unregister_remote_talker",
            "xhv_release(xhv->engine)",
            "xhv->engine = 0",
            "xhv->initialized = false"]:
        if fragment not in xhv_source:
            raise RuntimeError(f"sound-idle XHV disposal lost {fragment}")
    stream_flush = manifest.get("stream_flush_closure", {})
    if stream_flush.get("source") != "src/sound_stream_flush_native.cpp" or \
            stream_flush.get("provider") != \
            "?sound_stream_flush@@YAXPAUs_sound_stream@@@Z" or \
            stream_flush.get("checked_layout") != {
                "stream_size": 52,
                "state_offset": 1,
                "flags_offset": 3,
                "interface_offset": 36,
                "codec_offset": 48
            } or stream_flush.get("retail_verified_constants") != {
                "paused_status_bit": 131072,
                "resume_argument": 0,
                "async_envelope_flags": 3,
                "codec_stop_vtable_offset": 28
            } or not stream_flush.get("semantics") or \
            len(stream_flush.get("execution_gates", [])) != 3:
        raise RuntimeError("sound-idle stream flush closure changed")
    stream_flush_source = (args.root / stream_flush["source"]).read_text()
    for fragment in [
            "k_stream_status_paused = 0x00020000",
            "k_stream_pause_resume = 0",
            "k_stream_flush_async_envelope = 3",
            "IDirectSoundStream_FlushEx",
            "stream->codec->stop()",
            "stream->flags |= 4"]:
        if fragment not in stream_flush_source:
            raise RuntimeError(
                f"sound-idle stream flush lost {fragment}")
    looping_root = manifest.get("looping_channel_state_machine_closure", {})
    if looping_root.get("source") != "src/unknown_12a1b0.cpp" or \
            looping_root.get("root") != \
            "?function_21c400@@YAXFMPBUs_looping_channel_spatialization@@@Z" or \
            looping_root.get("recursive_definitions") != [
                "?function_21c190@@YA_NPAUs_type_5ef569@@PAUs_type_06b94f@@JPAK@Z",
                "?looping_sound_get_permutations@@YAKPAUs_type_5ef569@@PAUlooping_sound_definition@@PAUs_looping_playback_definition@@JF_N@Z",
                "?looping_sound_set_permutations@@YAXPAUs_type_5ef569@@PAUlooping_sound_definition@@PAUs_looping_playback_definition@@JFKF@Z"
            ] or not looping_root.get("semantics") or \
            looping_root.get("source_owned_transition_tables") != [
                "g_44a150", "g_44a16c", "g_44a188"
            ] or len(looping_root.get("new_mutable_imports", [])) != 3 or \
            len(looping_root.get("execution_gates", [])) != 3:
        raise RuntimeError("sound-idle looping channel closure changed")
    looping_source = (args.root / looping_root["source"]).read_text()
    for fragment in [
            "function_21f8a0(voice->channel_index",
            "function_21c190(sound, track",
            "sound_playback_set_chunk(voice->sound_index",
            "sound_voice_queue_chunk(voice_index",
            "function_21fa80(",
            "g_44a150[7] = { 1, 1, 2, 5, 5, 1, 6 }",
            "g_44a16c[7] = { 3, 3, 2, 4, 4, 3, 6 }",
            "g_44a188[7] = { 20, 28, 36, 64, 44, 72, 52 }"]:
        if fragment not in looping_source:
            raise RuntimeError(
                f"sound-idle looping root lost {fragment}")
    shared_update = manifest.get("shared_channel_update_closure", {})
    expected_update_symbol = \
        "?function_21fa80@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@_N@Z"
    if shared_update.get("source") != \
            "src/sound_looping_channel_update_native.cpp" or \
            shared_update.get("symbol") != expected_update_symbol or \
            shared_update.get("checked_layouts") != {
                "channel_update_input_size": 72,
                "channel_levels_offset": 56,
                "sound_stream_size": 52,
                "driver_channels_offset": 12,
                "driver_voices_offset": 6680,
                "driver_effects_offset": 10932
            } or shared_update.get("source_owned_providers") != [
                "?function_2201f0@@YAXJPBUs_sound_channel_parameters@@_N@Z",
                "?sound_mixbins_add@@YAXPAUs_mixbin_settings@@JM@Z",
                "?function_2215d0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundStream@@@Z",
                "?function_2216f0@@YAXPAUs_mixbin_list@@PAUs_mixbin_settings@@PAUIDirectSoundBuffer@@@Z"
            ] or len(shared_update.get("directsound_operations", [])) != 9 or \
            len(shared_update.get("effects_virtual_methods", [])) != 4 or \
            not shared_update.get("semantics") or \
            len(shared_update.get("execution_gates", [])) != 4 or \
            not shared_update.get("prohibited_substitution"):
        raise RuntimeError("sound-idle shared channel update frontier changed")
    shared_source = (args.root / shared_update["source"]).read_text()
    for fragment in [
            "IDirectSoundStream_GetVoiceProperties",
            "function_2215d0",
            "IDirectSoundBuffer_GetVoiceProperties",
            "function_2216f0",
            "channel_effects(globals)->add_effect_sends",
            "IDirectSoundStream_SetMixBins",
            "IDirectSoundStream_SetPitch",
            "function_2201f0"]:
        if fragment not in shared_source:
            raise RuntimeError(
                f"sound-idle shared channel source lost {fragment}")
    looping_channel_start = manifest.get("looping_channel_start_closure", {})
    if looping_channel_start.get("providers") != [
            "?sound_stream_set_envelope@@YAXPAUs_sound_stream@@MM@Z",
            "?function_21f8a0@@YAXJPBUs_looping_channel_properties@@PBUs_looping_effect_playback@@@Z"
            ] or not looping_channel_start.get("semantics") or \
            not looping_channel_start.get("execution_gate"):
        raise RuntimeError("sound-idle looping channel start closure changed")
    looping_impulse = manifest.get("looping_impulse_submission_closure", {})
    if looping_impulse.get("providers") != [
            "?function_13bb90@@YAMPBUs_tag_data@@MM@Z",
            "?function_21f720@@YAXPBUs_looping_impulse_parameters@@@Z"
            ] or not looping_impulse.get("semantics") or \
            not looping_impulse.get("execution_gate"):
        raise RuntimeError("sound-idle looping impulse closure changed")
    looping_chunk_policy = manifest.get("looping_chunk_policy_closure", {})
    if looping_chunk_policy.get("providers") != [
            "?sound_playback_set_chunk@@YAXJJDDF@Z",
            "?sound_voice_queue_chunk@@YAXFPBUs_sound_permutation@@F@Z",
            "?sound_stream_add_chunk@@YAXPAUs_sound_stream@@PAUs_sound_chunk@@@Z"
            ] or not looping_chunk_policy.get("semantics") or \
            not looping_chunk_policy.get("execution_gate"):
        raise RuntimeError("sound-idle looping chunk policy closure changed")
    looping_voice_policy = manifest.get("looping_voice_policy_closure", {})
    if looping_voice_policy.get("providers") != [
            "?function_128500@@YAXJPAUs_looping_voice_counts@@@Z",
            "?function_128a60@@YAFJFPBF@Z",
            "?function_12a420@@YAXF@Z"
            ] or not looping_voice_policy.get("semantics") or \
            not looping_voice_policy.get("execution_gate"):
        raise RuntimeError("sound-idle looping voice policy closure changed")
    looping_selection = manifest.get("looping_selection_closure", {})
    if looping_selection.get("providers") != [
            "?function_218f50@@YAFPAUs_looping_playback_definition@@FM@Z",
            "?function_219110@@YAFPAUs_set_ref@@FPAKF1PA_N_N@Z",
            "?function_2192b0@@YAXPAUs_animation_state@@PAUs_animation_ref@@E@Z",
            "?function_219370@@YAJPAUs_animation_state@@PAUs_animation_ref@@@Z",
            "?function_2194c0@@YAKPAUs_packed_value@@PAUs_animation_ref@@@Z",
            "?function_219560@@YAXPAUs_packed_value@@PAUs_animation_ref@@K@Z"
            ] or not looping_selection.get("semantics") or \
            not looping_selection.get("execution_gate"):
        raise RuntimeError("sound-idle looping selection closure changed")
    general_function = manifest.get("general_function_closure", {})
    if general_function.get("root") != "?function_13b390@@YAMPBXMM@Z" or \
            general_function.get("providers") != [
                "?function_13b390@@YAMPBXMM@Z",
                "?function_17c900@@YAMFM@Z",
                "?function_17ca10@@YAMMF@Z",
                "g_44178c"
            ] or general_function.get("numeric_boundary") != \
            "?h2_xbox_rsqrtss@@YAMM@Z" or \
            not general_function.get("semantics") or \
            not general_function.get("execution_gate"):
        raise RuntimeError("sound-idle general-function closure changed")
    playback_gain = manifest.get("playback_gain_closure", {})
    if playback_gain.get("root") != "?function_1251e0@@YAJPBXJM@Z" or \
            playback_gain.get("recursive_definitions") != [
                "?function_127010@@YAJF@Z",
                "?sound_definition_gain_lower@@YAJPBUs_sound_definition@@@Z",
                "?sound_definition_gain_upper@@YAJPBUs_sound_definition@@@Z"
            ] or not playback_gain.get("semantics") or \
            not playback_gain.get("execution_gate"):
        raise RuntimeError("sound-idle playback gain closure changed")
    playback_chunk_abi = manifest.get("playback_chunk_abi", {})
    if playback_chunk_abi != {
            "symbol": "?sound_playback_set_chunk@@YAXJJDDF@Z",
            "parameters": ["long", "long", "char", "char", "short"],
            "rejected_widened_symbol":
                "?sound_playback_set_chunk@@YAXJJFFF@Z",
            "semantics":
                "preserve the recovered signed byte pitch-range and "
                "permutation indices plus signed short chunk index"}:
        raise RuntimeError("sound-idle playback chunk ABI changed")
    playback_reference = manifest.get("playback_reference_closure", {})
    if playback_reference.get("root") != \
            "?function_125e60@@YAJPAUs_looping_track_sound@@@Z" or \
            playback_reference.get("recursive_definitions") != [
                "?sound_playback_acquire_reference@@YAXPAUs_sound_playback@@@Z"
            ] or not playback_reference.get("semantics") or \
            not playback_reference.get("execution_gate"):
        raise RuntimeError("sound-idle playback-reference closure changed")

    expected_definitions = set(manifest.get("definitions", []))
    expected_imports = set(manifest.get("imports", []))
    if len(expected_definitions) != 115 or len(expected_imports) != 98:
        raise RuntimeError("sound-idle provider cluster coverage ratchet changed")
    if playback_chunk_abi["rejected_widened_symbol"] in expected_imports:
        raise RuntimeError("widened playback chunk ABI returned to the cluster")
    if not set(provider_symbols).issubset(expected_definitions):
        raise RuntimeError("sound-idle direct provider definition is missing")
    if manifest.get("production_linked") is not False:
        raise RuntimeError("sound-idle direct cluster unexpectedly marked linked")
    if manifest.get("execution_enabled") is not False:
        raise RuntimeError("sound-idle direct cluster unexpectedly marked executable")
    if not manifest.get("blocker"):
        raise RuntimeError("sound-idle direct cluster blocker is missing")

    actual_definitions = symbols(
        args.llvm_nm, args.cluster_object, "--defined-only")
    if actual_definitions != expected_definitions:
        raise RuntimeError(
            "sound-idle direct definitions changed: " +
            repr(sorted(actual_definitions)))
    actual_imports = symbols(
        args.llvm_nm, args.cluster_object, "--undefined-only")
    if actual_imports != expected_imports:
        raise RuntimeError(
            "sound-idle direct imports changed: " +
            repr(sorted(actual_imports)))

    production_definitions = symbols(
        args.llvm_nm, args.production_object, "--defined-only")
    linked = sorted(expected_definitions & production_definitions)
    if linked:
        raise RuntimeError(
            "dormant sound-idle direct providers entered production: " +
            repr(linked))

    expected_production_imports = set(
        manifest.get("production_score_imports", []))
    if len(expected_production_imports) != 63:
        raise RuntimeError("sound-idle production score ratchet changed")
    runtime_frontier = manifest.get("remaining_runtime_frontier", {})
    categories = [
        runtime_frontier.get("mutable_lifetimes", []),
        runtime_frontier.get("backend_operations", []),
        runtime_frontier.get("numeric_boundaries", [])]
    if [len(category) for category in categories] != [17, 36, 10]:
        raise RuntimeError("sound-idle runtime frontier category changed")
    flattened = [symbol for category in categories for symbol in category]
    if len(flattened) != len(set(flattened)) or \
            set(flattened) != expected_production_imports:
        raise RuntimeError(
            "sound-idle runtime frontier no longer partitions production")
    controller = runtime_frontier.get("controller_storage_lifetime", {})
    if controller.get("storage") != "?g_51ebdc@@3PAXA" or \
            controller.get("record_pool") != "?g_51ebd8@@3PAXA" or \
            controller.get("initializer") != \
            "?looping_sound_controllers_initialize@@YA_NXZ" or \
            controller.get("startup_owner") != \
            "?function_1253d0@@YAXXZ" or \
            controller.get("teardown_owner") != \
            "?function_125600@@YAXXZ" or \
            controller.get("capacity") != 64 or \
            controller.get("alignment") != 4 or \
            len(controller.get("required_order", [])) != 7 or \
            not controller.get("semantics") or \
            not controller.get("prohibited_shortcut"):
        raise RuntimeError("sound-idle controller storage lifetime changed")
    controller_source = (args.root / controller["source"]).read_text()
    teardown_source = (args.root / controller["teardown_source"]).read_text()
    for fragment in [
            "looping_sound_controllers_initialize(void)",
            "function_11cc20(64, \"sound playback controllers\"",
            "((dword)top + 3) & ~3",
            "function_163ba0(&g_510808_pool_checksum",
            "g_51ebdc = memory",
            "record_pool_release_all(CONTROLLERS)"]:
        if fragment not in controller_source:
            raise RuntimeError(
                f"sound-idle controller initialization lost {fragment}")
    startup_body = teardown_source[
        teardown_source.index("// @retail 0x1253d0"):]
    startup_order = [
        startup_body.index("looping_sound_controllers_initialize()"),
        startup_body.index("function_21a0c0()"),
        startup_body.index("sound_effects_initialize()"),
        startup_body.index("sound_voices_initialize("),
        startup_body.index("function_21e4b0(&counts)"),
        startup_body.index("system->initialized = true")]
    if startup_order != sorted(startup_order):
        raise RuntimeError("sound-idle controller startup order changed")
    for fragment in [
            "void function_125600(void)",
            "if (g_51ebd8)",
            "g_51ebd8 = NULL",
            "if (g_51ebdc)",
            "g_51ebdc = NULL"]:
        if fragment not in teardown_source:
            raise RuntimeError(
                f"sound-idle controller teardown lost {fragment}")
    if manifest.get("production_inadequate_resolutions") != [{
            "symbol": "?voice_start_engine@@YAXXZ",
            "provider": "port/runtime/guest_online_runtime.cpp",
            "reason":
                "the current native-offline provider is intentionally empty "
                "and cannot satisfy DSP recovery restart semantics"}]:
        raise RuntimeError("sound-idle inadequate production resolution changed")
    with tempfile.TemporaryDirectory(
            prefix="h2-sound-idle-direct-score-") as temporary:
        scored = Path(temporary) / "scored.o"
        subprocess.run([
            args.lld, "-r", "--allow-multiple-definition",
            "-o", scored, args.cluster_object, args.production_object,
        ], check=True)
        production_imports = symbols(
            args.llvm_nm, scored, "--undefined-only")
    if production_imports != expected_production_imports:
        raise RuntimeError(
            "sound-idle production score changed: " +
            repr(sorted(production_imports)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
