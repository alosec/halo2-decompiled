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
    for symbol in ("GetTickCount", "Sleep", "cos"):
        assert symbol not in imports
    for symbol in ("host_h2_get_milliseconds", "host_h2_sleep", "host_h2_cos"):
        assert imports[symbol]["category"] == "host_abi"
    text_renderer = imports["?function_13e9c0@@YAXPBGPBUshort_rectangle2d@@PAU1@2M@Z"]
    assert text_renderer["category"] == "recovered_provider"
    assert "unknown_13e8a0.cpp" in text_renderer["providers"]
    assert imports["host_h2_video_control"]["category"] == "host_abi"
    assert imports["host_h2_render_request"]["category"] == "host_abi"
    assert imports["host_h2_text_request"]["category"] == "host_abi"
    assert "?function_18f1c0@@YAXJ@Z" not in imports
    assert "?function_18f5e3@@YAXXZ" not in imports
    assert "?function_215790@@YAXXZ" not in imports
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
    assert "?function_22a664@@YAXPBUs_widget_quad_2b11@@PBUs_float_rect@@JJJ@Z" not in imports
    assert "?function_199893@@YAXXZ" not in imports
    assert "?function_199b08@@YAXXZ" not in imports
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
