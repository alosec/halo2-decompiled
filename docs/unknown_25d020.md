# Updating what an actor knows of an object (unknown_25d020)

Retail range covered: `0x25d020`–`0x25d413`. This is one inventory entry,
1,012 retail bytes, `todo`, and `config/functions.csv` gives it no source; its
only body is a disabled draft (`#if 0`) in `src/unknown_25d020.cpp`.
`function_25d020` makes sure that an actor has a prop for an object, and
brings what that prop holds up to date from another actor's prop for the same
object or from the object itself, without the actor perceiving the object
(inferred from its callees' source).
**Analysis only:** this document adds no source, and nothing in it has been
built or checked against retail with the original compiler. Names are
provisional. The `function_<va>` form is primary; the descriptions are offered
for whoever decompiles it.

Arguments are numbered from 1 here (`arg1` to `arg6`). The stub and the draft
number them from 0, so their `arg_4` is `arg5`.

**What it does.**

1. It takes the actor's prop_ref for the object from `arg4`. When `arg4` is
   NONE, `function_25d810` finds the prop_ref or creates one. When there is
   still none, it returns false; every other path returns true.
2. It reads the prop_ref's `state` (`+0x24`):
   - **below 1** (not noticed): `function_25c230` makes the actor notice the
     prop (state 1). The priority it passes is that of the prop_ref in `arg6`,
     or 2. Then steps 3 to 7.
   - **1 or 2** (perceived, or just lost: state 2; inferred): only step 7.
   - **3 or more** (remembered): steps 3 to 7.
3. Steps 3 to 5 run only when `function_25da40` allows them. When step 2 has
   just noticed the prop, `function_25c860` makes it a remembered prop
   (state 3).
4. **With a reference** (`arg6`, another prop_ref; the callers with source
   pass one for the same object): when the reference's state is newer, it
   copies that state and the view's direction, and measures the distance
   again. **Without one:** only when the state's `unknown00` is older than
   `game_time` (in effect once per tick, when `function_264330` refreshes it;
   inferred), it samples the object and runs the perception update `function_264330` with `force` set.
5. With a tracking view, it restarts the view's countdown at 30 seconds' worth of ticks. Unless
   the prop has just been noticed, it then looks at the remembered point. If
   there is none, or the position has moved beyond a radius from it, it:
   - clears the view's last update (`function_25c4e0`);
   - stores the direction of the move;
   - stores the new point.
6. When `arg3` (the type) is 2, `function_25b620` may have the actor's unit
   start event `0xd` (the prop was just noticed) or `0xf` about the object.
7. With a tracking view, it stores the prop's score from `function_265d30`
   at the view's `+0x3c`.

**The states.** State 0 marks a prop that the actor has not noticed, or has
forgotten. States 1 and 2 mark a prop it perceives, 2 just after it stopped
perceiving it. States 3 and 4 mark a prop it remembers. This rests on the code
of the perception loop `function_262ba0` and of `function_2967b0` (see
[The prop_ref](#the-prop_ref)), so it is inferred.

So this function gives an actor knowledge of an object without perception:
- **A new prop** becomes a remembered one at once, when `function_25da40`
  allows it; otherwise it stays in state 1. The perception loop does the same
  only on its gradual-notice path, for a prop whose `unknown27` is below 1
  (`src/unknown_264330.cpp:457`–`458`), not on the direct notice at `:449`.
  Its 2 to 3 step is skipped when the view's `+0xa0` is set and the object is
  within 1.0 (`:484`–`491`). The countdown runs only with a view (`:510`) and
  drains by 0 to 3 a tick (`:512`–`513`), so 30 seconds is only its start.
- **A remembered prop** gets a fresh position and a new countdown before it is
  forgotten.
- **A prop the actor perceives** is left to the perception update.

It uses the names of:

- `include/props.h`:
  - `s_prop_datum` (`tracking_index`, `unknown1a`, `object_index`, `state`,
    `unknown27`, `unknown28`) and `s_prop_node`;
  - `s_type_5cfb45` (`unknown00`, `position`);
  - `s_type_f95cd3` (`unknown3a`, `unknown8e`, `unknown94`, `unknowna2`,
    `unknowna4`);
  - `s_type_e5ff81` (`state`, `view`);
  - `g_470f10` (`kind`, `unknown8`), `prop_ref_get` and `tracking_get`.
- `include/props.h`, for the prop: `s_type_76cf92` (`unknown23`, `unknown25`,
  `unknown32` to `unknown34`).
- `include/slot_handler.h`: `s_prop_node_view` (`view_index`, `unknown24`),
  `s_prop_view_fields` (`unknown3c`) and `s_actor_view` (`unknown004`,
  `unknown07c`, `unknown267`, `unknown268`, `prop_index`).
- `include/unknown_26b230.h`: `s_clump_node` (`state`).
- `include/unknown_25d020.h`: `s_2640c0` and `s_2641c0` (`field_c`).
- `include/globals.h`: `g_510c54` (`s_game_time_globals`: `field_2_3`,
  `game_time`), `g_4687a4`, `g_502414` and `g_502418`
  (`g_prop_pool_storage`).
- `include/data_array.h`: `s_record_pool` (`data`).
- `include/unknown_0259d0.h` and `src/unknown_0259d0.cpp`: `distance3d`,
  `length_sq3f` and `vector3d_from_points3d`.
- `src/unknown_25d690.cpp`:
  - the callees `function_25d810`, `function_25d690`, `function_25c230`,
    `function_25da40`, `function_25c860`, `function_25d740`,
    `function_25c4e0` and `function_25b620`;
  - `function_25c570`, `function_25c820`, `function_25ae40`,
    `function_25ccd0`, `prop_ref_view`, `prop_view_reset` and
    `s_clump_prop_view`;
  - the callers `function_25bcb0`, `function_25bcf0`, `function_25b440` and
    `function_25b6b0`.
- `src/unknown_265cb0.cpp`: `function_2640c0` (`s_object_motion_view`),
  `function_2641c0` and `function_265d30`.
- `src/unknown_264330.cpp`: `function_264330`, `function_262ba0` and the
  caller `function_265680`.
- `src/unknown_030290.cpp`: `function_30bf0`.
- `src/ai.cpp` and `src/unknown_1469f0.cpp`: `ai_seconds_to_ticks` and
  `function_1469f0`.
- `src/unknown_050650.cpp`: `function_2967b0`.
- `src/unknown_1fb7e0.cpp`: `function_1fb7e0`.
- For the other callers: `src/unknown_1e3920.cpp` (`function_1e2930`),
  `src/unknown_1fb940.cpp` (`function_1fc160`, `function_1fc210` and
  `function_1fbff0`) and `src/unknown_28fe50.cpp` (`function_28fe50`).

## Boundary

- **Before:** `0x25ccd0` (`function_25ccd0`, 841 bytes, `todo`,
  `src/unknown_25d690.cpp:1287`) is excluded. It ends with `ret 0xc` at
  `0x25d016`, and `int3` padding follows up to `0x25d020`.
  `function_25c230` calls it.
- **After:** the `ret 0x18` at `0x25d411` ends the entry, and 12 bytes of
  `int3` padding follow. The next entry is `0x25d420` (`function_25d420`,
  226 bytes, `matched`, `src/unknown_25d690.cpp:507`), which is excluded.
- **Lanes:** lane I's row of the Active claims table covers the function.
  That row (issue #9) is `0x250000`–`0x25ffff`, except the UI screens at
  `0x250155`–`0x2541b2`, for the maintainers' Codex lane on the second
  machine. No `@user` row covers it. Lane I's last round on main is deep
  round 10 (#301, 2026-10-10). This document makes no claim and is offered
  to lane I.
- **Open pull requests:** none touches it. On 2026-10-11 there were nine
  open: #321 and #344 to #351. None of their diffs mentions `0x25d020` or
  `function_25d020`, and #321 changes no files.
- **Existing source:** `0x25d020` has no `@retail` marker at `2bd2fc7d`. It
  has a stub, the disabled draft and five declarations. See
  [Existing declarations](#existing-declarations) and
  [The draft compared with retail](#the-draft-compared-with-retail).
- **Likely home:** `src/unknown_25d020.cpp`, which holds the draft and its
  helpers (inferred).

## Conventions

"Callers" counts the functions that call an entry directly, from outside the
range and then from inside it. The ten callers make 13 calls.

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x25d020` | 1012 | stack: actor index, object index, type, prop_ref index, unused, reference prop_ref index (`ret 0x18`) | bool | 10 + 0 | Brings an actor's prop for an object up to date, without perception |

**The frame.** There is no frame pointer.
- It starts with `sub esp, 0x7c` (`0x25d020`), then pushes `ebx`
  (`0x25d02a`) and `edi` (`0x25d030`).
- It pushes `ebp` (`0x25d098`) and `esi` (`0x25d0ab`) only after the lookup
  that can fail. So the early exit pops only `edi` and `ebx`, and the main
  path pops `esi` and `ebp` first (`0x25d3e2`, `0x25d3e3`).

Below, L is `esp` after `sub esp, 0x7c`, that is, the entry `esp` − 0x7c. The
arguments are at L+0x80 to L+0x94. A slot's raw `esp` displacement changes
with the pushes, but its L-offset does not. For example, `arg4` is
`[esp + 0x94]` at `0x25d060` and `[esp + 0x9c]` at `0x25d0f8`, both L+0x8c.

| Argument | Slot | Name at `src/unknown_1e3920.cpp:1115` | Use here |
| --- | --- | --- | --- |
| `arg1` | L+0x80 | `actor_index` | Read at `0x25d06a`, `0x25d0ff`, `0x25d1c6`, `0x25d261`, `0x25d3d3` and `0x25d3ed` |
| `arg2` | L+0x84 | `object_index` | Read once, for `function_25d810` (`0x25d071`) |
| `arg3` | L+0x88 | `type` | Read once, as a word (`0x25d3bb`) |
| `arg4` | L+0x8c | `prop_index` | The prop_ref. `function_25d810`'s result replaces a NONE (`0x25d083`) |
| `arg5` | L+0x90 | `unknown` | Never read |
| `arg6` | L+0x94 | `reference_index` | Read before the pushes (`0x25d023`) and at `0x25d168` |

**Locals.**

| Local | Type | Use |
| --- | --- | --- |
| L+0x0 | `s_type_5cfb45 *` | The prop's state (`0x25d0bd`, `0x25d13a`). `esi` holds it too and is reloaded from here (`0x25d1c2`, `0x25d1ec`). The countdown's `fistp` reuses the slot (`0x25d218`) |
| L+0x4 | bool | Set when this call noticed the prop (`0x25d13e`). It is cleared from `bl` (`0x25d03e`), and loaded as a dword and pushed for `function_25b620` (`0x25d3c8`, `0x25d3da`) |
| L+0x8 | `s_prop_datum *` | The reference's datum, or NULL (`0x25d031`, `0x25d05c`). Reused as a float temporary (`0x25d20e`, `0x25d2b3`) |
| L+0xc | 0x38 bytes | On the reference path, `function_2641c0`'s sample (`0x25d1d1`), whose `field_c` is L+0x18 (`0x25d1e0`). On the other path, `function_2640c0`'s result (`0x25d251`) |
| L+0x44 | `s_2641c0` | The sample for `function_264330` (`0x25d26c`, `0x25d28d`) |

**Registers.**
- `ebx`: first `bl` = 0 (`0x25d02b`), the early exit's result. From
  `0x25d0b8` on, the view (`s_type_f95cd3 *`) or NULL.
- `edi`: the priority for `function_25c230`. It is 2 (`0x25d039`), or the
  reference's `unknown1a` in the low word (`0x25d055`). Later it is the
  `rep movsd` destination (`0x25d18b`), `function_2640c0`'s result
  (`0x25d251`) and the actor index (`0x25d261`).
- `ebp`: the prop_ref's datum (`0x25d099`–`0x25d0a9`).
- `esi`: the prop's state.

**Register inputs:** there are none. `eax` is written first (`0x25d023`), and
`ecx` and `edx` are written before they are read on every path. `ebx`, `edi`,
`ebp` and `esi` are saved.

**Return value:** a bool in `al`.
- The main exit sets `al` = 1 (`0x25d401`).
- The early exit returns `bl` (`0x25d40b`). It is still 0 there: that exit is
  reached only from `0x25d08a`, before `ebx` is reused at `0x25d0b8`.
- Two callers read it: `0x25b440` (`test al, al` at `0x25b50e`) and
  `0x25b6b0` (`0x25b7f6`).

**The callers.** Every call pushes six arguments. The stack-depth walk of each
of the ten callers agrees with `0x25d020` popping 0x18. The constants below are
retail's pushes.

| Caller | Status | Calls | `arg3` | `arg4` | `arg5`, `arg6` | The call in the source |
| --- | --- | --- | --- | --- | --- | --- |
| `0x1b47b0` | `todo`, no source | `0x1b4a4e`, `0x1b4b50` | NONE | a register | registers | |
| `0x1e2930` | `matched` | `0x1e297a` | 4 | NONE | NONE | `src/unknown_1e3920.cpp:1345` |
| `0x1fc160` | `todo` | `0x1fc1b8` | NONE | NONE | NONE | `src/unknown_1fb940.cpp:100` |
| `0x1fc210` | `matched` | `0x1fc293` | 4 | the prop_ref | NONE | `src/unknown_1fb940.cpp:144` |
| `0x25b440` | `todo` | `0x25b509` | 0 | the prop_ref | NONE | `src/unknown_25d690.cpp:1456` |
| `0x25b6b0` | `todo` | `0x25b7f1` | 0 | the prop_ref | NONE | `src/unknown_25d690.cpp:1506` |
| `0x25bcb0` | `matched` | `0x25bcbd` | 2 | NONE | NONE | `src/unknown_25d690.cpp:332` |
| `0x25bcf0` | `todo` | `0x25bd67`, `0x25be8b`, `0x25bef2` | 2 | the prop_ref, NONE, NONE | NONE twice, then another actor and its `prop_index` | `src/unknown_25d690.cpp:1665`, `:1690`, `:1701` |
| `0x265680` | `todo` | `0x265b49` | `esi` (0 in the source) | the prop_ref | NONE | `src/unknown_264330.cpp:136` |
| `0x28fe50` | `matched` | `0x28ff48` | NONE | NONE | another actor and its `prop_index` | `src/unknown_28fe50.cpp:96` |

## Data

### The prop_ref

The prop_ref is `ebp`: `prop_ref_get(arg4)`. That is the `data` of
`g_502418` (`+0x44` of `s_record_pool`) plus (`arg4` & 0xffff) × 0x3c
(`0x25d090`–`0x25d0a9`). The reference's datum is found the same way
(`0x25d044`–`0x25d05a`).

The element is `s_prop_datum` (`include/props.h:122`). Two other structs view
the same 0x3c bytes: `s_prop_node_view` (`include/slot_handler.h:322`) and
`s_clump_node` (`include/unknown_26b230.h:38`).

| Offset | `s_prop_datum` | `s_prop_node_view` | Use here |
| --- | --- | --- | --- |
| `+0x14` | `tracking_index` | `view_index` | The tracking element, or NONE (`0x25d0b5`, `0x25d10e`) |
| `+0x1a` | `unknown1a` | inside `unknown1a[6]` | Read from the reference only: the priority for `function_25c230` (`0x25d055`) |
| `+0x20` | `object_index` | `object_index` | `function_2640c0`'s object (`0x25d24d`) |
| `+0x24` | `state` | `unknown24` | The branch (`0x25d0de`). `s_clump_node` also calls it `state` |
| `+0x28` | `unknown28` | `unknown28` | The distance. It is written at `0x25d1e9`, and read for the radius at `0x25d230` and `0x25d29e` |

**`unknown1a` is a priority** (inferred).
- The matched `function_25c230` raises it to the largest of itself, the
  prop's word `+0xc` and its own third argument
  (`src/unknown_25d690.cpp:1399`–`1401`).
- The matched `function_25c570` compares it with its parameter `priority` when
  it chooses a tracking slot to take over (`:860`–`862`).
- `function_262ba0` sets it to 3 for the actor's current prop
  (`src/unknown_264330.cpp:426`).

**`unknown28` is a distance.** `function_264330` sets it to the distance from
the sample's `field_c` to the state's position
(`src/unknown_264330.cpp:301`, `:307`). This function does the same with
`distance3d` (`0x25d1e4`).

**`state`.** `function_262ba0` (`src/unknown_264330.cpp:394`, `todo`) is the
update an actor runs over its prop_refs. It switches on `state` (`:444`;
`movsx eax, word ptr [ebp + 0x24]` at `0x262d5c` in retail):
- **0** becomes 1 through `function_25c230` once the prop is noticed
  (`:446`–`463`). When `unknown27` is below 1 and `function_25da40` allows
  it, `function_25c860` then makes it 3 at once (`:457`–`458`).
- **1** becomes 2 when `unknown27` is 0, and `function_25d610` and
  `function_25da40` allow it (`:464`–`475`; `0x262f63` in retail).
- **2** becomes 1 again through `function_25c230` when `unknown27` is above 0,
  or 3 through `function_25c860` (`:476`–`493`).
- **3** becomes 4 when `unknown27` is above 0 (`0x263042`). Otherwise the
  view's `unknown8e` counts down (`sub word ptr [edi + 0x8e], ax` at
  `0x263080`). Below 0 the prop is forgotten: `function_25c820` sets the state
  to 0 (`0x26309f`; `src/unknown_25d690.cpp:482`–`492`).
- **4** becomes 3 through `function_25c860` when `unknown27` is 0, or 1
  through `function_25c230` once the level at `+0x10` reaches 1.0
  (`src/unknown_264330.cpp:517`–`524`).
- States 1 to 3 also become 0 through `function_25c820` when `function_25da40`
  fails (`src/unknown_264330.cpp:473`, `:477`, `:495`).

`function_2967b0` (`src/unknown_050650.cpp:573`, `todo`) uses the object
itself for states 1 and 2 (`:604`–`620`), and the remembered prop for 3 and
more (`goto remembered_prop`, `:621`). The meanings given in **The states**,
at the top, follow from these (inferred).

### The prop's state (`s_type_5cfb45`, `include/props.h:20`)

`function_25d690` takes the datum in `ecx` and returns a state
(`src/unknown_25d690.cpp:610`–`621`):
- the prop's own state, when the type's `kind` in `g_470f10` is 1;
- otherwise the tracking element's state, when there is a tracking;
- otherwise the prop's.

The state is 0x6c bytes.

| Offset | Name | Use here |
| --- | --- | --- |
| `+0` | `unknown00` | The game time of the last update: `function_25ccd0` stores `game_time` there (`src/unknown_25d690.cpp:1386`). It is compared with the reference's (`0x25d183`–`0x25d187`) and with the current time (`0x25d246`–`0x25d24b`) |
| `+4` | `position` | The point for `function_2641c0` and `distance3d`, the radius test and the remembered point |
| all | | Overwritten with the reference's state: `rep movsd` of 0x1b dwords (`0x25d18f`–`0x25d196`) |

### The tracking element and its view (`s_type_e5ff81`, `include/props.h:150`)

The view is `ebx`. It is the element at `g_502414`'s data plus
(`tracking_index` & 0xffff) × 0x124, plus 0x70: `lea ebx, [eax + 0x70]` at
`0x25d0db` and `0x25d130`. It is NULL when `tracking_index` is NONE or the
element's address is 0.

That lookup is the inline `prop_ref_view` (`src/unknown_25d690.cpp:77`), the
same lookup as the matched `function_25d740`. For the reference's view, the
function calls `function_25d740` instead (`0x25d19e`).

| Offset | `s_type_f95cd3` | Use here |
| --- | --- | --- |
| `+0x3c` | inside `unknown3a[]`; `s_prop_view_fields` calls it `unknown3c` (`include/slot_handler.h:352`) | The score from `function_265d30` (`0x25d3fb`) |
| `+0x8e` | `unknown8e` (short) | The countdown: (short) round(`field_2_3` × 30.0) (`0x25d1f8`–`0x25d229`). That is 30 seconds in ticks, the shape of `ai_seconds_to_ticks(30.0f)` ("game ticks in the given number of seconds, rounded", `src/ai.cpp:896`); `function_1469f0` does the same out of line (`src/unknown_1469f0.cpp:9`) |
| `+0x94` | `unknown94` (`vector3f`) | The direction of the last move: the reference view's (`0x25d1a7`–`0x25d1bf`), the move normalised (`0x25d33d`–`0x25d378`), or the vector at `g_4687a4` (`0x25d37f`–`0x25d398`) |
| `+0xa2` | `unknowna2` (bool) | Set when `unknowna4` holds a point (`0x25d3b4`) |
| `+0xa4` | `unknowna4` (`point3f`) | The remembered point (`0x25d39b`–`0x25d3b1`) |

`function_25c860` gives these fields their first values when it makes a prop
remembered (`src/unknown_25d690.cpp:937`–`966`).

### Globals and constants

| Where | Repository name | Use here |
| --- | --- | --- |
| `0x502418` | `g_502418`, defined as `g_prop_pool_storage.field_2418` (`include/globals.h:1288`); the "prop_ref" array (`include/props.h:6`) | The prop_refs (`0x25d044`, `0x25d090`) |
| `0x502414` | `g_502414` (`s_record_pool *`, `include/globals.h:754`); the "tracking" array (`include/props.h:8`) | The tracking elements (`0x25d0c3`, `0x25d118`) |
| `0x510c54` | `g_510c54` (`s_game_time_globals *`, `include/globals.h:27`) | `field_2_3` (`+2`, a short) for the countdown (`0x25d1fe`); `game_time` (`+8`) against the state's `unknown00` (`0x25d248`) |
| `0x4687a4` | `g_4687a4` (`vector3f *`, `include/globals.h:423`, "a default vector") | It points to (0, 0, 0) at `0x440af4`, which is copied to `unknown94` (`0x25d37f`) |
| `0x45dcfc` | none | 30.0, the countdown's seconds (`0x25d206`) |
| `0x45dc00` | none | 10.0, where the radius starts to grow (`0x25d221`, `0x25d2a1`) |
| `0x43ff08` | none | 1.5, the radius below 10 (`0x25d236`) |
| `0x45dd90` | none | The double 0.5 (`0x25d2a7`) |
| `0x45e1d8` | none | The double 1.5 (`0x25d2ad`) |

So the radius is 1.5 when 10.0 > `unknown28` (`comiss` at `0x25d230`).
Otherwise it is (`unknown28` − 10.0) × 0.5 + 1.5, computed on the x87 and
stored as a float (`0x25d29e`–`0x25d2b7`).

## The function

**Control flow.**
- It has 46 basic blocks and no jump table.
- It has no loop. The part at `0x25d240`–`0x25d299` is placed after `0x25d1f0`
  and jumps back to it, but nothing leads from there back to `0x25d240`.
- It returns at two exits, each a `ret 0x18`: at `0x25d407` and at
  `0x25d411`.
- A stack-depth walk takes each direct callee's stack effect from its own
  `ret` in retail. It finds one depth at every join, and depth 0 at both
  `ret`s.
- The fall-through from `0x25d0f2` to `0x25d0f8` is never taken. That block is
  reached only with the state above 2, and it jumps when the state is at
  least 1.

### Setup (`0x25d020`–`0x25d05c`)

1. It reads `arg6` before the pushes (`0x25d023`). On both paths it sets
   `bl` = 0 (`0x25d02b`), L+8 = 0 (`0x25d031`), `edi` = 2 (`0x25d039`) and
   L+4 = 0 (`0x25d03e`).
2. When `arg6` is not NONE, L+8 = its datum and `di` = its `unknown1a`
   (`0x25d044`–`0x25d05c`). Only the low word changes, so `edi`'s high word
   stays 0.

### The prop_ref (`0x25d060`–`0x25d08a`)

3. When `arg4` is NONE, it calls `function_25d810(arg2, arg1, true)`. The
   object index goes in `eax`; the actor index and 1 are pushed
   (`0x25d06a`–`0x25d07b`; `ret 8`).
   - Its source (`matched`) finds the actor's prop_ref for the object, or
     creates one (`src/unknown_25d690.cpp:1025`–`1062`).
   - It returns NONE when the object is NONE, when the object's actor is this
     actor, or when there is no prop_ref and none can be created.
   - The result replaces `arg4` (`0x25d083`). When it is NONE, the function
     returns false (`0x25d08a`).

### The datum, its state and its view (`0x25d090`–`0x25d0db`)

4. `ebp` = the datum (`0x25d099`–`0x25d0a9`), between the pushes of `ebp` and
   `esi`.
5. `esi` and L+0 = `function_25d690(ebp)` (`ecx`, `0x25d0ae`).
6. `ebx` = the view (`0x25d0b5`–`0x25d0db`).

### The state (`0x25d0de`–`0x25d0f6`)

7. It reads `state` as a word (`0x25d0de`):
   - below 1, it goes to `0x25d0f8`;
   - 1 or 2, to `0x25d3e0`, which skips everything but the score;
   - above 2, to `0x25d145`.

### Noticing the prop (`0x25d0f8`–`0x25d143`)

8. It calls `function_25c230(arg1, arg4, edi)`, with all three pushed
   (`0x25d106`–`0x25d109`; `ret 0xc`). The source (`matched`,
   `src/unknown_25d690.cpp:1394`–`1423`):
   - copies the prop's type (its word `+2`) to the datum, raises `unknown1a`
     to the priority, and sets the state to 1;
   - when the type's `unknown8` in `g_470f10` is 2 and there is no tracking,
     starts one with `function_25c570`;
   - when the state was not 1 or 2, makes a first perception update
     (`function_2640c0`, `function_2641c0`, and `function_264330` with
     `force` clear), then calls `function_25ae40` with whether the state was 0.
9. A tracking may have started, so it looks the view up again
   (`0x25d10e`–`0x25d130`). It calls `function_25d690` again (`0x25d135`), and
   L+0 and `esi` take the result. Then it sets L+4 = 1 (`0x25d13e`).

### The update (`0x25d145`–`0x25d3b4`)

10. It calls `function_25da40(ebp)` (`eax`, `0x25d147`). When that is false, it
    goes to the type test at `0x25d3bb`. The source (`todo`,
    `src/unknown_25d690.cpp:704`–`727`) makes the same tests as retail at
    `0x25da40`:
    - with a tracking, it is false only when the prop's actor has
      `unknown004` = 15, or has `unknown267` set and `unknown268` clear;
    - without one, it is true when the prop's `unknown25` is set and its
      `unknown23` clear.
11. When L+4 is set, it calls `function_25c860(arg4)` (`eax`,
    `0x25d15c`–`0x25d163`). That sets the state to 3. For a view whose prop
    was below 3, it also gives the view its first values (see
    [Data](#data)).
12. **With a reference** (`arg6` not NONE, `0x25d168`):
    1. It takes the reference's state, `function_25d690(L+8)` (`ecx`,
       `0x25d17a`). It goes on only when that is not `esi` and its
       `unknown00` is greater (`0x25d17f`–`0x25d187`).
    2. It copies the reference's state over the prop's
       (`0x25d189`–`0x25d196`).
    3. With a view (`test ebx, ebx` at `0x25d189`, `je` at `0x25d198`), it
       calls `function_25d740(L+8)` (`ecx`, `0x25d19e`). When that returns a
       view, it copies the view's `unknown94` (`0x25d1a7`–`0x25d1bf`).
    4. It calls `function_2641c0(arg1, &L+0xc, &position)`
       (`0x25d1c2`–`0x25d1d5`; `ret 4`). The actor index goes in `eax`, the
       sample in `ecx`, and the point is pushed.
    5. When that is true, `unknown28` = `distance3d(&position, &L+0x18)`,
       with `a` in `eax` and `b` in `ecx` (`0x25d1de`–`0x25d1e4`). The result
       is stored by `fstp` (`0x25d1e9`).
13. **Without a reference** (`0x25d240`), it goes on only when the state's
    `unknown00` is below `game_time` (`0x25d246`–`0x25d24b`):
    1. It calls `function_2640c0(object_index, &L+0xc)`, with the object
       pushed and the result in `edi` (`0x25d24d`–`0x25d255`; `ret 4`).
    2. It calls `function_2641c0(arg1, &L+0x44,
       &function_25d690(ebp)->position)` (`0x25d25a`–`0x25d272`).
    3. When that is true, it calls `function_264330(arg1, arg4, &L+0x44,
       &L+0xc, true)`, with all five pushed (`0x25d27f`–`0x25d294`;
       `ret 0x14`).

    In the source of `function_264330`, `force` sets its `update` flag
    (`src/unknown_264330.cpp:267`–`270`). With that flag it refreshes the
    state from the object through `function_25ccd0`, when it has a state to
    refresh (`:286`–`294`). It then sets `unknown28`, and, with a view, the
    direction to the prop. It also stores the score in the view (`:365`).
    Both paths go on at `0x25d1f0`.
14. **The view** (`0x25d1f0`–`0x25d3b4`). When there is none, it goes to
    `0x25d3bb`.
    1. `unknown8e` = the countdown (`0x25d1f8`–`0x25d229`): `cvtsi2ss` of
       `field_2_3`, `mulss` by 30.0, then `fld` and `fistp` through L+8 and
       L+0.
    2. It puts the radius in `xmm3` (`0x25d221`–`0x25d2b7`; see
       [Globals and constants](#globals-and-constants)).
    3. When L+4 is set, it goes to `0x25d3bb` (`0x25d2bd`–`0x25d2c3`).
    4. When `unknowna2` is set and the squared distance from the position to
       `unknowna4` is at most the radius squared, it goes to `0x25d3bb`
       (`0x25d2c9`–`0x25d321`). The sum is (k² + j²) + i²
       (`0x25d2fa`–`0x25d316`).
    5. It calls `function_25c4e0(arg4)` (`eax`, `0x25d327`–`0x25d32e`). The
       source (`matched`, `src/unknown_25d690.cpp:438`–`452`) does two
       things. It clears what the view remembers of its last update
       (`prop_view_reset`, `:92`). It also clears the prop's `unknown32` to
       `unknown34`.
    6. With `unknowna2` set, `unknown94` = the position − `unknowna4`
       (`0x25d33d`–`0x25d373`). `function_30bf0` normalises it (`eax` =
       `&unknown94`, `0x25d378`), and its result is not read. Otherwise
       `unknown94` = the vector at `g_4687a4` (`0x25d37f`–`0x25d398`).
    7. `unknowna4` = the position and `unknowna2` = 1
       (`0x25d39b`–`0x25d3b4`).

### The type (`0x25d3bb`–`0x25d3db`)

15. When (short) `arg3` is 2 (`movsx`, `sub eax, 2` and `jne` at
    `0x25d3bb`–`0x25d3c6`), it calls `function_25b620(arg4, arg1, L+4)`
    (`0x25d3c8`–`0x25d3db`; `ret 4`). The prop_ref goes in `eax`, the actor in
    `ecx`, and the bool is pushed.
    - The source (`matched`, `src/unknown_25d690.cpp:904`–`920`) acts only
      when the actor's `unknown07c` is not NONE and that clump's `unknown30`
      (`s_clump_prop_view`) is clear.
    - It then calls `function_1fb7e0(0xd, …)` when the bool is set and
      `function_1fb7e0(0xf, …)` otherwise, with the prop's object as the
      target.
    - The comment at the top of `function_1fb7e0`'s file describes the file
      as "events of an actor's unit" (`src/unknown_1fb7e0.cpp:2`).

    States 1 and 2 skip this step.

### The score and the exits (`0x25d3e0`–`0x25d413`)

16. It pops `esi` and `ebp` (`0x25d3e2`, `0x25d3e3`). With a view, view
    `+0x3c` = `function_265d30(arg1, arg4)`, with both pushed
    (`0x25d3e6`–`0x25d3f6`; `ret 8`).
    - The real comes back in `xmm0`, which `0x25d3fb` stores.
      `0x265d30` sets `xmm0` before each of its `ret 8`s (for example
      `xorps xmm0, xmm0` at `0x2662dc`).
    - Its source (`todo`) adds up range, state, distance and other scores
      (`src/unknown_265cb0.cpp:98`–`204`).
17. The main exit: `pop edi`, `al` = 1, `pop ebx`, `add esp, 0x7c` and
    `ret 0x18` (`0x25d400`–`0x25d407`).
18. The early exit: `pop edi`, `al` = `bl`, `pop ebx`, `add esp, 0x7c` and
    `ret 0x18` (`0x25d40a`–`0x25d413`).

## The draft compared with retail

**The draft.**
- It is at `src/unknown_25d020.cpp:42`–`138`, inside `#if 0` (lines 4–139).
  It came with Codex lane I's round 2 (`702c455e`, merged by #82).
- It carries `// pending 0x25d020` (line 41) instead of an `@retail` marker.
  No other source file has that comment.
- The comment at the top of its file (lines 2–3) says why it is disabled: LTCG
  drops the unused `arg_4` (`arg5` here) and breaks the matched caller
  `0x25bcb0`.

**The body.** Each statement of the draft has its counterpart in retail, with
retail's control flow, and its constants are retail's. One placement differs:
the draft computes the vector from the position to `unknowna4` (line 111)
before it tests `unknowna2`, while retail subtracts only after that test
(`0x25d2c9`, then `0x25d2d3`). An optimiser can sink the computation there
(inferred). Several of the draft's forms fit retail's code (inferred,
untested):
- The early exit returns the local `bool local_0 = false;` (lines 44 and 57).
  That fits `mov al, bl` (`0x25d40b`), with the zero kept in `bl`. L+4 is
  cleared from the same `bl` (`0x25d03e`).
- `switch ((short)arg_2) { case 2: … }` (lines 128–133) fits
  `movsx eax, word ptr …; sub eax, 2; jne` (`0x25d3bb`–`0x25d3c6`).
- `props_ticks_round` (lines 29–39) fits the `fld` and `fistp` through two
  stack slots (`0x25d214`–`0x25d21c`). The matched `function_25c860` uses the
  same form (`src/unknown_25d690.cpp:944`–`950`).
- `(local_4->unknown28 - 10.f) * 0.5 + 1.5` (line 107) has the double
  constants of `fmul qword` and `fadd qword` (`0x25d2a7`, `0x25d2ad`).
- The draft uses the inline `prop_ref_view` for the datum (lines 61 and 67)
  and a call to `function_25d740` for the reference (line 83). Retail does the
  same (`0x25d0b5`–`0x25d0db`, `0x25d10e`–`0x25d130`, `0x25d19e`).
- On the path without a reference, the draft calls `function_25d690` a second
  time (line 97), as retail does at `0x25d25c`.

**One difference.** For the radius test the draft calls `length_sq3f`
(line 112), which adds `i*i + j*j + k*k` (`include/unknown_0259d0.h:109`–`112`).
Retail adds (k² + j²) + i² (`0x25d2fa`–`0x25d316`).
- The repository found that this order decides the match for `distance3d`
  (`src/unknown_0259d0.cpp:5`–`6`).
- `distance3d`'s source adds k first (`:38`), and so do neighbouring sources,
  such as `src/unknown_265cb0.cpp:414`.
- Writing the sum as `k*k + j*j + i*i` would fit (inferred, untested).

**The convention.** Retail keeps the standard convention: the callers push all
six arguments, and the function pops them (`ret 0x18`), although it never
reads the fifth.
- The image-wide scan finds no reference to the function other than the 13
  calls.
- All ten callers are game code (`owner` `game`), which the repository builds
  with `/GL` (`docs/DECOMPILING.md:46`).
- That is the case of the last-resort `standard` marker in
  `docs/DECOMPILING.md:165`, which needs three items of evidence
  (`:167`–`170`). Retail gives the second: nothing holds the address, and the
  callers push every argument. The other two items need a build.
- The matched callers `0x1e2930`, `0x1fc210`, `0x25bcb0` and `0x28fe50`
  call the stub for now. Stubs are built without `/GL`, so they keep the
  standard convention (`docs/DECOMPILING.md:59`–`61`).

## Callees without source

None. All fourteen callees have source. Ten are `matched`, and
`function_25da40`, `function_2641c0`, `function_264330` and `function_265d30`
are `todo`. [Existing declarations](#existing-declarations) gives each one's
declaration.

## Existing declarations

**Of `0x25d020`.** All seven give it six `long` parameters and a `bool`
result, which agrees with retail. They disagree on the names.

| Kind | Where | Text |
| --- | --- | --- |
| Stub | `src/stubs/lane_i.cpp:12` | `bool __stdcall function_25d020(long arg_0, long arg_1, long arg_2, long arg_3, long arg_4, long arg_5)` |
| Disabled draft | `src/unknown_25d020.cpp:42` | The same as the stub |
| Declaration | `src/unknown_1e3920.cpp:1115` | `bool __stdcall function_25d020(long actor_index, long object_index, long type, long prop_index, long unknown, long reference_index);` |
| Declaration | `src/unknown_1fb940.cpp:49` | `bool __stdcall function_25d020(long actor_index, long object_index, long type, long prop_index, long unknown, long unknown2);` |
| Declaration | `src/unknown_264330.cpp:36` | `bool __stdcall function_25d020(long actor_index, long object_index, long type, long prop_index, long a, long b);` |
| Declaration | `src/unknown_25d690.cpp:60` | `bool __stdcall function_25d020(long actor_index, long prop_ref_index, long a, long b, long c, long d);` |
| Declaration | `src/unknown_28fe50.cpp:61` | `bool __stdcall function_25d020(long arg_0, long arg_1, long arg_2, long arg_3, long arg_4, long arg_5);` |

`include/unknown_25d020.h` declares none of them. It holds `s_2640c0`,
`s_2641c0` and the declarations of `function_2641c0`, `function_264330` and
`function_265d30`.

**The callees.** Each is declared with as many parameters as retail passes.
The draft declares seven of them again itself
(`src/unknown_25d020.cpp:7`–`13`), with the same types.

| Retail | Declared as | Where | Status | Retail at this function's calls |
| --- | --- | --- | --- | --- |
| `0x25d810` | `long function_25d810(long object_index, long actor_index, bool create)` | `src/unknown_25d690.cpp:1025` | `matched` | The object in `eax`; the actor and `create` pushed (`ret 8`) |
| `0x25d690` | `s_type_5cfb45 *function_25d690(s_prop_datum *datum)` | `src/unknown_25d690.cpp:610`; `include/props.h:209` | `matched` | The datum in `ecx` (`0x25d0ae`, `0x25d135`, `0x25d17a`, `0x25d25c`) |
| `0x25c230` | `bool __stdcall function_25c230(long arg_0, long arg_1, short arg_2)`, declared at `:1064` as `(long actor_index, long prop_ref_index, short unknown)` | `src/unknown_25d690.cpp:1394` | `matched` | All three pushed (`ret 0xc`); the result is not read |
| `0x25da40` | `bool function_25da40(s_prop_datum *datum)` | `src/unknown_25d690.cpp:704` | `todo` | The datum in `eax` |
| `0x25c860` | `void function_25c860(long prop_ref_index)` | `src/unknown_25d690.cpp:923` | `matched` | `eax` |
| `0x25d740` | `s_type_f95cd3 *function_25d740(s_prop_node *node)` | `src/unknown_25d690.cpp:641`; `include/props.h:211` | `matched` | `ecx` |
| `0x2641c0` | `bool __stdcall function_2641c0(long actor_index, s_2641c0 *sample, point3f const *point)` | `src/unknown_265cb0.cpp:1728`; `include/unknown_25d020.h:23` with `arg_N` names | `todo` | The actor in `eax`, the sample in `ecx`, the point pushed (`ret 4`) |
| `0x3ea30` | `real distance3d(point3f const *a, point3f const *b)` | `src/unknown_0259d0.cpp:32`; `include/unknown_0259d0.h:151` | `matched` | `a` in `eax`, `b` in `ecx`; the result on the x87 stack |
| `0x2640c0` | `void function_2640c0(long object_index, s_object_motion_view *result)` | `src/unknown_265cb0.cpp:39` | `matched` | The object pushed, `result` in `edi` (`ret 4`) |
| `0x264330` | `void __stdcall function_264330(long actor_index, long prop_ref_index, s_2641c0 *context, s_2640c0 *motion, bool force)` | `src/unknown_264330.cpp:257`; `include/unknown_25d020.h:24` with `arg_N` names | `todo` | All five pushed (`ret 0x14`) |
| `0x25c4e0` | `void function_25c4e0(long prop_ref_index)` | `src/unknown_25d690.cpp:438` | `matched` | `eax` |
| `0x30bf0` | `inline real function_30bf0(vector3f *v)`, with a `__declspec(noinline)` declaration at `:284` | `src/unknown_030290.cpp:286` | `matched` | `v` in `eax`; the result is not read |
| `0x25b620` | `void function_25b620(long prop_ref_index, long actor_index, bool unknown)` | `src/unknown_25d690.cpp:904` | `matched` | The prop_ref in `eax`, the actor in `ecx`, the bool pushed (`ret 4`) |
| `0x265d30` | `real __stdcall function_265d30(long actor_index, long prop_index)` | `src/unknown_265cb0.cpp:98`; `include/unknown_25d020.h:25` | `todo` | Both pushed (`ret 8`); the result in `xmm0` |

## Findings about existing names

- **The second parameter is an object index.**
  - `src/unknown_25d690.cpp:60` names it `prop_ref_index`. But retail passes it
    only to `function_25d810`, as its `object_index` (`0x25d071`).
  - The same file's `function_25bcb0` also names its own second parameter
    `prop_ref_index` (`:330`), and passes it on as this one (`:332`).
  - Its only caller passes an object index. `function_1fbff0` passes its
    `object_index` (`src/unknown_1fb940.cpp:220`). In retail, `0x1fc11f` loads
    `0x1fbff0`'s second stack argument, which is that `object_index`.
  - The declaration of `function_25bcb0` at `src/unknown_1fb940.cpp:163` calls
    it `prop_index`.
- **The fourth parameter is a prop_ref index.**
  - Three declarations call it `prop_index` (`src/unknown_1e3920.cpp:1115`,
    `src/unknown_1fb940.cpp:49`, `src/unknown_264330.cpp:36`).
  - Retail indexes `g_502418`, the prop_refs, with it
    (`0x25d099`–`0x25d0a9`). `s_prop_datum`'s own `prop_index` (`+8`) indexes
    `g_50241c`, the props.
  - `function_265d30` also calls its prop_ref parameter `prop_index`
    (`src/unknown_265cb0.cpp:98`), and reads it with `prop_ref_get`.
- **The fifth and sixth parameters.** Two callers with source pass something
  other than NONE in them: `0x25bcf0` (`src/unknown_25d690.cpp:1701`) and
  `0x28fe50` (`src/unknown_28fe50.cpp:96`).
  - Both pass an actor's index in the fifth, and an actor's `prop_index`
    (`s_actor_view` `+0x338`, `include/slot_handler.h:702`) in the sixth.
  - In `0x28fe50` both belong to the same actor. In `0x25bcf0`'s source the
    fifth is the object's actor, and the sixth is that actor's `prop_index`,
    unless the third argument names another actor
    (`src/unknown_25d690.cpp:1668`–`1701`).
  - So the fifth names an actor, likely the one whose prop_ref
    `reference_index` is (inferred). This function never reads it.
- **`unknown1a` is a priority.** See [The prop_ref](#the-prop_ref).
  `s_prop_datum` calls it `unknown1a` (`include/props.h:132`), and
  `s_prop_node_view` leaves it inside `unknown1a[6]`
  (`include/slot_handler.h:330`).
- **The view's `+0x3c` has a name in one view only.**
  - `s_type_f95cd3` leaves it inside `unknown3a[]` (`include/props.h:60`).
  - `s_prop_view_fields` calls it `unknown3c` (`include/slot_handler.h:352`).
  - Sources write it through casts, for example
    `*(real *)((byte *)view + 0x3c)` (`src/unknown_264330.cpp:365`,
    `src/unknown_265cb0.cpp:651`, and line 136 of the draft).
  - `function_265d30`'s source makes it a score.
- **One record, two names.**
  - `s_2640c0` (`include/unknown_25d020.h:7`) and `s_object_motion_view`
    (`src/unknown_265cb0.cpp:29`) are the same 0x38 bytes: two points, 12
    unknown bytes, a location and a vector.
  - `function_2640c0` fills one, and `function_264330` takes the other. The
    draft casts between them (`src/unknown_25d020.cpp:96`).
- **The header.** `include/unknown_25d020.h` is named after this function,
  and three of its callers' files include it. Yet it does not declare
  `function_25d020`, and five files declare it themselves.

## Evidence

- **Disassembly.** The function and its callees were disassembled with
  capstone 5.0.7 through `tools/xbe.py`, by `tools/disasm.py`. A workspace
  wrapper runs it, and accepts the workspace's XBE by the hash of its bytes
  after the header.
- **Stack and control flow.** A control-flow graph (a workspace tool kept
  outside the repository, with the frame size 0x7c) walked the stack depth
  over every path. It took each direct callee's stack effect from its own
  `ret` in retail. It found 46 blocks and two `ret`s, the same depth at every
  join, and depth 0 at both `ret`s. The same walk over the ten callers agrees
  with `0x25d020` popping 0x18.
- **References.** An image-wide scan looked for calls, jumps and absolute
  dwords. For `0x25d020` it finds the 13 `call` instructions in ten functions
  and nothing else. For `0x25bcb0` it finds one call, at `0x1fc125` in
  `0x1fbff0`.
- **What was read where.** Offsets, constants and conventions were read from
  retail code and data, and the callees with source from that source. The
  tests of `0x25da40` and the state writes of `0x262ba0` were checked against
  retail.
- **Checks.** An independent review checked the document against retail and
  the repository; its corrections are applied.
- **Prior coverage.** No document covered this range before.
  [unknown_1fc7e0](unknown_1fc7e0.md) reads the same prop_ref data
  (`function_25d690`, `function_25d740`, `unknown28`). Source line numbers are
  at `2bd2fc7d`.
- **Outside sources.** No emulator, runtime testing, SDK or outside dataset was
  used. Names are the repository's own, or describe behaviour.
