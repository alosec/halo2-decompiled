# Native port (experimental)

This directory is the clean, open-source host/runtime track for running the recovered Halo 2 code on modern systems. It is separate from the byte-matching Xbox build. It never embeds or installs game data, an XBE, an Xbox SDK, or other proprietary material.

The first product milestone is an authentic loading main menu on Apple Silicon using files supplied by the user from the supported retail disc. The current checkpoint loads the authentic main-menu tag index through recovered cache code and builds a fifty-two-unit Microsoft-ABI UI core with freestanding CRT support, but the UI dependency graph is not closed and the game does **not** boot yet.

## Local build

Requirements on macOS:

```sh
brew install cmake ninja openssl@3 sdl3
cmake -S . -B build/native -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"
cmake --build build/native
ctest --test-dir build/native --output-on-failure
```

Point the host at an ignored directory containing files extracted from your own supported retail disc:

```sh
build/native/port/halo2-port --headless --data-root orig/game
build/native/port/halo2-port --data-root orig/game
```

`H2_DATA_ROOT` may be used instead of `--data-root`. The optional `--guest-image PATH` loads a completed AArch64 guest ELF into the same arena after the retail XBE state, applies its final protections, and patches the standard native services. It does not execute an entry point yet. The graphical command currently opens only a diagnostic host window. It is not a recreation of the menu and must not be presented as one.

To refresh the native-portability census without generating or linking game code:

```sh
python3 port/tools/compile_census.py --root . \
  --llvm /opt/homebrew/opt/llvm@22/bin \
  --output build/native/compile-census.json
```

A normal build also writes `build/native/port/main-menu-symbol-closure.json`. This deterministic report records which references the selected UI objects resolve among themselves and which symbols still need another recovered unit or a host service. Its companion `main-menu-import-plan.json` separates imports with recovered providers from platform runtime calls, named engine services, and addresses that remain unrecovered; it currently identifies 74 recovered providers, 18 named services, and 5 versioned host-ABI calls. The current fifty-two recovered units plus C/C++ runtime support have 97 unresolved symbols after 36 recovered-address retail globals are bound to the guest arena. Recovered UI geometry construction now reaches the shared low-level rendering request (`function_52040`) through a versioned guest-to-host renderer adapter; the matching Bink/video boundary is adapted the same way. The native host supplies concrete monotonic-clock, sleep, cosine, video-control, and render-request targets; video and render requests dispatch through explicit handlers while retaining call counters for bootstrap diagnostics. CTest intentionally ratchets that number, so update the expected count only after reviewing a deliberate dependency-set change. When one recovered source file also contains unrelated high-fan-out systems, the guest compiler can retain named IR functions, transitive callees, explicitly selected globals, and exact aliases; this keeps the matching source intact while linking only the required recovered service and data.

The same selected core is independently compiled through the Darwin-assembly-to-AArch64-ELF pipeline and cross-linked into `build/native/port/main-menu-core.partial.o`. `runtime/main_menu_bindings.json` supplies only guest addresses already encoded in recovered global names, and the host initializes those locations from the user's mapped XBE sections. The relocatable image proves that the real Microsoft-ABI UI units coexist with correct weak aliases. Its five host-ABI references are resolved through patchable AArch64 trampolines with explicit 64-bit target slots; the remaining unresolved set exactly matches the recovered/named services in the closure report. The native ELF loader validates AArch64 executable images, copies loadable segments into the reserved guest arena, applies read/write/execute protections without writable-executable memory, resolves symbols, and patches all five standard host slots. A regression executes loaded ARM code and verifies data plus 64-bit host targets. The selected core is not yet a final executable image and cannot run until its remaining engine-service closure is supplied.

## Architecture

The port uses two cooperating layers:

