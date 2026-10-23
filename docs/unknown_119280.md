# A creature's death timer and damage notice (unknown_119280)

Retail range covered: `0x119280`–`0x11937f`. This is one inventory entry, 256
retail bytes, `todo`, and `config/functions.csv` gives it no source. Only a
stub exists (`src/stubs/damage.cpp:21`). `function_119280` is the creature
branch of the damage report that `function_d9490` sends to the object it hit
(object type 12; the type is inferred from `function_119020`'s parameter
name, see [Data](#data)). It runs only when the damage has depleted the
creature's body. It then:

1. starts a countdown on the creature, as long as a random time between two
   reals in the creature's tag, if the larger of the two is above zero; and
2. when the creature has a controller kind, tells it the damage's category,
   and, through `edi`, the object that caused the damage (the report's owner
   object; see [Callees](#callees-without-source)).

**Analysis only:** this document adds no source, and nothing in it has been
built or checked against retail with the original compiler. Names are
provisional. The `function_<va>` form is primary; the descriptions are offered
for whoever decompiles it.

## Boundary

- **The range** is one function, `0x119280`–`0x11937f`. Its last instruction
  is `ret 4` (`0x11937d`, three bytes), so there is no padding.
- **The caller** is `function_d9490` (`0xd9490`, `todo`, with source in
  `src/damage.cpp`), once, at `0xd962f` (`src/damage.cpp:2444`).
- **The only callee** is `function_211600` (`0x211600`, `matched`,
  `src/unknown_211600.cpp:10`), which reaches `function_28fc50` and
  `function_295210`. The random draw is inline (see
  [The timer](#the-timer-0x1192a4-0x119352)).

## Conventions

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x119280` | 256 | `eax`: object index; `ebx`: the damage report; stack: flags (`ret 4`) | nothing | 1 + 0 | Starts a depleted creature's death timer and tells its controller |

**The frame.** There is no frame pointer.
- It starts with `push ecx` (`0x119280`), which only reserves a dword at the
  entry `esp` − 4. That slot is `[esp + 8]` at depth 0xc.
- `edi` is pushed at `0x1192ac`, after the early exit, and popped at
  `0x11937b`. The early exit (`0x11929e` to `0x11937c`) skips both.
- `esi` is pushed only on the timer path (`0x1192e4`) and popped at
  `0x119352`.
- The stack-depth walk (`tools/fncfg.py 0x119280`) finds 12 blocks, consistent
  depth at every join and the one `ret` at depth 0.

| Argument | Where | Use here |
| --- | --- | --- |
| object index | `eax` | Masked and looked up at `0x11928a`–`0x119295`. It is not changed afterwards, so it is still the object index at the call (`0x119376`) |
| flags | `[esp + 4]` at entry (`arg1`) | Only bit 0 is tested (`0x119299`). The slot is then reused as scratch (`0x119306`, `0x11932a`, `0x119338`) |
| damage report | `ebx` | Read at `0x11935c` (`+8`) and `0x11936c` (`+0x10`). Never written |

**Register inputs:** `eax` and `ebx`. `ecx`, `edx`, `esi` and `edi` are written
before they are read on every path. `esi` and `edi` are saved; `ebx` is not
written, so it needs no saving. `edi` is also an input of the callee: it is
loaded from the report just before the call (`0x11936c`) and carries the
owner object index on (see [Callees](#callees-without-source)).

**Return value:** none. `eax` is the object index at the exit when no callee
ran; after a call it is whatever the callee left. The caller `0xd9490` pops
and returns without reading it (`0xd9634`–`0xd9637`).

**The caller's registers.** `function_d9490` receives its report in `eax` and
copies it to `ebx` (`0xd949c`); no other instruction in `0xd9490` writes
`ebx` before its epilogue pops it (`0xd961d`, `0xd9635`). At the call:
`edi` holds the damage region accumulator (`[edi + 0x30]` is its `flags`,
loaded at `0xd9627`), `eax` is loaded from `arg1`, the object index
(`0xd962a`), and the flags are pushed (`0xd962e`). The call is reached only when `(1 << type)` has
bit 12 set (`test dh, 0x10`, `0xd9622`), that is object type 12.

## Data

**The creature** (the object, `g_4e0300`; `src/unknown_118e80.cpp:35`
`s_forward_object` names the first field):

| Offset | Type | What it holds |
| --- | --- | --- |
| `+0x00` | long | `definition_index`, the creature's tag. Only its low word is used (`0x1192a6`) |
| `+0x134` | long | The controller kind: `NONE`, or a value taken from the tag's short at `+0xb4` (`src/unknown_118e80.cpp:603`, `:608`). `function_211600` switches on 0 and 1 |
| `+0x206` | short | A countdown in ticks. `function_118510` zeroes it (`:594`). The creature's update, `function_118700`, counts it down while it is non-zero and calls `function_d6bc0` when it reaches 0 or less (`:1038`–`1044`) |

**The creature's tag** (`g_4e3b44`, 16-byte instances with the data pointer at
`+8`, `include/globals.h:370`–`385`):
- `+0x178` and `+0x17c`: two reals. Nothing in the repo names them. The code
  below treats them as a range of seconds: it interpolates between them and
  multiplies the result by the ticks per second. In effect they are a "time
  before the dead creature is destroyed" range, inferred from the use of
  `function_d6bc0` as the countdown's end.

**The damage report** (`src/damage.cpp:2367`, `s_damage_report`):
- `+0x08` `definition_index`: the damage effect's tag.
- `+0x10`: `owner.object_index` (`s_damage_owner` starts at `+0xc`,
  `src/damage.cpp:135`). It is loaded into `edi` at `0x11936c` and used by the
  callees (see [Callees](#callees-without-source)).

**The damage effect's tag** (`s_damage_effect_definition`,
`src/damage.cpp:206`, "a jpt! tag's fields from +0x10"): the word at tag
`+0x12` is `category` (`side_effect` is at `+0x10`).

**The flags** are the damage region accumulator's `flags`. Bit 0 means "this
damage depleted the object's body": it is set right after `function_d6800`
depleted the body (`src/damage.cpp:1813`), and after a damage permutation's
flags zero the body (`:2140`–`2147`). `function_d9490` copies it into the
report's own bit 0 (`:2416`).

**Globals and constants:**

| Address | What | Use |
| --- | --- | --- |
| `0x4e0300` | `g_4e0300`, object headers | `+0x44` data, 12 bytes each, the object at `+8` (`0x119281`) |
| `0x4e3b44` | `g_4e3b44`, tag instances | Read at `0x1192ad`; tag data at `+8` of 16 bytes |
| `0x4e7408` | `g_4e7408`, pointer to `s_random_globals` | `[esi]` is `unknown0`, the deterministic seed (`include/globals.h:782`–`791`) |
| `0x510c54` | `g_510c54`, `s_game_time_globals` | `field_2_3` (word at `+2`) is the ticks per second (`src/ai.cpp:898` multiplies it by seconds) |
| `0x45dbd8` | float 0.0 | The `> 0` test (`0x1192d5`) |
| `0x45dc50` | float 1.5259022e-05 (1/65535) | Scales the draw (`0x119316`) |
| `0x45dbb8` | float 4294967296.0 | The unsigned fix-up (`0x119310`), which never runs |

## The function

### Setup and the flag (`0x119280`–`0x11929e`)

1. `0x119281`–`0x119295`: `edx` is the creature: object header table, index
   `eax & 0xffff`, 12 bytes each, the object at `+8`.
2. `0x119299`, `0x11929e`: `test byte ptr [esp + 8], 1` and `je 0x11937c`. When
   the flags have bit 0 clear, the function pops `ecx` and returns. Nothing
   else happens: the controller notice is under the same test.

### The timer (`0x1192a4`–`0x119352`)

3. `0x1192a4`–`0x1192b6`: the creature's tag data: `ecx = [edx] & 0xffff`, the
   tag table (`edi`), `shl ecx, 4`, the data at `+8`.
4. `0x1192ba`–`0x1192cd`: `a = [tag + 0x178]`, `b = [tag + 0x17c]`.
   `comiss xmm0, [tag + 0x17c]` with `jbe` leaves `xmm0 = (a > b) ? a : b`.
   When either is NaN, `jbe` is taken too, so `xmm0` is `b`.
5. `0x1192d5`, `0x1192dc`: `comiss xmm0, 0.0` and `jbe 0x119353`. When the
   larger of the two is not above zero, the timer is skipped and `+0x206` is
   not written.
6. `0x1192de`–`0x1192e5`: `b` and then `a` are pushed on the x87 stack
   (`fld`, with `push esi` between them). `0x1192eb`–`0x119306`: the draw.
   `esi = g_4e7408`; `seed = [esi]`; `seed = seed * 0x19660d + 0x3c6ef35f` and
   the store back (`0x1192f3`–`0x1192ff`); `r = seed >> 16` (`0x119301`).
7. `0x11930a`–`0x119334`: `t = (real)r * (1/65535)` (`0x11930a`, `0x119316`),
   then `(a + (b - a) * t) * ticks`, with `ticks` the word at
   `g_510c54 + 2` (sign-extended, `movsx`, `0x119322`) multiplied in as an
   integer (`fimul`, `0x119334`). The `test ecx, ecx` / `jge` / `fadd 2^32` (`0x119304`,
   `0x11930e`, `0x119310`) is the unsigned-to-real fix-up. A draw is below
   0x10000, so it never runs.
8. `0x119338`–`0x11934b`: the product is stored as a float (`fstp`), the
   leftover `a` is popped (`fstp st(0)`, `0x11933c`), and the float is reloaded
   and converted with `fistp` (the FPU's rounding mode, nearest by default, not
   truncation). Its low word is stored at `creature + 0x206`. A zero result leaves the countdown
   idle, because the creature's update tests `+0x206` for non-zero.
9. `0x119352`: `pop esi`.

### The controller notice (`0x119353`–`0x11937d`)

10. `0x119353`, `0x11935a`: `cmp dword ptr [edx + 0x134], -1` and `je
    0x11937b`. This runs on both paths of the timer, so a creature with no
    timer range is still told.
11. `0x11935c`–`0x119376`: the damage effect's tag data (`[ebx + 8] & 0xffff`,
    the tag table, `+8`). `edi = [ebx + 0x10]` (`0x11936c`), the owner object
    index. `edx` is the word at tag `+0x12` zero-extended (`0x119371`), pushed
    (`0x119375`), and `function_211600` is called with `eax` still the object
    index and `edi` the owner object index.
12. `0x11937b`–`0x11937d`: `pop edi`, `pop ecx`, `ret 4`.

## A possible source form

This is inferred from the instructions above. It was not built.

```cpp
void function_119280(long object_index, dword flags, s_damage_report const *report)
{
	if (flags & 1)
	{
		s_forward_object *creature = /* object_index */;
		byte *tag = g_4e3b44[creature->definition_index & 0xffff].bytes;
		real a = *(real *)(tag + 0x178);
		real b = *(real *)(tag + 0x17c);

		if ((a > b ? a : b) > 0.0f)
		{
			real t = (real)random_draw_next(&g_4e7408->unknown0) * (1.0f / 65535.0f);
			real ticks = (a + (b - a) * t) * g_510c54->field_2_3;
			/* rounded by fistp, as unknown_118e80.cpp:1019-1026 does */
			*(short *)((byte *)creature + 0x206) = (short)ticks;
		}
		if (*(long *)((byte *)creature + 0x134) != NONE)
			function_211600(object_index, *(word *)(damage_effect_tag + 0x12),
				report->owner.object_index /* edi in retail */);
	}
}
```

**Code-shape notes:**
- `random_draw_next` is the repo's helper in `src/unknown_050690.cpp:18`: the
  same multiplier, increment and `>> 16`. Its `random_draw_real` (`:24`) is the
  same scale, `(real)random_draw_next(seed) * (1.f / 65535.f)`. Both are
  `static inline` in that file.
- The product is stored as a float before `fistp`, so the source had a `real`
  local. The repo spells such a rounding conversion as `__asm { fld x; fistp y
  }` (`src/unknown_118e80.cpp:1019`–`1026`).
- `ticks` is multiplied in as an integer (`fimul`), so the conversion goes
  from a `short` field, as in `ai_seconds_to_ticks` (`src/ai.cpp:898`).
- The maximum is `a > b ? a : b`: `comiss xmm0, [b]; jbe` takes `b` when `a`
  is not above it, and when either is NaN.

## Callees without source

None. Its callee `function_211600` is `matched`. The functions it reaches are
`function_28fc50` (`todo`, with a draft in `src/unknown_28fc50.cpp:27`) and
`function_295210` (`matched`, `src/unknown_293070.cpp:157`).

`function_211600` in retail (`0x211600`–`0x211636`): the object in `eax`, the
category as a word on the stack (`ret 4` at `0x21162f`), and `edi`, which it
never touches.
- **kind 0:** `jmp 0x28fc50` (`0x211632`). `function_28fc50` pops the category
  (`ret 4`, `0x28fcf0`). It finds the creature's block at `+0x13a` and, when
  that block has its two indices, calls `function_1c9580` (`0x28fcac`) and
  then `function_1e2570` (`0x28fce8`) with the category as the second
  argument. It never writes `edi`, and `function_1c9580` reads it first
  (`cmp edi, -1`, `0x1c9584`; `mov eax, edi`, `0x1c958b`): `edi` is its
  `object_index` (`src/ai.cpp:239`, `function_1c9580(long object_index, bool
  a)`). So the owner object index reaches `function_1c9580`. Its result is
  `unit->unknown24c` when `a` is true and that is set, else `unit->unknown248`
  when set, else the object itself (`src/ai.cpp:249`–`259`). It is kept in
  `eax` and pushed at `0x28fce2` as `function_1e2570`'s third argument.
- **kind 1:** `function_295210` with the object in `ebx` (`push ebx; mov ebx,
  eax; call 0x295210`, `0x211626`–`0x21162e`). It ignores the category and
  pushes `edi` before it writes it (`0x295248`), so the owner object index is
  not used on this path.
- **anything else:** nothing.

## Existing declarations

| Where | Declaration | Retail |
| --- | --- | --- |
| `src/damage.cpp:2389` | `void function_119280(long object_index, dword flags);` | Also reads the damage report, from `ebx`. The call (`:2444`) passes two arguments |
| `src/stubs/damage.cpp:22` | `void function_119280(long object_index, unsigned long flags) { }` | The same; `unsigned long` and `dword` are the same type |
| `src/damage.cpp:2444` | `function_119280(object_index, accumulator->flags);` | Retail's caller also has the report (`ebx`) |
| `src/unknown_211600.cpp:10` | `void function_211600(long arg_0, long arg_1)` | Also takes the owner object index in `edi` and passes it on |
| `src/unknown_28fc50.cpp:27` | `void function_28fc50(long arg_0, word arg_1)` | The same |

So a source for `0x119280` needs a third parameter, `s_damage_report const
*report`, and its call in `function_d9490` needs `report`. `function_211600`
and `function_28fc50` each need the owner object index too, down to
`function_1c9580`. The repo builds with whole-program optimization and picks
the registers itself, as the note on `0x5efd0` records
(`src/unknown_059ad0.cpp:2472`). In retail the report arrives in `ebx` and the
owner index in `edi`, so a match depends on the compiler making the same
choices.

## Findings about existing names

- **`s_damage_report` offset `+0x10`** is `owner.object_index`. This function
  loads it into `edi` (`0x11936c`) and it travels as a register argument
  through `function_211600` and `function_28fc50` to `function_1c9580`. It is
  unused only on the kind 1 path.
- **The draft of `function_28fc50`** (`src/unknown_28fc50.cpp:35`) calls
  `function_1c9580((long)local_2, *local_4 != 9)`. `local_2` is the perception
  pointer, which retail keeps in `esi` (`0x28fca9`). Retail does not pass it:
  `function_1c9580` reads `edi`, which is the owner object index above.
- **`s_damage_effect_definition`** (`src/damage.cpp:206`): `category` is the
  field this function passes on.
- **The creature's `+0x134` and `+0x206`** are accessed by raw offset in
  `src/unknown_118e80.cpp` (`:486`, `:540`, `:594`, `:603`, `:608`, `:1038`).
  `+0x206` is a countdown to destruction, and `+0x134` is a controller kind
  with `NONE` as "none".
- **The tag's `+0x178` and `+0x17c`** are not named anywhere in the repo.
- **`include/unknown_2420a0.h:63`** has a `byte unknown206[2]` that belongs to
  a different structure (`s_slot_table`).

## Evidence

- `tools/fncfg.py 0x119280`: 12 blocks; every join at a consistent depth; the
  one `ret` at depth 0.
- `tools/xrefs.py 119280`: one call, at `0xd962f`, in `0xd9490`.
- Constants read from the retail image: `0x45dbd8` = 0.0, `0x45dc50` =
  1.5259022e-05, `0x45dbb8` = 4294967296.0.
- Line numbers are at `b95396c0`.
