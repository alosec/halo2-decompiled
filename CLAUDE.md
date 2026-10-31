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
  - `H2_RECORD=file.mp4` records every presented game frame (H.264 via
    host ffmpeg, fragmented so a killed game still leaves a playable file;
    `H2_RECORD_FPS`, default 60). Launch the user's sessions with it (and
    `H2_TRACE=state`) so reports can be found by time in the video;
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
  (`press`, `mouse DX DY`, `snapshot`, `quit`) and saves a snapshot on SIGUSR1 to
  `H2_SNAPSHOT_DIR`. Ready-made runs:
  - `port/tools/playtest_armory_look.py`: the Armory "look at the lights"
    steps to the "Press A to accept normal aiming" prompt;
  - `port/tools/playtest_wander.py --map M --seconds S --seed N`: random
    walking, turning and jumping (`--fire` adds shooting, grenades and
    reloads; `--multiplayer` plays the map as a one-player split-screen
    Slayer game); fails on a fall through the floor;
  - `port/tools/playtest_progress.py --map M`: a script-directed bot that
    serves a level's `sleep_until` waits (decompiled each poll with
    `hs_decompile.py`) with port-only test hooks sent as `guest ...` control
    lines: `teleport X Y Z`, `probe X Y Z`, `volume I` (a trigger volume's
    live box, for volumes on objects like 03b's Scarab), `deathless 1`,
    `kill_enemies R`, `enter_vehicle L` (a free seat with seat label L,
    forced past `function_c92c0`'s checks as scripts load units; seated,
    `teleport` moves the vehicle), `place X Y Z` (teleport without the
    floor check; `teleport` refuses a spot with no floor within 20 units),
    `units R`, `operate NAME` (press a powered control on a machine's
    position group, from beside it), `destroy NAME` (unless a script made
    it unable to take damage), `kill_player`. The bot presses controls
    for `device_get_position` waits, breaks objects for
    `object_get_health ... 0` waits, clears enemies near a volume it
    waits inside, and crosses to another bsp only when nothing is left in
    this one. `src/devices.cpp`'s `s_device` names are swapped: retail's
    `device_get_position` (0x2a4850) reads +0x140, so +0x130/+0x134 are
    the power group and power, +0x13c/+0x140 the position group and
    position. The player
    teleport refuses the scenario's bsp switch volumes (+0x130) and detaches
    the player on any failure, so the hook places the unit itself (c5460)
    and the bot walks across switch volumes.
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
  `vs-<hash>.metal` (its translated vertex program), `c-f<frame>-NNNN.bin`
  (192 vertex constants) and `p-f<frame>-NNNN.bin` (the raw `h2_gpu_draw`
  packet).
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
- **Native placeholders beating recovered code.** `build_whole_image.py`
  ranks a `*_native.cpp` rewrite above an unmatched `src/` draft, so a
  stub written before the real function was recovered keeps winning.
  `src/first_person_marker_native.cpp` reported no first-person markers
  over `src/unknown_165ce5.cpp`'s recovered one: effects on the player's
  own weapon (BR and SMG muzzle flashes) had nowhere to appear. Such
  files go in `port/whole_image_exclude.txt`; check `overridden` in
  `whole-image-report.json` for a stub winner when something is missing.
- **Argument order at calls into custom-convention functions.**
  `0x41980` called `0x42850` with a light volume's colour where retail
  passes its direction (`0x41b8c`): plasma bolts were green smears drawn
  along their colour. Compare the registers and pushes at the call with
  the callee's prologue.
- **Host shims that differ from Win32 in edge cases.** The fatal
  "problem with the disc" screen at boot, about one boot in fifteen:
  file times jittered (a two-clock conversion), so the font table's
  time missed the saved one, and the font recopy it triggered failed
  because the find shim matched `*.*` literally (extensionless font
  files were never deleted). The fatal screen (`g_51ea00`, 0x121fc0,
  0x18eaa0, 0x12bf00) draws through the Xbox's `0xf0000000`
  write-combined view, which the port doesn't map. When the game stops
  on black with no draws, check for that state before anything else.
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
    `y & 31`, bit `x & 31`) is unconfirmed, but the game's patterns
    (`function_1c1b0`) are a tiled 4x4 ordered dither, so a mirrored order
    only moves dots within a cell at the same coverage.
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
- **The Armory end to end** (`port/tools/playtest_armory.py`, driven by
  the level script's waits): look lights, aiming prompt, zapper, shield
  test, elevator, tram, outro, and 01b loads (two runs of two, 2026-10-26).
  The blocker was `function_e24f0` keeping a stopped biped's move
  animation forever (moving class from the kept, not the requested,
  animation): marines move by root motion, so stopped AI walked on along
  their facing and shoved the player out of the zapper cage.
- **Cairo Station end to end** (`playtest_progress.py --map
  01b_spacestation`, with the test hooks; 2026-10-27): bsps 0, 2, 0, 3,
  the outro on bsp 4, `game_won`, and 03a Outskirts' intro loads. A level
  started with `H2_START_MAP` kept the menu's campaign map id (the
  introduction's), so `game_won` went to 01a: `function_18e960` now takes
  the id from the level's own name (port-only, `H2_WHOLE_IMAGE`).
- **Outskirts (03a) and Metropolis (03b) end to end** with the same bot
  (2026-10-27): 03a to `game_won` and 03b's intro (fixed on the way: the
  Havok collector `nearest_value_2f9950` lacked its flipped `add` slot, a
  null vtable call); 03b through the Scarab boarding to 04a The Arbiter's
  intro. 03b's Scarab volumes sit on the Scarab's node (trigger volume
  +4 object name), so the bot asks the game for their live box.
