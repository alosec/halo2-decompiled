#!/usr/bin/env python3
"""Validate the cross-linked recovered main-menu ELF core."""

import argparse
import json
from pathlib import Path
import subprocess


EXPECTED_BINDINGS = {
    "g_44a3f0": 0x0044A3F0,
    "?g_4686f8@@3PBUcolor4f@@B": 0x004686F8,
    "?g_468710@@3PATpoint3f@@A": 0x00468710,
    "?g_468734@@3PBUcolor3f@@B": 0x00468734,
    "?g_468724@@3PAUcolor3f@@A": 0x00468724,
    "?g_46873c@@3PBUcolor3f@@B": 0x0046873C,
    "?g_470200@@3PAUs_text_parse_entry@@A": 0x00470200,
    "?g_4e0300@@3PAUs_record_pool@@A": 0x004E0300,
    "?g_4e3b44@@3PAUs_tag_instance@@A": 0x004E3B44,
    "?g_4e696c@@3MA": 0x004E696C,
    "?g_4c99c0@@3Us_online_mutelist_globals@@A": 0x004C99C0,
    "?g_485ac0@@3FA": 0x00485AC0,
    "?g_4cf78c@@3PAUs_record_pool@@A": 0x004CF78C,
    "?g_4cf7cc@@3PAEA": 0x004CF7CC,
    "?g_4d8ba0@@3_NA": 0x004D8BA0,
    "?g_4de2d0@@3PAJA": 0x004DE2D0,
    "?g_4e61c0@@3Us_saved_game_drive@@A": 0x004E61C0,
    "?g_4e9188@@3Us_bink_globals@@A": 0x004E9188,
    "?g_502234@@3PAKA": 0x00502234,
    "?g_502244@@3EA": 0x00502244,
    "?global_preferences_globals@@3Us_global_preferences_globals@@A": 0x00510818,
    "?g_510c49@@3EA": 0x00510C49,
    "?g_510c4c@@3PAUs_510c4c@@A": 0x00510C4C,
    "?g_51ea14@@3PAXA": 0x0051EA14,
    "?main_globals@@3Us_main_globals@@A": 0x00547F28,
    "?g_54d598@@3Vc_window_manager@@A": 0x0054D598,
    "?g_54d59f@@3_NA": 0x0054D59F,
    "?g_54e8e0@@3PAUs_player_slot@@A": 0x0054E8E0,
    "?g_54eae8@@3PAY0MHA@EA": 0x0054EAE8,
    "?g_47ff38@@3JA": 0x0047FF38,
    "?g_51ebec@@3KA": 0x0051EBEC,
    "?g_551ae4@@3_NA": 0x00551AE4,
    "?g_55c14d@@3_NA": 0x0055C14D,
    "?g_55c14f@@3EA": 0x0055C14F,
    "?g_55c150@@3JA": 0x0055C150,
}

