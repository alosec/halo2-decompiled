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
        "?function_1a0e70@@YAJPATs_async_task@@@Z",
        "?function_1a0f10@@YAJUs_file_handle@@PAXKKJJPAKPC_N@Z",
    }
    if read_defined != expected_read_definitions:
        raise SystemExit(
            f"guest async read definition drift: expected "
            f"{sorted(expected_read_definitions)}, got {sorted(read_defined)}")
    read_imports = symbols(args.nm, args.read_request, "--undefined-only")
    expected_read_imports = {
        "?async_globals@@3Us_async_globals@@A", "?g_46e488@@3KA",
        "ReadFile", "ReleaseMutex", "SetFilePointer", "SwitchToThread",
        "WaitForSingleObject", "h2_guest_memcpy", "h2_guest_memset",
        "h2_native_async_publish_admitted", "h2_native_async_try_admit",
    }
    if read_imports != expected_read_imports:
        raise SystemExit(
            f"guest async read import drift: expected "
            f"{sorted(expected_read_imports)}, got {sorted(read_imports)}")

    abi = args.abi.read_text()
    if "#define H2_HOST_ABI_VERSION 12u" not in abi:
        raise SystemExit("guest async routing requires host ABI version 12")
    for symbol in expected_imports:
        if symbol not in abi:
            raise SystemExit(f"host ABI does not declare {symbol}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
