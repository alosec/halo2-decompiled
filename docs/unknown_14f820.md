# Player action applied to the unit (unknown_14f820)

Retail range covered: `0x14f820`–`0x1502d3`. This is one inventory entry,
`todo`: `function_14f820` (`0x14f820`, 2,740 retail bytes). Deep lane 2 added
a draft of it, which does not match yet (`src/unknown_14b560.cpp:1758`).
[The draft compared with retail](#the-draft-compared-with-retail) lists where
its code differs.

Once per tick, for one player, it applies that tick's action record
(`s_player_action`) to the player's unit:

- It offers the action's trigger presses and releases to the target and weapon
  helpers of `src/unknown_1502e0.cpp`. For each one a helper uses, it sets
  the player's `flags_word` bit 4 or 5, which holds that trigger until it is
  released.
- It starts the unit's grenade throw (request `0x16`) and melee attack (`0x1b`).
- For a unit in a vehicle seat, it starts boarding (`0x1f`), a grenade (`0x16`)
  or the exit (`0x1d`).
- It copies the unit's two pending weapon slots into the action's
  `weapon_index` and `grenade_index`.
- It keeps the two tick counters (`s_player` `+0x17c` and `+0x17e`) from which
  `function_dfa60` derives a speed bonus.
- It keeps a remembered value (`+0x184`–`+0x18c`). It drops the value after
  five seconds without a refresh, or at once when neither hand's weapon passes
  `function_101160`.

**Analysis only:** this document adds no source, and nothing in it has been
built or checked against retail with the original compiler. Names are
provisional. The `function_<va>` form is primary; the descriptions are offered
for whoever decompiles it.

It uses the names of:

- `src/unknown_14b560.cpp`: `s_player`, `s_player_action`,
  `player_action_initialize`, `player_action_update_latches` and the
  `player_speed_request_*` functions.
- `src/unknown_1e77c0.cpp`: `function_1e6b00`.
- `src/unit_object_type.cpp`: `s_unit`, `s_unit_animation` and
  `s_unit_state_tail`.
- `include/units.h`: `s_unit_seat_definition`.
- `src/unknown_1502e0.cpp`: `s_target_candidate`, `s_player_pickup_unit`,
  `s_view_entry`, `s_tail_player` and the five helpers it calls there.
- `include/unit_requests.h` and `src/unknown_0e6900.cpp`: `s_unit_request`,
  `function_e6900`, `function_e68c0` and the `unit_action_*` names.
- `src/unknown_0a76b0.cpp`: `s_unit_request_melee_attack` and
  `s_unit_request_throw_grenade`.
- `src/weapons.cpp`: `s_weapon_definition`'s flag names and `value_294`.
- `src/unknown_0f5dc0.cpp`: `s_f5dc0_settings`.
- `include/globals.h`: the globals and their structures.

The request names come from the request table `g_4677c8`
(`src/unknown_0e6900.cpp:144`). Each type's perform callback is:

- `0x16`: `function_e7fb0`, which starts a grenade throw
  (`src/unknown_0a76b0.cpp:972`; its update is
  `unit_action_throw_grenade_update`);
- `0x1b`: `unit_action_melee_attack`;
- `0x1d`: `unit_action_vehicle_exit`;
- `0x1f`: `unit_action_vehicle_board`.

`function_dfa60` (`src/bipeds.cpp:1464`) reads the counters this function
keeps.

## Boundary

- **Before:** `0x14f6f0` (`player_action_update_latches`, `todo`, 304 bytes,
  `src/unknown_14b560.cpp:1160`) ends at `0x14f820`, where this function
  begins.
- **After:** after the `ret 0xc` at `0x1502d1`, `0x1502d4`–`0x1502df` are
  `int3` padding, and `0x1502e0` (`src/unknown_1502e0.cpp`) follows. Both
  neighbours are excluded.
- **Lanes:** lane N's row of the Active claims table (issue #9:
  `0x140000`–`0x14ffff`) covers the start. The code runs past `0x150000` into
  lane Q's range (`0x150000`–`0x15ffff`), where the five helpers of
  `src/unknown_1502e0.cpp` and `0x151ea0` also are. This document makes no
  claim and is offered to lane N, with a note for lane Q.
- **Existing source:** deep lane 2's round 14 (`9b8d483`) added drafts of
  `0x14f820`, of its caller `0x14d3a0` (both in `src/unknown_14b560.cpp`) and
  of `0x151ea0` (`src/unknown_1502e0.cpp:152`). Round 16 (`1c38a90`) revised
  the `0x14f820` draft, adding the `locals` struct. All three are `todo`. No
  document named `0x14f820` before this one.

## Conventions

