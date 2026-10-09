#!/usr/bin/env python3
"""Verify function extraction retains referenced constant initializers."""

import argparse
from pathlib import Path
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    args = parser.parse_args()

    with tempfile.TemporaryDirectory(prefix="h2-guest-extract-") as temporary:
        output = Path(temporary) / "probe.o"
        command = [
            sys.executable, args.root / "port/tools/guest_cc.py",
            "--llvm", args.llvm,
            "--plugin", args.plugin,
        ]
        for directory in (args.root / "port/include", args.root / "port/include/crt",
                          args.root / "port/include/xdk", args.root / "include"):
            command.extend(("--include", directory))
        command.extend((
            "--ms-abi",
            "--extract-symbol", "h2_extract_selected",
            "--extract-global", "h2_extract_constant",
            "--extract-alias", "h2_extract_alias",
            "--output", output,
            args.root / "port/tests/guest_extract_probe.cpp",
        ))
        subprocess.run([str(value) for value in command], check=True)

        defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", output],
            text=True,
        )
        undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", output],
            text=True,
        )
        if "_h2_extract_selected" not in defined:
            raise RuntimeError("selected guest function was not retained")
        if "_h2_extract_constant" not in defined:
            raise RuntimeError("selected guest constant was not retained")
        if "_h2_extract_alias" not in defined:
            raise RuntimeError("selected guest alias was not retained")
        if "_h2_extract_unrelated" in defined:
            raise RuntimeError("unrelated guest function was retained")
        if undefined.strip():
            raise RuntimeError(f"extracted guest function has undefined constants: {undefined}")
        if b"retained recovered constant" not in output.read_bytes():
            raise RuntimeError("referenced guest constant initializer was discarded")

        text_output = Path(temporary) / "text-font.o"
        text_command = command[:]
        text_command[text_command.index("h2_extract_selected")] = "?function_13ed90@@YAXJ@Z"
        text_command[text_command.index("--extract-global"):text_command.index("--output")] = []
        text_command[text_command.index(output)] = text_output
        text_command[-1] = args.root / "src/unknown_13e8a0.cpp"
        subprocess.run([str(value) for value in text_command], check=True)
        text_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", text_output],
            text=True,
        )
        if "_?function_13ed90@@YAXJ@Z" not in text_defined:
            raise RuntimeError("recovered text renderer source was not selectively compilable")

        appearance_output = Path(temporary) / "player-appearance.o"
        appearance_command = command[:]
        appearance_command[appearance_command.index("h2_extract_selected")] = "?function_1392f8@@YAXPBUs_player_appearance@@@Z"
        appearance_command[appearance_command.index("--extract-global"):appearance_command.index("--output")] = []
        appearance_command[appearance_command.index(output)] = appearance_output
        appearance_command[-1] = args.root / "src/unknown_139296.cpp"
        subprocess.run([str(value) for value in appearance_command], check=True)
        appearance_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", appearance_output],
            text=True,
        )
        if "_?function_1392f8@@YAXPBUs_player_appearance@@@Z" not in appearance_defined:
            raise RuntimeError("recovered appearance source was not selectively compilable")

        predicted_output = Path(temporary) / "predicted-resources.o"
        predicted_command = command[:]
        predicted_command[predicted_command.index("h2_extract_selected")] = "?function_16e5e0@@YA_NPBUs_predicted_resource_block@@F@Z"
        predicted_command[predicted_command.index("--extract-global"):predicted_command.index("--output")] = []
        predicted_command[predicted_command.index(output)] = predicted_output
        predicted_command[-1] = args.root / "src/unknown_16e290.cpp"
        subprocess.run([str(value) for value in predicted_command], check=True)
        predicted_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", predicted_output],
            text=True,
        )
        if "_?function_16e5e0@@YA_NPBUs_predicted_resource_block@@F@Z" not in predicted_defined:
            raise RuntimeError("predicted-resource source was not selectively compilable")

        press_start_output = Path(temporary) / "press-start.o"
        press_start_command = command[:]
        press_start_command[press_start_command.index("h2_extract_selected")] = "?function_22f11e@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z"
        press_start_command[press_start_command.index("--extract-global"):press_start_command.index("--output")] = []
        press_start_command[press_start_command.index(output)] = press_start_output
        press_start_command[-1] = args.root / "src/unknown_22f118.cpp"
        subprocess.run([str(value) for value in press_start_command], check=True)
        press_start_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", press_start_output],
            text=True,
        )
        if "_?function_22f11e@@YAPAVc_class_1473c9@@PAUs_screen_parameters@@@Z" not in press_start_defined:
            raise RuntimeError("press-start source was not selectively compilable")

        preview_output = Path(temporary) / "preview-renderer.o"
        preview_command = command[:]
        preview_command[preview_command.index("h2_extract_selected")] = "?function_22a664@@YAXPBUs_widget_quad_2b11@@PBUs_float_rect@@JJJ@Z"
        preview_command[preview_command.index("--extract-global"):preview_command.index("--output")] = []
        preview_command[preview_command.index(output)] = preview_output
        preview_command[-1] = args.root / "src/unknown_22a664.cpp"
        subprocess.run([str(value) for value in preview_command], check=True)
        preview_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", preview_output],
            text=True,
        )
        if "_?function_22a664@@YAXPBUs_widget_quad_2b11@@PBUs_float_rect@@JJJ@Z" not in preview_defined:
            raise RuntimeError("preview renderer was not selectively compilable")

        profile_storage_output = Path(temporary) / "profile-storage.o"
        profile_storage_command = command[:]
        profile_storage_command[profile_storage_command.index("h2_extract_selected")] = "?function_215790@@YAXXZ"
        profile_storage_command[profile_storage_command.index("--extract-global"):profile_storage_command.index("--output")] = []
        profile_storage_command[profile_storage_command.index(output)] = profile_storage_output
        profile_storage_command[-1] = args.root / "src/unknown_2157e0.cpp"
        subprocess.run([str(value) for value in profile_storage_command], check=True)
        profile_storage_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", profile_storage_output],
            text=True,
        )
        if "_?function_215790@@YAXXZ" not in profile_storage_defined:
            raise RuntimeError("profile storage source was not selectively compilable")

        keyboard_output = Path(temporary) / "virtual-keyboard.o"
        keyboard_command = command[:]
        keyboard_command[keyboard_command.index("h2_extract_selected")] = "?function_238c21@@YAXJJPAGJ@Z"
        keyboard_command[keyboard_command.index("--extract-global"):keyboard_command.index("--output")] = []
        keyboard_command[keyboard_command.index(output)] = keyboard_output
        keyboard_command[-1] = args.root / "src/unknown_23760b.cpp"
        subprocess.run([str(value) for value in keyboard_command], check=True)
        keyboard_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", keyboard_output],
            text=True,
        )
        if "_?function_238c21@@YAXJJPAGJ@Z" not in keyboard_defined:
            raise RuntimeError("virtual-keyboard source was not selectively compilable")

    print("guest extraction retained constants and parsed recovered UI/rendering sources")


if __name__ == "__main__":
    main()
