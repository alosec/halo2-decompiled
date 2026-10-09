#!/usr/bin/env python3
"""Validate the fully linked native main-menu guest image."""

import argparse
import struct
import subprocess
from pathlib import Path

PT_LOAD = 1
PF_X = 1
PF_W = 2
PF_R = 4


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nm", type=Path, required=True)
    parser.add_argument("--image", type=Path, required=True)
    args = parser.parse_args()

    data = args.image.read_bytes()
    if data[:6] != b"\x7fELF\x02\x01":
        raise RuntimeError("guest image is not little-endian ELF64")
    if struct.unpack_from("<H", data, 16)[0] != 2:
        raise RuntimeError("guest image is not an executable ELF")
    if struct.unpack_from("<H", data, 18)[0] != 183:
        raise RuntimeError("guest image is not AArch64")

    entry = struct.unpack_from("<Q", data, 24)[0]
    phoff = struct.unpack_from("<Q", data, 32)[0]
    phentsize, phnum = struct.unpack_from("<HH", data, 54)
    loads = []
    for index in range(phnum):
        offset = phoff + index * phentsize
        p_type, flags = struct.unpack_from("<II", data, offset)
        if p_type != PT_LOAD:
            continue
        file_offset, vaddr, _, file_size, memory_size, alignment = struct.unpack_from(
            "<QQQQQQ", data, offset + 8
        )
        if file_offset + file_size > len(data) or memory_size < file_size:
            raise RuntimeError("guest load segment is out of bounds")
        if flags & PF_W and flags & PF_X:
            raise RuntimeError("guest image contains a writable-executable segment")
        if alignment != 0x4000 or vaddr % 0x4000:
            raise RuntimeError("guest load segment is not 16 KiB aligned")
        loads.append((vaddr, memory_size, flags))

    if [flags for _, _, flags in loads] != [PF_R | PF_X, PF_R, PF_R | PF_W]:
        raise RuntimeError(f"unexpected guest segment protections: {loads!r}")
    if not any(start <= entry < start + size and flags & PF_X
               for start, size, flags in loads):
        raise RuntimeError("guest entry point is outside executable memory")

    undefined = subprocess.check_output(
        [str(args.nm), "--undefined-only", "--format=posix", str(args.image)],
        text=True,
    ).strip()
    if undefined:
        raise RuntimeError(f"guest image has undefined symbols:\n{undefined}")

    symbols = subprocess.check_output(
        [str(args.nm), "--defined-only", "--format=posix", str(args.image)],
        text=True,
    )
    symbol_lines = {line.split(maxsplit=1)[0]: line for line in symbols.splitlines()}
    required_symbols = {
        "_Z20cache_files_load_mapPKc",
        "_Z30cache_files_load_structure_bspP25s_structure_bsp_reference",
        "h2_main_menu_guest_bootstrap",
        "h2_main_menu_guest_frame",
        "h2_main_menu_guest_load_strings",
        "h2_main_menu_guest_press_start",
        "h2_main_menu_guest_accept",
        "h2_main_menu_guest_load_selected_campaign",
        "h2_main_menu_guest_map_location_probe",
        "h2_main_menu_guest_game_options_probe",
        "?function_07ad80",
        "?function_11c840",
        "?function_1c2390",
        "?function_205550",
        "?function_2122b0",
        "?function_246c60",
        "?function_2fbb30",
        "?function_2fdfe0",
        "?function_2fbfe0",
        "h2_native_physics_runtime_vtable",
        "h2_native_physics_runtime_reset",
        "h2_native_physics_runtime_object_count",
        "h2_native_physics_model_census",
        "h2_native_tag_initialization_status",
        "?function_138110",
        "?function_1462b0",
        "?function_1537f0",
        "?function_18fd94",
        "?function_23643f",
        "?function_23654b",
        "h2_native_campaign_options_ready",
        "h2_native_campaign_bsp_metadata",
        "h2_native_campaign_load_first_bsp",
        "?network_session_interface_get_user_properties",
        "?map_location_get",
        "?map_location_progress_get",
        "h2_main_menu_guest_up",
        "h2_main_menu_guest_down",
        "h2_main_menu_guest_cancel",
        "h2_native_controller_connect",
        "host_h2_cache_read",
        "host_h2_map_preflight",
        "host_h2_map_select",
        "host_h2_map_is_active",
        "host_h2_physical_memory_malloc_low",
        "host_h2_profile_list",
        "host_h2_profile_create",
        "host_h2_profile_read",
        "host_h2_profile_write",
        "host_h2_profile_delete",
        "h2_native_profile_list",
        "h2_native_profile_create",
        "h2_native_profile_read",
        "h2_native_profile_write",
        "h2_native_profile_delete",
        "h2_native_profile_round_trip",
        "?function_238c21",
        "?function_216120",
        "??_7c_virtual_keyboard_screen",
        "h2_native_virtual_keyboard_accept",
    }
    missing = required_symbols - symbol_lines.keys()
    if missing:
        raise RuntimeError(f"guest image is missing required map bootstrap symbols: {sorted(missing)}")
    entry_lines = [line for line in symbols.splitlines()
                   if line.startswith("h2_main_menu_guest_bootstrap ")]
    if len(entry_lines) != 1:
        raise RuntimeError("guest startup entry symbol is missing or duplicated")
    symbol_value = int(entry_lines[0].rsplit(maxsplit=3)[2], 16)
    if symbol_value != entry:
        raise RuntimeError(
            f"ELF entry {entry:#x} does not match startup symbol {symbol_value:#x}"
        )

    print(
        f"complete main-menu guest image passed: entry={entry:#x} "
        f"segments={len(loads)} undefined=0"
    )


if __name__ == "__main__":
    main()
