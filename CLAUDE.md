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
  - `H2_START_MAP=01b_spacestation` starts the campaign on a level; a
    multiplayer name (`lockout`) replaces the map of the first game started
    from a lobby;
  - `H2_TRACE_FRAME=n[,m...]` traces every draw of those frames (with
    several, `h2-frame: #k frame n` lines and dumps named `back-f<n>-<k>`;
    compare one view before and after an input in a single run, since
    spawns and timing differ between runs);
  - `H2_TRACE_DUMP=dir` and `H2_TRACE_DUMP_DRAWS=i,j,...|all` dump render
    targets after those draws (`H2_TRACE_DUMP_TARGET=back|target-<data>`
    keeps only one; draw numbers shift between runs, so find a draw by
    dumping all of one frame in a single run);
  - `H2_TRACE_TEXTURE=hex|all` dumps texture bytes and palettes;
  - `H2_TRACE=d3d` gives push-buffer method counts;
  - `H2_AUDIO_RECORD=file.wav` records the mix;
  - `H2_TRACE=audio` lists voice formats;
  - `H2_TRACE=audio-voices` gives per-voice levels.
- **Playtesting harness (play the game yourself; don't ask the user to):**
  `port/tools/h2_driver.py` launches the game hidden and silent with a fresh
  profile and drives it while it runs: `press(button, frames)` (any
  input-script button, partial sticks like `rx+40`), `snapshot()` (the game
  frame and the window as BMP), `wait_frame`/`settle`, and `aim_at(point)`
  (closed loop from the head marker). Each frame the guest logs `h2-state`:
  position, velocity, look, head marker, body forward, and the ground height
  a ray finds under the unit (`port/runtime/guest_test_state.cpp`,
  `H2_TRACE=state`). The host reads commands from `H2_CONTROL=file`
  (`press`, `snapshot`, `quit`) and saves a snapshot on SIGUSR1 to
  `H2_SNAPSHOT_DIR`. Ready-made runs:
  - `port/tools/playtest_armory_look.py`: the Armory "look at the lights"
    steps to the "Press A to accept normal aiming" prompt;
  - `port/tools/playtest_wander.py --map M --seconds S --seed N`: random
    walking, turning and jumping (`--fire` adds shooting, grenades and
    reloads; `--multiplayer` plays the map as a one-player split-screen
    Slayer game); fails on a fall through the floor.
  `H2_WINDOW_SIZE=WxH` sizes the (hidden) window, e.g. to check full-screen
  presentation in the window snapshot. Convert snapshots with `sips -s format png`; tile several with ffmpeg
  `tile=4x2` to look at a run in one image.
- **Stack overruns:** `H2_STACK_PROTECT=1` makes `build_whole_image.py`
  compile every function with a stack canary (build it into its own
  `--work` directory, e.g. `build/release/port/whole-ssp`, and run it with
  `H2_DRIVER_IMAGE=.../whole-image.elf`). An overrun logs `h2-state stack
  overrun in the function at 0x...`: that address is the start of the
  function *after* the culprit (the failing call is its last instruction).
  A crash where a callee-saved register holds garbage is usually one of
  these.
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
- **Swapped register roles in unmatched ("todo") functions.** Retail passes
  many arguments in registers; a decompiled body that reads them in the
  wrong roles still compiles and runs. Found: `function_cba50` (look frame
  forward/up swapped: the look vector wandered, so the Armory "look at the
  lights" step never registered) and `function_11e800` (triangle edges
  swapped: object lighting sampled the wrong lightmap texels, so characters
  were near black). Check `port/tools/retail_dis.py` for which register
  holds what before trusting an unmatched function.
- **Mirrored conventions in unmatched functions.** `function_e01d0`
  (biped turning) computed the goal's side with the opposite cross product
  from retail and flipped its rotation to match, so turning worked but the
  chosen turn animation (`turn_left`/`turn_right`, 0x900000e/0xa00000f) was
  the wrong one: after a turn the body spun forever around the direction
  opposite the aim (and in the Armory the head circled, so the look checks
  flickered). Self-consistent code can still be wrong where a value leaves
  the function; compare sign conventions with retail.
- **Callbacks declared with an integer where the pointer type has a float.**
  A function cast to a callback type must take the same parameter kinds: on
  x86 a `long` and a `real` share a stack slot, on ARM64 they travel in
  different registers and every later argument shifts. The first-person
  sound handlers took `(long, long, event *)` but are called as
  `(long, real, event *)`: firing crashed (fixed). `hs_real_format` the other
  way round. A scan for `(T)function` casts whose typedef and definition
  differ in float/int position found these three.
  The same through a virtual call, which the cast audit can't see: the
  Havok time search (`src/unknown_3163c0.cpp`) declared the motion's slot 29
  `move_to_time(float, long)` while the motion (its step-position setter,
  0x31b700) takes `(float, float)`: it read the step length from a stale s1,
  so rewinding a body to a contact's time moved it by a garbage time. The
  player snapped back along their path when touching walls and was pushed
  through the floor when jumping into an overhang (the user's "fell through
  the floor"). When a class declares a placeholder vtable, check each slot's
  parameter kinds against the implementation.
- **Native duplicates that read flags differently.** The native copy of
  the texture request (0x12ce00, `particle_texture_upload_request_native.cpp`)
  took bit 0 as "load" and bit 1 as "wait"; retail is the other way round.
  The user interface asks with 2 (load, don't wait), so its bitmaps never
  became resident: the main menu logo, panel frames, level pictures and
  difficulty icons were missing. It also kept every texture the game only
  ever requests that way at its low-detail copy, so object lighting fell
  back to defaults (objects lit brighter than their surroundings); with
  real lightmap samples, objects in dark spots are dark, like the floor
  around them.
- **Locals smaller than what the callee writes.** Local copies of a
  callee's output type that are too short overrun the stack onto saved
  registers: the world ray test's result (`s_collision_result_1697c0`) is
  0x5c bytes, but the shared header and several files declared 0x4c; the
  light shape (`s_light_shape_ab`) is 0x7c, one caller had 0x74; a
  location (`s_location`) is 8 bytes, one caller passed 6. On x86 the
  frame's slack absorbed it; on ARM64 an AI ally's weapon pointer was
  overwritten (a crash in its combat tick). Find them with the
  stack-protected build.
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
- **Xbox D3D semantics.** Render states the renderer ignores silently drop
  what depends on them: `D3DRS_DEPTHCLIPCONTROL` clamp (the skies), the
  visibility test counts (lens flares). Clear flags are Xbox values (ZBUFFER 1, STENCIL 2,
  colour channels 0x10..0x80). Pushed draws must be parsed at EndPush, with
  the state they were pushed with. The pixel shader lives in render states
  0..56.

## Status (2026-10-26)

- **Works:**
  - menus, profiles, the keyboard, campaign and split-screen lobbies;
  - every campaign level loads and survives a combat wander soak;
  - all twelve multiplayer maps play (split-screen Slayer) and survive a
    combat soak;
  - cinematics (01b's intro sampled every 30 frames: textured and lit;
    its white frames are the scripted fades between scenes, gradual in and
    out, not flashes);
  - the Armory lamps: the side lamps glow red as targets and the one looked
    at turns green (`playtest_armory_look.py` snapshots each step);
  - checkpoint save/revert, full speed (the 30 Hz cap), WMA music through
    optional host FFmpeg; A (Space) skips cutscenes, as on the Xbox.
- **Fixed 2026-10-26:**
  - lens flares, among them the Armory look-device lamps (machines
    `looker_light_red`/`_green` with only a flare): the runtime reported
    every D3D visibility test as zero pixels; the renderer now counts
    samples (Metal visibility results, read back after each present);
  - outdoor skies: the sky model lies beyond the far plane and is drawn
    with `D3DRS_DEPTHCLIPCONTROL` = `D3DDCC_CLAMP` (0x10), which the
    renderer ignored, so Metal clipped it; draws now carry `depth_clamp`.
  - weather: points take their size from the vertex program only with
    `D3DRS_POINTSCALEENABLE` (else `D3DRS_POINTSIZE`, left at 1), and with
    `D3DRS_POINTSPRITEENABLE` texture stage 3 reads the position within
    the point (as xemu's NV2A model does); Lockout's snow had ~12000-pixel
    points and drew as haze.
  - stipple (`D3DRS_STIPPLEENABLE`, the pattern from `SetStipple`) discards
    fragments per game pixel; xemu doesn't model it, so the bit order (row
    `y & 31`, bit `x & 31`) is a guess that only matters for dither fades.
  - the flashlight and decals: `function_144f0` (bind a bitmap, else the
    rasterizer default of the wanted type) takes the type to match in ebx
    and the fallback index first on the stack; three callers (the light
    gel in `function_19530` and its native copy, decals in `0x44040`)
    passed them swapped, so a 2D bitmap never matched type 1 and they drew
    with `rasterizer\default_3d` (flashlight alpha 0, decals missing). The
    flashlight is campaign only (retail `0xc50a0` exits otherwise; White).
  - the polygon offset (`POLYGONOFFSETZOFFSET`/`ZSLOPESCALE`) applies only
    while `SOLIDOFFSETENABLE` is on; the runtime applied it always, and the
    game leaves 550 set after decal passes.
- **Open:**
  - render states still ignored: `SHADOWFUNC` (shadow-map compare,
    `function_0496f0` light passes), `LOGICOP`. Check visually before
  assuming they matter.

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
`hs_real_format`), `function_cba50` (look frame) and `function_11e800`
(barycentric edge order).
