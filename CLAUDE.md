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

- **Unit tests:** `cd build/release && ctest -R "gpu-|audio-"` runs the pixel
  shader, vertex program, texture format, mixer/ADPCM and WMA decoder tests.
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
- **Tracer:** `/usr/bin/python3 port/tools/guest_breaks.py --break SYM:fmt
  --watchpoint POINTER:OFF[:OFF] | @ADDRESS -- <host command>`. Retail
  disassembly: `port/tools/retail_dis.py ADDR`.

## Bug classes found so far (check these first)

- **Private copies of retail data.** A native global that retail keeps at a
  fixed address: the writer and reader disagree, or the copy stays zero.
  Examples: `g_current_map` (fixed as `g_547844`), and the two tables bound
  as `g_4672e0` and `g_4456fc`. The fix is the `g_<address>` name.
- **By-value returns.** `hkBool` or other classes returned by value use a
  hidden result pointer (x8 on ARM), not a `bool *` out-parameter.
- **Vtable binding.** Classes with no virtuals of their own store retail
  tables inline; identical-code folding shares destructors; stack layout
  differences expose retail's out-of-bounds writes.
- **Xbox DSound semantics.** A submix (MIXIN) buffer passes its input even
  if the game never plays it; dialogue goes through one (fixed 625675f1).
  The Armory tutorial's "look at the light" steps need those lines.
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
    Armory tutorial asks you to look at draw as white bars);
  - cutscenes can't be skipped;
  - characters tinted yellow-green;
  - the difficulty-screen backdrop is missing.

## Missing geometry/textures: investigation notes (2026-10-25)

Example: on Cairo Station's intro (01b, about frame 10000) Hood's torso and a
background officer's body are missing; heads, collar and medals draw.
- **Not the renderer:**
  - every texture uploads (`H2_TRACE=textures`: 1177 ok, 1 all-zero);
  - all draws in the frame have normal state;
  - the torso is never submitted.
- **Not geometry streaming:** the geometry cache's requests complete; nothing
  stays pending.
- **Not `object_set_permutation` (built-in 110):** it only sets crew head
  variants (region `04000095`).
- **Section choice per LOD** (`function_4c190` -> `function_4c2b0`,
  `src/unknown_03f2b0.cpp`): some models get `res 0` (not resident, skipped
  that frame).
- **Model `eb5a0927` (9 regions):** region 1 is never wanted (permutation
  `ff`). `model_variant_region_get_choices` matches retail, so this may be
  legitimate (an empty region).
- **Marine model** (hlmt `eb590926` / mode `eb5a0927`): its regions are body,
  comm_pack, head, helmet, mic_head and packs_*; variants include
  default_nrl, odst_cky, dress_grf, johnson_dress and dress_hat. Body
  selection and drawing are fine. Only five objects lacked a body, briefly,
  until their geometry was resident; empty packs are legitimate.
- **Reading string IDs offline:** the map header holds the index table at
  `+0x16c` and the data at `+0x170`. A string ID is (length << 24) | index;
  `0x95` is "head", `0xbf2` is "helmet".
- **Next step:** Hood is probably a different, smaller model. Identify it, or
  log every object drawn in the Hood shot with its model, then check its
  regions the same way.

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
return fixes (`hkBool` is allowed) and the inline-vtable fixes.
