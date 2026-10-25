# Halo 2 native macOS port: working notes

This fork (`alosec/halo2-decompiled`, branch `macos-native-port`) runs the
decompiled Halo 2 source natively on Apple Silicon. Upstream is
`kirklandsig/halo2-decompiled` (remote `upstream`); this fork is remote
`fork`. The tracking issue is kirklandsig/halo2-decompiled#792.

## Ground rules

- **Pure decomp:** only decompiled C/C++ runs. No emulator, and no lifting
  or recompiling retail x86. SDK libraries (XDK D3D, DirectSound, XAPI, Bink,
  WMA) may get native shims; game code and Havok must be decompiled.
- **Game data:** never committed. The user supplies `orig/game/`
  (`default.xbe`, `maps/`, ...); `orig/` and `build/` are git-ignored.
- **Commit identity:** commit as
  `Alexander Garcia <7227187+alosec@users.noreply.github.com>` (set in the
  repo config). Upstream's publish step rejects other addresses.
- **Publishing:** never post publicly (issues, PRs, comments) without the
  user asking.

## Build and run

See `port/README.md` "Play" for the full commands.
- **One build:** `build/release` (RelWithDebInfo, Ninja) builds both the
  guest image (`--target h2-whole-image`) and the host (`halo2-port`).
  `build/native` is a Debug host build used for older traces.
- **Image builder:** `port/tools/build_whole_image.py` compiles `src/` plus
  `port/recovered/` with clang (MSVC ABI, thumbv7 target) and retargets the
  result to an ILP32 arm64 guest. Guest pointers are biased by 1 TB, and the
  XBE loads at its retail addresses.
- **Silent skips:** it skips any unit that fails to compile and still
  "succeeds". Always check its line `frontend: N compiled, 0 skipped`.
- **Address-named globals:** `g_<hex>` globals bind to that retail address
  automatically. Other names get native storage unless listed in
  `port/runtime/main_menu_bindings.json`.
- **Static initialisers:** retail's are translated by
  `port/tools/gen_initializer_recoveries.py` into
  `port/recovered/retail_initializers_generated.cpp`, and run before main.

## Testing and verification

- **Unit tests:** `cd build/release && ctest -R "gpu-|audio-|abi-"` runs the pixel
  shader, vertex program, texture format, mixer/ADPCM and WMA decoder tests,
  and `port/tools/audit_function_casts.py` (float/int function casts).
  The older menu-track tests need the legacy build, which is currently
  broken (`guest_crt.c` redefines `_ReadWriteBarrier` under LLVM 22).
- **One game at a time.** Trial runs are silent and hidden:
  `H2_AUDIO=off H2_WINDOW_HIDDEN=1`.
- **Regression:** the scripted menu → campaign → Armory run is
  `H2_INPUT_SCRIPT="1300:start:8,1700:down:8,1850:a:8,2300:a:8,2900:a:8,3300:a:8,3700:a:8,4100:a:8"`
  with `--frames 13000`. Check the final frame (`--screenshot`, plus
  `H2_SCREENSHOT_EVERY=n`) and the summary line (`skipped 0, failed 0`).
- **Other environment variables:**
  - `H2_START_MAP=01b_spacestation` starts the campaign on a level;
  - `H2_TRACE_FRAME=n` traces every draw of frame n;
  - `H2_TRACE_DUMP=dir` and `H2_TRACE_DUMP_DRAWS=i,j,...` dump render
    targets after those draws;
  - `H2_TRACE_TEXTURE=hex|all` dumps texture bytes and palettes;
  - `H2_TRACE=d3d` gives push-buffer method counts;
  - `H2_AUDIO_RECORD=file.wav` records the mix;
  - `H2_TRACE=audio` lists voice formats;
  - `H2_TRACE=audio-voices` gives per-voice levels.
- **Probes in src/:** declare `extern "C" void h2_probe(char const *,
  unsigned, unsigned, unsigned);` at file scope and call it (printf-style, up
  to 3 values). An undefined symbol is silently stubbed by the image builder
  (check `stubbed` in `whole-image-report.json`), so a misdeclared logger
  prints nothing. `log_message` is internal to the D3D runtime; don't use it.
- **Deterministic runs:** use a fresh `--profile-root` per run. A reused
  profile keeps campaign progress, which changes the menu flow and timing.
- **Frame trace extras:** with `H2_TRACE_DUMP`, each traced draw also saves
  `vs-<hash>.metal` (its translated vertex program), `c-NNNN.bin` (192 vertex
  constants) and `p-NNNN.bin` (the raw `h2_gpu_draw` packet).
- **Offline map reading:** `port/tools/h2_map.py MAP [name...]` (tag index at
  header `+0x10`; names table `+0x2c4`/`+0x2cc`; meta follows the index, so
  the address base is the lowest tag address minus (index offset + index
  size)) lists tags by name and follows tag references, e.g. a shader to its
  template.
- **Tracer:** `/usr/bin/python3 port/tools/guest_breaks.py --break SYM:fmt
  --watchpoint POINTER:OFF[:OFF] | @ADDRESS -- <host command>`. Retail
  disassembly: `port/tools/retail_dis.py ADDR`.

## Bug classes found so far (check these first)

