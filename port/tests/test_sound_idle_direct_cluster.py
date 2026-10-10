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
    ]
    if [(entry.get("source"), entry.get("symbol"))
            for entry in providers] != expected_providers:
        raise RuntimeError("sound-idle provider inventory changed")
    provider_symbols = [entry.get("symbol") for entry in providers]
    if len(set(provider_symbols)) != 20:
        raise RuntimeError("sound-idle direct provider symbols are not unique")
    for entry in providers:
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

    expected_definitions = set(manifest.get("definitions", []))
    expected_imports = set(manifest.get("imports", []))
    if len(expected_definitions) != 21 or len(expected_imports) != 44:
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
    if len(expected_production_imports) != 33:
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
