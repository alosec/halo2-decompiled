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
        assert imports[symbol]["category"] == "platform_runtime"
    text_renderer = imports["?function_13e9c0@@YAXPBGPBUshort_rectangle2d@@PAU1@2M@Z"]
    assert text_renderer["category"] == "recovered_provider"
    assert "unknown_13e8a0.cpp" in text_renderer["providers"]
    video_bridge = imports["?function_18f1c0@@YAXJ@Z"]
    assert video_bridge["category"] == "stubbed_external"
    assert "stubs/bink.cpp" in video_bridge["providers"]
    renderer_bridge = imports["?function_52040@@YAXPBX0@Z"]
    assert renderer_bridge["category"] == "stubbed_external"
    assert "stubs/lane_p.cpp" in renderer_bridge["providers"]
    assert imports["?online_task_poll@@YAJJ@Z"]["category"] == "named_engine_service"
    print("main-menu import plan classification passed")


if __name__ == "__main__":
    main()
