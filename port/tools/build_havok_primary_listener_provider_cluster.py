#!/usr/bin/env python3
import argparse
import subprocess
import tempfile
from pathlib import Path


PRIMARY_SOURCE_SYMBOLS = [
    "?test@c_callback_277820@@UAA?AVhkBool@@PAX0@Z",
    "?event0@c_interface_277890@@UAAXPAX@Z",
    "?event1@c_interface_277890@@UAAXPAX@Z",
    "?event2@c_interface_277890@@UAAXPAX@Z",
    "?slot1@c_interface_277890@@UAAXPAVhkEntity@@@Z",
    "?slot2@c_interface_277890@@UAAXPAVhkEntity@@@Z",
    "?test@c_interface_277890@@UAA_NPAUs_278a70_body@@0@Z",
    "?slot2@c_callback_2789b0@@UAAXPAUs_278a70_event@@@Z",
    "?slot4@c_callback_2789b0@@UAAXPAX@Z",
    "h2_native_primary_listener_embedded_delete",
    "h2_native_primary_listener_delete",
    "h2_native_primary_listener_callback_adjusted_delete",
    "h2_native_primary_listener_callback_delete",
]
PRIMARY_SOURCE_ALIASES = []
SECONDARY_SYMBOL = "?function_2789f0@c_2789f0@@UAAXPAUs_2789f1@@@Z"


def run(command):
    subprocess.run([str(value) for value in command], check=True)


def compile_slice(args, source, output, symbols, aliases=(), globals=()):
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
    for alias in aliases:
        command.extend(("--extract-alias", alias))
    for global_name in globals:
        command.extend(("--extract-global", global_name))
    command.extend(("--output", output, source))
    run(command)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--python", type=Path, required=True)
    parser.add_argument("--llvm", type=Path, required=True)
    parser.add_argument("--lld", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--tables", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="h2-primary-listener-") as temporary:
        temporary = Path(temporary)
        primary = temporary / "primary.o"
        secondary = temporary / "secondary.o"
        constructor = temporary / "constructor.o"
        transition = temporary / "transition.o"
        scale = temporary / "scale.o"
        compile_slice(args, args.root / "src/unknown_2777d0.cpp", primary,
                      PRIMARY_SOURCE_SYMBOLS, PRIMARY_SOURCE_ALIASES)
        compile_slice(args, args.root / "src/unknown_2789f0.cpp", secondary,
                      [SECONDARY_SYMBOL])
        compile_slice(args,
                      args.root / "src/havok_primary_listener_construct_native.cpp",
                      constructor, ["??0c_interface_277890@@QAA@XZ"])
        compile_slice(
            args, args.root / "src/unknown_315340_native.cpp", transition,
            ["?function_315340@@YAXPAXPAVhkEntity@@1MM@Z"], globals=[
                "g_h2_native_contact_transition_pending_table",
                "g_h2_native_contact_transition_complete_table",
            ])
        compile_slice(args, args.root / "src/unknown_181e90_native.cpp", scale, [
            "?function_181e90@@YA_NMM@Z",
            "?function_181ee0@@YAXPBUs_scale_owner@@0PAM1@Z",
            "?get_inverse_scale@s_scale_owner@@QBAMXZ",
        ], globals=["?g_54e870@@3MA"])
        run([args.lld, "-r", "--allow-multiple-definition", "-o", args.output,
             args.tables, primary, secondary, constructor, transition, scale])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
