#!/bin/sh
# check_unit.sh SOURCE: compile one source the way the whole guest image does
# (retail mode first, native fallback) and print its diagnostics. Needs a
# configured build/native tree for the compiler plugin.
root=$(cd "$(dirname "$0")/../.." && pwd)
exec python3 "$root/port/tools/build_whole_image.py" --root "$root" \
    --llvm /opt/homebrew/opt/llvm@22/bin --lld /opt/homebrew/bin/ld.lld \
    --plugin "$root/build/native/port/compiler/h2_guest_rebase.dylib" \
    --work "$root/build/native/port/whole" \
    --start "$root/port/runtime/whole_image_start.s" \
    --linker-script "$root/port/runtime/main_menu_guest.ld" \
    --check "$1"