- **Private copies of retail data.** A native global that retail keeps at a
  fixed address: the writer and reader disagree, or the copy stays zero.
  Examples: `g_current_map` (fixed as `g_547844`), and the two tables bound
  as `g_4672e0` and `g_4456fc`. The fix is the `g_<address>` name.
- **Callbacks declared with an integer where the pointer type has a float.**
  A function cast to a callback type must take the same parameter kinds: on
  x86 a `long` and a `real` share a stack slot, on ARM64 they travel in
  different registers and every later argument shifts. The first-person
  sound handlers took `(long, long, event *)` but are called as
  `(long, real, event *)`: firing crashed (fixed). `hs_real_format` the other
  way round. A scan for `(T)function` casts whose typedef and definition
  differ in float/int position found these three.
- **By-value returns.** `hkBool` or other classes returned by value use a
  hidden result pointer (x8 on ARM), not a `bool *` out-parameter.
- **Vtable binding.** Classes with no virtuals of their own store retail
  tables inline; identical-code folding shares destructors; stack layout
  differences expose retail's out-of-bounds writes.
- **Xbox DSound semantics.** A submix (MIXIN) buffer passes its input even
  if the game never plays it; dialogue goes through one (fixed 625675f1).
  The Armory tutorial's "look at the light" steps need those lines.
- **Local type redefinitions in port-made `*_native.cpp` duplicates.** A
  file that declares its own copy of a shared type with a different layout
  silently breaks every call that crosses into code using the real one
  (`color4f` is alpha first; a local {r,g,b,a} copy shifted every
  colour-range constant one channel: the yellow-green tint). Include the
  shared header instead. Duplicates win over `src/` copies in the image
  (see `overridden` in `whole-image-report.json`), so check those first.
- **Depth surfaces.** The game binds depth separately from the colour
  target, and several targets share one depth buffer (fixed: shared
  depth keyed by `depth_stencil`).
- **Xbox D3D semantics.** Clear flags are Xbox values (ZBUFFER 1, STENCIL 2,
  colour channels 0x10..0x80). Pushed draws must be parsed at EndPush, with
  the state they were pushed with. The pixel shader lives in render states
  0..56.

## Status (2026-10-25, upstream merged at 58bb9a0d)

- **Works:**
  - menus, profiles and the keyboard;
  - the intro cinematic and The Armory;
  - Cairo Station reaches gameplay;
  - checkpoint save/revert;
  - full speed (the 30 Hz cap);
  - WMA music through optional host FFmpeg.
- **Open, from the user's first live play:**
  - many missing textures in cinematics and gameplay (the lights the
    Armory tutorial asks you to look at draw as white bars); missing bodies,
    black change colours and the yellow-green tint are fixed;
  - white flash frames in cinematics;
  - cutscene skipping works: A (Space) skips, as on the Xbox (verified on
    the 00a intro and the 01b cinematic); the README says so now;
  - the difficulty-screen backdrop is missing.

## Missing bodies ("floating heads"): solved 2026-10-25

Characters drawn with the `tex_bump_four_change_color` template (Hood's
uniform, officers' dress uniforms) lost their bodies. The lighting pass of a
model draws into an off-screen 640x480 target with the back buffer's depth
surface, writing depth, and the colour pass then tests depth EQUAL on the back
buffer. The renderer gave every target a private depth texture, so the colour
pass tested against depth that never saw the first pass. Fix: depth buffers
are keyed by the game's depth surface (`h2_gpu_draw::depth_stencil`) and
shared; screen-sized targets render at the back buffer's scale so they can
share it. Heads survived only because other passes had already written their
depth. How it was found: the frame trace showed the torso draws were submitted
(an earlier note claiming they weren't was wrong), and forcing EQUAL to
LESS_EQUAL brought the bodies back.

The follow-on black change-colour surfaces were the scaled lighting target
read with unscaled texel coordinates (fixed 3524f814), and the yellow-green
tint was a local `color4f` with the wrong field order (fixed 4f16c747).

## Upstream contribution

The owner's rules (issue #792):
- **Scope:** one shared `src/` fix per PR.
- **Claims:** check claims issue #9 first.
- **Retail data:** point declarations at it with `g_<address>`.
- **Havok:** class names found in the retail executable's own strings
  (`hkBool`, `hkRigidBody`, `hkTransform`, ...) are fine and main uses
  them. Nothing taken only from Havok's SDK headers or documentation.
- **Port-only pieces** (host, shims, FFmpeg) stay on the fork.

How we work:
- **Branches:** make PRs from fresh worktrees off `upstream/main`, as
  drafts, with a "Retail range claimed:" line.
- **Byte checks:** we can't run `tools/check.py` (no XDK), so each PR says
  matching is unverified.
- **Provenance:** the PR template's checklist is the user's own attestation.
  Leave it unticked for them.
- **Coordination:** the maintainers coordinate on #104; machine 3 clears
  files.

Open drafts:
- **#793:** `g_547844`;
- **#794:** `g_4672e0`;
- **#795:** `g_4456fc`.

All three are reviewed clean and cleared by machine 3, waiting for the user
to tick provenance and mark them ready. Queued next: the Havok by-value
return fixes (`hkBool` is allowed), the inline-vtable fixes, and the
float/int callback parameter fixes (first-person and weapon sound events,
`hs_real_format`).
