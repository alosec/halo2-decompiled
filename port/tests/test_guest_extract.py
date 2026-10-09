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

        part_init_output = Path(temporary) / "campaign-part-init.o"
        part_init_command = command[:]
        part_init_command[part_init_command.index("h2_extract_selected")] = "?function_1c2390@s_part_table@@QAAXXZ"
        part_init_command[part_init_command.index("--extract-global"):part_init_command.index("--output")] = []
        part_init_command[part_init_command.index(output)] = part_init_output
        part_init_command[-1] = args.root / "src/unknown_1c22c0.cpp"
        subprocess.run([str(value) for value in part_init_command], check=True)
        part_init_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", part_init_output],
            text=True,
        )
        if "_?function_1c2390@s_part_table@@QAAXXZ" not in part_init_defined:
            raise RuntimeError("Campaign part-table initializer was not selectively compilable")

        physics_runtime_output = Path(temporary) / "physics-runtime-constructor.o"
        physics_runtime_command = command[:]
        physics_runtime_command[physics_runtime_command.index("h2_extract_selected")] = "?function_246c60@@YAXPAXJ@Z"
        physics_runtime_command[physics_runtime_command.index("--extract-global"):physics_runtime_command.index("--output")] = []
        physics_runtime_command[physics_runtime_command.index("--output"):physics_runtime_command.index("--output")] = [
            "--extract-symbol", "h2_native_physics_runtime_vtable",
            "--extract-symbol", "h2_native_physics_runtime_reset",
            "--extract-symbol", "h2_native_physics_runtime_object_count",
            "--extract-global", "g_native_physics_runtime_vtables",
            "--extract-global", "g_native_physics_runtime_object_count",
        ]
        physics_runtime_command[physics_runtime_command.index(output)] = physics_runtime_output
        physics_runtime_command[-1] = args.root / "src/unknown_246c60.cpp"
        subprocess.run([str(value) for value in physics_runtime_command], check=True)
        physics_runtime_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_runtime_output],
            text=True,
        )
        if "_?function_246c60@@YAXPAXJ@Z" not in physics_runtime_defined:
            raise RuntimeError("physics runtime-object constructor was not selectively compilable")
        for symbol in (
            "_h2_native_physics_runtime_vtable",
            "_h2_native_physics_runtime_reset",
            "_h2_native_physics_runtime_object_count",
        ):
            if symbol not in physics_runtime_defined:
                raise RuntimeError(f"native physics constructor lost owned runtime support: {symbol}")

        physics_type_output = Path(temporary) / "physics-runtime-type.o"
        physics_type_command = command[:]
        physics_type_command[physics_type_command.index("h2_extract_selected")] = "?function_2fbb30@@YAPAXXZ"
        physics_type_command[physics_type_command.index("--extract-global"):physics_type_command.index("--output")] = []
        physics_type_command[physics_type_command.index("--output"):physics_type_command.index("--output")] = [
            "--extract-symbol", "?function_2fdfe0@@YAPAXXZ",
            "--extract-symbol", "?function_2fbfe0@@YAPAXXZ",
        ]
        physics_type_command[physics_type_command.index(output)] = physics_type_output
        physics_type_command[-1] = args.root / "src/unknown_2fbb30.cpp"
        subprocess.run([str(value) for value in physics_type_command], check=True)
        physics_type_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_type_output],
            text=True,
        )
        for symbol in (
            "_?function_2fbb30@@YAPAXXZ",
            "_?function_2fdfe0@@YAPAXXZ",
            "_?function_2fbfe0@@YAPAXXZ",
        ):
            if symbol not in physics_type_defined:
                raise RuntimeError(f"physics runtime type-descriptor method was not retained: {symbol}")

        physics_model_type_output = Path(temporary) / "physics-model-runtime-types.o"
        physics_model_type_command = command[:]
        physics_model_type_command[physics_model_type_command.index("h2_extract_selected")] = "?function_2df310@@YAPAXXZ"
        physics_model_type_command[physics_model_type_command.index("--extract-global"):physics_model_type_command.index("--output")] = []
        extra_physics_model_types = (
            "?function_2fe260@@YAPAXXZ", "?function_2df4b0@@YAPAXXZ",
            "?function_2fd0a0@@YAPAXXZ", "?function_2fde30@@YAPAXXZ",
            "?function_2fe0b0@@YAPAXXZ", "?function_2fdfa0@@YAPAXXZ",
        )
        insert_at = physics_model_type_command.index("--output")
        for symbol in extra_physics_model_types:
            physics_model_type_command[insert_at:insert_at] = ["--extract-symbol", symbol]
            insert_at += 2
        physics_model_type_command[physics_model_type_command.index(output)] = physics_model_type_output
        physics_model_type_command[-1] = args.root / "src/unknown_2df310.cpp"
        subprocess.run([str(value) for value in physics_model_type_command], check=True)
        physics_model_type_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_type_output],
            text=True,
        )
        for symbol in ("?function_2df310@@YAPAXXZ",) + extra_physics_model_types:
            if f"_{symbol}" not in physics_model_type_defined:
                raise RuntimeError(f"physics-model runtime type method was not retained: {symbol}")

        physics_model_count_output = Path(temporary) / "physics-model-runtime-count.o"
        physics_model_count_command = command[:]
        physics_model_count_command[physics_model_count_command.index("h2_extract_selected")] = "?function_2fc1a0@@YAJPAX@Z"
        physics_model_count_command[physics_model_count_command.index("--extract-global"):physics_model_count_command.index("--output")] = []
        extra_physics_model_accessors = (
            "?function_2fc1b0@@YAJPAXJ@Z", "?function_2fc1d0@@YAJPAXJJ@Z",
            "?function_2fc1e0@@YAJPAXJ@Z", "?function_2fc1f0@@YAJPAXJ@Z",
        )
        insert_at = physics_model_count_command.index("--output")
        for symbol in extra_physics_model_accessors:
            physics_model_count_command[insert_at:insert_at] = ["--extract-symbol", symbol]
            insert_at += 2
        physics_model_count_command[physics_model_count_command.index(output)] = physics_model_count_output
        physics_model_count_command[-1] = args.root / "src/unknown_2fc1a0.cpp"
        subprocess.run([str(value) for value in physics_model_count_command], check=True)
        physics_model_count_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_count_output],
            text=True,
        )
        for symbol in ("?function_2fc1a0@@YAJPAX@Z",) + extra_physics_model_accessors:
            if f"_{symbol}" not in physics_model_count_defined:
                raise RuntimeError(f"physics-model runtime accessor was not retained: {symbol}")

        physics_model_kind_output = Path(temporary) / "physics-model-runtime-kinds.o"
        physics_model_kind_command = command[:]
        physics_model_kind_command[physics_model_kind_command.index("h2_extract_selected")] = "?function_2de7a0@@YAJXZ"
        physics_model_kind_command[physics_model_kind_command.index("--extract-global"):physics_model_kind_command.index("--output")] = [
            "--extract-symbol", "?function_2fe190@@YAJXZ",
        ]
        physics_model_kind_command[physics_model_kind_command.index(output)] = physics_model_kind_output
        physics_model_kind_command[-1] = args.root / "src/unknown_2de7a0.cpp"
        subprocess.run([str(value) for value in physics_model_kind_command], check=True)
        physics_model_kind_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_kind_output],
            text=True,
        )
        for symbol in ("?function_2de7a0@@YAJXZ", "?function_2fe190@@YAJXZ"):
            if f"_{symbol}" not in physics_model_kind_defined:
                raise RuntimeError(f"physics-model runtime kind was not retained: {symbol}")

        physics_model_box_bounds_output = Path(temporary) / "physics-model-box-bounds.o"
        physics_model_box_bounds_symbols = (
            "?function_2fe350@@YAXPBUs_physics_bounds_matrix@@PBMMPAUs_physics_bounds@@@Z",
            "?function_2fe420@c_physics_box_shape@@QAAXPBUs_physics_bounds_matrix@@MPAUs_physics_bounds@@@Z",
        )
        physics_model_box_bounds_command = command[:]
        physics_model_box_bounds_command[physics_model_box_bounds_command.index("h2_extract_selected")] = physics_model_box_bounds_symbols[0]
        physics_model_box_bounds_command[physics_model_box_bounds_command.index("--extract-global"):physics_model_box_bounds_command.index("--output")] = [
            "--extract-symbol", physics_model_box_bounds_symbols[1],
        ]
        physics_model_box_bounds_command[physics_model_box_bounds_command.index(output)] = physics_model_box_bounds_output
        physics_model_box_bounds_command[-1] = args.root / "src/unknown_2fe350.cpp"
        subprocess.run([str(value) for value in physics_model_box_bounds_command], check=True)
        physics_model_box_bounds_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_box_bounds_output],
            text=True,
        )
        for symbol in physics_model_box_bounds_symbols:
            if f"_{symbol}" not in physics_model_box_bounds_defined:
                raise RuntimeError(f"physics-model box-bounds method was not retained: {symbol}")

        physics_model_radius_output = Path(temporary) / "physics-model-support-radius.o"
        physics_model_radius_command = command[:]
        physics_model_radius_symbol = (
            "?function_2df0b0@c_physics_convex_shape@@UAAMPBM@Z"
        )
        physics_model_radius_command[physics_model_radius_command.index("h2_extract_selected")] = physics_model_radius_symbol
        physics_model_radius_command[physics_model_radius_command.index("--extract-global"):physics_model_radius_command.index("--output")] = []
        physics_model_radius_command[physics_model_radius_command.index(output)] = physics_model_radius_output
        physics_model_radius_command[-1] = args.root / "src/unknown_2df0b0.cpp"
        subprocess.run([str(value) for value in physics_model_radius_command], check=True)
        physics_model_radius_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_radius_output],
            text=True,
        )
        if f"_{physics_model_radius_symbol}" not in physics_model_radius_defined:
            raise RuntimeError("physics-model support-radius method was not retained")

        physics_model_transform_output = Path(temporary) / "physics-model-transform.o"
        physics_model_transform_symbols = (
            "?function_2da450@s_physics_affine_matrix@@QAAXPBU1@0@Z",
            "?function_2de850@c_physics_transformed_shape@@QAAXPBUs_physics_affine_matrix@@PAX1@Z",
        )
        physics_model_transform_command = command[:]
        physics_model_transform_command[physics_model_transform_command.index("h2_extract_selected")] = physics_model_transform_symbols[0]
        physics_model_transform_command[physics_model_transform_command.index("--extract-global"):physics_model_transform_command.index("--output")] = [
            "--extract-symbol", physics_model_transform_symbols[1],
        ]
        physics_model_transform_command[physics_model_transform_command.index(output)] = physics_model_transform_output
        physics_model_transform_command[-1] = args.root / "src/unknown_2de850.cpp"
        subprocess.run([str(value) for value in physics_model_transform_command], check=True)
        physics_model_transform_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_transform_output],
            text=True,
        )
        for symbol in physics_model_transform_symbols:
            if f"_{symbol}" not in physics_model_transform_defined:
                raise RuntimeError(f"physics-model transform method was not retained: {symbol}")

        physics_model_base_output = Path(temporary) / "physics-model-runtime-base.o"
        physics_model_base_symbols = (
            "?function_2d7210@c_physics_runtime_base@@QAAJJJ@Z",
            "?function_2d7240@c_physics_runtime_base@@QAAX_N@Z",
        )
        physics_model_base_command = command[:]
        physics_model_base_command[physics_model_base_command.index("h2_extract_selected")] = physics_model_base_symbols[0]
        physics_model_base_command[physics_model_base_command.index("--extract-global"):physics_model_base_command.index("--output")] = [
            "--extract-symbol", physics_model_base_symbols[1],
        ]
        physics_model_base_command[physics_model_base_command.index(output)] = physics_model_base_output
        physics_model_base_command[-1] = args.root / "src/unknown_2d7210.cpp"
        subprocess.run([str(value) for value in physics_model_base_command], check=True)
        physics_model_base_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_base_output],
            text=True,
        )
        for symbol in physics_model_base_symbols:
            if f"_{symbol}" not in physics_model_base_defined:
                raise RuntimeError(f"physics-model base method was not retained: {symbol}")

        physics_model_marker_output = Path(temporary) / "physics-model-shape-marker.o"
        physics_model_marker_symbols = (
            "?function_2ddca0@c_physics_shape_455500@@QAAXPAUs_physics_shape_marker_455500@@@Z",
            "?function_2ddcc0@c_physics_shape_455500@@QAAPAXJ@Z",
            "?function_2ddd30@c_physics_shape_455500@@QAAXPBMPAM@Z",
            "?function_2dddb0@c_physics_shape_455500@@QAAXPBGJPAM@Z",
        )
        physics_model_marker_command = command[:]
        physics_model_marker_command[physics_model_marker_command.index("h2_extract_selected")] = physics_model_marker_symbols[0]
        physics_model_marker_command[physics_model_marker_command.index("--extract-global"):physics_model_marker_command.index("--output")] = []
        insert_at = physics_model_marker_command.index("--output")
        for symbol in physics_model_marker_symbols[1:]:
            physics_model_marker_command[insert_at:insert_at] = ["--extract-symbol", symbol]
            insert_at += 2
        physics_model_marker_command[physics_model_marker_command.index(output)] = physics_model_marker_output
        physics_model_marker_command[-1] = args.root / "src/unknown_2ddca0.cpp"
        subprocess.run([str(value) for value in physics_model_marker_command], check=True)
        physics_model_marker_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_marker_output],
            text=True,
        )
        for symbol in physics_model_marker_symbols:
            if f"_{symbol}" not in physics_model_marker_defined:
                raise RuntimeError(f"physics-model shape-marker method was not retained: {symbol}")

        physics_model_leaf_output = Path(temporary) / "physics-model-runtime-leaf.o"
        physics_model_leaf_command = command[:]
        physics_model_leaf_command[physics_model_leaf_command.index("h2_extract_selected")] = (
            "?function_2dae30@c_physics_model_leaf@@QAAXPAX00@Z"
        )
        physics_model_leaf_command[physics_model_leaf_command.index("--extract-global"):physics_model_leaf_command.index("--output")] = [
            "--extract-symbol", "?function_2fdfb0@c_physics_model_leaf@@QAAXPAX0PAM@Z",
        ]
        physics_model_leaf_command[physics_model_leaf_command.index(output)] = physics_model_leaf_output
        physics_model_leaf_command[-1] = args.root / "src/unknown_2dae30.cpp"
        subprocess.run([str(value) for value in physics_model_leaf_command], check=True)
        physics_model_leaf_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_leaf_output],
            text=True,
        )
        for symbol in (
            "?function_2dae30@c_physics_model_leaf@@QAAXPAX00@Z",
            "?function_2fdfb0@c_physics_model_leaf@@QAAXPAX0PAM@Z",
        ):
            if f"_{symbol}" not in physics_model_leaf_defined:
                raise RuntimeError(f"physics-model leaf method was not retained: {symbol}")

        physics_model_accessor_output = Path(temporary) / "physics-model-runtime-accessors.o"
        physics_model_accessor_symbols = (
            "?function_2fd210@c_physics_shape_456008@@QAAXPAUs_physics_shape_marker@@@Z",
            "?function_2fd230@c_physics_shape_456008@@QAAXPAJ@Z",
            "?function_2fe330@c_physics_shape_455ef0@@QAAXPAUs_physics_shape_marker@@@Z",
            "?function_2fe410@c_physics_shape_455ef0@@QAAXPAM@Z",
            "?function_2fe450@c_physics_shape_455ef0@@QAAXPBMPAM@Z",
            "?function_2fe490@c_physics_shape_455ef0@@QAAXPBGJPAM@Z",
            "?function_2fe4f0@c_physics_shape_455ef0@@QAAXPAM@Z",
        )
        physics_model_accessor_command = command[:]
        physics_model_accessor_command[physics_model_accessor_command.index("h2_extract_selected")] = physics_model_accessor_symbols[0]
        physics_model_accessor_command[physics_model_accessor_command.index("--extract-global"):physics_model_accessor_command.index("--output")] = []
        insert_at = physics_model_accessor_command.index("--output")
        for symbol in physics_model_accessor_symbols[1:]:
            physics_model_accessor_command[insert_at:insert_at] = ["--extract-symbol", symbol]
            insert_at += 2
        physics_model_accessor_command[physics_model_accessor_command.index(output)] = physics_model_accessor_output
        physics_model_accessor_command[-1] = args.root / "src/unknown_2fd210.cpp"
        subprocess.run([str(value) for value in physics_model_accessor_command], check=True)
        physics_model_accessor_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_accessor_output],
            text=True,
        )
        for symbol in physics_model_accessor_symbols:
            if f"_{symbol}" not in physics_model_accessor_defined:
                raise RuntimeError(f"physics-model compact accessor was not retained: {symbol}")

        physics_model_aabb_output = Path(temporary) / "physics-model-collection-aabb.o"
        physics_model_aabb_command = command[:]
        physics_model_aabb_symbol = (
            "?function_2fc120@c_physics_shape_collection@@QAAXPAX0PAUs_physics_aabb@@@Z"
        )
        physics_model_aabb_command[physics_model_aabb_command.index("h2_extract_selected")] = physics_model_aabb_symbol
        physics_model_aabb_command[physics_model_aabb_command.index("--extract-global"):physics_model_aabb_command.index("--output")] = []
        physics_model_aabb_command[physics_model_aabb_command.index(output)] = physics_model_aabb_output
        physics_model_aabb_command[-1] = args.root / "src/unknown_2fc120.cpp"
        subprocess.run([str(value) for value in physics_model_aabb_command], check=True)
        physics_model_aabb_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_aabb_output],
            text=True,
        )
        if f"_{physics_model_aabb_symbol}" not in physics_model_aabb_defined:
            raise RuntimeError("physics-model collection AABB method was not retained")

        physics_model_forward_output = Path(temporary) / "physics-model-shape-forwarder.o"
        physics_model_forward_symbols = (
            "?function_2fe1a0@c_physics_shape_forwarder@@QAAPAXPAX00@Z",
            "?function_2fe1c0@c_physics_shape_forwarder@@QAAXPAXPAJ0@Z",
        )
        physics_model_forward_command = command[:]
        physics_model_forward_command[physics_model_forward_command.index("h2_extract_selected")] = physics_model_forward_symbols[0]
        physics_model_forward_command[physics_model_forward_command.index("--extract-global"):physics_model_forward_command.index("--output")] = [
            "--extract-symbol", physics_model_forward_symbols[1],
        ]
        physics_model_forward_command[physics_model_forward_command.index(output)] = physics_model_forward_output
        physics_model_forward_command[-1] = args.root / "src/unknown_2fe1a0.cpp"
        subprocess.run([str(value) for value in physics_model_forward_command], check=True)
        physics_model_forward_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_forward_output],
            text=True,
        )
        for symbol in physics_model_forward_symbols:
            if f"_{symbol}" not in physics_model_forward_defined:
                raise RuntimeError(f"physics-model shape forwarder was not retained: {symbol}")

        physics_model_aabb_forward_output = Path(temporary) / "physics-model-aabb-forwarder.o"
        physics_model_aabb_forward_command = command[:]
        physics_model_aabb_forward_symbol = (
            "?function_2df5d0@c_physics_aabb_forwarder@@QAAXPAX0PAUs_physics_forwarded_aabb@@@Z"
        )
        physics_model_aabb_forward_command[physics_model_aabb_forward_command.index("h2_extract_selected")] = physics_model_aabb_forward_symbol
        physics_model_aabb_forward_command[physics_model_aabb_forward_command.index("--extract-global"):physics_model_aabb_forward_command.index("--output")] = []
        physics_model_aabb_forward_command[physics_model_aabb_forward_command.index(output)] = physics_model_aabb_forward_output
        physics_model_aabb_forward_command[-1] = args.root / "src/unknown_2df5d0.cpp"
        subprocess.run([str(value) for value in physics_model_aabb_forward_command], check=True)
        physics_model_aabb_forward_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_aabb_forward_output],
            text=True,
        )
        if f"_{physics_model_aabb_forward_symbol}" not in physics_model_aabb_forward_defined:
            raise RuntimeError("physics-model AABB forwarder was not retained")

        vehicle_init_output = Path(temporary) / "campaign-vehicle-init.o"
        vehicle_init_command = command[:]
        vehicle_init_command[vehicle_init_command.index("h2_extract_selected")] = "?function_205550@@YAXPAX@Z"
        vehicle_init_command[vehicle_init_command.index("--extract-global"):vehicle_init_command.index("--output")] = []
        vehicle_init_command[vehicle_init_command.index(output)] = vehicle_init_output
        vehicle_init_command[-1] = args.root / "src/unknown_205550.cpp"
        subprocess.run([str(value) for value in vehicle_init_command], check=True)
        vehicle_init_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", vehicle_init_output],
            text=True,
        )
        if "_?function_205550@@YAXPAX@Z" not in vehicle_init_defined:
            raise RuntimeError("Campaign vehicle initializer was not selectively compilable")

        campaign_levels_output = Path(temporary) / "campaign-levels.o"
        campaign_levels_command = command[:]
        campaign_levels_command[campaign_levels_command.index("h2_extract_selected")] = "?get_item_count@c_campaign_level_handles_list@@UAAJXZ"
        campaign_levels_command[campaign_levels_command.index("--extract-global"):campaign_levels_command.index("--output")] = []
        campaign_levels_command[campaign_levels_command.index(output)] = campaign_levels_output
        campaign_levels_command[-1] = args.root / "src/unknown_2c4e9c.cpp"
        subprocess.run([str(value) for value in campaign_levels_command], check=True)
        campaign_levels_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", campaign_levels_output],
            text=True,
        )
        if "_?get_item_count@c_campaign_level_handles_list@@UAAJXZ" not in campaign_levels_defined:
            raise RuntimeError("campaign-level list source was not selectively compilable")

        level_screen_output = Path(temporary) / "campaign-level-screen.o"
        level_screen_command = command[:]
        level_screen_command[level_screen_command.index("h2_extract_selected")] = "?v18@c_level_select_screen@@UAAXPAX@Z"
        level_screen_command[level_screen_command.index("--extract-global"):level_screen_command.index("--output")] = []
        level_screen_command[level_screen_command.index(output)] = level_screen_output
        level_screen_command[-1] = args.root / "src/unknown_2b116a.cpp"
        subprocess.run([str(value) for value in level_screen_command], check=True)
        level_screen_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", level_screen_output],
            text=True,
        )
        if "_?v18@c_level_select_screen@@UAAXPAX@Z" not in level_screen_defined:
            raise RuntimeError("campaign-level screen lifecycle was not selectively compilable")

    print("guest extraction retained constants and parsed recovered UI/rendering sources")


if __name__ == "__main__":
    main()