- **The Arbiter (04a) end to end** (2026-10-27): hangar, bottling plant,
  the Banshee (`vehicle_test_seat ... banshee_d`), `vol_04a_game_won`, and
  04b The Oracle loads and plays (HUD, energy sword). The bot serves each
  script's structure bsp wait once per bsp (04a's `kill_volumes` and
  `wind` wait for each bsp in turn forever).
- **The Oracle (04b) end to end** (2026-10-27): lab, airlock, control
  room, cable room (`destroy` each cable as a script lets it take damage,
  `operate` the elevator), the station's fall, the bottling plant and
  hangar a second time, and 05a Delta Halo loads and plays.
- **Delta Halo (05a) and Regret (05b) end to end** (2026-10-28): 05a's
  overlook (a teleport spot over the ravine: "no floor" tries the other
  spots), the approach (inside the bsp 0-1 switch volume: placed; a switch
  volume lying mostly in the next bsp is crossed by holding the player at
  its points this bsp has), ambient bsp waits served only when idle; 05b's
  gondolas and elevator (`operate` presses a device group's control), and
  Regret (board his throne and pin his vitality, round after round), then
  06a Sacred Icon loads.
- **After the 2026-10-29 merge (938 commits)** the bot finishes every
  level from Cairo Station to The Great Journey again (2026-10-30). The
  bot needed three fixes on the way, none in the game: 03b's Scarab
  pilots (`goto_unit`, attached units first), endless reinforcements
  (at most 24 kill tries that found someone), and boarding the throne
  Regret rides (`enter_vehicle 0 boss`). A pre-merge build (worktree at
  19b4975d3, its own host and image) stalled at the same spots with
  the old bot. Open: a rare High Charity crash (2 runs in 7):
  `effect_location_next` follows a garbage location index for an
  effect a projectile impact creates (`fc330` -> `fcea0` ->
  `effect_new_from_parameters` -> `0x177310`); not a stack overrun
  (stack-protected build).
- **Death reverts** (2026-10-27): `0xcaa60` (a unit dies) passed the dead
  unit to `0x14cad0` where retail passes NONE (ebx), so the player kept
  it, the census (`0x152340`) never asked for a revert, and a dead player
  lay there forever (upstream draft #858).
- **Mouse look** (2026-10-27) turns the view directly: the host gathers
  motion (`host_h2_mouse_take`) and `0x186ab0` adds it to controller 0's
  yaw/pitch each tick (whole image only), 0.003 rad a point times
  `H2_MOUSE_SENSITIVITY`. It was the right stick (dead zone, top rate).
- **Open:**
  - from the user's play (2026-10-27): the HUD shield meter showed empty
    while shields were full (fixed by the 2026-10-27 merge of main); the
    Chief's shield shimmer "always on" (not reproduced: object +0xf4,
    which gates it in 0x4bcc0, stays 0 at rest; 01b's intro shows no
    shimmer). Fixed 2026-10-29: plasma bolts (`0x41980`'s argument
    order) and BR/SMG muzzle flashes (the first-person marker stub).
    The Chief's shield shimmer "always on": fixed 2026-10-29 (0xd5de0
    passed 0xd8b50 the damage info, not the character's vitality
    properties: +0xf4 never faded; upstream draft #883). In combat the
    screen takes 0x40890's full-screen flash (mode 1, cyan from damage
    effects) on hits; frequent under fire, not shown to be wrong.
    Save and quit's dialog opening on Cancel is retail: 0x2321fc,
    0x23661f and 0x19bc60 are matched and take the default from the UI
    data. SMG flash confirmed too. Still open: shadows that shift with
    the view angle.
  - "transparent marines" after the tram: 01b's intro, the bridge doors
    (frame 5490 with `H2_START_MAP=01b_spacestation` and only the menu
    script). The glow post-process (bright pass into a 160x120 target,
    blur, then a composite adding rgb + 2 x alpha of the blurred buffer)
    turns the space outside the bridge white around the figures. The
    one alpha-only draw of that frame (mask 8, blend ONE/ONE) adds
    t1.b x c0.a to the frame buffer's alpha: the `transparent_bloom` pass
    of `scenarios\shaders\solo\spacestation\bloom_wall`, whose bitmap 0
    is `color_white` (so t1 = 1 is right). c0.a = 1 comes from
    `function_1b2c0` component 3: the shader's scalar parameter at
    state + 0x5c, already 1.0 there (native and recovered 0x1b2c0, 0x18a90
    and 0x19530 agree). Later probes (2026-10-29): the bloom pass's c0 is
    not written by 0x19be0, 0x1a170, 0x17000 or 0x19ca0; 0x17170 loads
    packed shader values (0x00ffffff / 0xff000000 pairs) into c0/c1 alpha
    from shared value blocks. The glow chain itself (bright pass: avg x
    alpha x c + max(4(rgb - 0.75), 0), alpha x 0.25; four blurs; composite
    2(rgb/2 + a)) matches the recovered code, and bloom_wall exists to make
    that window glow, so the white doorway may be retail's own bloom. No
    reference image to settle it. Settled 2026-10-29: the bloom pass's
    c0.a comes from 0x17170 loading bloom_wall's shared tag value block
    (0x8087f9f4, entry 1 = 0xff000000), the same template binding giving
    other bloom shaders 0.8/0.4/0.35: retail data asks for full bloom.
  - shadows that shift with the view (the user: lighting on bipeds and
    walls, not only the flashlight). Measured 2026-10-29 in 01b's opening:
    a pillar's shading brightens about 35% as a pure camera turn moves it
    toward the screen edge. The environment vertex program (map microcode,
    `vs 16dad4554a203ba2`) feeds the pixel shader c6, the camera's back
    axis (c4-c6 are the camera basis), in the bump tangent frame next to
    the lightmap's incident direction (v5), so retail's own shading follows
    camera orientation. Its pixel shader (`fs 2319a1c6ac7713d0`) decodes as
    lightmap x (1 + bump N.L detail) x base (no camera input) plus a
    Phong highlight (R.V)^8, R = 2(N.L)N - L, scaled by the gloss alpha
    and the shader's c0[4]/c0[5]; V is c6 (toward the eye, the right
    sign). So the view-dependent part is retail's specular with one view
    vector per draw, its strength from map data.
    Characters: the skinned programs (e.g. `vs d119ee40`) read no camera
    vectors, and one marine's lighting and shadow-projection constants
    (c40-c45, c42 = the 128x128 shadow map) change by at most 1% between
    the two views (its idle animation): no view dependence there. c50-c52 there are instanced
    geometry transforms, not lights. Earlier: On 03a's
    opening the dark block beside the vent is the wall lightmap's baked
    shadow (static, with parallax against the protruding vent); each
    lightmapped geometry keeps one lightmap page across views (traced
    with the `va` lines). Six of the user's own 01b bsp-2 positions
    (play3.log; scratchpad tools/user_spots.py crosses in with the bot's
    switch volumes) showed nothing clear. Ruled out as well: the 128x128
    object shadow maps (A8R8G8B8 color, not depth compare, so SHADOWFUNC
    is moot), 0x497a0's light frame (its camera-attached case needs flags
    0x20 and 0x40000, as retail) and 0x494b0's light window (matched).
  - render states still ignored, with no effect seen: `SHADOWFUNC` (set
    for shadowing light passes; no depth-format texture is sampled in any
    trace so far, flashlight included) and `LOGICOP` (the game only sets
    it to 0). Check visually before
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
