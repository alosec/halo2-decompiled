#!/usr/bin/env python3
"""Validate the cross-linked recovered main-menu ELF core."""

import argparse
import json
from pathlib import Path
import subprocess


EXPECTED_BINDINGS = {
    "?g_4e3b44@@3PAUs_tag_instance@@A": 0x004E3B44,
    "?g_4c99c0@@3Us_online_mutelist_globals@@A": 0x004C99C0,
    "?g_4cf78c@@3PAUs_record_pool@@A": 0x004CF78C,
    "?g_4d8ba0@@3_NA": 0x004D8BA0,
    "?g_4e61c0@@3Us_saved_game_drive@@A": 0x004E61C0,
    "?g_51ea14@@3PAXA": 0x0051EA14,
    "?main_globals@@3Us_main_globals@@A": 0x00547F28,
    "?g_54e8e0@@3PAUs_player_slot@@A": 0x0054E8E0,
    "?g_551ae4@@3_NA": 0x00551AE4,
}

REQUIRED = {
    "??0c_main_menu_screen@@QAA@JJG@Z",
    "??0c_window_channel@@QAA@XZ",
    "??_Ec_main_menu_screen@@UAAPAXI@Z",
    "?function_1473b6@@YA_NPAVc_window_channel@@@Z",
    "?function_16b790@@YAXPAUs_record_pool@@@Z",
    "?data_last_index@@YAJPAUs_record_pool@@@Z",
    "?player_slot_count_active@@YAFXZ",
    "?player_slot_find_controller@@YAJJ@Z",
    "?player_slot_get_profile@@YAXJPAUs_player_profile@@PAJ@Z",
    "?initialize@s_player_slot_profile@@QAAXJ@Z",
    "?function_18f58c@@YAXXZ",
    "?function_1901fc@@YAGXZ",
    "?function_1906b4@@YAXXZ",
    "?function_199ebc@@YAJXZ",
    "?function_199f34@@YAJXZ",
    "?function_0592d0@@YAKXZ",
    "?get_values_4d08@c_class_58d20@@QAA_NPAJ0PAPAE@Z",
    "?function_199e3c@@YAXJ@Z",
    "?function_199f6d@@YAJXZ",
    "?function_217300@@YAJJJ@Z",
    "?function_2523bc@@YAXPAVc_online_task_screen@@@Z",
    "?function_6b640@@YAXJ@Z",
    "?function_6b910@@YAPAUs_type_9df9da@@J@Z",
    "?function_6c7e0@@YA_NXZ",
    "?function_6d080@@YA_NJPAUs_channel_message@@@Z",
    "?online_get_logon_status@@YA_NPAJ@Z",
    "?online_get_logon_users@@YAXPAUXONLINE_USER@@@Z",
    "?online_task_continue@@YAJPAUs_type_9df9da@@@Z",
    "?online_task_poll@@YAJJ@Z",
    "?online_task_screen_finish@@YAXPAVc_online_task_screen@@@Z",
    "?online_team_get_details@@YAXJPBUXUID@@PAUXONLINE_TEAM@@@Z",
    "?online_team_member_get_details@@YAXJPBUXUID@@PAUXONLINE_TEAM_MEMBER@@@Z",
    "?online_team_members_enumerate@@YAJJPBUXUID@@@Z",
    "?online_team_members_enumerate_get_results@@YAXJPAKPAUXUID@@@Z",
    "?online_teams_enumerate_get_results@@YAXJPAKPAUXUID@@@Z",
    "?set_description@c_online_task_screen@@QAAXJ@Z",
    "?network_session_get_membership@@YA_NPAVc_class_58d20@@PAJ1111PAPAUs_session_member@@1PAKPAPAUs_network_session_player@@@Z",
    "?network_session_interface_get_data_4db0@@YAPAEXZ",
    "?online_mutelist_reset@@YAXJ@Z",
    "?function_2523b7@@YA_NJ@Z",
    "?function_2323ab@@YA_NJ@Z",
    "?function_2360c3@@YAXPBUshort_rectangle2d@@PATpoint3f@@@Z",
    "?function_23618e@@YAPAUs_float_rect@@PAU1@MPBUshort_rectangle2d@@@Z",
    "?function_1a47fd@@YAPAXI@Z",
    "?saved_game_file_size_in_blocks@@YAJJ@Z",
    "?saved_game_file_type_size_in_blocks@@YAJJ@Z",
    "?unicode_string_copy@@YAXPAGPBGJ@Z",
    "?g_4e3b44@@3PAUs_tag_instance@@A",
    "?g_54e8e0@@3PAUs_player_slot@@A",
}


