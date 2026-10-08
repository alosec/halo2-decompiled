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
    assert "?function_18f1c0@@YAXJ@Z" not in imports
    assert "?function_52040@@YAXPBX0@Z" not in imports
    assert imports["?online_task_poll@@YAJJ@Z"]["category"] == "named_engine_service"
    print("main-menu import plan classification passed")


if __name__ == "__main__":
    main()
