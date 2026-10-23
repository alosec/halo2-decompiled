# A capture the flag or assault object's tick (unknown_240820)

Retail range covered: `0x240820`–`0x240d63`. This is one inventory entry, 1,348
retail bytes, `todo` with no source: the per-tick update of one team's flag or
bomb object in the capture the flag and assault engine (vtable `0x459d18`,
`src/unknown_2420a0.cpp`). It runs the object's two timers in `g_51ec80` (the
return or detonation countdown, and how long the object has sat untouched),
fires the engine events for them and for falling below the kill height, and
posts messages to the local players of the object's team. It also covers its
only caller, `0x240d70` (181 bytes, `todo`, no source), in a short section.
**Analysis only:** this document adds no source, and nothing in it has been
built or checked against retail with the original compiler. Names are
provisional. The `function_<va>` form is primary; the descriptions are offered
for whoever decompiles it.

It uses the names of `src/unknown_2420a0.cpp` and `include/unknown_2420a0.h`
(`s_slot_table` and `g_51ec80`, `s_ctf_options` through `ctf_options()`,
`s_team_entry`, `slot_object_get`), `src/unknown_15e410.cpp`
(`function_15e410`), `include/game_engine_events.h` and
`src/unknown_19de80.cpp` (`s_event`, `s_event_player` and the event
functions), `include/globals.h` (`s_game_time_globals`, `s_mp_globals`,
`s_index_table`), `src/unknown_2007b3.cpp`, `src/unknown_196d20.cpp`,
`src/unknown_0a7c50.cpp`, `src/unknown_1600f0.cpp` and
`src/unknown_14b560.cpp`. No document in `docs/` covers the function or its
caller.

## Boundary

- `0x240740`, just before, is `todo` with source in `src/unknown_2420a0.cpp`;
  `0x240d70`, just after, is `todo` with no source. Both are excluded, and
  `0x240d70` is described below as the caller.
