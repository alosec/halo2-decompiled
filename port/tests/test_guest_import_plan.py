#!/usr/bin/env python3
"""Validate the generated main-menu import classification."""

import argparse
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--plan", type=Path, required=True)
    parser.add_argument("--expect-unresolved", type=int, required=True)
    args = parser.parse_args()

    plan = json.loads(args.plan.read_text())
    assert plan["schema"] == 1
    assert plan["unresolved_count"] == args.expect_unresolved
    assert sum(plan["counts"].values()) == args.expect_unresolved
    imports = {entry["symbol"]: entry for entry in plan["imports"]}
    for symbol in (
        "GetTickCount", "Sleep", "cos", "acos", "exp", "expf", "fmod",
        "log", "log10", "logf", "pow", "sinf", "XPhysicalAlloc",
        "XPhysicalProtect", "VirtualAlloc", "VirtualFree",
    ):
        assert symbol not in imports
    for symbol in (
        "host_h2_get_milliseconds", "host_h2_sleep", "host_h2_cos",
        "host_h2_physical_alloc", "host_h2_physical_protect",
        "host_h2_virtual_alloc", "host_h2_virtual_free",
        "host_h2_math_acos", "host_h2_math_exp", "host_h2_math_expf", "host_h2_math_fmod",
        "host_h2_math_log", "host_h2_math_log10", "host_h2_math_logf", "host_h2_math_pow",
        "host_h2_math_sinf",
    ):
        assert imports[symbol]["category"] == "host_abi"
    assert imports["host_h2_video_control"]["category"] == "host_abi"
    assert imports["host_h2_render_request"]["category"] == "host_abi"
    assert imports["host_h2_text_request"]["category"] == "host_abi"
    assert imports["host_h2_map_preflight"]["category"] == "host_abi"
    assert imports["host_h2_map_select"]["category"] == "host_abi"
    assert imports["host_h2_map_is_active"]["category"] == "host_abi"
    for symbol in (
        "host_h2_profile_list", "host_h2_profile_create", "host_h2_profile_read",
        "host_h2_profile_write", "host_h2_profile_delete",
    ):
        assert imports[symbol]["category"] == "host_abi"
    assert "?function_18f1c0@@YAXJ@Z" not in imports
    assert "?function_18f5e3@@YAXXZ" not in imports
    assert "?function_18f62b@@YAXXZ" not in imports
    assert "?function_6cb60@@YAXXZ" not in imports
    assert "?function_215790@@YAXXZ" not in imports
    assert "?function_18fb34@@YAXJPAUs_player_profile_settings@@J@Z" not in imports
    assert "?function_215367@@YAXJJPAXJ@Z" not in imports
    assert "?function_2153dd@@YAXJJPAUs_player_profile_settings@@J@Z" not in imports
    assert "?function_2157c0@@YAXJ@Z" not in imports
    assert "?function_215e60@@YAXJ@Z" not in imports
    assert "?function_215f40@@YA_NJPAE@Z" not in imports
    assert "?function_2238f4@@YAXJKKK@Z" not in imports
    assert "?set_profile_index@s_player_slot_profile@@QAAXJ@Z" not in imports
    assert "?show_dialog@s_player_slot_profile@@QAAXP6AXJ_N@ZJ@Z" not in imports
    assert "?sign_out@s_player_slot_profile@@QAAXXZ" not in imports
    assert "?function_2133a1@@YAXXZ" not in imports
    assert "?function_213484@@YAXXZ" not in imports
    assert "?function_52040@@YAXPBX0@Z" not in imports
    assert "?online_task_poll@@YAJJ@Z" not in imports
    assert "?online_get_logon_status@@YA_NPAJ@Z" not in imports
    assert "?online_team_get_details@@YAXJPBUXUID@@PAUXONLINE_TEAM@@@Z" not in imports
    assert "?online_task_screen_load@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z" not in imports
    assert "?main_menu_music_update@@YAXPAUs_main_menu_music@@@Z" not in imports
    assert "?function_125a90@@YAXJ@Z" not in imports
    assert "?function_16e5e0@@YA_NPBUs_predicted_resource_block@@F@Z" not in imports
    assert "?function_1bbd0@@YAXXZ" not in imports
    assert "?function_1a0180@@YAXJJPAG@Z" not in imports
    assert "?function_2afeae@@YAXPAUs_widget_item@@PAVc_class_1a2c81@@@Z" not in imports
    for symbol in (
        "?function_10f5c0@@YA_NJMJJJ@Z",
        "?function_12360@@YAPAUD3DTexture@@PAUs_bitmap_view@@M@Z",
        "?function_13e9c0@@YAXPBGPBUshort_rectangle2d@@PAU1@2M@Z",
        "?function_13ed90@@YAXJ@Z",
        "?function_13ee20@@YA_NPBGJ@Z",
        "?function_211a0@@YAJJMMMJJJJJMM@Z",
        "?function_235d69@@YAXPBUshort_rectangle2d@@M0PBUcolor4f@@@Z",
    ):
        assert symbol not in imports
    assert "?function_22a664@@YAXPBUs_widget_quad_2b11@@PBUs_float_rect@@JJJ@Z" not in imports
    for symbol in (
        "?function_22f11e@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_25240c@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_252433@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_25245a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_252481@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_2524a8@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    ):
        assert symbol not in imports
    assert "?function_223976@@YA_NXZ" not in imports
    assert "?function_223a21@@YAXXZ" not in imports
    for symbol in (
        "?function_230616@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_230691@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_2310b7@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_231995@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_231db5@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_2320c4@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_2323c3@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_23246a@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_23252e@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_2325fb@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_23334f@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
        "?function_24b4a9@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z",
    ):
        assert symbol not in imports
    assert "?function_199893@@YAXXZ" not in imports
    assert "?function_199b08@@YAXXZ" not in imports
    assert "?function_236299@@YAXJ@Z" not in imports
    assert "?function_238c21@@YAXJJPAGJ@Z" not in imports
    assert "?function_238ea7@@YAXJ@Z" not in imports
    assert "?function_238eb5@@YAXJJ@Z" not in imports
    for symbol in (
        "?function_199a03@@YAXJ@Z",
        "?function_199a57@@YAXXZ",
        "?function_199d7c@@YAJXZ",
        "?function_199df9@@YA_N_N0@Z",
        "?function_19a0c5@@YA_NXZ",
        "?function_19a1bd@@YA_NXZ",
    ):
        assert symbol not in imports
    assert "?network_session_manager_leave_session_a@@YAX_N@Z" not in imports
    assert "?network_session_manager_leave_session_b@@YAX_N@Z" not in imports
    for symbol in (
        "D3DDevice_Begin",
        "D3DDevice_End",
        "D3DDevice_SetVertexData2f",
        "D3DDevice_SetVertexData4f",
        "D3DDevice_SetVertexDataColor",
    ):
        assert symbol not in imports
    assert "?build@c_class_1473c9@@QAAXPAUs_screen_layout@@@Z" not in imports
    for symbol in (
        "?function_11c0b0@@YAFPBDPAUs_name_table_11c0b0@@@Z",
        "?function_137550@@YAPAUs_type_7ba8e9@@JF@Z",
        "?function_1392f8@@YAXPBUs_player_appearance@@@Z",
        "?function_22fa30@@YAPAVc_class_1a2c81@@PAV1@PAUs_bitmap_block@@@Z",
        "?function_235f31@@YAXPAUs_sprite_element@@PBUs_float_rect@@PBUpoint2f@@KF@Z",
        "?function_bb760@@YAJF@Z",
    ):
        assert symbol not in imports
    print("main-menu import plan classification passed")


if __name__ == "__main__":
    main()
