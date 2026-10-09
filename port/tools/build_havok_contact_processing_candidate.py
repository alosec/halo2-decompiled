#!/usr/bin/env python3
import argparse
import subprocess
import tempfile
from pathlib import Path


def run(command):
    subprocess.run([str(value) for value in command], check=True)


def compile_slice(args, source, output, symbols):
    command = [
        args.python, args.root / "port/tools/guest_elf_cc.py",
        "--llvm", args.llvm,
        "--plugin", args.plugin,
        "--include", args.root / "port/include",
        "--include", args.root / "port/include/crt",
        "--include", args.root / "port/include/xdk",
        "--include", args.root / "include",
        "--ms-abi",
    ]
    for symbol in symbols:
        command.extend(("--extract-symbol", symbol))
    command.extend(("--output", output, source))
    run(command)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--python", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="h2-contact-material-") as temporary:
        temporary = Path(temporary)
        lookup = temporary / "lookup.o"
        properties = temporary / "properties.o"
        velocity = temporary / "velocity.o"
        rigid_velocity = temporary / "rigid-velocity.o"
        combine = temporary / "combine.o"
        impact_data = temporary / "impact-data.o"
        impact_validity = temporary / "impact-validity.o"
        candidate = temporary / "candidate.o"
        compile_slice(args, args.root / "src/unknown_1ee410_native.cpp", lookup,
                      ["?function_1ee410@@YAXPBUs_lookup_source@@PAF@Z"])
        compile_slice(args, args.root / "src/unknown_182020.cpp", properties,
                      ["?function_182b90@@YAXPAVc_material_shape@@PBVhkEntity@@PAM2PAF@Z"])
        compile_slice(args, args.root / "src/unknown_0b8bd0.cpp", velocity,
                      ["?function_ba1d0@@YAXJPATvector3f@@0@Z"])
        compile_slice(
            args, args.root / "src/unknown_1cec30.cpp", rigid_velocity,
            ["?havok_component_rigid_body_linear_velocity_get@@YAXJPAUs_havok_component@@PATvector3f@@@Z",
             "?havok_component_rigid_body_angular_velocity_get@@YAXJPAUs_havok_component@@PATvector3f@@@Z"])
        compile_slice(args, args.root / "src/unknown_277ec0.cpp", combine,
                      ["?function_277ec0@@YAXPAM0PAF1PAUs_277ec0@@2JJ@Z"])
        compile_slice(
            args, args.root / "src/unknown_2055f0.cpp", impact_data,
            ["?impact_data_set@@YAXPAUs_impact_data@@_NJJVc_type_47f957@@JJ2PBTpoint3f@@PBTvector3f@@JPBUs_physics_model_shape_key@@@Z",
             "?havok_component_impact_add@@YAXPAUs_havok_component@@J@Z"])
        compile_slice(args, args.root / "src/impacts.cpp", impact_validity,
                      ["?impact_components_valid@@YA_NJJ@Z"])
        compile_slice(
            args, args.root / "src/unknown_277b80.cpp", candidate,
            ["?function_277b80@@YAJPAX_NMPAVc_1cf520@@FPAUs_277ec0@@3PAVhkEntity@@4PAPAVc_277b80@@@Z"])
        run([args.lld, "-r", "--allow-multiple-definition", "-o", args.output,
             lookup, properties, velocity, rigid_velocity, combine, impact_data,
             impact_validity, candidate])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
