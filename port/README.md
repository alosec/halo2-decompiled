# Native port (experimental)

This directory is the clean, open-source host/runtime track for running the recovered Halo 2 code on modern systems. It is separate from the byte-matching Xbox build. It never embeds or installs game data, an XBE, an Xbox SDK, or other proprietary material.

The first product milestone is an authentic loading main menu on Apple Silicon using files supplied by the user from the supported retail disc. The current checkpoint is deliberately smaller: a native ARM64 SDL3 host that strictly validates the executable and two map files required for that milestone. It does **not** boot the game yet.

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

`H2_DATA_ROOT` may be used instead of `--data-root`. The graphical command currently opens only a diagnostic host window. It is not a recreation of the menu and must not be presented as one.

## Architecture

The port uses two cooperating layers:

1. **Guest engine image.** Recovered Halo 2 source retains the original 32-bit layouts, globals, and pointer representation. On Apple Silicon it will compile as an ARM ILP32 guest image, with an LLVM transform rebasing original low virtual addresses into a reserved 4 GB host arena. This follows the proven CC0 approach in `bnunu/halo-ce-universal`, generalized for Halo 2 rather than executing original x86 code.
2. **Native host.** A normal ARM64 process owns SDL3, the window, Metal-backed rendering, audio, input, Darwin files and networking, memory mapping, threads, timing, and calls into the guest image through a versioned ABI.

Port-only substitutions must be selected by build flags or adapter interfaces. They must not alter the source used by the Xbox byte-matching build.

## Milestones

- [x] Isolated port worktree and branch.
- [x] Native ARM64 build system, SDL host process, tests, and strict retail-data preflight.
- [ ] Extract and validate the menu data set under ignored local storage.
- [ ] Generalize and test the ILP32 guest compiler/rebase pass for Halo 2.
- [ ] Compile the first recovered Halo 2 translation units natively without proprietary headers or libraries.
- [ ] Map recovered globals and establish a versioned guest/host import table.
- [ ] Mount retail paths and parse `mainmenu.map` through recovered cache/tag code.
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
