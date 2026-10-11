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
    ]
    if [(entry.get("source"), entry.get("symbol", entry.get("global")))
            for entry in providers] != expected_providers:
        raise RuntimeError("sound-idle provider inventory changed")
    provider_symbols = [entry.get("symbol", entry.get("global"))
                        for entry in providers]
    if len(set(provider_symbols)) != 64:
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
            not effect_lookups.get("semantics") or \
            not effect_lookups.get("execution_gate"):
        raise RuntimeError("sound-idle effect-lookup closure changed")

    expected_definitions = set(manifest.get("definitions", []))
    expected_imports = set(manifest.get("imports", []))
    if len(expected_definitions) != 69 or len(expected_imports) != 75:
        raise RuntimeError("sound-idle provider cluster coverage ratchet changed")
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
    if len(expected_production_imports) != 45:
        raise RuntimeError("sound-idle production score ratchet changed")
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
