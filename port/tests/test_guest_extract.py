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

        physics_model_capsule_output = Path(temporary) / "physics-model-capsule.o"
        physics_model_capsule_symbol = (
            "?function_2de1b0@c_physics_capsule_shape@@UAAPA_NPA_NPBUs_physics_capsule_segment@@PAUs_physics_capsule_hit@@@Z"
        )
        physics_model_capsule_command = command[:]
        physics_model_capsule_command[physics_model_capsule_command.index("h2_extract_selected")] = physics_model_capsule_symbol
        physics_model_capsule_command[physics_model_capsule_command.index("--extract-global"):physics_model_capsule_command.index("--output")] = []
        physics_model_capsule_command[physics_model_capsule_command.index(output)] = physics_model_capsule_output
        physics_model_capsule_command[-1] = args.root / "src/unknown_2de1b0.cpp"
        subprocess.run([str(value) for value in physics_model_capsule_command], check=True)
        physics_model_capsule_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_capsule_output],
            text=True,
        )
        if f"_{physics_model_capsule_symbol}" not in physics_model_capsule_defined:
            raise RuntimeError("physics-model capsule intersection method was not retained")

        physics_model_offset_box_output = Path(temporary) / "physics-model-offset-box.o"
        physics_model_offset_box_symbol = (
            "?function_2fd8a0@c_physics_offset_box_shape@@UAAXPBUs_physics_offset_box_matrix@@MPAUs_physics_offset_box_bounds@@@Z"
        )
        physics_model_offset_box_command = command[:]
        physics_model_offset_box_command[physics_model_offset_box_command.index("h2_extract_selected")] = physics_model_offset_box_symbol
        physics_model_offset_box_command[physics_model_offset_box_command.index("--extract-global"):physics_model_offset_box_command.index("--output")] = []
        physics_model_offset_box_command[physics_model_offset_box_command.index(output)] = physics_model_offset_box_output
        physics_model_offset_box_command[-1] = args.root / "src/unknown_2fd8a0.cpp"
        subprocess.run([str(value) for value in physics_model_offset_box_command], check=True)
        physics_model_offset_box_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_offset_box_output],
            text=True,
        )
        if f"_{physics_model_offset_box_symbol}" not in physics_model_offset_box_defined:
            raise RuntimeError("physics-model offset-box bounds method was not retained")

        physics_model_collection_query_output = Path(temporary) / "physics-model-collection-query.o"
        physics_model_collection_query_symbols = (
            "?function_2fbb70@c_physics_collection_shape@@UAAPA_NPA_NPAUs_physics_collection_query@@PAUs_physics_collection_hit@@@Z",
            "?function_2fbe90@c_physics_collection_shape@@UAAXPAUs_physics_collection_query@@PAUs_physics_collection_context@@PAX@Z",
        )
        physics_model_collection_query_command = command[:]
        physics_model_collection_query_command[physics_model_collection_query_command.index("h2_extract_selected")] = physics_model_collection_query_symbols[0]
        physics_model_collection_query_command[physics_model_collection_query_command.index("--extract-global"):physics_model_collection_query_command.index("--output")] = [
            "--extract-symbol", physics_model_collection_query_symbols[1],
        ]
        physics_model_collection_query_command[physics_model_collection_query_command.index(output)] = physics_model_collection_query_output
        physics_model_collection_query_command[-1] = args.root / "src/unknown_2fbb70.cpp"
        subprocess.run([str(value) for value in physics_model_collection_query_command], check=True)
        physics_model_collection_query_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_collection_query_output],
            text=True,
        )
        for symbol in physics_model_collection_query_symbols:
            if f"_{symbol}" not in physics_model_collection_query_defined:
                raise RuntimeError(f"physics-model collection query method was not retained: {symbol}")

        physics_model_collection_projection_output = Path(temporary) / "physics-model-collection-projection.o"
        physics_model_collection_projection_symbol = (
            "?function_2fbdc0@c_physics_shape_collection@@UAAMPBM@Z"
        )
        physics_model_collection_projection_command = command[:]
        physics_model_collection_projection_command[physics_model_collection_projection_command.index("h2_extract_selected")] = physics_model_collection_projection_symbol
        physics_model_collection_projection_command[physics_model_collection_projection_command.index("--extract-global"):physics_model_collection_projection_command.index("--output")] = []
        physics_model_collection_projection_command[physics_model_collection_projection_command.index(output)] = physics_model_collection_projection_output
        physics_model_collection_projection_command[-1] = args.root / "src/unknown_2fbdc0.cpp"
        subprocess.run([str(value) for value in physics_model_collection_projection_command], check=True)
        physics_model_collection_projection_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_collection_projection_output],
            text=True,
        )
        if f"_{physics_model_collection_projection_symbol}" not in physics_model_collection_projection_defined:
            raise RuntimeError("physics-model collection projected-support method was not retained")

        physics_model_plane_intersection_output = Path(temporary) / "physics-model-plane-intersection.o"
        physics_model_plane_intersection_symbol = (
            "?function_2fd5c0@c_physics_plane_cloud_shape@@UAAPA_NPA_NPBUs_physics_plane_segment@@PAUs_physics_plane_hit@@@Z"
        )
        physics_model_plane_intersection_command = command[:]
        physics_model_plane_intersection_command[physics_model_plane_intersection_command.index("h2_extract_selected")] = physics_model_plane_intersection_symbol
        physics_model_plane_intersection_command[physics_model_plane_intersection_command.index("--extract-global"):physics_model_plane_intersection_command.index("--output")] = []
        physics_model_plane_intersection_command[physics_model_plane_intersection_command.index(output)] = physics_model_plane_intersection_output
        physics_model_plane_intersection_command[-1] = args.root / "src/unknown_2fd5c0.cpp"
        subprocess.run([str(value) for value in physics_model_plane_intersection_command], check=True)
        physics_model_plane_intersection_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_plane_intersection_output],
            text=True,
        )
        if f"_{physics_model_plane_intersection_symbol}" not in physics_model_plane_intersection_defined:
            raise RuntimeError("physics-model convex-plane intersection was not retained")

        physics_model_vertex_cloud_output = Path(temporary) / "physics-model-vertex-cloud.o"
        physics_model_vertex_cloud_symbols = (
            "?function_2fd260@c_physics_vertex_cloud_shape@@UAAXPBMPAM@Z",
            "?function_2fd430@c_physics_vertex_cloud_shape@@UAAXPBGPAMJ@Z",
            "?function_2fd730@c_physics_vertex_cloud_shape@@UAAPAMPAM@Z",
        )
        physics_model_vertex_cloud_command = command[:]
        physics_model_vertex_cloud_command[physics_model_vertex_cloud_command.index("h2_extract_selected")] = physics_model_vertex_cloud_symbols[0]
        physics_model_vertex_cloud_command[physics_model_vertex_cloud_command.index("--extract-global"):physics_model_vertex_cloud_command.index("--output")] = []
        for symbol in physics_model_vertex_cloud_symbols[1:]:
            insertion = physics_model_vertex_cloud_command.index("--output")
            physics_model_vertex_cloud_command[insertion:insertion] = ["--extract-symbol", symbol]
        physics_model_vertex_cloud_command[physics_model_vertex_cloud_command.index(output)] = physics_model_vertex_cloud_output
        physics_model_vertex_cloud_command[-1] = args.root / "src/unknown_2fd260.cpp"
        subprocess.run([str(value) for value in physics_model_vertex_cloud_command], check=True)
        physics_model_vertex_cloud_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_vertex_cloud_output],
            text=True,
        )
        for symbol in physics_model_vertex_cloud_symbols:
            if f"_{symbol}" not in physics_model_vertex_cloud_defined:
                raise RuntimeError(f"physics-model vertex-cloud method was not retained: {symbol}")

        physics_model_two_vertex_bounds_output = Path(temporary) / "physics-model-two-vertex-bounds.o"
        physics_model_two_vertex_bounds_symbol = (
            "?function_2de0f0@c_physics_two_vertex_shape@@UAAXPBUs_physics_two_vertex_matrix@@MPAUs_physics_two_vertex_bounds@@@Z"
        )
        physics_model_two_vertex_bounds_command = command[:]
        physics_model_two_vertex_bounds_command[physics_model_two_vertex_bounds_command.index("h2_extract_selected")] = physics_model_two_vertex_bounds_symbol
        physics_model_two_vertex_bounds_command[physics_model_two_vertex_bounds_command.index("--extract-global"):physics_model_two_vertex_bounds_command.index("--output")] = []
        physics_model_two_vertex_bounds_command[physics_model_two_vertex_bounds_command.index(output)] = physics_model_two_vertex_bounds_output
        physics_model_two_vertex_bounds_command[-1] = args.root / "src/unknown_2de0f0.cpp"
        subprocess.run([str(value) for value in physics_model_two_vertex_bounds_command], check=True)
        physics_model_two_vertex_bounds_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_two_vertex_bounds_output],
            text=True,
        )
        if f"_{physics_model_two_vertex_bounds_symbol}" not in physics_model_two_vertex_bounds_defined:
            raise RuntimeError("physics-model two-vertex bounds method was not retained")

        physics_model_transformed_forward_output = Path(temporary) / "physics-model-transformed-forward.o"
        physics_model_transformed_forward_symbol = (
            "?function_2deb20@c_physics_transformed_forward_shape@@UAAXPBUs_physics_forward_segment@@PAUs_physics_query_context@@PAX@Z"
        )
        physics_model_transformed_forward_command = command[:]
        physics_model_transformed_forward_command[physics_model_transformed_forward_command.index("h2_extract_selected")] = physics_model_transformed_forward_symbol
        physics_model_transformed_forward_command[physics_model_transformed_forward_command.index("--extract-global"):physics_model_transformed_forward_command.index("--output")] = []
        physics_model_transformed_forward_command[physics_model_transformed_forward_command.index(output)] = physics_model_transformed_forward_output
        physics_model_transformed_forward_command[-1] = args.root / "src/unknown_2deb20.cpp"
        subprocess.run([str(value) for value in physics_model_transformed_forward_command], check=True)
        physics_model_transformed_forward_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_transformed_forward_output],
            text=True,
        )
        if f"_{physics_model_transformed_forward_symbol}" not in physics_model_transformed_forward_defined:
            raise RuntimeError("physics-model transformed query forwarder was not retained")

        physics_model_transformed_hit_output = Path(temporary) / "physics-model-transformed-hit.o"
        physics_model_transformed_hit_symbols = (
            "?function_2da6e0@s_physics_direction@@QAAXPBUs_physics_transform_matrix@@PBM@Z",
            "?function_2de9e0@c_physics_transformed_intersection_shape@@UAAPA_NPA_NPBUs_physics_transformed_segment@@PAUs_physics_transformed_hit@@@Z",
        )
        physics_model_transformed_hit_command = command[:]
        physics_model_transformed_hit_command[physics_model_transformed_hit_command.index("h2_extract_selected")] = physics_model_transformed_hit_symbols[0]
        physics_model_transformed_hit_command[physics_model_transformed_hit_command.index("--extract-global"):physics_model_transformed_hit_command.index("--output")] = [
            "--extract-symbol", physics_model_transformed_hit_symbols[1],
        ]
        physics_model_transformed_hit_command[physics_model_transformed_hit_command.index(output)] = physics_model_transformed_hit_output
        physics_model_transformed_hit_command[-1] = args.root / "src/unknown_2da6e0.cpp"
        subprocess.run([str(value) for value in physics_model_transformed_hit_command], check=True)
        physics_model_transformed_hit_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_transformed_hit_output],
            text=True,
        )
        for symbol in physics_model_transformed_hit_symbols:
            if f"_{symbol}" not in physics_model_transformed_hit_defined:
                raise RuntimeError(f"physics-model transformed-hit method was not retained: {symbol}")

        physics_model_transformed_projection_output = Path(temporary) / "physics-model-transformed-projection.o"
        physics_model_transformed_projection_symbol = (
            "?function_2de8e0@c_physics_transformed_projection_shape@@UAAMPBM@Z"
        )
        physics_model_transformed_projection_command = command[:]
        physics_model_transformed_projection_command[physics_model_transformed_projection_command.index("h2_extract_selected")] = physics_model_transformed_projection_symbol
        physics_model_transformed_projection_command[physics_model_transformed_projection_command.index("--extract-global"):physics_model_transformed_projection_command.index("--output")] = []
        physics_model_transformed_projection_command[physics_model_transformed_projection_command.index(output)] = physics_model_transformed_projection_output
        physics_model_transformed_projection_command[-1] = args.root / "src/unknown_2de8e0.cpp"
        subprocess.run([str(value) for value in physics_model_transformed_projection_command], check=True)
        physics_model_transformed_projection_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_transformed_projection_output],
            text=True,
        )
        if f"_{physics_model_transformed_projection_symbol}" not in physics_model_transformed_projection_defined:
            raise RuntimeError("physics-model transformed projected-support method was not retained")

        physics_model_projection_output = Path(temporary) / "physics-model-projection.o"
        physics_model_projection_symbol = (
            "?function_2df220@c_physics_projected_shape@@UAAMPBM@Z"
        )
        physics_model_projection_command = command[:]
        physics_model_projection_command[physics_model_projection_command.index("h2_extract_selected")] = physics_model_projection_symbol
        physics_model_projection_command[physics_model_projection_command.index("--extract-global"):physics_model_projection_command.index("--output")] = []
        physics_model_projection_command[physics_model_projection_command.index(output)] = physics_model_projection_output
        physics_model_projection_command[-1] = args.root / "src/unknown_2df220.cpp"
        subprocess.run([str(value) for value in physics_model_projection_command], check=True)
        physics_model_projection_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_projection_output],
            text=True,
        )
        if f"_{physics_model_projection_symbol}" not in physics_model_projection_defined:
            raise RuntimeError("physics-model projected-shape support method was not retained")

        physics_model_box_ray_output = Path(temporary) / "physics-model-box-ray.o"
        physics_model_box_ray_symbol = (
            "?function_2fe550@c_physics_box_shape@@UAAPA_NPA_NPBUs_physics_line_segment@@PAUs_physics_line_hit@@@Z"
        )
        physics_model_box_ray_command = command[:]
        physics_model_box_ray_command[physics_model_box_ray_command.index("h2_extract_selected")] = physics_model_box_ray_symbol
        physics_model_box_ray_command[physics_model_box_ray_command.index("--extract-global"):physics_model_box_ray_command.index("--output")] = []
        physics_model_box_ray_command[physics_model_box_ray_command.index(output)] = physics_model_box_ray_output
        physics_model_box_ray_command[-1] = args.root / "src/unknown_2fe550.cpp"
        subprocess.run([str(value) for value in physics_model_box_ray_command], check=True)
        physics_model_box_ray_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_box_ray_output],
            text=True,
        )
        if f"_{physics_model_box_ray_symbol}" not in physics_model_box_ray_defined:
            raise RuntimeError("physics-model box segment-intersection method was not retained")

        physics_model_hit_forward_output = Path(temporary) / "physics-model-hit-forward.o"
        physics_model_hit_forward_symbol = (
            "?function_2df110@c_physics_convex_shape@@UAAXPBXPAUs_physics_shape_query_transform@@PAVc_physics_shape_hit_collector@@@Z"
        )
        physics_model_hit_forward_command = command[:]
        physics_model_hit_forward_command[physics_model_hit_forward_command.index("h2_extract_selected")] = physics_model_hit_forward_symbol
        physics_model_hit_forward_command[physics_model_hit_forward_command.index("--extract-global"):physics_model_hit_forward_command.index("--output")] = []
        physics_model_hit_forward_command[physics_model_hit_forward_command.index(output)] = physics_model_hit_forward_output
        physics_model_hit_forward_command[-1] = args.root / "src/unknown_2df110.cpp"
        subprocess.run([str(value) for value in physics_model_hit_forward_command], check=True)
        physics_model_hit_forward_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", physics_model_hit_forward_output],
            text=True,
        )
        if f"_{physics_model_hit_forward_symbol}" not in physics_model_hit_forward_defined:
            raise RuntimeError("physics-model transformed hit forwarder was not retained")

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

        bsp_switch_output = Path(temporary) / "structure-bsp-switch.o"
        bsp_switch_command = command[:]
        bsp_switch_symbol = "?function_b6bb0@@YAXXZ"
        bsp_switch_command[bsp_switch_command.index("h2_extract_selected")] = bsp_switch_symbol
        bsp_switch_command[bsp_switch_command.index("--extract-global"):bsp_switch_command.index("--output")] = []
        bsp_switch_command[bsp_switch_command.index(output)] = bsp_switch_output
        bsp_switch_command[-1] = args.root / "src/unknown_0b6bb0.cpp"
        subprocess.run([str(value) for value in bsp_switch_command], check=True)
        bsp_switch_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", bsp_switch_output],
            text=True,
        )
        if f"_{bsp_switch_symbol}" not in bsp_switch_defined:
            raise RuntimeError("structure-BSP switch callback was not selectively compilable")

        object_relocation_output = Path(temporary) / "object-placement-relocation.o"
        object_relocation_command = command[:]
        object_relocation_symbol = "?function_b8600@@YAXJJ@Z"
        object_relocation_command[object_relocation_command.index("h2_extract_selected")] = object_relocation_symbol
        object_relocation_command[object_relocation_command.index("--extract-global"):object_relocation_command.index("--output")] = []
        object_relocation_command[object_relocation_command.index(output)] = object_relocation_output
        object_relocation_command[-1] = args.root / "src/unknown_0b8600.cpp"
        subprocess.run([str(value) for value in object_relocation_command], check=True)
        object_relocation_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", object_relocation_output],
            text=True,
        )
        if f"_{object_relocation_symbol}" not in object_relocation_defined:
            raise RuntimeError("object placement relocation was not selectively compilable")

        placement_sources = (
            ("?function_bef30@@YAXJJJJJ@Z", "src/unknown_0bef30.cpp"),
            ("?function_b8b70@@YAXJ@Z", "src/unknown_0b8b70.cpp"),
            ("?function_c3220@@YAXJ@Z", "src/unknown_0b8b70.cpp"),
            ("?function_b8540@@YAXJ@Z", "src/unknown_0b8540.cpp"),
        )
        for placement_symbol, placement_source in placement_sources:
            placement_output = Path(temporary) / f"placement-{len(placement_symbol)}-{placement_symbol[10:16]}.o"
            placement_command = command[:]
            placement_command[placement_command.index("h2_extract_selected")] = placement_symbol
            placement_command[placement_command.index("--extract-global"):placement_command.index("--output")] = []
            placement_command[placement_command.index(output)] = placement_output
            placement_command[-1] = args.root / placement_source
            subprocess.run([str(value) for value in placement_command], check=True)
            placement_defined = subprocess.check_output(
                [args.llvm / "llvm-nm", "--defined-only", "--format=posix", placement_output],
                text=True,
            )
            if f"_{placement_symbol}" not in placement_defined:
                raise RuntimeError(f"placement dependency was not selectively compilable: {placement_symbol}")

        decals_output = Path(temporary) / "structure-bsp-decals.o"
        decals_symbol = "?decals_update_locations@@YAXXZ"
        decals_command = command[:]
        decals_command[decals_command.index("h2_extract_selected")] = decals_symbol
        decals_command[decals_command.index("--extract-global"):decals_command.index("--output")] = []
        decals_command[decals_command.index(output)] = decals_output
        decals_command[-1] = args.root / "src/unknown_17d2a0.cpp"
        subprocess.run([str(value) for value in decals_command], check=True)
        decals_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", decals_output],
            text=True,
        )
        if f"_{decals_symbol}" not in decals_defined:
            raise RuntimeError("structure-BSP decal update was not selectively compilable")

        looping_sound_output = Path(temporary) / "structure-bsp-looping-sound.o"
        looping_sound_symbol = "?object_looping_sounds_initialize_for_new_structure_bsp@@YAXXZ"
        looping_sound_command = command[:]
        looping_sound_command[looping_sound_command.index("h2_extract_selected")] = looping_sound_symbol
        looping_sound_command[looping_sound_command.index("--extract-global"):looping_sound_command.index("--output")] = []
        looping_sound_command[looping_sound_command.index(output)] = looping_sound_output
        looping_sound_command[-1] = args.root / "src/unknown_1887d0.cpp"
        subprocess.run([str(value) for value in looping_sound_command], check=True)
        looping_sound_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", looping_sound_output],
            text=True,
        )
        if f"_{looping_sound_symbol}" not in looping_sound_defined:
            raise RuntimeError("structure-BSP looping-sound reset was not selectively compilable")

        ai_bsp_output = Path(temporary) / "structure-bsp-ai.o"
        ai_bsp_symbol = "?function_1c7bb0@@YAXXZ"
        ai_bsp_command = command[:]
        ai_bsp_command[ai_bsp_command.index("h2_extract_selected")] = ai_bsp_symbol
        ai_bsp_command[ai_bsp_command.index("--extract-global"):ai_bsp_command.index("--output")] = []
        ai_bsp_command[ai_bsp_command.index(output)] = ai_bsp_output
        ai_bsp_command[-1] = args.root / "src/unknown_1c7bb0.cpp"
        subprocess.run([str(value) for value in ai_bsp_command], check=True)
        ai_bsp_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", ai_bsp_output],
            text=True,
        )
        if f"_{ai_bsp_symbol}" not in ai_bsp_defined:
            raise RuntimeError("structure-BSP AI update was not selectively compilable")

        audio_bsp_output = Path(temporary) / "structure-bsp-audio-state.o"
        audio_bsp_symbol = "?function_210df0@@YAXXZ"
        audio_bsp_command = command[:]
        audio_bsp_command[audio_bsp_command.index("h2_extract_selected")] = audio_bsp_symbol
        audio_bsp_command[audio_bsp_command.index("--extract-global"):audio_bsp_command.index("--output")] = []
        audio_bsp_command[audio_bsp_command.index(output)] = audio_bsp_output
        audio_bsp_command[-1] = args.root / "src/unknown_210df0.cpp"
        subprocess.run([str(value) for value in audio_bsp_command], check=True)
        audio_bsp_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", audio_bsp_output],
            text=True,
        )
        if f"_{audio_bsp_symbol}" not in audio_bsp_defined:
            raise RuntimeError("structure-BSP audio-state reset was not selectively compilable")

        renderer_bsp_output = Path(temporary) / "structure-bsp-renderer-state.o"
        renderer_bsp_symbol = "?function_226a0@@YAXXZ"
        renderer_bsp_command = command[:]
        renderer_bsp_command[renderer_bsp_command.index("h2_extract_selected")] = renderer_bsp_symbol
        renderer_bsp_command[renderer_bsp_command.index("--extract-global"):renderer_bsp_command.index("--output")] = []
        renderer_bsp_command[renderer_bsp_command.index(output)] = renderer_bsp_output
        renderer_bsp_command[-1] = args.root / "src/unknown_0226a0.cpp"
        subprocess.run([str(value) for value in renderer_bsp_command], check=True)
        renderer_bsp_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", renderer_bsp_output],
            text=True,
        )
        if f"_{renderer_bsp_symbol}" not in renderer_bsp_defined:
            raise RuntimeError("structure-BSP renderer-state reset was not selectively compilable")

        visibility_bsp_output = Path(temporary) / "structure-bsp-visibility-state.o"
        visibility_bsp_symbol = "?function_03f2b0@@YAXXZ"
        visibility_bsp_command = command[:]
        visibility_bsp_command[visibility_bsp_command.index("h2_extract_selected")] = visibility_bsp_symbol
        visibility_bsp_command[visibility_bsp_command.index("--extract-global"):visibility_bsp_command.index("--output")] = []
        visibility_bsp_command[visibility_bsp_command.index(output)] = visibility_bsp_output
        visibility_bsp_command[-1] = args.root / "src/unknown_03f2b0_native.cpp"
        subprocess.run([str(value) for value in visibility_bsp_command], check=True)
        visibility_bsp_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", visibility_bsp_output],
            text=True,
        )
        if f"_{visibility_bsp_symbol}" not in visibility_bsp_defined:
            raise RuntimeError("structure-BSP visibility reset was not selectively compilable")

        sound_bsp_output = Path(temporary) / "structure-bsp-sound-location.o"
        sound_bsp_symbol = "?sound_update_locations@@YAXXZ"
        sound_bsp_command = command[:]
        sound_bsp_command[sound_bsp_command.index("h2_extract_selected")] = sound_bsp_symbol
        sound_bsp_command[sound_bsp_command.index("--extract-global"):sound_bsp_command.index("--output")] = []
        sound_bsp_command[sound_bsp_command.index(output)] = sound_bsp_output
        sound_bsp_command[-1] = args.root / "src/unknown_125920.cpp"
        subprocess.run([str(value) for value in sound_bsp_command], check=True)
        sound_bsp_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", sound_bsp_output],
            text=True,
        )
        if f"_{sound_bsp_symbol}" not in sound_bsp_defined:
            raise RuntimeError("structure-BSP sound-location update was not selectively compilable")

        bsp_table_output = Path(temporary) / "structure-bsp-callback-table.o"
        bsp_table_command = command[:]
        bsp_table_command[bsp_table_command.index("h2_extract_selected")] = "h2_native_structure_bsp_idle"
        bsp_table_command[bsp_table_command.index("--extract-global") + 1] = "h2_native_structure_bsp_switch_callbacks"
        alias_index = bsp_table_command.index("--extract-alias")
        del bsp_table_command[alias_index:alias_index + 2]
        bsp_table_command[bsp_table_command.index(output)] = bsp_table_output
        bsp_table_command[-1] = args.root / "port/runtime/guest_structure_bsp_callbacks.cpp"
        subprocess.run([str(value) for value in bsp_table_command], check=True)
        bsp_table_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", bsp_table_output],
            text=True,
        )
        for symbol in ("h2_native_structure_bsp_idle", "h2_native_structure_bsp_switch_callbacks"):
            if symbol not in bsp_table_defined:
                raise RuntimeError(f"structure-BSP callback assembly omitted {symbol}")
        bsp_table_relocations = subprocess.check_output(
            [args.llvm / "llvm-readelf", "-r", bsp_table_output], text=True,
        )
        if bsp_table_relocations.count("ARM64_RELOC_UNSIGNED") != 21:
            raise RuntimeError("structure-BSP callback assembly must retain exactly 21 providers")

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

        bsp_collision_output = Path(temporary) / "structure-bsp-collision-correction.o"
        bsp_collision_command = command[:]
        bsp_collision_command[bsp_collision_command.index("h2_extract_selected")] = "?function_16a8e0@@YA_NJPBTpoint3f@@MJJPAT1@PAM@Z"
        bsp_collision_command[bsp_collision_command.index("--extract-global"):bsp_collision_command.index("--output")] = []
        bsp_collision_command[bsp_collision_command.index(output)] = bsp_collision_output
        bsp_collision_command[-1] = args.root / "src/unknown_1689b0.cpp"
        subprocess.run([str(value) for value in bsp_collision_command], check=True)
        bsp_collision_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", bsp_collision_output],
            text=True,
        )
        if "_?function_16a8e0@@YA_NJPBTpoint3f@@MJJPAT1@PAM@Z" not in bsp_collision_defined:
            raise RuntimeError("structure-BSP collision correction was not selectively compilable")

        object_relocation_output = Path(temporary) / "object-relocation.o"
        object_relocation_command = command[:]
        object_relocation_command[object_relocation_command.index("h2_extract_selected")] = "?function_b7430@@YA_NJPBTpoint3f@@PBTvector3f@@1PBUs_location@@_N333@Z"
        object_relocation_command[object_relocation_command.index("--extract-global"):object_relocation_command.index("--output")] = []
        object_relocation_command[object_relocation_command.index("--output"):object_relocation_command.index("--output")] = [
            "--extract-symbol", "?function_b87b0@@YAXJ@Z",
        ]
        object_relocation_command[object_relocation_command.index(output)] = object_relocation_output
        object_relocation_command[-1] = args.root / "src/unknown_0b7430.cpp"
        subprocess.run([str(value) for value in object_relocation_command], check=True)
        object_relocation_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", object_relocation_output],
            text=True,
        )
        for symbol in (
            "_?function_b7430@@YA_NJPBTpoint3f@@PBTvector3f@@1PBUs_location@@_N333@Z",
            "_?function_b87b0@@YAXJ@Z",
        ):
            if symbol not in object_relocation_defined:
                raise RuntimeError(f"recovered object relocation method was not retained: {symbol}")

        scenario_placement_output = Path(temporary) / "scenario-object-placement.o"
        scenario_placement_command = command[:]
        scenario_placement_command[scenario_placement_command.index("h2_extract_selected")] = "?function_bf0f0@@YAXPBUs_type_4f0dcc@@JJPAUs_scenario_block@@_N2@Z"
        scenario_placement_command[scenario_placement_command.index("--extract-global"):scenario_placement_command.index("--output")] = []
        scenario_placement_command[scenario_placement_command.index(output)] = scenario_placement_output
        scenario_placement_command[-1] = args.root / "src/unknown_0bf0f0.cpp"
        subprocess.run([str(value) for value in scenario_placement_command], check=True)
        scenario_placement_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", scenario_placement_output],
            text=True,
        )
        if "_?function_bf0f0@@YAXPBUs_type_4f0dcc@@JJPAUs_scenario_block@@_N2@Z" not in scenario_placement_defined:
            raise RuntimeError("scenario object placement primitive was not selectively compilable")

        vehicle_exit_output = Path(temporary) / "vehicle-exit.o"
        vehicle_exit_symbol = "?function_ea8e0@@YAXJ_N@Z"
        vehicle_exit_command = command[:]
        vehicle_exit_command[vehicle_exit_command.index("h2_extract_selected")] = vehicle_exit_symbol
        vehicle_exit_command[vehicle_exit_command.index("--extract-global"):vehicle_exit_command.index("--output")] = []
        vehicle_exit_command[vehicle_exit_command.index(output)] = vehicle_exit_output
        vehicle_exit_command[-1] = args.root / "src/unknown_0a76b0.cpp"
        subprocess.run([str(value) for value in vehicle_exit_command], check=True)
        vehicle_exit_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", vehicle_exit_output],
            text=True,
        )
        if f"_{vehicle_exit_symbol}" not in vehicle_exit_defined:
            raise RuntimeError("immediate vehicle-exit implementation was not selectively compilable")

        contact_listener_output = Path(temporary) / "havok-contact-listener.o"
        contact_listener_symbol = "??0c_interface_277890@@QAA@XZ"
        contact_filter_symbol = "?function_2792c0@@YA_NPAUs_2792c0_context@@@Z"
        contact_leaf_symbols = [
            "??1c_callback_277820@@UAA@XZ",
            "??1c_callback_2789b0@@UAA@XZ",
            "?h2_native_primary_listener_callback_construct@@YAPAVc_callback_2789b0@@PAX@Z",
        ]
        contact_listener_command = command[:]
        contact_listener_command[contact_listener_command.index("h2_extract_selected")] = contact_listener_symbol
        contact_listener_command[contact_listener_command.index("--extract-global"):contact_listener_command.index("--output")] = [
            "--extract-symbol", contact_filter_symbol,
            *[value for symbol in contact_leaf_symbols for value in ("--extract-symbol", symbol)],
        ]
        contact_listener_command[contact_listener_command.index(output)] = contact_listener_output
        contact_listener_command[-1] = args.root / "src/unknown_2777d0.cpp"
        subprocess.run([str(value) for value in contact_listener_command], check=True)
        contact_listener_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", contact_listener_output],
            text=True,
        )
        for symbol in (contact_listener_symbol, contact_filter_symbol, *contact_leaf_symbols):
            if f"_{symbol}" not in contact_listener_defined:
                raise RuntimeError(f"Havok contact listener source was not retained: {symbol}")

        component_rebuild_output = Path(temporary) / "havok-component-rebuild.o"
        component_rebuild_symbols = [
            "?function_1c3850@@YAXJ@Z",
            "?function_1c3770@@YAXJK@Z",
        ]
        component_rebuild_command = command[:]
        component_rebuild_command[component_rebuild_command.index("h2_extract_selected")] = component_rebuild_symbols[0]
        component_rebuild_command[component_rebuild_command.index("--extract-global"):component_rebuild_command.index("--output")] = [
            "--extract-symbol", component_rebuild_symbols[1],
        ]
        component_rebuild_command[component_rebuild_command.index(output)] = component_rebuild_output
        component_rebuild_command[-1] = args.root / "src/unknown_1c3850_native.cpp"
        subprocess.run([str(value) for value in component_rebuild_command], check=True)
        component_rebuild_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", component_rebuild_output],
            text=True,
        )
        for symbol in component_rebuild_symbols:
            if f"_{symbol}" not in component_rebuild_defined:
                raise RuntimeError(f"Havok component rebuild source was not retained: {symbol}")

        primary_constructor_output = Path(temporary) / "havok-primary-listener-constructor.o"
        primary_constructor_symbol = "??0c_interface_277890@@QAA@XZ"
        primary_constructor_command = command[:]
        primary_constructor_command[primary_constructor_command.index("h2_extract_selected")] = primary_constructor_symbol
        primary_constructor_command[primary_constructor_command.index("--extract-global"):primary_constructor_command.index("--output")] = []
        primary_constructor_command[primary_constructor_command.index(output)] = primary_constructor_output
        primary_constructor_command[-1] = args.root / "src/havok_primary_listener_construct_native.cpp"
        subprocess.run([str(value) for value in primary_constructor_command], check=True)
        primary_constructor_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", primary_constructor_output],
            text=True,
        )
        if f"_{primary_constructor_symbol}" not in primary_constructor_defined:
            raise RuntimeError("native primary Havok listener constructor was not retained")

        impact_creation_output = Path(temporary) / "impact-creation.o"
        impact_creation_symbol = "?function_226a60@@YAXJ@Z"
        impact_creation_command = command[:]
        impact_creation_command[impact_creation_command.index("h2_extract_selected")] = impact_creation_symbol
        impact_creation_command[impact_creation_command.index("--extract-global"):impact_creation_command.index("--output")] = []
        impact_creation_command[impact_creation_command.index(output)] = impact_creation_output
        impact_creation_command[-1] = args.root / "src/impacts.cpp"
        subprocess.run([str(value) for value in impact_creation_command], check=True)
        impact_creation_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", impact_creation_output],
            text=True,
        )
        if f"_{impact_creation_symbol}" not in impact_creation_defined:
            raise RuntimeError("recovered impact-creation source was not selectively compilable")

        area_damage_output = Path(temporary) / "area-damage.o"
        area_damage_symbol = "?function_d6c80@@YAJPAUs_type_1e6529@@J@Z"
        area_damage_command = command[:]
        area_damage_command[area_damage_command.index("h2_extract_selected")] = area_damage_symbol
        area_damage_command[area_damage_command.index("--extract-global"):area_damage_command.index("--output")] = []
        area_damage_command[area_damage_command.index(output)] = area_damage_output
        area_damage_command[-1] = args.root / "src/damage_area_effect_native.cpp"
        subprocess.run([str(value) for value in area_damage_command], check=True)
        area_damage_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", area_damage_output],
            text=True,
        )
        if f"_{area_damage_symbol}" not in area_damage_defined:
            raise RuntimeError("native area-damage dispatcher was not selectively compilable")

        damage_submit_output = Path(temporary) / "damage-submit.o"
        damage_submit_symbol = "?function_184250@@YAXPBUs_type_1e6529@@@Z"
        damage_submit_command = command[:]
        damage_submit_command[damage_submit_command.index("h2_extract_selected")] = damage_submit_symbol
        damage_submit_command[damage_submit_command.index("--extract-global"):damage_submit_command.index("--output")] = []
        damage_submit_command[damage_submit_command.index(output)] = damage_submit_output
        damage_submit_command[-1] = args.root / "src/unknown_184250.cpp"
        subprocess.run([str(value) for value in damage_submit_command], check=True)
        damage_submit_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", damage_submit_output],
            text=True,
        )
        if f"_{damage_submit_symbol}" not in damage_submit_defined:
            raise RuntimeError("damage-event submission source was not selectively compilable")

        light_curve_output = Path(temporary) / "light-curve.o"
        light_curve_symbol = "?function_17ca10@@YAMMF@Z"
        light_curve_command = command[:]
        light_curve_command[light_curve_command.index("h2_extract_selected")] = light_curve_symbol
        light_curve_command[light_curve_command.index("--extract-global"):light_curve_command.index("--output")] = []
        light_curve_command[light_curve_command.index(output)] = light_curve_output
        light_curve_command[-1] = args.root / "src/light_curve_lookup_native.cpp"
        subprocess.run([str(value) for value in light_curve_command], check=True)
        light_curve_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", light_curve_output],
            text=True,
        )
        if f"_{light_curve_symbol}" not in light_curve_defined:
            raise RuntimeError("native light lookup-curve evaluator was not retained")

        material_value_output = Path(temporary) / "particle-material-value.o"
        material_value_symbol = "?function_16f60@@YAXPAEPBG@Z"
        material_value_command = command[:]
        material_value_command[material_value_command.index("h2_extract_selected")] = material_value_symbol
        material_value_command[
            material_value_command.index("--extract-global"):material_value_command.index("--output")
        ] = []
        material_value_command[material_value_command.index(output)] = material_value_output
        material_value_command[-1] = args.root / "src/particle_material_value_native.cpp"
        subprocess.run([str(value) for value in material_value_command], check=True)
        material_value_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_value_output],
            text=True,
        )
        if material_value_symbol not in material_value_defined:
            raise RuntimeError("particle material-value loader was not selectively compilable")
        material_value_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_value_output],
            text=True,
        )
        if "?g_485a80@@3PAEA" not in material_value_undefined:
            raise RuntimeError("particle material-value mutable state boundary missing")

        material_bank_output = Path(temporary) / "particle-material-constant-bank.o"
        material_bank_symbol = "?function_17000@@YAXPAEPBG@Z"
        material_bank_command = command[:]
        material_bank_command[material_bank_command.index("h2_extract_selected")] = material_bank_symbol
        material_bank_command[
            material_bank_command.index("--extract-global"):material_bank_command.index("--output")
        ] = []
        material_bank_command[material_bank_command.index(output)] = material_bank_output
        material_bank_command[-1] = args.root / "src/particle_material_constant_bank_native.cpp"
        subprocess.run([str(value) for value in material_bank_command], check=True)
        material_bank_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_bank_output],
            text=True,
        )
        if material_bank_symbol not in material_bank_defined:
            raise RuntimeError("particle material constant-bank loader was not selectively compilable")
        material_bank_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_bank_output],
            text=True,
        )
        if material_bank_undefined.strip():
            raise RuntimeError("particle material constant-bank loader gained imports")

        material_masks_output = Path(temporary) / "particle-material-shader-masks.o"
        material_masks_symbol = "?function_17170@@YAXPAEPBG@Z"
        material_masks_global = "?g_467068@@3QBKB"
        material_masks_command = command[:]
        material_masks_command[material_masks_command.index("h2_extract_selected")] = material_masks_symbol
        material_masks_command[
            material_masks_command.index("--extract-global"):material_masks_command.index("--output")
        ] = ["--extract-global", material_masks_global]
        material_masks_command[material_masks_command.index(output)] = material_masks_output
        material_masks_command[-1] = args.root / "src/particle_material_shader_masks_native.cpp"
        subprocess.run([str(value) for value in material_masks_command], check=True)
        material_masks_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_masks_output],
            text=True,
        )
        if material_masks_symbol not in material_masks_defined or material_masks_global not in material_masks_defined:
            raise RuntimeError("particle material shader masks were not selectively compilable")
        material_masks_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_masks_output],
            text=True,
        )
        if material_masks_undefined.strip():
            raise RuntimeError("particle material shader-mask loader gained imports")

        material_curve_output = Path(temporary) / "particle-material-curve-route.o"
        material_curve_symbol = "?function_174d0@@YAXPAEJJJJ@Z"
        material_curve_dependency = "?function_17550@@YAXPAEPBG@Z"
        material_curve_command = command[:]
        material_curve_command[material_curve_command.index("h2_extract_selected")] = material_curve_symbol
        material_curve_command[
            material_curve_command.index("--extract-global"):material_curve_command.index("--output")
        ] = []
        material_curve_command[material_curve_command.index(output)] = material_curve_output
        material_curve_command[-1] = args.root / "src/particle_material_curve_route_native.cpp"
        subprocess.run([str(value) for value in material_curve_command], check=True)
        material_curve_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_curve_output],
            text=True,
        )
        if material_curve_symbol not in material_curve_defined:
            raise RuntimeError("particle material curve router was not selectively compilable")
        material_curve_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_curve_output],
            text=True,
        )
        if material_curve_dependency not in material_curve_undefined:
            raise RuntimeError("particle material curve router hid its evaluator boundary")

        material_curve_apply_output = Path(temporary) / "particle-material-curve-apply.o"
        material_curve_apply_symbol = "?function_17550@@YAXPAEPBG@Z"
        material_curve_eval_symbol = "?function_1b230@@YAMPBUs_1b230_function@@JJM@Z"
        material_curve_apply_command = command[:]
        material_curve_apply_command[material_curve_apply_command.index("h2_extract_selected")] = material_curve_apply_symbol
        material_curve_apply_command[
            material_curve_apply_command.index("--extract-global"):material_curve_apply_command.index("--output")
        ] = []
        material_curve_apply_command[material_curve_apply_command.index(output)] = material_curve_apply_output
        material_curve_apply_command[-1] = args.root / "src/particle_material_curve_apply_native.cpp"
        subprocess.run([str(value) for value in material_curve_apply_command], check=True)
        material_curve_apply_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_curve_apply_output],
            text=True,
        )
        if material_curve_eval_symbol not in material_curve_apply_undefined:
            raise RuntimeError("particle material curve application hid its evaluator bridge")

        material_curve_eval_output = Path(temporary) / "particle-material-curve-evaluate.o"
        material_curve_eval_command = command[:]
        material_curve_eval_command[material_curve_eval_command.index("h2_extract_selected")] = material_curve_eval_symbol
        material_curve_eval_command[
            material_curve_eval_command.index("--extract-global"):material_curve_eval_command.index("--output")
        ] = []
        material_curve_eval_command[material_curve_eval_command.index(output)] = material_curve_eval_output
        material_curve_eval_command[-1] = args.root / "src/particle_material_curve_evaluate_native.cpp"
        subprocess.run([str(value) for value in material_curve_eval_command], check=True)
        material_curve_eval_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_curve_eval_output],
            text=True,
        )
        expected_curve_eval_undefined = {
            "_?function_13b390@@YAMPBXMM@Z", "_?g_4858a0@@3NA",
            "_?g_485a58@@3PAUs_33a0b_view@@A", "_fmod",
        }
        actual_curve_eval_undefined = {
            line.split()[0] for line in material_curve_eval_undefined.splitlines() if line.strip()
        }
        if actual_curve_eval_undefined != expected_curve_eval_undefined:
            raise RuntimeError(
                f"particle material curve evaluator frontier changed: {sorted(actual_curve_eval_undefined)}")

        texture_cache_acquire_output = Path(temporary) / "particle-texture-cache-acquire.o"
        texture_cache_acquire_symbol = "?function_12310@@YAPAUD3DTexture@@PAUs_bitmap_view@@M@Z"
        texture_cache_acquire_command = command[:]
        texture_cache_acquire_command[
            texture_cache_acquire_command.index("h2_extract_selected")
        ] = texture_cache_acquire_symbol
        texture_cache_acquire_command[
            texture_cache_acquire_command.index("--extract-global"):texture_cache_acquire_command.index("--output")
        ] = []
        texture_cache_acquire_command[texture_cache_acquire_command.index(output)] = texture_cache_acquire_output
        texture_cache_acquire_command[-1] = args.root / "src/particle_texture_cache_acquire_native.cpp"
        subprocess.run([str(value) for value in texture_cache_acquire_command], check=True)
        texture_cache_acquire_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", texture_cache_acquire_output],
            text=True,
        )
        texture_cache_acquire_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", texture_cache_acquire_output],
            text=True,
        )
        if texture_cache_acquire_symbol not in texture_cache_acquire_defined:
            raise RuntimeError("particle texture cache acquisition was not selectively compilable")
        expected_texture_cache_acquire_imports = {
            "_?function_12ce00@@YAPAUD3DTexture@@PAUs_bitmap_data@@KM@Z",
            "_?g_4e6488@@3JA",
            "_?texture_cache_bitmap_get_texture@@YAPAUD3DTexture@@PAUs_bitmap_data@@KM@Z",
        }
        actual_texture_cache_acquire_imports = {
            line.split()[0] for line in texture_cache_acquire_undefined.splitlines() if line.strip()
        }
        if actual_texture_cache_acquire_imports != expected_texture_cache_acquire_imports:
            raise RuntimeError("particle texture cache acquisition frontier changed")

        texture_residency_request_output = Path(temporary) / "particle-texture-residency-request.o"
        texture_residency_request_symbol = "?function_143c0@@YA_NJFFM@Z"
        texture_residency_request_command = command[:]
        texture_residency_request_command[
            texture_residency_request_command.index("h2_extract_selected")
        ] = texture_residency_request_symbol
        texture_residency_request_command[
            texture_residency_request_command.index("--extract-global"):texture_residency_request_command.index("--output")
        ] = []
        texture_residency_request_command[
            texture_residency_request_command.index(output)
        ] = texture_residency_request_output
        texture_residency_request_command[-1] = args.root / "src/particle_texture_residency_request_native.cpp"
        subprocess.run([str(value) for value in texture_residency_request_command], check=True)
        texture_residency_request_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", texture_residency_request_output],
            text=True,
        )
        texture_residency_request_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", texture_residency_request_output],
            text=True,
        )
        if texture_residency_request_symbol not in texture_residency_request_defined:
            raise RuntimeError("particle texture residency request was not selectively compilable")
        expected_texture_residency_request_imports = {
            "_?function_12310@@YAPAUD3DTexture@@PAUs_bitmap_view@@M@Z",
            "_?function_137550@@YAPAUs_type_7ba8e9@@JF@Z",
            "_?g_485a80@@3PAEA", "_?g_485af4@@3PAJA", "_?g_485b04@@3PAJA",
            "_?g_4e3b44@@3PAUs_tag_instance@@A", "_?g_51f3c8@@3PAY03PAXA",
        }
        actual_texture_residency_request_imports = {
            line.split()[0] for line in texture_residency_request_undefined.splitlines() if line.strip()
        }
        if actual_texture_residency_request_imports != expected_texture_residency_request_imports:
            raise RuntimeError("particle texture residency request frontier changed")

        material_color_range_output = Path(temporary) / "particle-material-color-range.o"
        material_color_range_symbol = "?function_19ca0@@YAXPAEPBG@Z"
        material_color_range_command = command[:]
        material_color_range_command[
            material_color_range_command.index("h2_extract_selected")
        ] = material_color_range_symbol
        material_color_range_command[
            material_color_range_command.index("--extract-global"):material_color_range_command.index("--output")
        ] = []
        material_color_range_command[material_color_range_command.index(output)] = material_color_range_output
        material_color_range_command[-1] = args.root / "src/particle_material_color_range_native.cpp"
        subprocess.run([str(value) for value in material_color_range_command], check=True)
        material_color_range_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_color_range_output],
            text=True,
        )
        material_color_range_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_color_range_output],
            text=True,
        )
        if material_color_range_symbol not in material_color_range_defined:
            raise RuntimeError("particle material color-range expansion was not selectively compilable")
        expected_material_color_range_imports = {
            "_?function_1b2c0@@YA_NPBEJJJJPAM@Z", "_?g_467068@@3QBKB",
            "_?pack_color4f@@YAKPBUcolor4f@@@Z", "_h2_guest_memset",
        }
        actual_material_color_range_imports = {
            line.split()[0] for line in material_color_range_undefined.splitlines() if line.strip()
        }
        if actual_material_color_range_imports != expected_material_color_range_imports:
            raise RuntimeError("particle material color-range frontier changed")

        material_texture_binding_output = Path(temporary) / "particle-material-texture-binding.o"
        material_texture_binding_symbol = "?function_18a90@@YAXPAEPBG@Z"
        material_texture_binding_command = command[:]
        material_texture_binding_command[
            material_texture_binding_command.index("h2_extract_selected")
        ] = material_texture_binding_symbol
        material_texture_binding_command[
            material_texture_binding_command.index("--extract-global"):material_texture_binding_command.index("--output")
        ] = []
        material_texture_binding_command[
            material_texture_binding_command.index(output)
        ] = material_texture_binding_output
        material_texture_binding_command[-1] = args.root / "src/particle_material_texture_binding_native.cpp"
        subprocess.run([str(value) for value in material_texture_binding_command], check=True)
        material_texture_binding_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_texture_binding_output],
            text=True,
        )
        material_texture_binding_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_texture_binding_output],
            text=True,
        )
        if material_texture_binding_symbol not in material_texture_binding_defined:
            raise RuntimeError("particle material texture binding refresh was not selectively compilable")
        expected_material_texture_binding_imports = {
            "_?function_19530@@YAXJPAEFPAJ1@Z", "_?g_485a60@@3JA",
            "_?g_485a80@@3PAEA", "_?g_4e0344@@3PAUs_structure_bsp_globals@@A",
            "_?g_4e0348@@3PAUs_match_globals@@A", "_?g_4e3b44@@3PAUs_tag_instance@@A",
            "_floor", "_h2_guest_memcpy", "_log",
        }
        actual_material_texture_binding_imports = {
            line.split()[0] for line in material_texture_binding_undefined.splitlines() if line.strip()
        }
        if actual_material_texture_binding_imports != expected_material_texture_binding_imports:
            raise RuntimeError("particle material texture binding frontier changed")

        material_combiner_output = Path(temporary) / "particle-material-combiner-state.o"
        material_combiner_symbol = "?function_1ba00@@YAXPAE@Z"
        material_combiner_command = command[:]
        material_combiner_command[material_combiner_command.index("h2_extract_selected")] = material_combiner_symbol
        material_combiner_command[
            material_combiner_command.index("--extract-global"):material_combiner_command.index("--output")
        ] = []
        material_combiner_command[material_combiner_command.index(output)] = material_combiner_output
        material_combiner_command[-1] = args.root / "src/particle_material_combiner_state_native.cpp"
        subprocess.run([str(value) for value in material_combiner_command], check=True)
        material_combiner_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_combiner_output],
            text=True,
        )
        material_combiner_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_combiner_output],
            text=True,
        )
        if material_combiner_symbol not in material_combiner_defined:
            raise RuntimeError("particle material combiner-state expansion was not selectively compilable")
        if material_combiner_undefined.strip():
            raise RuntimeError("particle material combiner-state expansion gained imports")

        material_residency_output = Path(temporary) / "particle-material-texture-residency.o"
        material_residency_symbol = "?function_1b0a0@@YAXPAE@Z"
        material_residency_command = command[:]
        material_residency_command[material_residency_command.index("h2_extract_selected")] = material_residency_symbol
        material_residency_command[
            material_residency_command.index("--extract-global"):material_residency_command.index("--output")
        ] = []
        material_residency_command[material_residency_command.index(output)] = material_residency_output
        material_residency_command[-1] = args.root / "src/particle_material_texture_residency_native.cpp"
        subprocess.run([str(value) for value in material_residency_command], check=True)
        material_residency_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_residency_output],
            text=True,
        )
        material_residency_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_residency_output],
            text=True,
        )
        if material_residency_symbol not in material_residency_defined:
            raise RuntimeError("particle material texture residency refresh was not selectively compilable")
        expected_material_residency_imports = {
            "_?function_143c0@@YA_NJFFM@Z", "_?g_4ba019@@3_NA",
            "_?g_4e3b44@@3PAUs_tag_instance@@A", "_fabs", "_fmod",
        }
        actual_material_residency_imports = {
            line.split()[0] for line in material_residency_undefined.splitlines() if line.strip()
        }
        if actual_material_residency_imports != expected_material_residency_imports:
            raise RuntimeError("particle material texture residency frontier changed")

        material_policy_output = Path(temporary) / "particle-material-policy-state.o"
        material_policy_symbol = "?function_34060@@YAJJK@Z"
        material_policy_command = command[:]
        material_policy_command[material_policy_command.index("h2_extract_selected")] = material_policy_symbol
        material_policy_command[
            material_policy_command.index("--extract-global"):material_policy_command.index("--output")
        ] = []
        material_policy_command[material_policy_command.index(output)] = material_policy_output
        material_policy_command[-1] = args.root / "src/particle_material_policy_state_native.cpp"
        subprocess.run([str(value) for value in material_policy_command], check=True)
        material_policy_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_policy_output],
            text=True,
        )
        material_policy_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_policy_output],
            text=True,
        )
        if material_policy_symbol not in material_policy_defined:
            raise RuntimeError("particle material policy state selector was not selectively compilable")
        expected_material_policy_imports = {
            "_?g_4858c4@@3PAEA", "_?g_485a2c@@3JA", "_?g_485a64@@3JA",
            "_?g_485a68@@3JA", "_?g_4ba014@@3KA",
        }
        actual_material_policy_imports = {
            line.split()[0] for line in material_policy_undefined.splitlines() if line.strip()
        }
        if actual_material_policy_imports != expected_material_policy_imports:
            raise RuntimeError("particle material policy state frontier changed")
        if "switch.table" in material_policy_undefined:
            raise RuntimeError("particle material policy selector gained a false switch provider")

        material_external_output = Path(temporary) / "particle-material-external-state.o"
        material_external_symbol = "?function_1ae70@@YAXPAE@Z"
        material_external_command = command[:]
        material_external_command[material_external_command.index("h2_extract_selected")] = material_external_symbol
        material_external_command[
            material_external_command.index("--extract-global"):material_external_command.index("--output")
        ] = []
        material_external_command[material_external_command.index(output)] = material_external_output
        material_external_command[-1] = args.root / "src/particle_material_external_state_native.cpp"
        subprocess.run([str(value) for value in material_external_command], check=True)
        material_external_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_external_output],
            text=True,
        )
        material_external_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_external_output],
            text=True,
        )
        if material_external_symbol not in material_external_defined:
            raise RuntimeError("particle material external state refresh was not selectively compilable")
        expected_material_external_imports = {
            "_?function_336f0@@YAMJ@Z", "_?function_33980@@YAXJPAUvector3f@@@Z",
            "_?function_34060@@YAJJK@Z",
        }
        actual_material_external_imports = {
            line.split()[0] for line in material_external_undefined.splitlines() if line.strip()
        }
        if actual_material_external_imports != expected_material_external_imports:
            raise RuntimeError("particle material external state frontier changed")

        material_table_output = Path(temporary) / "particle-material-table-select.o"
        material_table_symbol = "?function_17420@@YAXPAEJJJJ@Z"
        material_table_command = command[:]
        material_table_command[material_table_command.index("h2_extract_selected")] = material_table_symbol
        material_table_command[
            material_table_command.index("--extract-global"):material_table_command.index("--output")
        ] = []
        material_table_command[material_table_command.index(output)] = material_table_output
        material_table_command[-1] = args.root / "src/particle_material_table_select_native.cpp"
        subprocess.run([str(value) for value in material_table_command], check=True)
        material_table_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_table_output],
            text=True,
        )
        material_table_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_table_output],
            text=True,
        )
        if material_table_symbol not in material_table_defined:
            raise RuntimeError("particle material table selector was not selectively compilable")
        expected_material_table_imports = {
            "_?function_176a0@@YAXPAEPBG@Z", "_?function_17960@@YAXPAEPBG@Z",
            "_?function_17b60@@YAXPAEPBG@Z", "_?function_18560@@YAXPAEPBG@Z",
        }
        actual_material_table_imports = {
            line.split()[0] for line in material_table_undefined.splitlines() if line.strip()
        }
        if actual_material_table_imports != expected_material_table_imports:
            raise RuntimeError("particle material table selection frontier changed")

        material_parameter_output = Path(temporary) / "particle-material-parameter.o"
        material_parameter_symbol = "?function_1b2c0@@YA_NPBEJJJJPAM@Z"
        material_parameter_command = command[:]
        material_parameter_command[material_parameter_command.index("h2_extract_selected")] = material_parameter_symbol
        material_parameter_command[
            material_parameter_command.index("--extract-global"):material_parameter_command.index("--output")
        ] = []
        material_parameter_command[material_parameter_command.index(output)] = material_parameter_output
        material_parameter_command[-1] = args.root / "src/particle_material_parameter_native.cpp"
        subprocess.run([str(value) for value in material_parameter_command], check=True)
        material_parameter_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_parameter_output],
            text=True,
        )
        material_parameter_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_parameter_output],
            text=True,
        )
        if material_parameter_symbol not in material_parameter_defined:
            raise RuntimeError("particle material parameter unpacker was not selectively compilable")
        if "sin" not in material_parameter_undefined or "cos" not in material_parameter_undefined:
            raise RuntimeError("particle material parameter math frontier changed")
        if "switch.table" in material_parameter_undefined:
            raise RuntimeError("particle material parameter unpacker gained a false switch provider")

        material_range_output = Path(temporary) / "particle-material-parameter-range.o"
        material_range_symbol = "?function_19be0@@YAXPAEPBG@Z"
        material_range_command = command[:]
        material_range_command[material_range_command.index("h2_extract_selected")] = material_range_symbol
        material_range_command[
            material_range_command.index("--extract-global"):material_range_command.index("--output")
        ] = []
        material_range_command[material_range_command.index(output)] = material_range_output
        material_range_command[-1] = args.root / "src/particle_material_parameter_native.cpp"
        subprocess.run([str(value) for value in material_range_command], check=True)
        material_range_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_range_output],
            text=True,
        )
        if material_range_symbol not in material_range_defined:
            raise RuntimeError("particle material parameter range was not selectively compilable")

        material_vertex_output = Path(temporary) / "particle-material-vertex-data-flush.o"
        material_vertex_symbol = "?function_18d70@@YAXPAUs_18d70_state@@@Z"
        material_vertex_boundary = "D3DDevice_SetVertexData4f"
        material_vertex_command = command[:]
        material_vertex_command[material_vertex_command.index("h2_extract_selected")] = material_vertex_symbol
        material_vertex_command[
            material_vertex_command.index("--extract-global"):material_vertex_command.index("--output")
        ] = []
        material_vertex_command[material_vertex_command.index(output)] = material_vertex_output
        material_vertex_command[-1] = args.root / "src/particle_material_vertex_data_flush_native.cpp"
        subprocess.run([str(value) for value in material_vertex_command], check=True)
        material_vertex_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_vertex_output],
            text=True,
        )
        material_vertex_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_vertex_output],
            text=True,
        )
        if material_vertex_symbol not in material_vertex_defined:
            raise RuntimeError("particle material vertex-data flush was not selectively compilable")
        if material_vertex_boundary not in material_vertex_undefined:
            raise RuntimeError("particle material vertex-data flush hid its D3D boundary")

        material_constant_output = Path(temporary) / "particle-material-shader-constant-flush.o"
        material_constant_symbol = "?function_18e80@@YAXPAUs_shader_constant_state@@@Z"
        material_constant_boundary = "D3DDevice_SetVertexShaderConstantFast"
        material_constant_command = command[:]
        material_constant_command[material_constant_command.index("h2_extract_selected")] = material_constant_symbol
        material_constant_command[
            material_constant_command.index("--extract-global"):material_constant_command.index("--output")
        ] = []
        material_constant_command[material_constant_command.index(output)] = material_constant_output
        material_constant_command[-1] = args.root / "src/particle_material_shader_constant_flush_native.cpp"
        subprocess.run([str(value) for value in material_constant_command], check=True)
        material_constant_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_constant_output],
            text=True,
        )
        material_constant_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_constant_output],
            text=True,
        )
        if material_constant_symbol not in material_constant_defined:
            raise RuntimeError("particle material shader constant flush was not selectively compilable")
        if material_constant_boundary not in material_constant_undefined:
            raise RuntimeError("particle material shader constant flush hid its D3D boundary")

        material_pixel_output = Path(temporary) / "particle-material-pixel-shader-submit.o"
        material_pixel_symbol = "?function_1b050@@YAXPAUs_01b050_shader_state@@@Z"
        material_pixel_boundary = "D3DDevice_SetPixelShaderProgram"
        material_pixel_command = command[:]
        material_pixel_command[material_pixel_command.index("h2_extract_selected")] = material_pixel_symbol
        material_pixel_command[
            material_pixel_command.index("--extract-global"):material_pixel_command.index("--output")
        ] = []
        material_pixel_command[material_pixel_command.index(output)] = material_pixel_output
        material_pixel_command[-1] = args.root / "src/particle_material_pixel_shader_submit_native.cpp"
        subprocess.run([str(value) for value in material_pixel_command], check=True)
        material_pixel_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", material_pixel_output],
            text=True,
        )
        material_pixel_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", material_pixel_output],
            text=True,
        )
        if material_pixel_symbol not in material_pixel_defined:
            raise RuntimeError("particle material pixel shader submission was not selectively compilable")
        if material_pixel_boundary not in material_pixel_undefined:
            raise RuntimeError("particle material pixel shader submission hid its D3D boundary")

        render_reset_output = Path(temporary) / "particle-render-state-reset.o"
        render_reset_symbols = [
            "?function_16b10@@YAXPAUs_render_reset_state@@@Z",
            "?function_18900@@YAXPBUs_render_state_source@@PBG@Z",
        ]
        render_reset_command = command[:]
        render_reset_command[render_reset_command.index("h2_extract_selected")] = render_reset_symbols[0]
        render_reset_global_index = render_reset_command.index("--extract-global")
        render_reset_command[render_reset_global_index:render_reset_global_index] = [
            "--extract-symbol", render_reset_symbols[1]]
        render_reset_command[
            render_reset_command.index("--extract-global"):render_reset_command.index("--output")
        ] = []
        render_reset_command[render_reset_command.index(output)] = render_reset_output
        render_reset_command[-1] = args.root / "src/particle_render_state_reset_native.cpp"
        subprocess.run([str(value) for value in render_reset_command], check=True)
        render_reset_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", render_reset_output],
            text=True,
        )
        for symbol in render_reset_symbols:
            if symbol not in render_reset_defined:
                raise RuntimeError(f"particle render-state reset provider missing: {symbol}")
        render_reset_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", render_reset_output],
            text=True,
        )
        if "D3DDevice_SetRenderStateNotInline" not in render_reset_undefined:
            raise RuntimeError("particle render-state submission boundary missing")

        sprite_setup_output = Path(temporary) / "particle-sprite-setup.o"
        sprite_setup_symbol = "?function_23ea80@@YA_NJPBUtransform4x3f@@JJJ@Z"
        sprite_setup_command = command[:]
        sprite_setup_command[sprite_setup_command.index("h2_extract_selected")] = sprite_setup_symbol
        sprite_setup_command[sprite_setup_command.index("--extract-global"):sprite_setup_command.index("--output")] = []
        sprite_setup_command[sprite_setup_command.index(output)] = sprite_setup_output
        sprite_setup_command[-1] = args.root / "src/particle_sprite_setup_native.cpp"
        subprocess.run([str(value) for value in sprite_setup_command], check=True)
        sprite_setup_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", sprite_setup_output],
            text=True,
        )
        if f"_{sprite_setup_symbol}" not in sprite_setup_defined:
            raise RuntimeError("native particle sprite setup was not selectively compilable")

        sprite_geometry_output = Path(temporary) / "particle-sprite-geometry.o"
        sprite_geometry_symbol = "?function_23ed30@@YAXPATpoint3f@@0MMKMPAM11@Z"
        sprite_geometry_command = command[:]
        sprite_geometry_command[sprite_geometry_command.index("h2_extract_selected")] = sprite_geometry_symbol
        sprite_geometry_command[sprite_geometry_command.index("--extract-global"):sprite_geometry_command.index("--output")] = []
        sprite_geometry_command[sprite_geometry_command.index(output)] = sprite_geometry_output
        sprite_geometry_command[-1] = args.root / "src/particle_sprite_geometry_native.cpp"
        subprocess.run([str(value) for value in sprite_geometry_command], check=True)
        sprite_geometry_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", sprite_geometry_output],
            text=True,
        )
        if f"_{sprite_geometry_symbol}" not in sprite_geometry_defined:
            raise RuntimeError("native particle sprite geometry was not selectively compilable")

        push_buffer_output = Path(temporary) / "particle-push-buffer.o"
        push_buffer_symbol = "?function_015d00@@YA_NJPAPAK@Z"
        push_buffer_command = command[:]
        push_buffer_command[push_buffer_command.index("h2_extract_selected")] = push_buffer_symbol
        push_buffer_command[
            push_buffer_command.index("--extract-global"):push_buffer_command.index("--output")
        ] = []
        push_buffer_command[push_buffer_command.index(output)] = push_buffer_output
        push_buffer_command[-1] = args.root / "src/particle_push_buffer_native.cpp"
        subprocess.run([str(value) for value in push_buffer_command], check=True)
        push_buffer_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", push_buffer_output],
            text=True,
        )
        if push_buffer_symbol not in push_buffer_defined:
            raise RuntimeError("particle push-buffer reservation was not selectively compilable")
        push_buffer_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", push_buffer_output],
            text=True,
        )
        if "D3DDevice_BeginPush" not in push_buffer_undefined:
            raise RuntimeError("particle push-buffer allocation boundary missing")

        mesh_callback_output = Path(temporary) / "particle-mesh-callback.o"
        mesh_callback_symbol = "?function_224fd0@@YAXPBUs_draw_mesh_seed@@PAUs_draw_value_cache@@PBTpoint3f@@PBTvector3f@@MPBUcolor4f@@PBUs_draw_mesh_context@@@Z"
        mesh_callback_command = command[:]
        mesh_callback_command[mesh_callback_command.index("h2_extract_selected")] = mesh_callback_symbol
        mesh_callback_command[mesh_callback_command.index("--extract-global"):mesh_callback_command.index("--output")] = []
        mesh_callback_command[mesh_callback_command.index(output)] = mesh_callback_output
        mesh_callback_command[-1] = args.root / "src/unknown_224fd0.cpp"
        subprocess.run([str(value) for value in mesh_callback_command], check=True)
        mesh_callback_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", mesh_callback_output],
            text=True,
        )
        if f"_{mesh_callback_symbol}" not in mesh_callback_defined:
            raise RuntimeError("particle mesh callback was not selectively compilable")
        mesh_callback_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", mesh_callback_output],
            text=True,
        )
        if "switch.table." in mesh_callback_undefined:
            raise RuntimeError("particle mesh callback exposed a compiler switch table")

        texture_state_output = Path(temporary) / "particle-texture-state.o"
        texture_state_symbols = ["?function_1cf50@@YAXXZ", "?function_1cf80@@YAXXZ"]
        texture_state_command = command[:]
        texture_state_command[texture_state_command.index("h2_extract_selected")] = texture_state_symbols[0]
        texture_global_index = texture_state_command.index("--extract-global")
        texture_state_command[texture_global_index:texture_global_index] = [
            "--extract-symbol", texture_state_symbols[1]]
        texture_state_command[texture_state_command.index("--extract-global"):texture_state_command.index("--output")] = []
        texture_state_command[texture_state_command.index(output)] = texture_state_output
        texture_state_command[-1] = args.root / "src/particle_texture_state_native.cpp"
        subprocess.run([str(value) for value in texture_state_command], check=True)
        texture_state_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", texture_state_output],
            text=True,
        )
        for symbol in texture_state_symbols:
            if f"_{symbol}" not in texture_state_defined:
                raise RuntimeError(f"particle texture state provider missing: {symbol}")

        vertex_register_output = Path(temporary) / "particle-vertex-register-state.o"
        vertex_register_symbol = "?function_23ecd0@@YAXXZ"
        vertex_register_command = command[:]
        vertex_register_command[vertex_register_command.index("h2_extract_selected")] = vertex_register_symbol
        vertex_register_command[
            vertex_register_command.index("--extract-global"):vertex_register_command.index("--output")
        ] = []
        vertex_register_command[vertex_register_command.index(output)] = vertex_register_output
        vertex_register_command[-1] = args.root / "src/particle_vertex_register_state_native.cpp"
        subprocess.run([str(value) for value in vertex_register_command], check=True)
        vertex_register_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", vertex_register_output],
            text=True,
        )
        if f"_{vertex_register_symbol}" not in vertex_register_defined:
            raise RuntimeError("particle vertex register initializer was not selectively compilable")

        shader_state_output = Path(temporary) / "particle-shader-cache-state.o"
        shader_state_symbols = [
            "?function_1c4a0@@YAXPAUs_shader_cache@@@Z",
            "?function_1c6b0@@YAXPAX@Z",
            "?function_1ccb0@@YAXJ@Z",
        ]
        shader_state_command = command[:]
        shader_state_command[shader_state_command.index("h2_extract_selected")] = shader_state_symbols[0]
        shader_global_index = shader_state_command.index("--extract-global")
        shader_state_command[shader_global_index:shader_global_index] = [
            "--extract-symbol", shader_state_symbols[1],
            "--extract-symbol", shader_state_symbols[2]]
        shader_state_command[shader_state_command.index("--extract-global") + 1] = "g_43f408"
        shader_alias_index = shader_state_command.index("--extract-alias")
        del shader_state_command[shader_alias_index:shader_alias_index + 2]
        shader_state_command[shader_state_command.index(output)] = shader_state_output
        shader_state_command[-1] = args.root / "src/particle_shader_cache_state_native.cpp"
        subprocess.run([str(value) for value in shader_state_command], check=True)
        shader_state_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", shader_state_output],
            text=True,
        )
        for symbol in shader_state_symbols:
            if f"_{symbol}" not in shader_state_defined:
                raise RuntimeError(f"particle shader state provider missing: {symbol}")
        if "g_43f408" not in shader_state_defined:
            raise RuntimeError("particle shader descriptor table was not retained")

        shader_input_output = Path(temporary) / "particle-shader-input.o"
        shader_input_symbols = [
            "?function_1c330@@YAXPAEPAUs_1c330_output@@@Z",
            "?function_1c710@@YAXPAX@Z",
            "?function_35790@@YAJJ@Z",
            "?function_35850@@YAJJ@Z",
        ]
        shader_input_command = command[:]
        shader_input_command[shader_input_command.index("h2_extract_selected")] = shader_input_symbols[0]
        shader_input_command[
            shader_input_command.index("--extract-global"):shader_input_command.index("--output")
        ] = sum((["--extract-symbol", symbol] for symbol in shader_input_symbols[1:]), [])
        shader_input_command[shader_input_command.index(output)] = shader_input_output
        shader_input_command[-1] = args.root / "src/particle_shader_descriptor_native.cpp"
        subprocess.run([str(value) for value in shader_input_command], check=True)
        shader_input_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", shader_input_output],
            text=True,
        )
        for symbol in shader_input_symbols:
            if f"_{symbol}" not in shader_input_defined:
                raise RuntimeError(f"particle shader input provider missing: {symbol}")
        shader_input_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", shader_input_output],
            text=True,
        )
        for provider in ("D3DDevice_SelectVertexShaderDirect",
                         "D3DDevice_SetVertexShaderInputDirect"):
            if provider not in shader_input_undefined:
                raise RuntimeError(f"particle shader input boundary missing: {provider}")
        if "switch.table." in shader_input_undefined:
            raise RuntimeError("particle shader input extraction exposed a compiler switch table")

        shader_program_output = Path(temporary) / "particle-shader-program-state.o"
        shader_program_symbols = [
            "?function_1c2e0@@YAPBKJJPAJ@Z",
            "?function_1c590@@YAXPAUs_shader_cache@@JJ@Z",
        ]
        shader_program_command = command[:]
        shader_program_command[shader_program_command.index("h2_extract_selected")] = shader_program_symbols[0]
        shader_program_command[
            shader_program_command.index("--extract-global"):shader_program_command.index("--output")
        ] = [
            "--extract-symbol", shader_program_symbols[1],
            "--extract-global", "g_43f2b0",
            "--extract-global", "?g_4670ec@@3PAUs_shader_program@@A",
        ]
        shader_program_command[shader_program_command.index(output)] = shader_program_output
        shader_program_command[-1] = args.root / "src/particle_shader_program_state_native.cpp"
        subprocess.run([str(value) for value in shader_program_command], check=True)
        shader_program_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", shader_program_output],
            text=True,
        )
        for symbol in [*shader_program_symbols, "g_43f2b0",
                       "?g_4670ec@@3PAUs_shader_program@@A"]:
            if symbol not in shader_program_defined:
                raise RuntimeError(f"particle shader program provider missing: {symbol}")
        shader_program_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", shader_program_output],
            text=True,
        )
        if "D3DDevice_LoadVertexShaderProgram" not in shader_program_undefined:
            raise RuntimeError("particle shader program upload boundary missing")

        shader_constants_output = Path(temporary) / "particle-shader-constants.o"
        shader_constants_symbol = "?function_23e460@@YAXXZ"
        shader_constants_command = command[:]
        shader_constants_command[shader_constants_command.index("h2_extract_selected")] = shader_constants_symbol
        shader_constants_command[
            shader_constants_command.index("--extract-global"):shader_constants_command.index("--output")
        ] = []
        shader_constants_command[shader_constants_command.index(output)] = shader_constants_output
        shader_constants_command[-1] = args.root / "src/particle_shader_constants_native.cpp"
        subprocess.run([str(value) for value in shader_constants_command], check=True)
        shader_constants_defined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--defined-only", "--format=posix", shader_constants_output],
            text=True,
        )
        if shader_constants_symbol not in shader_constants_defined:
            raise RuntimeError("particle shader constants provider was not selectively compilable")
        shader_constants_undefined = subprocess.check_output(
            [args.llvm / "llvm-nm", "--undefined-only", "--format=posix", shader_constants_output],
            text=True,
        )
        for provider in ("?g_4856c4@@3PAMA", "?g_485af0@@3EA",
                         "D3DDevice_SetVertexShaderConstant", "cos", "sin"):
            if provider not in shader_constants_undefined:
                raise RuntimeError(f"particle shader constants boundary missing: {provider}")

    print("guest extraction retained constants and parsed recovered UI/rendering sources")


if __name__ == "__main__":
    main()