REQUIRED = {
    "D3DDevice_Begin",
    "D3DDevice_End",
    "D3DDevice_SetVertexData2f",
    "D3DDevice_SetVertexData4f",
    "D3DDevice_SetVertexDataColor",
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
    "?invoke@c_list_item_handler@@UAAXPAPAUs_controller_reference@@PAJ@Z",
    "?function_18f58c@@YAXXZ",
    "?function_18f5e3@@YAXXZ",
    "?function_18f62b@@YAXXZ",
    "?function_190186@@YAXJ@Z",
    "?function_1901fc@@YAGXZ",
    "?function_1906b4@@YAXXZ",
    "?function_1bbd0@@YAXXZ",
    "?function_1a0180@@YAXJJPAG@Z",
    "?utf8_string_to_utf16_string@@YAXPBDPAGJ@Z",
    "?function_199893@@YAXXZ",
    "?function_199b08@@YAXXZ",
    "?function_199ebc@@YAJXZ",
    "?function_199f34@@YAJXZ",
    "?function_0592d0@@YAKXZ",
    "?function_121040@@YAXJ@Z",
    "?function_125a90@@YAXJ@Z",
    "?function_597d0@@YA_NPAPAUs_597d0_object@@@Z",
    "?get_values_4d08@c_class_58d20@@QAA_NPAJ0PAPAE@Z",
    "?function_199e3c@@YAXJ@Z",
    "?function_199f6d@@YAJXZ",
    "?function_2133a1@@YAXXZ",
    "?function_213484@@YAXXZ",
    "?function_215790@@YAXXZ",
    "?function_2164c0@@YAXJPAUs_save_list_2164c0@@@Z",
    "?function_217080@@YAXPBUs_game_variant@@@Z",
    "?function_217300@@YAJJJ@Z",
    "?function_22a664@@YAXPBUs_widget_quad_2b11@@PBUs_float_rect@@JJJ@Z",
    "?function_10f5c0@@YA_NJMJJJ@Z",
    "?function_12360@@YAPAUD3DTexture@@PAUs_bitmap_view@@M@Z",
    "?function_13e9c0@@YAXPBGPBUshort_rectangle2d@@PAU1@2M@Z",
    "?function_13ed90@@YAXJ@Z",
    "?function_13ee20@@YA_NPBGJ@Z",
    "?function_211a0@@YAJJMMMJJJJJMM@Z",
    "?function_235d69@@YAXPBUshort_rectangle2d@@M0PBUcolor4f@@@Z",
    "?function_223976@@YA_NXZ",
    "?function_223a21@@YAXXZ",
    "?function_22d2ee@@YAXPAGJ@Z",
    "?function_22fba9@@YAXPAVc_class_1473c9@@@Z",
    "?function_22f11e@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    "??_7c_press_start_screen@@6B@",
    "?function_25240c@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    "?function_252433@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    "?function_25245a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    "?function_252481@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    "?function_2524a8@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    "??_7c_4way_signin_screen@@6B@",
    "??0s_widget_item@@QAA@XZ",
    "?build@c_class_1473c9@@QAAXPAUs_screen_layout@@@Z",
    "?function_22fc08@@YAXPAVc_class_1473c9@@@Z",
    "?function_2bacbc@@YAXPAVc_class_1a2c81@@PAUs_widget_group_definition@@PAUs_widget_point@@@Z",
    "?function_2bafa4@@YAXPAVc_class_1a2c81@@PAUs_widget_bounds@@@Z",
    "??_7c_class_2b01eb@@6B@",
    "??_7c_class_2b0b5e@@6B@",
    "??_7c_widget_45ad18@@6B@",
    "??_7c_widget_45adf0@@6B@",
    "?v4@c_class_2b01eb@@UAAXJ@Z",
    "?v1@c_class_2b0b5e@@UAAXXZ",
    "?v3@c_widget_45adf0@@UAAXXZ",
    "?v4@c_widget_45adf0@@UAAXJ@Z",
    "?v1@c_widget_45ad18@@UAAXXZ",
    "?function_2afeae@@YAXPAUs_widget_item@@PAVc_class_1a2c81@@@Z",
    "?function_2b0a14@@YAXPAUs_widget_view_2b0a@@F@Z",
    "?function_2b0a57@@YA_NPAUs_widget_view_2b0a@@@Z",
    "?function_2b72e6@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    "?function_2b7333@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    "?function_2b739a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    "??_7c_screen_45bc60@@6B@",
    "??_7c_screen_45bcd0@@6B@",
    "??_7c_screen_45bd40@@6B@",
    "?dialog_choice_show@@YAXJJJGP6A_NJ@Z0P6A_NPAVc_class_1473c9@@J@Z@Z",
    "?dialog_choice_show_default@@YAXJJGP6A_NJ@ZJ@Z",
    "?dialog_ok_show@@YAXJJJFP6A_NJ@ZP6A_NPAVc_class_1473c9@@J@Z@Z",
    "?function_22fa30@@YAPAVc_class_1a2c81@@PAV1@PAUs_bitmap_block@@@Z",
    "?function_235e5e@@YAXPAUs_sprite_element@@PAUs_float_rect@@1KJJ@Z",
    "?function_235f31@@YAXPAUs_sprite_element@@PBUs_float_rect@@PBUpoint2f@@KF@Z",
    "?function_23625d@@YAXJ@Z",
    "?function_23661f@@YAXPAUs_dialog_definition@@J@Z",
    "?function_24c7c1@@YAXPAUs_item_list@@J@Z",
    "?function_24bac5@@YAXPAX@Z",
    "?function_24c7e4@@YAXPAXPAPAUs_event@@PAJ@Z",
    "?saved_game_file_type_from_variant@@YAJPAUs_game_variant@@@Z",
    "?set_bitmap@c_screen_45bd40@@QAAXF@Z",
    "?set_text@c_screen_45bd40@@QAAXPBD@Z",
    "?voice_start_engine@@YAXXZ",
    "?function_2523bc@@YAXPAVc_online_task_screen@@@Z",
    "?function_6b640@@YAXJ@Z",
    "?function_abc70@@YAJJPBU_XUID@@@Z",
    "?function_6b910@@YAPAUs_type_9df9da@@J@Z",
    "?function_6c7e0@@YA_NXZ",
    "?function_6cb60@@YAXXZ",
    "?function_6d080@@YA_NJPAUs_channel_message@@@Z",
    "?main_menu_music_update@@YAXPAUs_main_menu_music@@@Z",
    "?online_get_logon_status@@YA_NPAJ@Z",
    "?online_get_logon_users@@YAXPAUXONLINE_USER@@@Z",
    "?online_task_continue@@YAJPAUs_type_9df9da@@@Z",
    "?online_task_poll@@YAJJ@Z",
    "?online_task_get_description@@YAJJ@Z",
    "?online_task_get_title@@YAJJ@Z",
    "?online_task_screen_finish@@YAXPAVc_online_task_screen@@@Z",
    "?online_task_screen_load@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
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
    "?function_11c0b0@@YAFPBDPAUs_name_table_11c0b0@@@Z",
    "?function_137550@@YAPAUs_type_7ba8e9@@JF@Z",
    "?function_1392f8@@YAXPBUs_player_appearance@@@Z",
    "?function_13eb20@@YAXFPBF@Z",
    "?function_13ec70@@YAXPBUcolor4f@@@Z",
    "?function_13ed50@@YAXPBUcolor4f@@@Z",
    "?function_155f60@@YA_NXZ",
    "?function_16e5e0@@YA_NPBUs_predicted_resource_block@@F@Z",
    "?function_19c270@@YAPAUs_entry_a@@JJ@Z",
    "?function_1fa50@c_1fa50@@QBAXPBUshort_rectangle2d@@PBX1JMJ1@Z",
    "?function_1a47fd@@YAPAXI@Z",
    "?function_bb760@@YAJF@Z",
    "?function_1a06a0@@YAPAUcolor3f@@JPAU1@@Z",
    "?function_7f790@@YAXF_NPBUs_player_appearance@@PAUcolor3f@@@Z",
    "?pack_color3f@@YAKPBUcolor3f@@@Z",
    "?saved_game_file_size_in_blocks@@YAJJ@Z",
    "?saved_game_storage_has_free_blocks@@YA_NJ@Z",
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
