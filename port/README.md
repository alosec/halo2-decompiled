# Native port (experimental)

This directory is the clean, open-source host/runtime track for running the recovered Halo 2 code on modern systems. It is separate from the byte-matching Xbox build. It never embeds or installs game data, an XBE, an Xbox SDK, or other proprietary material.

The first product milestone is an authentic loading main menu on Apple Silicon using files supplied by the user from the supported retail disc. The current checkpoint loads the authentic main-menu tag index through recovered cache code and fully links an eighty-three-unit/slice Microsoft-ABI UI guest image with freestanding CRT support. Its constructor-aware startup now initializes the recovered UI manager and leaves an authentic press-start screen present in a recovered window channel. No frame is rendered yet, so this is not a loading-menu claim.

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
build/native/port/halo2-port --headless --data-root orig/game \
  --guest-image build/native/port/main-menu-core.elf \
  --guest-entry h2_main_menu_guest_bootstrap
build/native/port/halo2-port --data-root orig/game
```

`H2_DATA_ROOT` may be used instead of `--data-root`. The optional `--guest-image PATH` loads a completed AArch64 guest ELF into the same arena after the retail XBE state, applies its final protections, configures host ownership of the user's `mainmenu.map`, and patches the standard native services. `--guest-entry SYMBOL` explicitly invokes a zero-argument entry on a dedicated guest-address stack and requires `--guest-image`; the checked-in bootstrap runs its static constructor table, loads the authentic tag index through recovered cache code, initializes offline profile/save and UI state, constructs the recovered press-start screen, and verifies that a screen is present. The graphical command currently opens only a diagnostic host window. It is not a recreation of the menu and must not be presented as one.

To refresh the native-portability census without generating or linking game code:

```sh
python3 port/tools/compile_census.py --root . \
  --llvm /opt/homebrew/opt/llvm@22/bin \
  --output build/native/compile-census.json
```

A normal build also writes `build/native/port/main-menu-symbol-closure.json`. This deterministic report records which references the selected UI objects resolve among themselves and which symbols still need another recovered unit or a host service. Its companion `main-menu-import-plan.json` separates imports with recovered providers from platform runtime calls, named engine services, and addresses that remain unrecovered; it currently identifies no remaining recovered providers or named services, only 8 versioned host-ABI calls. The current eighty-three recovered UI units/slices, recovered cache-loader slice, and C/C++ runtime support have 8 unresolved host-ABI symbols after 68 recovered-address retail globals are bound to the guest arena. The complete recovered screen-layout builder, widget factories, vtables, deleting destructors, retail virtual methods, and widget-item text/appearance/bitmap application are selected together; this deliberately exposes their concrete renderer/model frontier instead of leaving `c_class_1473c9::build` as an opaque named service. Its preview path now includes recovered appearance/team color selection and ARM-safe pixel packing, while retaining the original x87 implementation for Xbox. Xbox cache prediction is currently an explicit conservative native miss rather than fabricated loaded resource state; actual resource ownership stays at the host renderer boundary. Likewise, unavailable Xbox network-session hosting, mode/state queries, readiness changes, leaves, Live teardown, and per-frame peer/screen/controller-slot updates return offline/default outcomes or complete as no-ops rather than inventing manager or transport state. Native profile startup preserves recovered slot invalidation and exposes an explicitly empty 8 GiB virtual save drive until host persistence exists; profile selection, application, write-task startup, naming/deletion, and sign-out therefore complete against empty offline state without fabricating saved or Live users. Xbox dashboard launch is explicitly unavailable on native builds. The complete recovered press-start and five-mode sign-in factories, methods, vtables, and deleting destructors are selected together; native bootstrap skips impossible Xbox Live invitations and reports disconnected controllers until an input backend exists, while keeping Xbox behavior intact. Secondary campaign/settings/multiplayer/profile screen transitions currently fail closed with no screen rather than constructing incomplete objects; native virtual-keyboard and Live friend-selection requests likewise do nothing until input/online backends exist. Those boundaries must be replaced before the menu is interactive. Native audio remains explicitly silent, so voice/impulse-buffer pause and resume complete without fabricating an audio graph. Xbox loading-screen image update/render also remains explicitly absent until its buffer has a native owner, and attract-mode startup stays disabled until the native host has a movie decoder. Recovered Xbox immediate-mode draw calls now collect a bounded four-vertex request with explicit primitive, color, texture coordinates, and positions; their native reset is a no-op because each request is self-contained. Solid UI rectangles use the recovered screen projection and now emit tested four-vertex host requests too. Requests cross the tested guest-to-host rendering boundary without carrying D3D ABI into the LP64 host. The recovered preview-quad geometry path is selected too; native builds send its retail bitmap tag/index and layer directly to host texture ownership instead of requiring an Xbox D3D texture-cache object. Recovered UI geometry construction also reaches the shared low-level rendering request (`function_52040`) through that boundary. Authentic English menu strings now come from the recovered retail string-table lookup and recovered UTF-8-to-UTF-16 conversion; recovered menu text is then converted to 32-bit code points and crosses a dedicated text-request boundary. Until host text metrics exist, measurement conservatively returns the caller's bounds and Xbox font-cache prediction misses. Model animation reports unavailable, texture ownership remains host-side, and Xbox post-processing passes its source surface through. The matching Bink/video boundary is adapted the same way. The native host supplies concrete cache-read, low physical-allocation, monotonic-clock, sleep, cosine, video-control, render-request, and text-request targets; cache reads are bounds checked against the user-supplied map and may write only inside the reserved guest arena, while video, geometry, and text requests dispatch through explicit handlers with call counters for bootstrap diagnostics. CTest intentionally ratchets that number, so update the expected count only after reviewing a deliberate dependency-set change. When one recovered source file also contains unrelated high-fan-out systems, the guest compiler can retain named IR functions, transitive callees, explicitly selected globals, and exact aliases; this keeps the matching source intact while linking only the required recovered service and data.

The same selected core is independently compiled through the Darwin-assembly-to-AArch64-ELF pipeline, cross-linked into `build/native/port/main-menu-core.partial.o`, and fully linked as `build/native/port/main-menu-core.elf` at a tested non-overlapping guest address. `runtime/main_menu_bindings.json` supplies only guest addresses already encoded in recovered global names, and the host initializes those locations from the user's mapped XBE sections. The relocatable image proves that the real Microsoft-ABI UI units coexist with correct weak aliases. Its eight host-ABI references are resolved through patchable AArch64 trampolines with explicit 64-bit target slots, leaving the cross-linked relocatable core with zero undefined ELF symbols. The native ELF loader validates AArch64 executable images, copies loadable segments into the reserved guest arena, applies read/write/execute protections without writable-executable memory, resolves symbols, and patches all eight standard host slots. A regression executes loaded ARM code and verifies data plus 64-bit host targets. The executable image has a deliberate bootstrap entry that runs the generated static-constructor table, loads `mainmenu.map` through the selectively linked recovered cache loader, initializes recovered player-slot/profile state and the empty native save list, creates a native-backed recovered UI heap, initializes the recovered window manager, constructs the press-start screen, and verifies a window contains it. The host invokes this on a dedicated guest-address stack alongside all 33 retail XBE sections. Frame update/render sequencing and actual host drawing still remain before any menu-run claim.

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
- [x] Link the recovered main-menu unit, cache loader, and dependency closure into the executable guest image.
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