- Lane O's row of the Active claims table (issue #9, `0x240000`–`0x24ffff`,
  machine 2) covers both. This document is offered to lane O; it reserves the
  analysis only (#339), not the code.
- `0x240820` and `0x240d70` have no `@retail` or `@stub` marker, declaration
  or stub at `276e71ec`. `0x240d70` is slot 18 of the vtable at `0x459d18`
  (the dword at `0x459d60`), declared there as the empty placeholder
  `virtual void v18() {}` of `c_game_engine_markers`
  (`src/unknown_2420a0.cpp:210`).

## Conventions

"Callers" counts the functions that call an entry directly.

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x240820` | 1348 | stack: object index (`ret 4`) | none | 1 | One tick of a team's flag or bomb: its timers, events, the kill height test and the local players' messages |
| `0x240d70` | 181 | none | none | slot 18 of `0x459d18` | Calls `0x240820` for each team's object, then the engine's other per-tick checks |

`0x240820`'s argument is an object index: `s_team_entry::object_index`
(`src/unknown_2420a0.cpp:139`) of the entry the caller found. It reads no
register input. It returns nothing (`ret 4` at `0x240ce4` and `0x240d61`).

The frame is `sub esp, 0x38`, then `push ebp`, `push esi`, `push edi`
(`0x240820`–`0x24084a`); `ebx` is pushed at `0x240863`, only after the slot
test, and the early exit for slot -1 (`0x240d5b`) pops the other three. Slots
below are `F+x`, offsets from `esp` after `push edi`; the argument is at
`F+0x48`. Each `[esp+x]` operand was resolved to its slot by tracking every
push, pop and callee `ret n`; the depths agree at every join.

## Data

The locals:

| Slot | Contents |
| --- | --- |
| `F+0xc` | byte: `engine_type` is 9 and the slot's flag bit 3 is set (`0x2408dc`–`0x2408e7`) |
| `F+0xd` | byte: a player was found in pickup range this tick (0 at `0x2408d7`, 1 at `0x2409fb`) |
| `F+0xe` | byte: the entry's `lc`, the carrier, is not -1 (`0x2408cf`; `bool carried = entry->lc != NONE`, `src/unknown_2420a0.cpp:1665`) |
| `F+0xf` | byte: the slot's flag bit 0 is clear (`0x2408c2`–`0x2408d3`) |
| `F+0x10` | the `s_team_entry *` from `function_15e410` (`0x24086e`) |
| `F+0x14` | the player `function_244300` reports, -1 first (`0x2408ee`) |
| `F+0x18` | the countdown's ceiling: the dword at `ctf_options()+0x230` (`0x24083d`, `0x240859`), raised to 5 when it is not above 5, unsigned (`0x24086b`–`0x240874`) |
| `F+0x1c` | the object (`0x240855`) |
| `F+0x20` | an `s_event` (0x24 bytes, `include/game_engine_events.h:14`, up to `F+0x44`); `F+0x38` is its `effect_team` |
| `F+0x48` | the argument; the capture the flag path reuses it as a byte (`0x240c20`, `0x240c27`), while the assault path still reads the object index there (`0x240cf1`) |

The object is `g_4e0300`'s header for the index (`+0x44`, entries of 12 bytes,
the object at `+8`; `0x240827`–`0x240846`); its slot is the short at `+0x17e`
(`slot_object_get(object_index)->slot`, `0x24084b`).

The slot's state in `g_51ec80` (`s_slot_table`, `include/unknown_2420a0.h:30`),
indexed by the slot in `edi`:

| Offset | Name | Use here |
| --- | --- | --- |
| `+0x114` | `d[9]` | the countdown in seconds; -1 when off |
| `+0x126` | `times[9]` | the ticks left in the countdown's current second |
| `+0x138` | inside `unknown138` | a `short[9]` (`+0x138`–`+0x149`): ticks the object has spent with flag bit 0 set, not carried and with no player in range |
| `+0x170` | `flags[9]` | bits 0 to 3 (below) |

`g_510c54->field_2_3` (`include/globals.h:16`) is read as the ticks of one
second, as other source treats it (`0x240979`–`0x24098a`,
`0x240a80`–`0x240a92`).

The flag bits, from their writers in retail:

- **Bit 0.** `function_2447f0` sets it (`0x2448d5`) when the entry's `l8` is
  not -1, or when the object's squared distance from its home marker is above
  0.04 (`0x45e468`, compared at `0x2448cb`). The pickup handler `0x240ff0` sets
  it, with bit 1 cleared, the first time the object is picked up
  (`0x24109c`–`0x241105`). `0x2403b0` (no source) sets it together with bit 3
  when its `e[slot]` counter reaches `scale_a * field_2_3` (`0x240570`–`0x2405ba`).
  `0x2422d0` clears the whole byte (`0x24234c`). The countdown below survives
  only while it is set.
- **Bit 1**: set when a bomb is armed (`g->flags[slot] |= 2`,
  `0x242d25`), with `carriers[slot]` and the countdown
  (`src/unknown_2420a0.cpp:2192`–`2201`).
- **Bit 2**: set here while `function_244300` finds an eligible player in
  pickup range (`0x2409f3`), cleared otherwise (`0x240a30`), and by the
  reset in `0x2422d0`.
- **Bit 3**: set by `0x2403b0` (`0x2405ae`); tested here only with
  `engine_type` 9 (`0x2408de`).

Constants: `0x45e468` is 0.04, read from retail `.rdata`.

## The function

### Entry and the preconditions (`0x240820`–`0x240899`)

It reads the object and its slot, keeps the `+0x230` dword, and returns at
once when the slot is -1 (`0x24085d`). It calls `function_15e410` with the
slot in `ebx` (`0x240866`, `src/unknown_15e410.cpp:40`). The rest of the
slot's update, up to `0x240bee`, is skipped, going to `0x240bfa`, when
`ctf_options()->mode` (`+0xc`) is 4 (`0x24087c`), the slot is not 0 to 8
(`0x240886`, `0x24088e`), or `function_15e410` returned null (`0x240897`).

### The carrier and the countdown's cancellation (`0x24089f`–`0x240941`)

When the object is carried, it calls `function_1972a0` (matched,
`src/unknown_196d20.cpp:232`) with the carrier in `ebx` and, pushed,
`(engine_type == 9) + 1` and the entry's `team` (`0x2408f8`–`0x24090d`).

When it is carried or bit 0 is clear, and `d[slot]` is not -1, it sets
`d[slot]` to -1 and calls `function_a7810(0x80)` (`0x240922`–`0x24093c`). So the
countdown only survives while the object is away from home and not carried.

### Who is in range (`0x240941`–`0x240a3d`)

With `engine_type` 9 and bit 0 clear, it skips the test: the counter at
`+0x138` goes to 0 and bit 2 is cleared (`0x240951` to `0x240a22`).

Otherwise it calls `function_244300` (`todo`,
`src/unknown_2420a0.cpp:1785`) with the object index in `eax` (`ebp`, the
argument, `0x240957`) and `&F+0x14` pushed (`0x240962`). Its draft looks for an
eligible player within the pickup radius: an enemy of the slot's team in
capture the flag, a member in assault (`engine_type` 9), anyone with
`team_mode` 2; slot 8 counts as team -1.

- **A player is in range.** If bit 2 was clear and the `+0x138` counter is
  above four seconds (`4 * field_2_3`, `0x240979`–`0x24098f`), it sends an
  event: `game_engine_event_initialize` with the event in `eax`, type in
  `ecx` and subtype in `edx` (`0x2409bf`, `src/unknown_19de80.cpp:608`): type
  3, subtype `0xc`, or type 10, subtype `0x10` with `engine_type` 9;
  `effect_team` is the slot, or -1 for slot 8 (`0x2409c4`–`0x2409d5`);
  `game_engine_event_set_cause_player` with the event in `ecx` and the player
  in `eax` (`0x2409e1`); `function_19eb90` with the event in `eax`
  (`0x2409e8`). Either way it sets bit 2, `F+0xd` and the counter to 0
  (`0x2409f3`–`0x240a00`).
- **No player.** If bit 0 is set and the object is not carried, the counter
  goes up by one (`0x240a18`); otherwise it goes to 0 (`0x240a26`; `bl`, still
  "bit 0 clear" from `0x2408c9`, is tested at `0x240a0c`). Bit 2 is cleared
  (`0x240a30`).

Then `function_a7810(0x200)` (`0x240a3d`).

### The countdown (`0x240a42`–`0x240b85`)

Nothing happens when `d[slot]` is -1 (`0x240a4e`), or when a player was in
range this tick (`F+0xd`), unless `F+0xc` is set (`0x240a54`–`0x240a71`).

`times[slot]` goes down by one, held to `0` … `field_2_3` (signed,
`0x240a77`–`0x240a99`). When it reaches 0 a second has passed:

1. `d[slot]` goes down by one: 0 when that is negative, otherwise at most the
   ceiling in `F+0x18`, compared unsigned (`0x240aa7`–`0x240ac5`). Then
   `function_a7810(0x80)` (`0x240acd`).
2. If `d[slot]` is not yet 0, `times[slot]` starts a new second
   (`field_2_3`, `0x240b7b`).
3. At 0, if the object is carried (`0x240ae7`) or the object's flag word at
   `+4` has bit 8 clear (`0x240aed`–`0x240afa`; `src/bipeds.cpp:4124` reads
   that bit as `sleeping`), `times[slot]` starts a new second
   (`0x240b6d`) and nothing else happens.
4. Otherwise, unless `engine_type` is 9 and bit 1 is set (`0x240b08`–`0x240b15`),
   and if bit 0 is set (`0x240b17`), it sends an event of type 3, or 10 with
   `engine_type` 9, subtype 4, `effect_team` the slot or -1, and no cause
   (`0x240b21`–`0x240b57`). Then, in every such case, `0x242660` with the
   object index in `eax` and 0 pushed (`0x240b60`).

### The kill height and the best player (`0x240b87`–`0x240bf8`)

`function_162b90` (matched, `src/unknown_1600f0.cpp:836`, "true when an item
fell below the scenario's kill height") with the object index in `edx`
(`0x240b89`). When it returns true: the same event as step 4 (type 3 or 10,
subtype 4, `effect_team`, no cause; `0x240b92`–`0x240bd3`), then `0x242660`
with the object index and 0 (`0x240bdc`).

Then `function_240740` with the object index in `eax` (`0x240be9`;
`src/unknown_2420a0.cpp:1967`), whose draft stores the best-scoring player of
the slot's team (of any team for slot 8) in `objects[slot]`.

### The local players' messages (`0x240bfa`–`0x240d61`)

This part runs whatever the preconditions decided. `eax` still holds the
`s_team_entry *` when it starts, including on the paths from `0x240899`,
where it is null: with `engine_type` other than 9, retail reads `[eax+0xc]`
at `0x240c0d` without a null test.

The local players are walked with `function_14dde0` (matched,
`players_first_active_local_player`, `src/unknown_14b560.cpp:608`). A player
counts when its entry in `g_4e8c20` (`s_index_table`, `+0xc`) is not -1 and
its `team` (`+0xc0`, `s_event_player`, `include/game_engine_events.h:28`; 0x21c
bytes in `g_4e8c24`) equals the slot (`0x240c45`–`0x240c72`).

`function_2007b3` (matched, `src/unknown_2007b3.cpp:31`) takes `arg_0` in
`edx`, which it multiplies by `0x6c` into `g_510c4c` (`0x2007cb`), `arg_1` in
`eax`, an entry of nine in that record, and `arg_2`, `arg_3` pushed. Here
`arg_0` is the local player.

- **Capture the flag** (`engine_type` not 9, `0x240c00`–`0x240ce4`). For each
  local player on the slot's team: `function_2007b3(local, 6, 1, 5)` when the
  object is carried (`0x240c78`–`0x240c83`), and
  `function_2007b3(local, 5, 1, 6)` when it is not carried and bit 0 is set
  (`0x240c90`–`0x240c9b`; that test is the byte at `F+0x48`,
  `0x240c0d`–`0x240c27`). The next local player is found inline: the first
  index after the current one, up to 3, whose `g_4e8c20` entry is not -1
  (`0x240ca0`–`0x240cd7`).
- **Assault** (`engine_type` 9, `0x240ce7`–`0x240d61`). Only when bit 1 is
  set: `function_242510` with the object index in `eax`, read from `F+0x48`
  (`0x240cf1`–`0x240cf5`; `src/unknown_2420a0.cpp:1522`), gives a team, and it
  returns when that is -1 (`0x240cfc`); otherwise, for each local player on
  that team, `function_2007b3(local, 8, 1, -1)` (`0x240d3c`–`0x240d47`). Here
  the next local player comes from a call to `function_14de10`
  (`0x240d4e`, `src/unknown_14b560.cpp:625`), not inline.

## The caller, `0x240d70`

It returns at once unless `g_55e4d0[g_4e9ae8->engine_index]` (`+0xc14`) is
non-null (`0x240d76`–`0x240d85`) and `s_mp_globals::w6c` (`+0x6c`,
`include/globals.h:206`) is 1 (`0x240d8b`). With `mode` other than 4 it also
needs `lc04` (`+0xc04`) to be 1 (`0x240da5`), and then calls `function_2447f0`
(`0x240db3`; it repeats the `mode` test first).

For each slot `ebx` from 0 to 8 (`0x240dc0`–`0x240dd5`) it calls
`function_15e410` and, for an entry, `0x240820(entry->object_index)`
(`0x240dcc`).

Then, with `mode` other than 4 (`0x240de3`), it calls `function_2431a0`
(matched, `0x240de7`). With `mode` still other than 4 (`0x240dec`), if
`team_mode` (`+0x240`) is 1,
`g_51ec80->l1f4` is 0 to 7, and `function_161e10(l1f4)` (`ecx`) returns false,
it calls `function_15b3a0` with -1 in `ebx` and 0 pushed (`0x240e1d`).

## Findings for existing source

These are naming notes; nothing here changes behaviour.

1. **Flag bit 0 and the local `away`.** The matched `0x2413f0` and `0x2431a0`
   name `!bit0` `away` (`src/unknown_2420a0.cpp:1279`, `:1291`, `:1668`). The
   capture the flag writers set bit 0 when the object leaves home:
   `function_2447f0` past 0.2 from the home marker, and the pickup handler
   `0x240ff0` on the first pickup. `0x240820` keeps the countdown, and counts
   the untouched ticks, only while it is set. By those writers `away` reads
   inverted. `0x2403b0` (assault, no source) also sets it with bit 3, on a
   timer whose meaning this document does not establish.
2. **Flag bit 2 and the local `dropped`.** `0x240820` is its only setter, while
   `function_244300` finds an eligible player in pickup range, and it clears
   it on every tick without one. The matched `0x2431a0` names it `dropped`
   (`src/unknown_2420a0.cpp:1667`).
3. **`s_slot_table` has a further `short[9]` parallel array at `+0x138`**,
   the counter above, inside `unknown138`.
4. **`ctf_options()+0x230` is read as a dword** here (`0x24083d`) and in
   `0x2440a0` (`0x2440e1`, which uses 1 when it is 0). `s_ctf_options` splits
   it into `s230` and `s232`; `0x2411b0` reads the low word into `d[slot]`
   (`0x24123a`, `src/unknown_2420a0.cpp:1190`), which a 32-bit field would
   also give.

## Callees without source

- `0x242660` (1,279 bytes, `todo`, no source): called with the object index
  in `eax` and 0 pushed when the countdown ends or the object falls below the
  kill height. Its callees include area damage (`function_d6c80`), effect
  markers (`function_176b60`) and `function_15e360`; not analysed here.

## Existing declarations

- `0x240820`: none.
- `0x240d70`: `c_game_engine_markers::v18`, declared `virtual void v18() {}`
  (`src/unknown_2420a0.cpp:210`), the repository's placeholder for a slot
  without source.
- `function_244300` (`bool (long object_index, long *player_index)`),
  `function_240740` (`void (long object_index)`) and `function_242510`
  (`long (long object_index)`) take the object index in `eax` here, and
  `function_244300` the pointer pushed.

## Evidence

- The function, its caller and the functions cited were disassembled from
  the retail XBE (the README's SHA-256) with capstone 5.0.7, through
  `tools/xbe.py` and `tools/disasm.py`. Stack slots were resolved by a pass
  that tracks `esp` through pushes, pops and each direct callee's `ret n`;
  every join point agreed.
- Callers come from an image-wide scan of every section for `E8` and `E9`
  rel32, `0F 80`–`8F` rel32 and absolute dwords at any alignment: the call at
  `0x240dcc` is the only reference to `0x240820`, and the dword at `0x459d60`
  the only one to `0x240d70`.
- The flag bits' writers come from a scan of the engine's code
  (`0x240000`–`0x245000`) for byte operations on `+0x170`.
- A second agent checked the finished document against retail and the
  repository; its corrections are applied.
- Constants are read from retail `.rdata`: `0x45e468` (0.04).
- Source line numbers are at `276e71ec`.
- No SDK or outside dataset was used. Names are the repository's own, or
  describe behaviour.