"Callers" counts the functions that call the entry directly, from outside the
range and then from inside it.

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x14f820` | 2740 | stack: player index, unit index, action (`ret 0xc`) | void | 1 + 0 | Applies one tick's action to the player's unit |

**Registers and returns.** There are no register inputs: nothing is read
before it is written.

- **`eax` at each `ret`** is whatever the last instruction before it left. It
  can be:
  - a counter;
  - NONE;
  - the action's `unknown34`, when `flags4c` bit 0 is set and `+0x184` is
    NONE;
  - the high word of `unknown34` with the low word of `unknown38`.

  The caller overwrites it at once (`0x14d7dc`).
- **Clobbered:** the body writes `eax`, `ecx`, `edx` and `xmm0`–`xmm3`, and
  `function_b9dd0` also writes `xmm4`.
- **The x87 stack is balanced:** every value loaded is popped by `fstp`,
  `fistp`, `fpatan`, `faddp` or `fcompi`.
- **`xmm2` across calls:** it holds the rate (settings `+0x28`, loaded at
  `0x14ffc4`) across the calls at `0x14ffe6`, `0x1500b2` and `0x1500df`, and
  is read at `0x150061` and `0x15012a`. Under LTCG the compiler knows those
  three callees leave it alone. The source has to keep the rate in one value
  for the build to do the same (inferred).

**The frame.** `sub esp,0x74` comes first (`0x14f820`), then the pushes of
`ebx` (`0x14f82f`), `ebp` (`0x14f836`), `esi` (`0x14f83e`) and `edi`
(`0x14f83f`), interleaved with the first loads. Below, L is `esp` after those
pushes (the entry `esp` less `0x84`). So the player index is at `L+0x88`, the
unit index at `L+0x8c` and the action at `L+0x90`.

| Local | Type | Use |
| --- | --- | --- |
| `L+0x12` | byte | The melee's "far" flag (zeroed `0x14fc14`, set `0x14fccb`, read `0x14fd9a`); then reused for the weapon's `flag18` (`0x14ffdf`, `0x14ffeb`, read `0x150108`) |
| `L+0x13` | byte | The forward flag of the counters (zeroed `0x14ff7d`, set `0x150046`, read `0x1501a8`) |
| `L+0x14` | `s_unit *` | The unit (`0x14f875`) |
| `L+0x18` | `s_player *` | The player (`0x14f850`) |
| `L+0x1c` | long | The unit's header offset, unit index × 12 (`0x14f871`) |
| `L+0x20`, `L+0x24`, `L+0x28` | | Rounding temporaries and the angle. The melee cooldown rounds through `L+0x20` into `L+0x24` (`0x14fdb8`, `0x14fdc2`). The counters round the delay through `L+0x24` into `L+0x20`, and the duration into `L+0x28` (`0x14ff82`–`0x14ffa7`); both are kept and read at `0x150069`, `0x150084` and `0x150166`. The angle is stored to `L+0x24` (`0x14fff7`). The rounding of `field_17e` × rate goes through `L+0x28` into `L+0x24` (`0x15012e`, `0x150138`) |
| `L+0x2c`, `L+0x38` | `point3f` | The positions of the unit and of its `+0x1c8` object (`0x14fc76`, `0x14fc64`) |
| `L+0x44` | `s_unit_request` | Request `0x1b` (`0x14fd4d`–`0x14fd89`) |
| `L+0x64` | `s_unit_request` | Request `0x16` (`0x14fe6a`–`0x14fe91`) |

**Registers held across the body.**

- `ebp` holds the action throughout.
- `edi` holds the player index × `0x21c`, the player's offset in
  `g_4e8c24`'s data, from `0x14f848` until `0x14fbd4`. Apart from forming the
  player's address at `0x14f84e`, only the `flags_word` bit sets use it.
- `ebx` holds:
  - the unit's header offset from `0x14f866`, reloaded from `L+0x1c` at
    `0x14fbe4` (redundantly; the bit-5 path skips that reload) and at
    `0x14fdda`;
  - the melee's flag `bl` from `0x14fc0f` until `0x14fdd1`;
  - the counters' reset flag `bl` from `0x1500bb`;
  - `g_4e0300` at `0x1501d8`, then hand 1's weapon (`0x150213`, `0x15021f`).
- `esi` holds:
  - `0xfffd` from `0x14f8a1`, the mask that clears action flag bit 1;
  - the player index from `0x14f968`, on one path;
  - NONE from `0x14f9eb`, the comparand of the primary-press slot tests;
  - the unit index from `0x14fa22` or `0x14fa3e`, last read at `0x14fbca` and
    overwritten at `0x14fbfc` (or at `0x14fdd1` when neither button bit 5
    nor bit 6 is set).

**The caller.** One function calls `function_14f820`: `0x14d3a0` (1,290 bytes,
`todo`; deep lane 2 drafted it in `src/unknown_14b560.cpp`), with `call rel32`
at `0x14d7d7`. Its only caller is `0x137fe0`, which has no source. `0x14d3a0`
walks the players: `ebp` is the element and `ebx` the full datum index
(`0x14d696`–`0x14d6a9`).

1. **The gates.** A player gets this call only when all of these hold:
   - its bit is set in the mask at `[arg1+8]` (`0x14d6c3`–`0x14d6ce`);
   - `function_14d0a0` accepts its action (`0x14d6df`);
   - its unit is not NONE (`0x14d6f4`), and the unit's `+0x134` bit 0 is set
     (`0x14d715`);
   - `g_4e8c20`'s byte `+6` is 0 (`0x14d760`–`0x14d76a`). Otherwise control
     goes to `0x14d7f1` instead.
2. **The copy.** It copies the action into a local at entry−0x5c (`rep movsd`
   of 0x17 dwords at `0x14d779`). If `g_4e8c20`'s byte `+7` is set and the
   player's `user_index` is not NONE (`0x14d77d`–`0x14d79d`), it resets the
   copy:
   - It calls `player_action_initialize` (`0x14d040`,
     `src/unknown_14b560.cpp:430`) with the copy in `edx` (`0x14d7a7`).
   - It keeps only `facing_yaw` and `facing_pitch` (`+4`, `+8`), which it
     saves in `esi` and `edi` and writes back (`0x14d79f`–`0x14d7b4`).

   This function then receives an action with no buttons or flags set,
   NONE indices and a cleared target, so most of its steps do nothing.
3. **The latches and `function_1e6b00`.** It calls
   `player_action_update_latches` (`0x14d7be`: the player in `eax`, the
   action in `ecx`) and `function_1e6b00` (`0x14d7c8`;
   `src/unknown_1e77c0.cpp:319`, which has source that does not match yet).
4. **This call.** It pushes the action, the unit index `[ebp+0x2c]`
   (`s_player` `unit_index`) and the player in `ebx`, and calls this function.
5. **After.** It passes the same action to `0x1505f0` (`0x14d7e7`).

The image-wide scan finds no other reference to `0x14f820`.

## Data

### The player (`s_player`, `src/unknown_14b560.cpp:179`)

The player is `g_4e8c24`'s element: data at `+0x44`, 0x21c bytes each.
`function_14c090` (`src/unknown_14b560.cpp:1616`) sets up the fields:

- `+0x17e` and `+0x17c` to 0 (lines 1648, 1649);
- `+0x18c` to 0 (line 1651);
- `+0x184` and `+0x188` to NONE (lines 1662, 1663).

| Offset | Name | Use here |
| --- | --- | --- |
| `+0x02` | `flags_word`, low byte | The latches: bit 4 (`0x10`) is set at `0x14f892`, `0x14f8c5`, `0x14f8f4` and `0x14fb67`; bit 5 (`0x20`) at `0x14f922` and `0x14f991`. While bit 4 is set, `player_action_update_latches` clears action flag bits 0–3, and it keeps bit 4 only while the primary is still down. Bit 5 works the same way with bits 4–7 and the secondary. So a latched press is ignored until its button is released |
| `+0x28` | `user_index` (short); `local_index` in `s_tail_player` (`src/unknown_1502e0.cpp:26`) | Read at `0x14f929` and `0x14f998`; it selects an entry of `g_4ed284` |
| `+0x3f` | byte, inside `unknown3e` | Signed. A positive value stops the melee (`0x14fcd3`, `0x14fd2c`). Set to the ticks of 1.1 seconds after a far melee (`0x14fdce`) |
| `+0x40` | byte, inside `unknown3e` | Signed. A positive value stops the bit-5 melee (`0x14fd12`) |
| `+0x17c` | `field_17c` (short) | The forward counter |
| `+0x17e` | `field_17e` (short) | The other counter |
| `+0x184` | long, inside `unknown180` | The remembered value, from action `+0x34` |
| `+0x188` | short, inside `unknown180` | The remembered value's word, from action `+0x38` |
| `+0x18c` | long, inside `unknown180` | Ticks since the remembered value was last set |

### The action (`s_player_action`, `src/unknown_14b560.cpp:386`)

The action is 0x5c bytes. `include/network_message_types.h:145` declares it
opaque; the fields here are `src/unknown_14b560.cpp`'s.

`player_action_update_latches` (`0x14f6f0`) derives the flag bits:

- bits 2 and 3 are the primary's press and release from bit 0;
- bits 6 and 7 are the secondary's press and release from bit 4;
- bits 1 and 5 are not written by it, only cleared with their group.

| Offset | Name | Use here |
| --- | --- | --- |
| `+0x00` | `buttons` | Read: bit 4 (`0x14fba7`), bits 5 and 6 (`0x14fbd4`–`0x14fbdb`, `0x14fc11`), bit 7 (`0x14fb7d`), bit 8 (`0x14fb94`), bit 26 (`0x14facd`, `0x14fe58`), bits 16 or 26 (`0x14ff28`), and bits 0, 4, 5, 16, 17, 26 and 28 for the counters (`0x1500ad`–`0x150105`). Set: bit 28 (`0x14fa30`). Cleared: bit 26 (`0x14fe51`) |
| `+0x0c` | `throttle_i` | The angle and the magnitude (`0x14fff2`, `0x14fffe`) |
| `+0x10` | `throttle_j` | The same (`0x14ffef`, `0x14fffb`) |
| `+0x1c` | `flags` (word) | Read: bit 2 (`0x14f86d`, `0x14f9d0`), bit 1 (`0x14f89d`, `0x14f8d0`), bit 5 (`0x14f8ff`), bit 7 (`0x14f961`–`0x14f966`), bits 1 or 2 (`0x14fae1`). Cleared: bit 2 (`0x14f899`, `0x14fa38`), bit 1 (`0x14f8cc`, `0x14f8fb`, `0x14f95d`), bit 7 (`0x14f9cc`), bits 1 and 2 (`0x14fb6e`) |
| `+0x1e` | `unknown1e`, `weapon_index`, `grenade_index` | Written as one dword from the unit's `+0x214` (`0x14fec7`, `0x14ff2f`) |
| `+0x24` | `unknown24` (short) | Compared with the unit's `unknown240` (`0x1500c5`) |
| `+0x28` | `target` | Its address is passed to `function_151c10`, `function_151c90` and `function_151940` (`0x14f87b`, `0x14f8a8`, `0x14f8dd`, `0x14f90c`) |
| `+0x30` | `target.unknown08` | An object index: the melee's target (`0x14fd37`) |
| `+0x34` | `unknown34` | Copied to the player's `+0x184` (`0x150246`, `0x150256`) |
| `+0x38` | `unknown38` | Its low word is copied to the player's `+0x188` (`0x15025c`, `0x1502b9`) |
| `+0x4c` | `flags4c` | Bit 0 stops that copy (`0x150250`) |

`s_player_action` types `target` as `s_player_action_target`
(`src/unknown_14b560.cpp:379`: the longs `type`, `index` and `unknown08`).
The helpers that receive its address read it as `s_target_candidate`
(`src/unknown_1502e0.cpp:53`): a short `priority` at `+0`, a byte `flags` at
`+2` and the long `object_index` at `+4`. `function_151c10` and
`function_151c90`, which read it that way, match retail.

`src/unknown_14b560.cpp` agrees: `player_action_target_valid` (line 455) and
`function_14d0a0` (line 476) read `(short)target->type` as 0 to 8, and
require `index` not to be NONE when it is not 0. So `type` is a `priority`
short followed by two bytes, and `index` is the candidate's object.
`function_151c90` also reads a short at `+2` as a seat index.

`function_14d0a0` also requires `weapon_index` and `grenade_index` each to be
NONE or 0 to 3. Unless `weapon_index` is NONE, the two must also differ
(lines 505–516). So both are weapon slot indices.

### The unit (`s_unit`, `src/unit_object_type.cpp:44`)

The unit is the object of `g_4e0300` element `+8`: data at `+0x44`, 12 bytes
each. The same layout gives the parent object and the weapons their
`definition_index` at `+0`.

| Offset | Name | Use here |
| --- | --- | --- |
| `+0x00` | `definition_index` | Read for the parent (`0x14fa7a`) and for a weapon (`0x14fe2e`, `0x14fefc`): their tag data |
| `+0x14` | `parent_index` | `0x14fa49` (the seat) and `0x14ff46` (the counters) |
| `+0xaa` | `type` (byte) | 0 makes `function_e4050` decide the counter step (`0x15018e`) |
| `+0x12a` | `animation_offset` (short) | The offset of the animation block from the unit (`0x14fb03`, `0x14fb0d`) |
| `+0x1c8` | `unknown1c8` (`s_unit_state_tail`), first dword | An object index, the far test's other end (`0x14fc51`) |
| `+0x1e4` | `unknown1c8.unknown1c` (real, `src/unit_object_type.cpp:39`) | Must be at least 1.0 for the far test (`0x14fc3a`) |
| `+0x1fc` | `parent_seat_index` (short) | `0x14fa55` |
| `+0x212` | `current_weapon_index` (char); `held_slots[0]` in `s_player_pickup_unit` (`src/unknown_1502e0.cpp:951`) | Hand 0's held slot: `0x14f9e3`, `0x14fbf4`, `0x14fce2`, `0x14fde2`, `0x14fe04`, `0x14feab`, `0x14fed3`, `0x14ffbc`, `0x1501e9` |
| `+0x213` | `next_weapon_index` (char); `held_slots[1]` | Hand 1's held slot: `0x14fa06`, `0x14feb4`, `0x15020b` |
| `+0x214` | `unknown214`, then `unknown216` and `unknown217`, which `s_player_pickup_unit` calls `pending_slots[2]` (line 953) | One dword copied to the action's `+0x1e` (`0x14fec1`, `0x14ff22`). `unknown216`, hand 0's pending slot, is also compared with its held slot `current_weapon_index` (`0x14fce8`) |
| `+0x218` | `weapon_object_indices[4]` | The weapon in a slot |
| `+0x238` | `unknown238` | An object index: passed to `0x151ea0`, and deleted by `function_ccd20` (`0x14fbb1`) |
| `+0x240` | `unknown240` (char) | Sign-extended and compared with the action's `unknown24` (`0x1500bd`) |

**The two hands.** `s_unit` calls `+0x212` and `+0x213` the current and
next weapon, but retail uses them as the held slots of hands 0 and 1:

- `function_151b80` computes the hand as `!primary` and reads
  `[unit + hand + 0x212]` (`0x151ba2`–`0x151bc2`).
- `0xce920` pairs `+0x212 + hand` with `+0x216 + hand` for both hands.

Below, "hand 0" is the current weapon and "hand 1" the next one.

**Slot tests.** A slot holds a weapon when its index is not NONE and
`weapon_object_indices[index]` is not NONE (`0x14f9e3`–`0x14fa1d`,
`0x14fde2`–`0x14fdfb`, `0x14fed3`–`0x14feee`). Every lookup reads the slot
index again from the unit, rather than keeping it. The reads take two forms,
and the source has to give both:

- Most sign-extend the byte to 16 bits with `movsx` and compare with −1
  (`0x14f9e3`, `0x14fa06`, `0x14fbf4`, `0x14fde2`, `0x14fe04`, `0x14fed3`,
  `0x14ffbc`, `0x1501e9`, `0x15020b`).
- Three compare bytes directly: `cmp byte ptr …, 0xff` (`0x14feab`,
  `0x14feb4`) and `cmp dl, byte ptr [eax+0x216]` (`0x14fce8`).

### Tags and globals

| Where | Use here |
| --- | --- |
| `g_4e3b44` | The tag instances, 16 bytes each, data pointer at `+8`. The array pointer is used directly, with no `+0x44` (`0x14fa7c`–`0x14fa93`) |
| Parent's tag data `+0x1cc` | The seat elements (`UNIT_SEATS`, `src/unit_object_type.cpp:272`): `s_unit_seat_definition` (`include/units.h:16`, 0xb0 bytes), indexed by `parent_seat_index`. Its first dword is `flags`, and the bit tested is `bit11` (`include/units.h:11`) (`0x14fa97`–`0x14faa3`). The count at `+0x1c8` (`UNIT_SEAT_COUNT`) is not read: the seat index is only tested against NONE (`0x14fa5c`), with no bounds check, so a bounds check in the source would not match |
| Weapon's tag data `+0x12c` | The flags of `s_weapon_definition` (`src/weapons.cpp:100`). `flag12` (line 117) suppresses the grenade (`0x14fe43`–`0x14fe4f`). This is not the bit the repository names `prevents_grenade_throwing`, which is bit 6 (line 111). `flag3` (line 108) allows the index copy when hand 1 holds no slot (`0x14ff10`–`0x14ff1c`). `flag18`, `flag24` and `flag28` are read through `src/weapons.cpp`'s functions |
| `[g_4e034c + 0xf4]` | A settings block (`s_f5dc0_settings`, `src/unknown_0f5dc0.cpp:21`, names only `+0x70`). The counters read the reals at `+0x20` (delay, seconds), `+0x24` (duration, seconds), `+0x28` (rate), `+0x30` (dead zone) and `+0x34` (half-angle, degrees) (`0x14ff6e`–`0x14ffc4`, `0x150011`, `0x15001c`, `0x150034`). `function_dfa60` reads `+0x20`, `+0x24`, `+0x28` and `+0x2c` |
| `g_4e6948` (`s_game_options_view`) | `state` at `+8`: 2 skips the counters and enables `function_1628f0` (`0x14fb77`, `0x14ff55`). `mode` at `+0xc`: 4 skips the board request (`0x14fab8`), and replaces the exit request with `function_a9390`'s network event (`0x14fb3b`) |
| `g_510c54` (`s_game_time_globals`) | `field_2_3` at `+2`, the ticks per second (short), for every seconds-to-ticks rounding; `rate` at `+4`, times the tick count, for the five seconds (`0x150281`) |
| `g_4ed284` (`s_unknown_185ab0`; `s_view_globals` in `src/unknown_1502e0.cpp`) | The entries (0x94 bytes, from `+0x14`), indexed by the player's `user_index`. `s_unknown_185ab0_entry` (`include/globals.h:1113`) leaves these offsets unnamed; `s_view_entry` (`src/unknown_1502e0.cpp:70`) names them `index28`, `flag8e` and `flag8f`. Its inline `clear_flags()` (line 79) is exactly the update here: if `flag8e`, set `flag8f`; then `index28` = NONE (`0x14f933`–`0x14f957`, `0x14f9a2`–`0x14f9c6`) |

### Constants

These are read from retail `.rdata`:

- `0x45dbc0`: 1.0.
- `0x45dc6c`: 1/30.
- `0x45e61c`: 0.81, as 0.9².
- `0x45dc80`: 1.1.
- `0x45e618`: −π/180.
- `0x45dc54`: π/180.
- `0x45dbf0`: 5.0.

`0x45e61c` is `0x3f4f5c28`, the float product of 0.9f × 0.9f. The literal
0.81f would be `0x3f4f5c29`, so the source has to give those exact bits:
inferred, a squared 0.9.

The request types, the masks and `0xfffd` are immediates.

## The function

**Sequence.** It runs these steps in this order, with no loop and no jump
table:

1. The trigger presses and releases offered to the target and weapon helpers.
2. A primary press while a hand holds a weapon.
3. A unit in a vehicle seat.
4. Game state 2.
5. Button bit 4.
6. The melee.
7. The grenade.
8. The pending slots.
9. The two counters.
10. The remembered value.

**Control flow.** It has 142 basic blocks. No jump goes backwards, and the
two `ret 0xc` are at `0x1502ad` and `0x1502d1`. A stack-depth walk over every
path finds the same depth at every join and depth 0 at both `ret`s.

**Calls.** Its 33 call sites reach 22 callees. Under LTCG most take their
arguments in registers. [Existing declarations](#existing-declarations) lists
them, and [`0x151ea0`](#0x151ea0) describes the one whose source is newest.

### Entry (`0x14f820`–`0x14f879`)

1. It reserves the frame and loads the arguments: the player index into `edx`
   (`0x14f823`), the action into `ebp` (`0x14f837`) and the unit index
   (`0x14f854`).
2. `edi` = (player index & 0xffff) × 0x21c (`0x14f840`–`0x14f848`). The
   player's address, data + `edi`, goes to `L+0x18` (`0x14f84e`, `0x14f850`).
3. `ebx` = (unit index & 0xffff) × 12 (`0x14f85b`–`0x14f866`) goes to
   `L+0x1c` (`0x14f871`). The unit's address, the header's `+8`
   (`0x14f869`), goes to `L+0x14` (`0x14f875`).

No index is checked for NONE; the caller only calls with a unit
(`0x14d6f4`–`0x14d6fa`).

Every setting of `flags_word` bit 4 or 5 below (the latch) has the same form:
`g_4e8c24`'s data is reloaded, then `or byte ptr [data + edi + 2], 0x10` (or
`0x20`), then `add eax, edi`. This is `s_player`'s `flags_word`, not its
`latched_flags` (`+0x3c`). That
leaves the player's address in `eax`, but only the two bit-5 sites use it,
to read `user_index` (`0x14f929`, `0x14f998`). The other four
(`0x14f897`, `0x14f8ca`, `0x14f8f9`, `0x14fb6c`) leave it unused. That
suggests a small inline that sets the bit and returns the player (inferred).

### The trigger presses and releases (`0x14f86d`–`0x14f9cc`)

1. **Flag bit 2, the primary's press** (`0x14f86d`): it calls
   `function_151c10` (`0x14f880`) with the player index in `eax` (from `edx`,
   `0x14f87e`) and the target in `ecx` (`0x14f87b`).
   - If that returns true, it sets `flags_word` bit 4 (`0x14f892`) and
     clears flag bit 2 (`0x14f899`).
   - `function_151c10` acts on a candidate of priority 8 that
     `function_1518f0(candidate, 2)` accepts. In mode 4 it calls
     `function_a90e0`; otherwise it sends request `0x21`
     (`unit_action_vehicle_flip`) to the candidate's object.
2. **Flag bit 1** (`0x14f89d`), with `esi` = `0xfffd` (`0x14f8a1`):
   - It calls `function_151c90` (`0x14f8b3`) with the target pushed
     (`0x14f8ab`) and the player index in `eax` (`0x14f8ac`). If that returns
     true, it sets `flags_word` bit 4 (`0x14f8c5`) and clears flag bit 1 with
     `and word ptr [ebp+0x1c], si` (`0x14f8cc`).
   - If bit 1 is still set (`0x14f8d0`), it calls `function_151940`
     (`0x14f8e2`) with the player index in `eax`, the target in `ecx` and
     `primary` = 1 pushed (`0x14f8e0`). If that returns true, it sets
     `flags_word` bit 4 (`0x14f8f4`) and clears flag bit 1 (`0x14f8fb`).
   - `function_151c90` uses one of three kinds of candidate:
     - a device (priority 3): `function_a9180` in mode 4, `function_107840`
       otherwise, then `function_1e8f60`;
     - a seat (4 to 6): `function_a9340` when `function_138880()` is true,
       request `0x1c` otherwise. When `function_c92c0` refuses the seat but
       reports a blocking unit, it calls `function_1c9500` (outside
       `function_138880()`, when the blocker's `+0x12c` actor is not NONE)
       and still returns true
       (`src/unknown_1502e0.cpp:1253`–`1259`). So that case also sets
       `flags_word` bit 4 and clears flag bit 1 here;
     - priority 2, outside mode 4: request `0x15` to the candidate's object.
   - `function_151940` picks up a weapon candidate (priority 1 or 7, whose
     `+0x154` is NONE) in a mode of the given hand: `function_a8f10` when
     `function_138880()` is true, request `0x14` otherwise.
3. **Flag bit 5** (`0x14f8ff`): it calls `function_151940` (`0x14f911`) with
   `primary` = 0 (`0x14f90f`). If that returns true:
   - It sets `flags_word` bit 5 (`0x14f922`).
   - If the player's `user_index` (`0x14f929`) is not NONE, it updates that
     user's `g_4ed284` entry (`0x14f942`). If the entry's `flag8e` is set
     (`0x14f946`–`0x14f94e`), it sets `flag8f` (`0x14f950`). In either case it
     sets `index28` to NONE (`0x14f957`).
   - That update is the body of the matched `function_1520f0`
     (`src/unknown_1502e0.cpp:261`): `local_player_view_clear` and
     `s_view_entry::clear_flags`. Here it is written out (inferred:
     inlined).
   - Then `and word ptr [ebp+0x1c], si` (`0x14f95d`) clears flag **bit 1**,
     not bit 5, which stays set. The NONE `user_index` path also reaches this
     `and` (`0x14f931`).
4. **Flag bit 7, the secondary's release** (`0x14f961`–`0x14f966`: the sign
   of the flags' low byte):
   - It sets `esi` to the player index (`0x14f968`) and calls
     `function_151aa0` (`0x14f973`) with the player in `eax` and `primary` = 0
     pushed (`0x14f96f`). If that returns false, it calls `function_151b80`
     (`0x14f980`) with the player in `eax` and `primary` = 0 in `cl`
     (`0x14f97c`).
   - If either returns true, it sets `flags_word` bit 5 (`0x14f991`), does
     the same entry update (`0x14f998`–`0x14f9c6`) and clears flag bit 7 with
     `and byte ptr [ebp+0x1c], 0x7f` (`0x14f9cc`).
   - With `primary` = 0, both act on hand 1's weapon.
     - `function_151aa0` acts when `function_cd7b0` allows that hand: through
       `function_a8f80` in mode 4, `function_cd4e0` otherwise.
     - `function_151b80` drops the weapon: through `function_a9030` in mode
       4, otherwise with request `0x13` (`unit_action_drop_weapon`; hand 0
       would use request 9).

### A primary press while a hand holds a weapon (`0x14f9d0`–`0x14fa3e`)

This step runs if flag bit 2 is still set (`0x14f9d0`) and either hand holds
a weapon: hand 0 (`0x14f9e3`–`0x14f9fd`) or, failing that, hand 1
(`0x14f9ff`–`0x14fa1d`). Then:

1. It sets buttons bit 28 (`0x14fa1f`–`0x14fa30`).
2. It calls `function_a8dd0` with the unit index pushed (`0x14fa2f`,
   `0x14fa33`) (`src/unknown_0a7c50.cpp:514`). That sends network event `0xd`
   for the unit in mode 5, and in mode 4 when `function_a76b0(unit, 1)` is
   true. In other modes it does nothing.
3. It clears flag bit 2 (`0x14fa38`).

On both paths `esi` holds the unit index from here (`0x14fa22`, `0x14fa3e`).

### A unit in a vehicle seat (`0x14fa45`–`0x14fb6e`)

This only applies when the unit's `parent_index` (`0x14fa49`) and
`parent_seat_index` (`0x14fa55`) are both not NONE. The seat is the element
pointer stored at the parent's tag data `+0x1cc` (`0x14fa97`), plus the seat
index × 0xb0 (`0x14fa8a`, `0x14fa9d`): `UNIT_SEATS(definition)[seat]`.

1. **Seat flag bit 11**, only when it is set (`0x14faa0`–`0x14faa5`:
   `test al, 1; je 0x14fae1`). If `function_0c7070` (unit in `ecx`,
   `0x14faa9`) returns false:
   - Unless the mode is 4 (`0x14fab2`–`0x14fabc`), it calls
     `function_e68c0` (`0x14fac4`) with type `0x1f` in `ecx` and the unit
     pushed (vehicle board).
   - If that returns false, or the mode is 4, and buttons bit 26 is set
     (`0x14facd`), it calls `function_e68c0(0x16, unit)` (`0x14fadc`,
     grenade).
   - Neither result is kept.
2. **Flag bits 1 or 2** (`0x14fae1`).
   - If `function_0c7070` (`0x14faed`) returns true, the unit's animation
     must have run long enough (`0x14faf6`–`0x14fb33`):
     - The block is `UNIT_ANIMATION(unit)`, an `s_unit_animation`
       (`src/unit_object_type.cpp:252`). If its `unknown00` (`+0`) and
       `unknown06` (`+6`) are both not NONE, the time is the real at `+0x1c`
       × 1/30; otherwise the time is 0.
     - The step stops if 1.0 is greater than that time. So with no valid
       animation the exit does not happen. 30 frames at 30 per second is the
       inferred reading.
     - `function_0c7070` (`src/unknown_0c7070.cpp:24`) tests whether that
       block's `name` (`+0x7c`) is one of the five values in `g_467430`
       (line 21).
   - In mode 4 (`0x14fb3b`) it calls `function_a9390` (`0x14fb48`, unit in
     `esi`). When `function_a76b0(unit, 0)` is true, that sends network event
     `0xb` with the unit, its parent and its seat. Either way, this function
     counts it as done.
   - Otherwise it calls `function_e68c0(0x1d, unit)` (`0x14fb55`, vehicle
     exit), and stops if that returns false.
   - When done, it sets `flags_word` bit 4 (`0x14fb67`) and clears flag
     bits 1 and 2 with `and byte ptr [ebp+0x1c], 0xf9` (`0x14fb6e`).

### Game state 2 (`0x14fb72`–`0x14fba2`)

When `g_4e6948`'s `state` is 2 (`0x14fb77`):

- Buttons bit 7 calls `function_1628f0` (`0x14fb8c`) with the player in `eax`
  and 5 in `cl`.
- Buttons bit 8 calls it with 4 (`0x14fba2`).

`function_1628f0` (`src/unknown_1600f0.cpp:708`) sets the state of the
player's entry in the game engine's players (`g_4e9ae8`) outside mode 4.

### Button bit 4 (`0x14fba7`–`0x14fbcc`)

If buttons bit 4 is set (`0x14fba7`) and the unit's `unknown238` is not NONE
(`0x14fbb1`–`0x14fbba`):

1. It calls `0x151ea0` (`0x14fbc5`) with the player and `unknown238` pushed
   (`0x14fbc4`, `0x14fbc3`).
2. It calls `function_ccd20` (`0x14fbcc`) with the unit in `eax` (from `esi`,
   `0x14fbca`). That deletes the object through `function_b8540` and sets
   `unknown238` to NONE.

### The melee (`0x14fbd1`–`0x14fdce`)

It loads the buttons into `edx` and `edi` = buttons & `0x20` (bit 5)
(`0x14fbd1`–`0x14fbd6`). Nothing happens unless bit 5 or bit 6 is set
(`0x14fbd9`, `0x14fbdb`). Then:

1. `esi` = hand 0's weapon, or NONE (`0x14fbe8`–`0x14fc08`); `bl` and
   `L+0x12` = 0 (`0x14fc0f`, `0x14fc14`).
2. **Bit 6 with a weapon** (`0x14fc11`, `0x14fc1e`). It calls
   `function_1012c0` (weapon in `ecx`, `0x14fc29`), which is true when the
   weapon's definition `flag24` is set.
   - **If true:**
     - **The far test.** It runs when the unit's real at `+0x1e4` is at least
       1.0 (`0x14fc36`–`0x14fc49`) and `function_bae20` finds the unit's
       `+0x1c8` object with type mask 3 (`0x14fc4f`–`0x14fc62`).
       `function_b9dd0` writes that object's position to `L+0x38`
       (`0x14fc6a`) and the unit's to `L+0x2c` (`0x14fc7a`). If the squared
       distance between them is greater than 0.81 (`0x14fc7f`–`0x14fcc7`),
       then `bl` and `L+0x12` = 1 (`0x14fcc9`, `0x14fccb`).
     - If the player's `+0x3f` is positive (`0x14fccf`–`0x14fcd8`), nothing
       happens. Otherwise the melee goes ahead only when hand 0's held slot
       equals its pending slot: `current_weapon_index` equals `unknown216`
       (`0x14fcde`–`0x14fcee`).
   - **If false:** it calls `function_101300` (weapon in `eax`, `0x14fcf7`),
     true when `flag28` is set. If that returns true the melee goes ahead;
     otherwise nothing happens.
3. **Otherwise** (bit 6 without a weapon, or bit 5 alone) (`0x14fd06`):
   - Nothing happens without bit 5.
   - Nothing happens if the player's `+0x40` is positive
     (`0x14fd0e`–`0x14fd17`).
   - Otherwise it calls `function_1012c0(esi)` (`0x14fd1f`). That is NONE
     when there is no weapon, and `function_1012c0` returns false for NONE.
     False lets the melee go ahead. True lets it go ahead unless the player's
     `+0x3f` is positive (`0x14fd28`–`0x14fd31`).
4. **Going ahead** (`0x14fd37`):
   - **The target.** The target is the action's `target.unknown08` if
     `function_bae20` (`0x14fd41`) finds it with type mask 3; otherwise it is
     NONE (`0x14fd3e`, `cmovne` at `0x14fd48`).
   - **The request at `L+0x44`.** Its eight dwords are zeroed
     (`0x14fd4d`–`0x14fd85`). Then:
     - `+8` = the target (`0x14fd70`);
     - the word at `+4` = 1 + (`bl` ≠ 0) (`0x14fd51`–`0x14fd79`);
     - the type = `0x1b` (`0x14fd89`).
   - **The call.** It calls `function_e6900` (`0x14fd91`) with the unit in
     `eax` (`0x14fd7e`) and the request in `edi` (`0x14fd74`).
   - **The cooldown.** If that returns true and `L+0x12` is set
     (`0x14fd96`–`0x14fda0`), the player's `+0x3f` gets the ticks of 1.1
     seconds. That is `field_2_3` × 1.1, rounded by `fld`/`fistp` through
     `L+0x20` and `L+0x24` (`0x14fda2`–`0x14fdce`).
   - **The request's names.** `unit_action_melee_attack` reads this request as
     `s_unit_request_melee_attack` (`src/unknown_0a76b0.cpp:2115`): `mode` at
     `+4` and `target_index` at `+8`. `include/unit_requests.h` has no view
     for type `0x1b`.
   - **`mode`.** It is 2 only on the bit-6 path with a `flag24` weapon, when
     the unit's `+0x1e4` is at least 1.0 and its `+0x1c8` object is found
     more than 0.9 away. Otherwise it is 1.

### The grenade (`0x14fdd1`–`0x14fe9e`)

1. If hand 0 holds a weapon whose definition has `flag12` set
   (`0x14fdd1`–`0x14fe4f`), it clears buttons bit 26 (`0x14fe51`).
2. Only if buttons bit 26 is still set (`0x14fe58`; otherwise `je 0x14fea4`
   at `0x14fe5f`):
   - It builds the request at `L+0x64`: eight dwords zeroed
     (`0x14fe68`–`0x14fe8a`), type `0x16` (`0x14fe91`).
   - It calls `function_e6900` (`0x14fe99`) with the unit in `eax`
     (`0x14fe61`) and the request in `edi` (`0x14fe86`). The result is not
     used.

**The request's view.** `function_e7fb0` reads this request as
`s_unit_request_throw_grenade` (`src/unknown_0a76b0.cpp:930`), so its
`cancel` (`+4`), `immediate` (`+5`), `origin` (`+8`) and `direction`
(`+0x14`) are all zero here.

**`function_e68c0` builds the same request** for type `0x16`, and the seat
step calls it. Here the request is written out instead (inferred: inlined).

### The pending slots (`0x14fea4`–`0x14ff3d`)

1. If both hands hold a slot (`current_weapon_index` and `next_weapon_index`
   are not NONE: byte compares with `0xff`, `0x14feab`, `0x14feb4`), the
   action's `+0x1e` gets the unit's dword at `+0x214` (`0x14febd`–`0x14fec7`).
2. Otherwise, if hand 0 holds a weapon whose definition has `flag3` set
   (`0x14fecc`–`0x14ff1c`), the action's `+0x1e` gets the same dword
   (`0x14ff1e`–`0x14ff2f`). If buttons bit 16 or 26 is also set
   (`0x14ff28`), it calls `function_151b80` (`0x14ff3d`) with the player in
   `eax` and `primary` = 1 in `cl` (`0x14ff3b`). That drops hand 0's weapon:
   through `function_a9030` in mode 4, with request 9 otherwise.

**What the copy maps.**

| The unit's | Goes to the action's |
| --- | --- |
| `unknown214` | `unknown1e` |
| `unknown216` (`pending_slots[0]`) | `weapon_index` |
| `unknown217` (`pending_slots[1]`) | `grenade_index` |

The action's two names do not fit what retail puts there: both are pending
weapon slots, one per hand, as `function_14d0a0`'s checks also show. The
unit's grenade indices are `current_grenade_index` and `next_grenade_index`
at `+0x23c` and `+0x23d`, which this function does not read.

### The two counters (`0x14ff42`–`0x1501cf`)

If the unit has a parent (`0x14ff46`) or the state is 2 (`0x14ff55`), both
counters are set to 0 (`0x1501c2`–`0x1501cf`). Otherwise:

1. **The delay and the duration.**
   - delay = round(`field_2_3` × settings `+0x20`), through `L+0x24` into
     `L+0x20` (`0x14ff5f`–`0x14ff8c`).
   - duration = round(`field_2_3` × settings `+0x24`), into `L+0x28`
     (`0x14ff90`–`0x14ffa7`).
   - `xmm2` = settings `+0x28`, the rate (`0x14ffc4`).
   - `L+0x13` = 0 (`0x14ff7d`).
2. **The weapon's `flag18`.** `L+0x12` = `function_101340` of hand 0's
   weapon (`0x14ffe6`, weapon in `ecx`), or 0 without a weapon
   (`0x14ffab`–`0x14ffeb`).
3. **The forward flag.**
   - On the x87 stack: angle = atan2(`throttle_j`, `throttle_i`), through
     `fpatan`, stored to `L+0x24` (`0x14ffef`–`0x14fff7`).
   - magnitude = √(`throttle_i`² + `throttle_j`²), computed and compared with
     settings `+0x30` by `fcompi`, without leaving the x87 stack
     (`0x14fffb`–`0x150018`).
   - If magnitude > `+0x30`, and −θ < angle < θ, with θ = settings `+0x34` ×
     π/180 (`0x15001c`–`0x150044`), then `L+0x13` = 1 (`0x150046`). That is,
     the throttle is pushed forward within `+0x34` degrees.
4. **Clamping.** remaining = `field_17c` − `field_17e` × rate, as a real
   (`0x15004b`–`0x15006d`).
   - If remaining is negative, both counters become 0
     (`0x150071`–`0x15009f`).
   - Else, if it is at least delay + duration (`0x150084`–`0x150092`),
     `field_17e` = 0 and `field_17c` = delay + duration
     (`0x150094`–`0x15009f`).
5. **Two resets** (`0x1500a6`–`0x150111`):
   - `bl` = `function_10f890` (unit in `eax`, `0x1500b2`), or `unknown24` ≠
     the unit's `unknown240`, or buttons bit 28, or buttons bit 0.
   - `al` = `function_cc3c0` (unit in `ecx`, `0x1500df`), or buttons bit 26,
     17, 16, 5, 4 or 0, or `L+0x12`.
   - If either is set (`0x150113`–`0x150119`):
     - `field_17c` = max(`field_17c` − round(`field_17e` × rate), 0), and
       `field_17e` = 0 (`0x15011b`–`0x15015d`).
     - Then, if `bl` is set, `field_17c` = min(`field_17c`, delay)
       (`0x150164`–`0x150172`).
     - If `al` is set, `field_17c` = 0 (`0x150179`–`0x15017d`).
6. **The step.** The counters step unless the unit's `type` is 0 and
   `function_e4050` (unit in `eax`, `0x15019f`) returns true
   (`0x15018a`–`0x1501a6`). The step adds 1 to `field_17c` if `L+0x13` is
   set, and to `field_17e` otherwise (`0x1501a8`–`0x1501b9`).

`function_dfa60` (`src/bipeds.cpp:1464`) turns these into what its comment
calls "the sprint boost while its sprint meter lasts" (lines 1461–1462). In
lines 1473–1490:

- remaining = `field_17c` − round(`field_17e` × settings `+0x28`).
- The speed starts from the player's `+0x19c`.
- While remaining > delay, the fraction (remaining − delay) / (duration − 1)
  is clamped to [0, 1] and passed through `function_17ca10(…, 1)`.
- The result, times (settings `+0x2c` − 1.0), is added to the speed.

So `field_17c` counts the ticks of pushing forward, which fill the meter, and
`field_17e` the ticks that wear it off.

### The remembered value (`0x1501d8`–`0x1502d1`)

1. `edi` = hand 0's weapon or NONE (`0x1501e1`–`0x1501fd`), and `ebx` =
   hand 1's weapon or NONE (`0x150204`–`0x15021f`).
2. If `function_101160` (weapon in `eax`) is true for either (`0x15022d`,
   `0x15023d`):
   - If the action's `unknown34` is not NONE and `flags4c` bit 0 is clear
     (`0x150246`–`0x150254`), the player's `+0x184` = `unknown34`
     (`0x150256`), `+0x188` = the low word of `unknown38` (`0x15025c`,
     `0x1502b9`) and `+0x18c` = 0 (`0x1502c0`).
   - Otherwise, if the player's `+0x184` is not NONE (`0x150262`), `+0x18c`
     grows by 1 (`0x15026a`–`0x150277`). When it × `g_510c54`'s `rate`
     (`0x150281`) exceeds 5.0 (`0x150286`), `+0x184` and `+0x188` = NONE and
     `+0x18c` = 0 (`0x150290`–`0x15029d`). It then returns from `0x1502ad`,
     with `pop edi` already done at `0x15028f`.
3. If neither weapon passes, `+0x184` and `+0x188` = NONE and `+0x18c` = 0
   (`0x1502b0`–`0x1502c0`).

`function_101160` (`src/weapons.cpp:431`) is true when any trigger of the
weapon is in state 2 (with the definition's `value_294`), 3, 5, 6 or 7.

## The draft compared with retail

Deep lane 2's draft (`src/unknown_14b560.cpp:1758`–`1976`, at `5d1a893`)
follows retail closely. It has the steps in retail's order, the bit-1 clear
after the bit-5 helper, the seat lookup with no bounds check, and the
constants with retail's exact bits (`0.809999942779541f`,
`0.03333333507180214f`, `±0.01745329238474369f`).

The differences below are in how the code is written rather than in what it
does. Each states what retail shows, as a fact, and then a form that would fit
it, which is inferred and untested, since nothing here was compiled.

1. **One-bit flags read as bitfields.** Retail reads three flag bits with a
   shift and a test of bit 0:
   - the seat's flags bit 11, `shr eax, 0xb; test al, 1`
     (`0x14fa9d`–`0x14faa3`);
   - the weapon definition's bit 12, `shr ecx, 0xc; test cl, 1`
     (`0x14fe43`–`0x14fe4c`);
   - the weapon definition's bit 3, `shr edx, 3; test dl, 1`
     (`0x14ff10`–`0x14ff19`).

   The draft tests masks: `& (1 << 11)` (line 1812), `& (1 << 12)` (line
   1896) and `& 8` (line 1914).

   The repository has already found what produces retail's form.
   `include/unknown_11c920.h:21`–`25` notes that a one-bit field converted to
   `bool` (`TEST_FIELD_BIT`) is the only form this compiler turns into
   `shr`/`test 1`. Masks, `(x >> n) & 1` and inline functions all fold to a
   single `test` with a mask. Two matched functions read these same bits that
   way:
   - `function_1bfc30` tests the same seat flag:
     `TEST_FIELD_BIT(tag->seats[unit->unknown1fc].flags.bit11)`
     (`src/unknown_1bf990.cpp:125`). Its retail code is the same sequence as
     `0x14fa49`–`0x14faa5`, up to scheduling, ending
     `mov eax, [ecx+edx]; shr eax, 0xb; test al, 1` (`0x1bfcb1`–`0x1bfcb7`).
   - `function_101240` reads `flag3` with `TEST_FIELD_BIT(definition->flag3)`
     (`src/weapons.cpp:467`), which compiles to
     `mov eax, [eax+0x12c]; shr eax, 3; and al, 1` (`0x10126a`–`0x101273`).

   Reading `bit11`, `flag12` and `flag3` through `TEST_FIELD_BIT` would fit
   (inferred, with these two matches as precedent).
2. **The two reset conditions have no branch** (lines 1940–1941). From the
   call at `0x1500b2` to `0x150113` retail has no jump:
   Below, `esi` is the buttons dword, loaded at `0x1500ad` before the first
   call.
   - `bl` = `function_10f890`'s result, then `or bl, cl` (the `setne` of the
     `unknown24` comparison), then `or bl, dl`. Here `dl` =
     ((`esi` >> 28) | the action's low byte) & 1, the low byte being read
     again from memory after the call (`mov al, byte ptr [ebp]`, `0x1500c9`).
   - `al` = `function_cc3c0`'s result, then `or al, cl`, then `or al, dl`
     with `dl` = `L+0x12`. Here `cl` = ((`esi` >> 26) | (`esi` >> 17) |
     (`esi` >> 16) | (`esi` >> 5) | (`esi` >> 4) | the action's low byte) & 1,
     with the low byte read again after that call (`or cl, byte ptr [ebp]`,
     `0x150105`). The `& 1` is applied once, after all the ORs.

   The draft's form differs on two counts:
   - It joins the terms with `||`, which short-circuits.
   - It tests masks of its local copy `activity` (`activity & 0x10000001`,
     `activity & 0x4030031`), which as operands of `||` usually become a test
     and a branch each (inferred).

   Terms joined with `|`, each button bit read as a one-bit field (as in item
   1), would fit better (inferred). The repository's note on
   `TEST_FIELD_BIT` concerns single-bit conditions, so this OR'ed form is
   untested.
3. **A 16-bit comparison** (line 1940). Retail sign-extends the unit's byte
   to 16 bits and compares words: `movsx ax, byte ptr [edx+0x240]`, then
   `cmp word ptr [ebp+0x24], ax` (`0x1500bd`–`0x1500c5`). The draft compares
   a `short` with a `signed char`, which C promotes to `int`. A `(short)` cast
   of the byte would fit (inferred).
4. **Store order in the clamping** (lines 1937–1938). In both branches retail
   stores `+0x17e` first, then `+0x17c` through one shared store at
   `0x15009f`:
   - one branch stores at `0x15007b`, then jumps to `0x15009f`;
   - the other stores at `0x150094`, then `0x15009f`.

   The draft assigns `idle` (`+0x17c`) first in both. Writing `rest = 0`
   before `idle = …` in both would fit the shared store (inferred). Retail
   also forms the sum as duration + delay: `lea esi, [eax+ecx]` and
   `add eax, ecx` with `eax` = `L+0x28` and `ecx` = `L+0x20`.
5. **The cap is a conditional move** (line 1947). Inside `if (bl)`, retail
   does `movsx ecx, cx; cmp ecx, edx; cmovg ecx, edx`, then an unconditional
   `mov word ptr [esi+0x17c], cx` (`0x15016a`–`0x150172`). The draft's
   `if (first_activity && idle > first) idle = (short)first;` puts the
   comparison in the condition, which usually becomes a branch around the
   store. A minimum assigned inside `if (first_activity)` would fit
   (inferred).
6. **Store order with a parent or state 2** (line 1956). Retail stores
   `+0x17c`, then `+0x17e` (`0x1501c8`, `0x1501cf`). `idle = rest = 0`
   assigns `rest` first; `idle = 0; rest = 0;` would fit (inferred).
7. **One player pointer for the view clear** (lines 1790, 1796). Retail reads
   `user_index` through the pointer that the `flags_word` store formed, with
   no reload of `g_4e8c24`: `add eax, edi`, then `mov ax, [eax+0x28]`
   (`0x14f927`–`0x14f929`, `0x14f996`–`0x14f998`).
   - The draft's `player_clear_weapon_view` calls `player_get` again after
     the store.
   - Since a store through a pointer can alias the global, the compiler may
     reload `g_4e8c24` there.
   - Keeping the pointer from the flag store for the `user_index` read would
     fit (inferred).
8. **The order of the locals** (line 1760). Retail keeps the unit at `L+0x14`
   and the player at `L+0x18` (`0x14f875`, `0x14f850`). The draft's `locals`
   struct puts `player` before `unit`. If the struct is there to steer the
   layout, the members are in the reverse of retail's order (inferred).
9. **The melee target is a conditional move** (line 1882). Retail sets `edi`
   to NONE before the call (`0x14fd3e`) and, after it, `cmovne edi, esi`
   (`0x14fd48`). That is `target = function_bae20(object, 3) ? object : NONE`;
   the draft's `if (!function_bae20(target, 3)) target = NONE;` may become a
   branch (inferred).
10. **The grenade step looks hand 0's weapon up twice** (lines 1892–1895).
    - **The test.** Retail first tests that hand 0 holds a weapon, comparing
      `weapon_object_indices[slot]` with −1 in memory without loading it
      (`0x14fde2`–`0x14fdfb`).
    - **The second lookup.** It then reloads the headers, the unit and the
      slot, and looks the weapon up again with a NONE default (`or eax, -1`
      at `0x14fe0c`, `0x14fdfd`–`0x14fe18`).
    - **No NONE test after it.** That value goes to the definition lookup
      with no NONE test (`and eax, 0xffff` at `0x14fe22`).

    The draft does one lookup and tests its value
    (`long weapon = player_current_weapon(unit_index, 0); if (weapon != NONE)`).
    That is the form retail uses in the pending-slot step: one lookup, then
    `cmp eax, -1` at `0x14feeb` (`0x14fecc`–`0x14feee`; draft lines
    1910–1911). So the draft's grenade step would likely compile like that
    one.

    A test, then a second lookup for the definition, would fit (inferred).
11. **The melee's first test is two tests** (line 1850). Retail tests bit 5,
    then bit 6:
    - `mov edi, edx; and edi, 0x20; jne 0x14fbe8`, then
      `test dl, 0x40; je 0x14fdd1` (`0x14fbd4`–`0x14fbde`);
    - it keeps `buttons & 0x20` in `edi` for the bit-5 path's `test edi, edi`
      (`0x14fd06`).

    The draft's `if (buttons & 0x60)` is a single mask test.
    `(buttons & 0x20) || (buttons & 0x40)`, with the bit-5 test at line 1876
    reusing the same expression, would fit (inferred).

Two further candidates are weaker, because the compiler may produce either
form:

- **The far flag is kept twice.** Retail keeps it in `bl` (zeroed
  `0x14fc0f`, set `0x14fcc9`, read for the mode at `0x14fd5b`) and in
  `L+0x12` (zeroed `0x14fc14`, set `0x14fccb`, read for the cooldown at
  `0x14fd9a`), although `ebx` survives `function_e6900`. The draft has one
  `lunge`, so two variables, one for the mode and one for the cooldown, may
  be the source's form.
- **The forward flag is zeroed early.** Retail zeroes `L+0x13` at `0x14ff7d`,
  before the `flag18` call and `fpatan`, and sets it at `0x150046` with no
  `else`. The draft computes `moving` as one `&&` expression (line 1933).
  `bool moving = false;` early, then `if (…) moving = true;`, may be the
  source's form.

Checked and not a difference: the draft's `& 0x80` tests (lines 1793 and
1842). Matched code elsewhere compiles `& 0x80` to retail's sign test.

## `0x151ea0`

`0x151ea0` is in lane Q's range: `todo`, 340 bytes. Deep lane 2 drafted it as
`void function_151ea0(long player_index, long object_index)`
(`src/unknown_1502e0.cpp:152`); `0x14f820`'s draft declares it
`void function_151ea0(long, long);` (`src/unknown_14b560.cpp:1721`).

Retail passes the player index and the object index on the stack (`ret 8`).
The `push ecx` at its entry only reserves a local. This function calls it
with the unit's `unknown238` (`0x14fbc5`); its other caller is `0x150e30`
(`0x15102f`). Its steps:

1. It rounds the object definition's real at `+0x130` (seconds) to ticks. It
   returns at once if that is not positive, or if bit 4 of the object
   header's byte `+2` is set.
2. It branches on the definition's short at `+0x12c`, the kind:
   - **Kind 1:** nothing here.
   - **Kind 2:** `function_d6af0` on the player's unit. If that is true,
     `player_speed_request_14ea20`.
   - **Kind 5:** `function_d6790` on the player's unit. If that is true,
     `player_speed_request_14ec00`.
   - **Any other kind:** `function_152000`, with the player in `eax`, an
     effect in `si` (0 for kind 3, 1 otherwise) and the rounded ticks pushed.
     If that is true and the kind is 3, `player_speed_request_14eb10`.
3. If `function_d6af0`, `function_d6790` or `function_152000` returns false,
   it returns at once (`0x151fec`), without deleting the object. In this
   caller, `function_ccd20` deletes it right afterwards anyway (`0x14fbcc`),
   so the object is gone either way.
4. Otherwise, and always for kind 1, it calls:
   - `function_191fab` (the definition in `eax`, `user_index` pushed);
   - `function_a89c0` (the unit in `ecx`, the definition in `edx`);
   - `function_f8110` (`src/equipment.cpp`, the object in `eax`), only when
     the player's `user_index` is not NONE;
   - `function_b8540` (the object pushed), which deletes the object.

## Existing declarations

None of these declarations is missing a parameter. The register conventions
are the compiler's whole-program choices.

| Retail | Declared as | Where | What retail shows at these calls |
| --- | --- | --- | --- |
| `0x151c10` | `bool function_151c10(long player_index, s_target_candidate *candidate)` | `src/unknown_1502e0.cpp:1054` | The player in `eax`, the candidate in `ecx`; `al` |
| `0x151c90` | `bool function_151c90(long player_index, s_target_candidate *candidate)` | `src/unknown_1502e0.cpp:1205` | The player in `eax`, the candidate pushed (`ret 4`) |
| `0x151940` | `bool function_151940(long player_index, s_target_candidate *candidate, bool primary)` | `src/unknown_1502e0.cpp:1153` | The player in `eax`, the candidate in `ecx`, `primary` pushed (`ret 4`) |
| `0x151aa0` | `bool function_151aa0(long player_index, bool primary)` | `src/unknown_1502e0.cpp:1121` | The player in `eax`, `primary` pushed (`ret 4`) |
| `0x151b80` | `bool function_151b80(long player_index, bool primary)` | `src/unknown_1502e0.cpp:1075` | The player in `eax`, `primary` in `cl` |
| `0xa8dd0` | `void function_a8dd0(long index)` | `src/unknown_0a7c50.cpp:514` | The unit pushed (`ret 4`) |
| `0xa9390` | `void function_a9390(long index)` | `src/unknown_0a7c50.cpp:173` | The unit in `esi` |
| `0xc7070` | `bool function_0c7070(long object_index)` | `src/unknown_0c7070.cpp:24` | The unit in `ecx` |
| `0xe68c0` | `bool function_e68c0(long type, long unit_index)` | `src/unknown_0e68c0.cpp:10` | The type in `ecx`, the unit pushed (`ret 4`) |
| `0x1628f0` | `void function_1628f0(long player_index, char state)` | `src/unknown_1600f0.cpp:708` | The player in `eax`, the state in `cl` |
| `0xccd20` | `void function_ccd20(long unit_index)` | `src/unit_object_type.cpp:899` | The unit in `eax` |
| `0x1012c0` | `bool function_1012c0(long weapon_index)` | `src/weapons.cpp:472` | The weapon in `ecx` |
| `0x101300` | `bool function_101300(long weapon_index)` | `src/weapons.cpp:486` | The weapon in `eax` |
| `0x101340` | `bool function_101340(long weapon_index)` | `src/weapons.cpp:496` | The weapon in `ecx` |
| `0x101160` | `bool function_101160(long weapon_index)` | `src/weapons.cpp:431` | The weapon in `eax` |
| `0xbae20` | `s_object *function_bae20(long object_index, dword type_mask)` | `src/unknown_0bad50.cpp:276` | The object in `eax`, the mask (3) pushed (`ret 4`); the result in `eax` |
| `0xb9dd0` | `point3f *function_b9dd0(long object_index, point3f *result)` | `src/unknown_0b58c0.cpp:215` | The object in `eax`, the result in `esi` |
| `0xe6900` | `bool function_e6900(long unit_index, s_unit_request *request)` | `src/unknown_0e6900.cpp:162` | The unit in `eax`, the request in `edi` |
| `0x10f890` | `bool function_10f890(long object_index)` | `src/unknown_10db60.cpp:126` | The unit in `eax` |
| `0xcc3c0` | `bool function_cc3c0(long unit_index)` | `src/unit_object_type.cpp:825` | The unit in `ecx` |
| `0xe4050` | `bool function_e4050(long object_index)` | `src/unknown_0e4050.cpp:32` | The unit in `eax` |

**A declaration for `0x14f820`.** The draft defines it as
`void __stdcall function_14f820(long player_index, long unit_index, byte *input)`
(`src/unknown_14b560.cpp:1758`), without the `standard` marker.

- **Retail's convention:** all three arguments on the stack, with no register
  argument, popped by the callee (`ret 0xc`).
- **Why that may need the marker:** its one caller is LTCG code that pushes
  every argument, and nothing in the image holds its address. If the build
  gives the draft register arguments, the bullet "Standard convention with no
  visible reason" under "Stand-ins: marking functions" in
  [DECOMPILING.md](DECOMPILING.md) may apply. By that bullet's "When it is
  allowed", the marker comes only once the body matches with the standard
  convention and the ordinary idioms have failed.

Only the compiler can tell. A typing of the third argument from the
repository's own structures would be the following; it is offered, not
applied.

```
void __stdcall function_14f820(long player_index, long unit_index,
    s_player_action *action);
```

## Evidence

- **Disassembly.** The function was disassembled with capstone 5.0.7 through
  `tools/xbe.py`.
- **Stack and control flow.** A control-flow graph with a stack-depth walk over
  every path put every stack operand at an offset from L. The depth is
  consistent at every join, and each callee's stack effect was taken from its
  own `ret` in retail.
- **What was read where.** Registers, offsets, flags and constants were read
  from retail code and data. The callees with source were read from it, and
  the calling conventions from each callee's entry in retail.
- **References.** An image-wide scan of every section looked for `call` and
  `jmp` (near, short and conditional) and for absolute dwords at any
  alignment. For `0x14f820` it finds only the call at `0x14d7d7`.
- **Checks.** The map and
  [The draft compared with retail](#the-draft-compared-with-retail) were
  checked against retail and the repository, and the corrections are
  applied.
- **Constants.** These are read from retail `.rdata`, as listed under
  [Constants](#constants).
- **Prior coverage.** No document covered this range before. Source line
  numbers are at `5d1a893`.
- **Outside sources.** No emulator, runtime testing, SDK or outside dataset was
  used. Names are the repository's own, or describe behaviour.