1. **Guest engine image.** Recovered Halo 2 source retains the original 32-bit layouts, globals, pointer representation, and Microsoft C++ object ABI. C++-heavy units use Clang's 32-bit ARM Microsoft frontend before their LLVM IR is retargeted to ARM ILP32; an LLVM transform then rebases original low virtual addresses into a reserved 4 GB host arena. This follows the proven CC0 address-rebasing approach in `bnunu/halo-ce-universal`, generalized for Halo 2 rather than executing original x86 code.
2. **Native host.** A normal ARM64 process owns SDL3, the window, Metal-backed rendering, audio, input, Darwin files and networking, memory mapping, threads, timing, and calls into the guest image through a versioned ABI.

Port-only substitutions must be selected by build flags or adapter interfaces. They must not alter the source used by the Xbox byte-matching build.

The initial LLVM rebase pass is adapted under CC0 from `bnunu/halo-ce-universal` commit `fd205407a6ac85937d963fd5e2d6ca8deeb26476`. It contains compiler/runtime engineering only. Halo CE compatibility headers derived from debug information are intentionally not imported because this project's provenance rules are stricter. Native compatibility declarations here are written only from public platform facts and the visible needs of recovered Halo 2 source.

## Milestones

- [x] Isolated port worktree and branch.
- [x] Native ARM64 build system, SDL host process, tests, and strict retail-data preflight.
- [x] Extract and validate the complete retail data set under ignored local storage.
- [x] Adapt and test the CC0 ILP32 guest compiler/rebase pass for Halo 2.
- [x] Compile and execute the first recovered Halo 2 translation unit on ARM64 without proprietary headers or libraries (`unknown_0259a0.cpp`), preserving its 32-bit guest memory semantics.
- [x] Bind recovered globals to their original guest virtual addresses and execute a recovered state mutation.
- [x] Establish and execute the versioned guest-to-host import ABI, including guest-pointer rebasing.
- [x] Reserve the 4 GB guest address space and initialize all 33 retail XBE sections, including zero-filled virtual tails, in non-executable memory.
- [x] Build and execute a reusable three-unit recovered guest archive, including a real cache-tag instance lookup with verified ILP32 structure layout.
- [ ] Implement the host services and clean compatibility declarations needed by startup.
- [x] Validate the authentic `mainmenu.map` and `shared.map` cache headers, build, types, and identities.
- [x] Execute recovered `cache_header_verify` against the authentic local `mainmenu.map` header.
- [x] Load the authentic `mainmenu.map` tag index through recovered cache code on a guest stack: 118 groups, 1,364 instances, scenario `0xe1780003` at `0x80121c44`, globals `0xe1740000` at `0x800e5000`.
- [x] Compile the recovered main-menu screen/list implementation and its vtables to ARM ILP32 with Microsoft-compatible C++ layout and symbols.
- [x] Link, map, and execute a Microsoft-ABI ARM guest ELF with 32-bit vtable relocations and verified virtual dispatch.
- [x] Cross-link the selected recovered main-menu core into one relocatable AArch64 ELF object with exact symbol-closure parity.
- [x] Add the production native ELF loader, segment protections, symbol lookup, and standard host-slot patching.
- [ ] Link the recovered main-menu unit and its dependency closure into the executable guest image.
- [ ] Bring up the renderer through the reusable CC0 GLES-to-ANGLE/Metal backend.
- [ ] Initialize input, UI, fonts, and sound; display and operate the authentic main menu.
- [ ] Load a campaign map, enter gameplay, save/reload, and validate sustained play.

## Provenance and legal rules

Follow the repository's `LEGAL.md`, `PROVENANCE.md`, and `CONTRIBUTING.md` without exception. In particular:

- Keep all extracted data below ignored paths such as `orig/`.
- Never commit or distribute an XBE, maps, media, disc image, proprietary SDK material, or generated artifacts containing them.
- Do not use leaked source, symbols, PDBs, linker maps, or confidential material.
- Credit and preserve the licenses of reused open-source platform code.
- Continue verifying matching-engine changes against the supported retail binary when the lawful local toolchain is available. Native-only adapters require their own tests and runtime evidence.
