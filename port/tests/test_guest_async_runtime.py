#!/usr/bin/env python3
import argparse
import subprocess
from pathlib import Path


def symbols(nm: str, obj: Path, flag: str) -> set[str]:
    output = subprocess.check_output([nm, flag, str(obj)], text=True)
    result = set()
    for line in output.splitlines():
        fields = line.split()
        if fields:
            result.add(fields[-1])
    return result


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--nm", required=True)
    parser.add_argument("--object", type=Path, required=True)
    parser.add_argument("--worker-step", type=Path, required=True)
    parser.add_argument("--queue-lifecycle", type=Path, required=True)
    parser.add_argument("--read-request", type=Path, required=True)
    parser.add_argument("--texture-upload-request", type=Path, required=True)
    parser.add_argument("--texture-cache-lifecycle", type=Path, required=True)
    parser.add_argument("--texture-cache-lookup", type=Path, required=True)
    parser.add_argument("--physical-allocator", type=Path, required=True)
    parser.add_argument("--physical-choice", type=Path, required=True)
    parser.add_argument("--record-pool", type=Path, required=True)
    parser.add_argument("--texture-mip-math", type=Path, required=True)
    parser.add_argument("--bitmap-pitch", type=Path, required=True)
    parser.add_argument("--abi", type=Path, required=True)
    args = parser.parse_args()

    defined = symbols(args.nm, args.object, "--defined-only")
    required_definitions = {
        "CreateMutexA", "CreateSemaphoreA", "WaitForSingleObject",
        "ReleaseMutex", "ReleaseSemaphore", "SetFilePointer", "ReadFile",
        "SetThreadPriority", "GetExitCodeThread", "CloseHandle",
        "SwitchToThread", "h2_native_async_try_admit",
        "h2_native_async_publish_admitted", "h2_native_async_abort_admitted",
    }
    missing = sorted(required_definitions - defined)
    if missing:
        raise SystemExit(f"guest async runtime is missing definitions: {missing}")
    if "CreateThread" in defined:
        raise SystemExit("guest async runtime must not fabricate worker entry routing")

    undefined = symbols(args.nm, args.object, "--undefined-only")
    expected_imports = {
        "host_h2_async_create_mutex",
        "host_h2_async_create_semaphore",
        "host_h2_async_wait",
        "host_h2_async_release_mutex",
        "host_h2_async_release_semaphore",
        "host_h2_async_set_file_pointer",
        "host_h2_async_read_file",
        "host_h2_async_set_thread_priority",
        "host_h2_async_thread_exit_code",
        "host_h2_async_close",
        "host_h2_async_yield",
        "host_h2_async_try_admit",
        "host_h2_async_publish_admitted",
        "host_h2_async_abort_admitted",
    }
    if undefined != expected_imports:
        raise SystemExit(
            f"guest async runtime import drift: expected {sorted(expected_imports)}, "
            f"got {sorted(undefined)}")

    worker_defined = symbols(args.nm, args.worker_step, "--defined-only")
    if worker_defined != {"h2_native_async_worker_step"}:
        raise SystemExit(
            f"guest async worker-step definition drift: {sorted(worker_defined)}")
    worker_imports = symbols(args.nm, args.worker_step, "--undefined-only")
    expected_worker_imports = {
        "?async_globals@@3Us_async_globals@@A",
        "?g_510810@@3PAUs_job_node@@A",
        "WaitForSingleObject",
        "ReleaseMutex",
    }
    if worker_imports != expected_worker_imports:
        raise SystemExit(
            f"guest async worker-step import drift: expected "
            f"{sorted(expected_worker_imports)}, got {sorted(worker_imports)}")

    lifecycle_defined = symbols(args.nm, args.queue_lifecycle, "--defined-only")
    expected_lifecycle_definitions = {
        "h2_native_async_queue_initialize", "h2_native_async_queue_teardown"}
    if lifecycle_defined != expected_lifecycle_definitions:
        raise SystemExit(
            f"guest async queue-lifecycle definition drift: expected "
            f"{sorted(expected_lifecycle_definitions)}, "
            f"got {sorted(lifecycle_defined)}")
    lifecycle_imports = symbols(args.nm, args.queue_lifecycle, "--undefined-only")
    expected_lifecycle_imports = {
        "?async_globals@@3Us_async_globals@@A",
        "?g_5020c8@@3Us_thread_stack@@A",
        "?g_510810@@3PAUs_job_node@@A",
        "CloseHandle", "CreateMutexA", "h2_guest_memset",
    }
    if lifecycle_imports != expected_lifecycle_imports:
        raise SystemExit(
            f"guest async queue-lifecycle import drift: expected "
            f"{sorted(expected_lifecycle_imports)}, got {sorted(lifecycle_imports)}")

    read_defined = symbols(args.nm, args.read_request, "--defined-only")
    expected_read_definitions = {
        "?async_task_queue@@YAJPAUs_job_node@@@Z",
        "?function_120900@@YAXPAUs_job_node@@@Z",
        "?function_120ba0@@YAJJPATs_async_task@@JP6AJ0@ZPC_N@Z",
        "?function_120ce0@@YA_NJJ@Z",
        "?function_120d50@@YAXPC_N_N@Z",
        "?function_12c990@@YAPAUD3DTexture@@PAUs_bitmap_data@@J_N11JPA_N2@Z",
        "?function_12d5a0@@YA_NPAUs_bitmap_data@@J_N1J@Z",
        "?function_12d6d0@@YAXPBUs_bitmap_texture_view@@JPAUD3DTexture@@@Z",
        "?function_1a0e70@@YAJPATs_async_task@@@Z",
        "?function_1a0f10@@YAJUs_file_handle@@PAXKKJJPAKPC_N@Z",
        "?function_213760@@YAJKJPAXPAKPA_NJJ@Z",
    }
    if read_defined != expected_read_definitions:
        raise SystemExit(
            f"guest async read definition drift: expected "
            f"{sorted(expected_read_definitions)}, got {sorted(read_defined)}")
    read_imports = symbols(args.nm, args.read_request, "--undefined-only")
    expected_read_imports = {
        "?async_globals@@3Us_async_globals@@A",
        "?bitmap_get_mipmap_count@@YAFFFFF_NF@Z",
        "?datum_new_at_index_with_salt@@YAJPAUs_record_pool@@J@Z",
        "?function_125d60@@YAXXZ",
        "?function_1365a0@@YAJPBUs_type_7ba8e9@@J@Z",
        "?function_13d370@@YAJPAUs_physical_object@@JJ@Z",
        "?g_450768@@3PAY0BI@JA", "?g_46e488@@3KA",
        "?g_4e6454@@3PAUs_record_pool@@A", "?g_4e6460@@3KA",
        "?g_4e6464@@3PAUs_physical_object@@A", "?g_4e6484@@3JA",
        "?g_557c90@@3PAUs_texture_cache_file@@A", "?g_55aca8@@3JA",
        "?log2_floor@@YAJK@Z",
        "ReadFile", "ReleaseMutex", "SetFilePointer", "SwitchToThread",
        "WaitForSingleObject", "h2_guest_memcpy", "h2_guest_memset",
        "h2_native_async_publish_admitted", "h2_native_async_try_admit",
    }
    if read_imports != expected_read_imports:
        raise SystemExit(
            f"guest async read import drift: expected "
            f"{sorted(expected_read_imports)}, got {sorted(read_imports)}")

    upload_defined = symbols(args.nm, args.texture_upload_request, "--defined-only")
    if upload_defined != {
            "?function_12ce00@@YAPAUD3DTexture@@PAUs_bitmap_data@@KM@Z"}:
        raise SystemExit(
            f"guest texture upload-request definition drift: {sorted(upload_defined)}")
    upload_imports = symbols(args.nm, args.texture_upload_request, "--undefined-only")
    expected_upload_imports = {
        "?function_12c990@@YAPAUD3DTexture@@PAUs_bitmap_data@@J_N11JPA_N2@Z",
        "?g_46883c@@3_NA", "?g_46883d@@3_NA", "?g_468841@@3_NA",
        "?g_485ae4@@3PAUD3DResource@@A", "?g_485ae8@@3PAUD3DResource@@A",
        "?g_485aec@@3PAUD3DResource@@A",
        "?g_4e6454@@3PAUs_record_pool@@A",
        "?g_4e6464@@3PAUs_physical_object@@A",
        "?g_4e6468@@3_NA", "?g_4e6470@@3JA", "?g_4e6474@@3JA",
        "?g_4e647a@@3_NA", "?g_4e647b@@3_NA", "?g_4e647c@@3MA",
        "?texture_cache_bitmap_build_texture@@YAXPAUs_bitmap_data@@@Z",
        "?texture_cache_bitmap_level@@YAJPBUs_bitmap_data@@M@Z",
        "?texture_cache_format_scalable@@YA_NJ@Z",
    }
    if upload_imports != expected_upload_imports:
        raise SystemExit(
            f"guest texture upload-request import drift: expected "
            f"{sorted(expected_upload_imports)}, got {sorted(upload_imports)}")

    lifecycle_defined = symbols(
        args.nm, args.texture_cache_lifecycle, "--defined-only")
    expected_lifecycle_definitions = {
        "?texture_cache_block_delete@@YAXJ@Z",
        "?texture_cache_entry_busy@@YA_NJ@Z",
        "?texture_cache_entry_state@@YAEJ@Z",
        "h2_native_texture_cache_flush",
    }
    if lifecycle_defined != expected_lifecycle_definitions:
        raise SystemExit(
            "guest texture cache-lifecycle definition drift: "
            f"{sorted(lifecycle_defined)}")
    lifecycle_imports = symbols(
        args.nm, args.texture_cache_lifecycle, "--undefined-only")
    expected_lifecycle_imports = {
        "?g_4e6454@@3PAUs_record_pool@@A",
        "?g_4e6464@@3PAUs_physical_object@@A",
        "?g_4e6479@@3_NA",
        "?physical_memory_flush@@YAXPAUs_physical_object@@@Z",
        "?record_pool_release@@YAXPAUs_record_pool@@J@Z",
        "SwitchToThread",
    }
    if lifecycle_imports != expected_lifecycle_imports:
        raise SystemExit(
            "guest texture cache-lifecycle import drift: expected "
            f"{sorted(expected_lifecycle_imports)}, got "
            f"{sorted(lifecycle_imports)}")

    lookup_defined = symbols(args.nm, args.texture_cache_lookup, "--defined-only")
    expected_lookup_definitions = {
        "?texture_cache_bitmap_build_texture@@YAXPAUs_bitmap_data@@@Z",
        "?texture_cache_bitmap_level@@YAJPBUs_bitmap_data@@M@Z",
        "?texture_cache_format_scalable@@YA_NJ@Z",
    }
    if lookup_defined != expected_lookup_definitions:
        raise SystemExit(
            f"guest texture cache-lookup definition drift: {sorted(lookup_defined)}")
    lookup_imports = symbols(args.nm, args.texture_cache_lookup, "--undefined-only")
    expected_lookup_imports = {
        "?bitmap_get_mipmap_count@@YAFFFFF_NF@Z",
        "?g_450768@@3PAY0BI@JA", "?g_4e646c@@3JA", "?log2_floor@@YAJK@Z",
    }
    if lookup_imports != expected_lookup_imports:
        raise SystemExit(
            f"guest texture cache-lookup import drift: expected "
            f"{sorted(expected_lookup_imports)}, got {sorted(lookup_imports)}")

    physical_defined = symbols(args.nm, args.physical_allocator, "--defined-only")
    expected_physical_definitions = {
        "?block_delete@s_physical_object@@QAAXJ@Z",
        "?function_13d370@@YAJPAUs_physical_object@@JJ@Z",
        "?physical_memory_flush@@YAXPAUs_physical_object@@@Z",
    }
    if physical_defined != expected_physical_definitions:
        raise SystemExit(
            f"guest physical allocator definition drift: "
            f"{sorted(physical_defined)}")
    physical_imports = symbols(args.nm, args.physical_allocator, "--undefined-only")
    expected_physical_imports = {
        "?function_13d320@@YA_NPBUs_physical_allocator_view@@"
        "PBUs_physical_block_view@@1@Z",
        "?record_pool_allocate@@YAJPAUs_record_pool@@@Z",
        "?record_pool_release@@YAXPAUs_record_pool@@J@Z",
        "h2_guest_memcpy",
    }
    if physical_imports != expected_physical_imports:
        raise SystemExit(
            f"guest physical allocator import drift: expected "
            f"{sorted(expected_physical_imports)}, got {sorted(physical_imports)}")

    choice_defined = symbols(args.nm, args.physical_choice, "--defined-only")
    if choice_defined != {
            "?function_13d320@@YA_NPBUs_physical_allocator_view@@"
            "PBUs_physical_block_view@@1@Z"}:
        raise SystemExit("guest physical allocation choice definition drift")
    if symbols(args.nm, args.physical_choice, "--undefined-only"):
        raise SystemExit("guest physical allocation choice gained an import")

    pool_defined = symbols(args.nm, args.record_pool, "--defined-only")
    expected_pool_definitions = {
        "?record_pool_allocate@@YAJPAUs_record_pool@@@Z",
        "?datum_new_at_index_with_salt@@YAJPAUs_record_pool@@J@Z",
        "?function_16ba00@@YAXPAUs_record_pool@@PAE@Z",
        "?record_pool_release@@YAXPAUs_record_pool@@J@Z",
    }
    if pool_defined != expected_pool_definitions:
        raise SystemExit(
            f"guest record-pool definition drift: {sorted(pool_defined)}")
    if symbols(args.nm, args.record_pool, "--undefined-only") != {
            "h2_guest_memset"}:
        raise SystemExit("guest record-pool import drift")

    mip_defined = symbols(args.nm, args.texture_mip_math, "--defined-only")
    if mip_defined != {
            "?bitmap_get_mipmap_count@@YAFFFFF_NF@Z",
            "?log2_floor@@YAJK@Z"}:
        raise SystemExit(f"guest texture mip-math definition drift: {sorted(mip_defined)}")
    if symbols(args.nm, args.texture_mip_math, "--undefined-only"):
        raise SystemExit("guest texture mip math gained an import")

    pitch_defined = symbols(args.nm, args.bitmap_pitch, "--defined-only")
    if pitch_defined != {"?function_1365a0@@YAJPBUs_type_7ba8e9@@J@Z"}:
        raise SystemExit(f"guest bitmap-pitch definition drift: {sorted(pitch_defined)}")
    if symbols(args.nm, args.bitmap_pitch, "--undefined-only"):
        raise SystemExit("guest bitmap pitch gained an import")

    abi = args.abi.read_text()
    if "#define H2_HOST_ABI_VERSION 15u" not in abi:
        raise SystemExit("guest async and virtual-memory routing requires host ABI version 15")
    for symbol in expected_imports:
        if symbol not in abi:
            raise SystemExit(f"host ABI does not declare {symbol}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