def nm_symbols(nm: Path, image: Path, option: str) -> set[str]:
    output = subprocess.check_output(
        [str(nm), option, "--format=posix", str(image)], text=True
    )
    return {
        line.rsplit(maxsplit=3)[0]
        for line in output.splitlines()
        if line.strip()
    }


def nm_values(nm: Path, image: Path) -> dict[str, int]:
    output = subprocess.check_output(
        [str(nm), "--defined-only", "--format=posix", str(image)], text=True
    )
    values: dict[str, int] = {}
    for line in output.splitlines():
        fields = line.rsplit(maxsplit=3)
        if len(fields) >= 3:
            values[fields[0]] = int(fields[2], 16)
    return values


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nm", type=Path, required=True)
    parser.add_argument("--image", type=Path, required=True)
    parser.add_argument("--closure", type=Path, required=True)
    parser.add_argument("--import-plan", type=Path, required=True)
    args = parser.parse_args()

    data = args.image.read_bytes()
    if data[:6] != b"\x7fELF\x02\x01":
        raise RuntimeError("main-menu core is not little-endian ELF64")
    if int.from_bytes(data[16:18], "little") != 1:
        raise RuntimeError("main-menu core is not relocatable ELF")
    if int.from_bytes(data[18:20], "little") != 183:
        raise RuntimeError("main-menu core is not AArch64")

    report = json.loads(args.closure.read_text())
    expected_undefined = {
        entry["symbol"][1:] if entry["symbol"].startswith("_") else entry["symbol"]
        for entry in report["unresolved"]
    }
    plan = json.loads(args.import_plan.read_text())
    host_imports = {
        entry["symbol"] for entry in plan["imports"]
        if entry["category"] == "host_abi"
    }
    expected_undefined -= host_imports
    actual_undefined = nm_symbols(args.nm, args.image, "--undefined-only")
    if actual_undefined != expected_undefined:
        raise RuntimeError(
            "ELF closure differs from selected objects: "
            f"extra={sorted(actual_undefined - expected_undefined)} "
            f"missing={sorted(expected_undefined - actual_undefined)}"
        )

    defined = nm_symbols(args.nm, args.image, "--defined-only")
    missing = REQUIRED - defined
    if missing:
        raise RuntimeError(f"main-menu ELF is missing required definitions: {sorted(missing)}")
    missing_imports = {
        symbol for symbol in host_imports
        if symbol not in defined or f"h2_host_slot_{symbol}" not in defined
    }
    if missing_imports:
        raise RuntimeError(
            f"main-menu ELF is missing host import trampolines: {sorted(missing_imports)}"
        )
    if "?function_16c2b0@@YAJXZ" in defined:
        raise RuntimeError("function extraction retained an unrelated object query")
    if "?function_18f58c@@YAXJ@Z" in defined:
        raise RuntimeError("function extraction retained unrelated player-slot behavior")
    stale_aliases = sorted(symbol for symbol in defined if symbol.startswith("_??_E"))
    if stale_aliases:
        raise RuntimeError(f"Darwin-decorated aliases leaked into ELF: {stale_aliases}")
    values = nm_values(args.nm, args.image)
    for symbol, expected in EXPECTED_BINDINGS.items():
        if values.get(symbol) != expected:
            raise RuntimeError(
                f"fixed binding {symbol} is {values.get(symbol)!r}, expected {expected:#x}"
            )

    print(
        f"cross-linked AArch64 main-menu ELF passed: "
        f"defined={len(defined)} unresolved={len(actual_undefined)}"
    )


if __name__ == "__main__":
    main()
