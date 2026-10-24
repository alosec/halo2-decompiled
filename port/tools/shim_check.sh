#!/bin/bash
# usage: port/tools/shim_check.sh FILE... -> first errors of each recovered unit under the whole-image flags
cd "$(dirname "$0")/../.."
for f in "$@"; do
  out=$(/opt/homebrew/opt/llvm@22/bin/clang++ --target=thumbv7-pc-windows-msvc -fsyntax-only -ferror-limit=${LIMIT:-8} \
    -DH2_NATIVE_PORT=1 -DH2_PORT_SHIMS=1 -DH2_WHOLE_IMAGE=1 -DH2_MS_ABI_FRONTEND=1 -Xclang -fno-wchar -fms-extensions -fno-exceptions -fno-rtti -w \
    -Wno-c++11-narrowing -Wno-address-of-temporary "-Dinline=inline __attribute__((used))" "-D__forceinline=inline __attribute__((used))" -include xmmintrin.h -include string.h \
    -Iport/include -Iport/include/crt -Iport/include/xdk -Iinclude "$f" 2>&1 | grep "error:")
  if [ -z "$out" ]; then echo "OK   $f"; else echo "FAIL $f"; echo "$out" | sed 's/^/     /'; fi
done
