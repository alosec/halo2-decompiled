# A rigid body's contact impact test (unknown_279410)

Retail range covered: `0x279410`–`0x27966f`. This is one inventory entry, 600
retail bytes, `todo`, and `config/functions.csv` gives it no source. Nothing in
`src/`, `include/` or `docs/` mentions it. Both neighbours, `0x2792c0` and
`0x279670`, are `matched` in `src/unknown_2777d0.cpp`, in lane AC's range.

`function_279410` is called twice by `0x2daae0`, a Havok contact callback,
once for each body of the contact (see [The caller](#the-caller)). It decides
whether the contact is an impact for one rigid body (`body`) against the other
(`other`):

1. It finds `body`'s havok component through `body`'s property `0x2001`, and
   the component's object.
2. It measures `body`'s speed: `|v_body|`, or the smaller of `|v_body|` and
   `|v_body − v_other|` when `other`'s motion type is not 7.
3. For an object that is not a biped, the contact is an impact when the
   speed is above `threshold`.
4. For a biped: no impact when the biped's `+0x3dd` is set and a second
   object test passes (see [the quirk](#the-second-object-quirk)). Then, when
   its `+0x34b` (the dead biped's ragdoll state) is set, the same plain test
   as step 3. Otherwise it is an impact when the smaller of `normal_speed`
   and the speed is above `threshold`, or when the speed alone is above
   `threshold` and `function_2792c0` reports the contact as not face-on to a
   triangle.
5. On an impact it calls `function_1cfd10(contact, component, speed −
   offset)`, which records the impact's strength on the component, and
   returns true. The caller then sets `body`'s linear velocity to the same
   direction with magnitude `offset`.

**Analysis only:** this document adds no source, and nothing in it has been
built or checked against retail with the original compiler. Names are
provisional. The `function_<va>` form is primary; the descriptions are offered
for whoever decompiles it.

## Boundary

- **The range** is one function, `0x279410`–`0x279667`, then eight `int3`
  bytes (`0xcc`) to `0x27966f`. The last instruction is `ret` (`0x279667`,
  one byte). The next entry is `0x279670` (`matched`,
  `src/unknown_2777d0.cpp:278`).
- **The caller** is `0x2daae0` (503 bytes, `todo`, no source; no lane row in
  #9 names it), twice: at `0x2dabbe` and `0x2dac78`.
- **The callees** are `function_2792c0` (`0x279629`; `matched`,
  `src/unknown_2777d0.cpp:531`), `function_1cfd10` (`0x279651`; `matched`,
  `src/unknown_1cec30.cpp:1083`) and one virtual call, `other`'s
  `c_havok_body_motion_state::kind_code` (`0x27954e`, vtable `+0x18`,
  `include/unknown_1cec30.h:127`).

## Conventions

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x279410` | 600 | stack: `contact`, `body`, `other`, `threshold`, `offset`, `normal_speed` (the caller pops `0x18`: `0x2dabc3`, `0x2dac7d`) | `bool` in `al` | 2 + 0 | Tests a contact for an impact on `body` and records it |

**The frame.** `push ebp; mov ebp, esp; and esp, 0xfffffff0; sub esp, 0x24`
(`0x279410`–`0x279416`), then `ebx`, `esi` and `edi` are pushed. Arguments are
read through `ebp`, locals through the 16-byte aligned `esp`. At depth `0x34`:

| Local | Type | What it holds |
| --- | --- | --- |
| `[esp + 0x14]` | real | `speed`, the smaller of the two lengths |
| `[esp + 0x18]` | dword, then real | At first `body`'s property `0x2002`, which is never read (see [Code-shape notes](#code-shape-notes)); then the store target of `|v_body|` (`0x279535`–`0x279539`) |
| `[esp + 0x1c]` | real | The store target of `|v_body − v_other|` (`0x2795a3`–`0x2795a7`) |
| `[esp + 0x20]` | 16 bytes | The `sqrtss` scratch (`0x279521`–`0x279530`, `0x27958f`–`0x27959e`) |

| Argument | Where | Use here |
| --- | --- | --- |
| `contact` | `[ebp + 8]` | Never read here. It is passed on in `esi` to `function_2792c0` (`0x279626`) and in `edi` to `function_1cfd10` (`0x27964c`) |
| `body` | `[ebp + 0xc]` | Its property list (`+0x30` data, `+0x34` count) and `m_motion` (`+0x3c`) |
| `other` | `[ebp + 0x10]` | Its property list and `m_motion` |
| `threshold` | `[ebp + 0x14]` | Loaded into `xmm2` at `0x279615`; compared at `0x27961a`, `0x279621` and `0x27963a` |
| `offset` | `[ebp + 0x18]` | Subtracted from the speed at `0x279640` |
| `normal_speed` | `[ebp + 0x1c]` | Loaded at `0x279608`, compared at `0x27960d` |

**Register inputs:** none. `eax`, `ecx` and `edx` are written before they are
read; `ebx`, `esi` and `edi` are saved first.

**Return value:** `al`. It is 1 only on the path through `0x279640`, which
sets `bl = 1` (`0x27964f`) before calling `function_1cfd10` and returns `bl`
(`0x279656`). Every other exit returns 0: `bl = 0` (`0x27961f`) returned at
`0x279656`, or `xor al, al` (`0x279661`) at `0x27965f`.

### The caller

`0x2daae0` ends with `ret 4` (`0x2dacd4`) and saves `ecx` on entry
(`0x2daaf7`), so it is a `this` call with one stack argument: `contact`
(`esi`, `0x2daaeb`). It walks each body of `contact` to its root through
`+0xc` (`0x2dab01`–`0x2dab0a` for `[contact + 8]`, `0x2dab20`–`0x2dab27` for
`[contact + 0xc]`), and returns early unless a filter test passes
(`0x2dab29`–`0x2dab56`). Its two calls are each gated by that body's shape:
`shape->type()` (vtable `+0x14`) must be `0x17` (`0x2dab63`–`0x2dab69` and
`0x2dac1d`–`0x2dac23`). The arguments of the first call
(`0x2dab6f`–`0x2dabbe`) are:

- `contact`: `esi`, unchanged;
- `body`: the entity at `+0x20` of the root of `[contact + 8]`;
- `other`: the entity at `+0x20` of the root of `[contact + 0xc]`;
- `threshold`: the real at `+0x24` of `[contact + 8]`'s shape;
- `offset`: the same shape's `+0x24` minus its `+0x28` (`fld`/`fsub`/`fstp`,
  `0x2dab7a`, `0x2dab81`, `0x2dab95`);
- `normal_speed`: the real at `contact + 0x1c`.

This reading assumes the filter's virtual call at `0x2dab50` pops its three
pushed arguments (a `this` call), so that the reloads at `0x2dabce` and
`0x2dac3e` find the values stored at `0x2dabba` and `0x2daafb`; the stack walk
of `tools/fncfg.py`, which assumes 0, reports mismatched joins there.

The second call (`0x2dac29`–`0x2dac78`) swaps the two bodies and uses
`[contact + 0xc]`'s shape. When a call returns true (`0x2dabca`, `0x2dac84`),
the caller computes `v_body × offset / |v_body|` and passes it to
`s_1d8e40::function_1d8e40` (`0x2dac07`, `0x2dacbf`;
`src/unknown_1d8dd0.cpp:44`, which activates the body and calls
`m_motion->setLinearVelocity`), then zeroes the dword at `+0xc` of
`[contact + 0x18]` (`0x2dac0c`, `0x2dacc4`). The `1.0` it divides is the float
at `0x45dbc0`.

## Data

**The entities** (`include/unknown_1cec30.h`):
- `hkEntity`: `m_properties` at `+0x30`, `m_property_count` at `+0x34`
  (`:43`–`44`), entries of 8 bytes (`s_havok_entity_property`, `:21`–`25`: the key, then the
  value). `havok_entity_property_get` (`:49`–`61`) returns 0 when the key is
  absent. The keys are `HAVOK_PROPERTY_COMPONENT_INDEX` (`0x2001`, `:456`) and
  `HAVOK_PROPERTY_2002` (`:457`).
- `hkRigidBody::m_motion` at `+0x3c` (`:181`). `c_havok_body_motion_state::m_linear_velocity`
  is at `+0x40` (`:162`) and `kind_code` is slot 6, vtable `+0x18` (`:127`). The
  repo compares `kind_code()` with 7 by number, e.g.
  `src/unknown_1c25a0.cpp:1200`.

**The havok component** (`s_havok_component`, `include/unknown_1cec30.h:381`;
`0xa0` bytes, `:250`–`251`): `object_index` at `+8` (`:405`). Its `unknown10`
and `unknown14` (`:407`–`408`) are what `function_1cfd10` writes: the game
time of the last contact, and the strongest recent impact scale
(`src/unknown_1cec30.cpp:1100`–`1116`).

**The object** (object headers `g_4e0300`, 12 bytes, the object at `+8`;
`s_havok_object_header`, `include/unknown_1cec30.h:236`):
- `+0xaa`: `type` (`s_damage_object`, `src/damage.cpp:46`, `:67`). Type 0 is
  the biped: `src/damage.cpp:3160` tests `(1 << object->type) & 1` on this
  field and then reads the biped's `physics_mode` at `+0x3dc` (`:3165`).

**The biped** (`s_biped`, `src/bipeds.cpp:35`; `BIPED_GET`, `:156`):
- `+0x34b`: `unknown34b` (`:84`), the dead biped's ragdoll state: 0, then 1
  when it falls over, 2 when it settles (`:2193`–`2195`).
- `+0x3dd`: the first byte of `unknown3dd[3]` (`:119`), right after
  `physics_mode` (`+0x3dc`, `:118`). Nothing in the repo reads it.

**Globals:**

| Address | What | Use |
| --- | --- | --- |
| `0x51e9b8` | `g_51e9b8` (`include/globals.h:922`), the havok components | `+0x44` data, `0xa0` bytes each (`0x279492`–`0x2794a6`) |
| `0x4e0300` | `g_4e0300`, the object headers | `+0x44` data, 12 bytes each, the object at `+8` (`0x2794a9`, `0x2795d1`) |

## The function

### The properties (`0x27941c`–`0x2794ed`)

1. `0x27941c`–`0x279442`, `0x2794da`: `eax` = `body`'s property `0x2001`, or
   0 when absent. The loop compares each entry's key and, when it matches,
   loads the value from `+4` (`mov eax, [esi + eax*8 + 4]`, `0x2794da`).
2. `0x279444`–`0x279465`, `0x2794e3`: `body`'s property `0x2002`, the same
   way, stored to `[esp + 0x18]` (`0x2794e7`), or 0 there when absent
   (`0x279465`). This value is never read.
3. `0x27946d`–`0x279490`, `0x2794ed`: `edx` = `other`'s property `0x2001`, or
   0 when absent.

### The component and the objects (`0x279492`–`0x2794f3`)

4. `0x279492`–`0x2794a6`: `edi` = the component: `g_51e9b8` data plus
   `(eax & 0xffff) × 0xa0` (`lea eax, [eax + eax*4]; shl eax, 5`).
5. `0x2794a9`–`0x2794bf`: `ebx` = the object at the component's
   `object_index` (`[edi + 8]`, through `g_4e0300`).
6. `0x2794b9`–`0x2794d4`, `0x2794f3`: `esi`, the second object. It is 0 when
   `other`'s `0x2001` value is −1 (`cmp edx, -1`, `0x2794b9`) or when `edi` is
   null (`0x2794c5`). Otherwise it is the object at `[edi + 8]` again
   (`0x2794c9`), that is, `body`'s component's object. See
   [the quirk](#the-second-object-quirk).

### The speed (`0x2794f5`–`0x2795be`)

7. `0x2794f5`–`0x279548`: the length of `body->m_motion->m_linear_velocity`
   (x, y and z; the w lane is squared but not added), stored to `[esp +
   0x18]` and copied to `speed` (`[esp + 0x14]`, `0x279548`).
8. `0x27953d`–`0x279554`: `other->m_motion->kind_code()`. When it returns 7,
   the relative speed is skipped (`0x279554` to `0x2795be`).
9. `0x279556`–`0x2795a7`: the length of `body`'s linear velocity minus
   `other`'s, stored to `[esp + 0x1c]`.
10. `0x2795ab`–`0x2795b6`: `comiss xmm1, xmm0` with `xmm1 = [esp + 0x18]`
    (`|v_body|`) and `jbe`: when `|v_body|` is above the relative length,
    `speed` becomes the relative length (`0x2795b6`). Otherwise `speed` is
    reloaded (`0x2795be`). Either way `xmm0 = speed` at `0x2795c4`.

### The biped tests (`0x2795c4`–`0x279632`)

11. `0x2795c4`–`0x2795cc`: when the object's `type` (`[ebx + 0xaa]`) is not 0,
    jump to the plain test (`0x27963a`).
12. `0x2795ce`–`0x2795e2`: `eax` = the object at the component's
    `object_index` again, looked up anew through `g_4e0300`.
13. `0x2795e6`–`0x2795fc`: when `[eax + 0x3dd]` is non-zero, `esi` is not
    null and `[esi + 0xaa]` is 0, return 0 (`0x27965f`).
14. `0x2795fe`–`0x279606`: when `[eax + 0x34b]` is non-zero, jump to the plain
    test (`0x27963a`).
15. `0x279608`–`0x279612`: `xmm1 = normal_speed`; when `normal_speed > speed`
    (`comiss xmm1, xmm0; jbe`) it becomes `speed`. So `xmm1` is the smaller.
16. `0x279615`–`0x27961d`: when `xmm1 > threshold` (`ja`), record the impact
    (`0x279640`).
17. `0x27961f`–`0x279624`: `bl = 0`; when `speed` is not above `threshold`,
    return `bl` (`0x279656`).
18. `0x279626`–`0x279630`: `function_2792c0` with `esi = contact`. When it
    returns non-zero, return `bl` (0). Otherwise `xmm0` is reloaded from
    `speed` (`0x279632`), and the impact is recorded.

### The plain test, the record and the exits (`0x27963a`–`0x279667`)

19. `0x27963a`, `0x27963e`: `comiss xmm0, threshold; jbe 0x27965f`: when
    `speed` is not above `threshold`, return 0.
20. `0x279640`–`0x279651`: push `speed − offset` and the component (`edi`),
    load `edi = contact`, set `bl = 1`, call `function_1cfd10`. It pops its
    two stack arguments (`ret 8`).
21. `0x279656`–`0x27965e`: return `bl`. `0x27965f`–`0x279667`: return 0.

## The second-object quirk

The second object (`esi`, step 6) is meant, by its use in step 13, to be
`other`'s object: "no impact between this biped and another biped while
`+0x3dd` is set". Retail does not read `other`'s component, though.
`other`'s `0x2001` value (`edx`) is only compared with −1 (`0x2794b9`), and
`edi`, the component of `body` since `0x2794a6`, is not written again before
`0x2794c9` reads `[edi + 8]`. So when `other` has any `0x2001` value but −1
(including 0, the "absent" result), `esi` is `body`'s own object. On the biped
path its `type` is 0, so step 13 returns 0 whenever `+0x3dd` is set. When
`other`'s value is −1, step 13 never returns.

A matching source has to reproduce this. Writing the lookup with `other`'s
index would compute a second address from `edx`.

## A possible source form

This is inferred from the instructions above. It was not built.

```cpp
bool function_279410(s_component_contact_pair const *contact, hkEntity *body, hkEntity *other,
	real threshold, real offset, real normal_speed)
{
	long component_index;
	long unused; /* 0x2002, never read */
	/* body's 0x2001 and 0x2002, e.g. through a helper like function_277b87 */
	long other_index = havok_entity_property_get(other, HAVOK_PROPERTY_COMPONENT_INDEX);
	s_havok_component *component = havok_component_get(component_index);
	s_damage_object *object = /* the object at component->object_index */;
	s_damage_object *other_object = other_index != NONE && component ?
		/* the object at component->object_index, as retail reads it */ : NULL;
	real speed = /* |((hkRigidBody *)body)->m_motion->m_linear_velocity| */;

	if (((hkRigidBody *)other)->m_motion->kind_code() != 7)
	{
		real relative = /* |v_body - v_other| */;
		if (speed > relative)
			speed = relative;
	}
	if (object->type == 0)
	{
		s_biped *biped = BIPED_GET(component->object_index);

		if (biped->unknown3dd[0] && other_object && other_object->type == 0)
			return false;
		if (!biped->unknown34b)
		{
			bool result = false;

			if ((normal_speed > speed ? speed : normal_speed) > threshold ||
				speed > threshold && !function_2792c0(contact))
			{
				result = true;
				function_1cfd10(contact, component, speed - offset);
			}
			return result;
		}
	}
	if (!(speed > threshold))
		return false;
	function_1cfd10(contact, component, speed - offset);
	return true;
}
```

### Code-shape notes

- **The lengths** are the same instruction sequence as the matched
  `0x1ce8d0` (`src/unknown_1ce8d0.cpp:33`–`38`, retail `0x1ce924`–`0x1ce958`):
  `_mm_mul_ps`, `_mm_add_ss` of the `0x55` shuffle and the square, `_mm_add_ss`
  of the `0xaa` shuffle and that sum, `_mm_sqrt_ss`, then `_mm_store_ss` into a
  `real`. Retail goes through memory for the square root (`movaps` to `[esp +
  0x20]`, `sqrtss`, `movss` back, `movaps` out) and stores the result through
  a register (`lea ecx, [esp + 0x18]; movss [ecx], xmm0`), in both functions.
- **The `0x2002` lookup** (`0x279444`–`0x279465`, `0x2794e3`–`0x2794eb`)
  stays in retail although its result is dead, so the source must contain it.
  `function_277b87` (not a retail function: a `PRIVATE __forceinline` helper
  in `src/unknown_277b80.cpp:42`) reads exactly `0x2001` and `0x2002` through two out pointers; an inlined
  helper of that shape would explain why the store survived, because its
  target is a local whose address was taken. That is a guess.
- **The object is looked up twice:** into `ebx` before the virtual call
  (`0x2794a9`–`0x2794bf`) and into `eax` after it (`0x2795ce`–`0x2795e2`),
  while `ebx` is still live (`0x2795c4`). So the source names it twice, for
  example once as the object and once through `BIPED_GET`.
- **The comparisons:** `speed > relative` (`0x2795b1`, `comiss` with `|v_body|`
  first), `normal_speed > speed ? speed : normal_speed` (`0x27960d`), and
  `> threshold` three times (`0x27961a` with `ja`; `0x279621` and `0x27963a`
  with `jbe`). With `jbe`, a NaN takes the "not above" branch.
- **Two exits:** `0x279656` returns `bl` and `0x27965f` returns a constant 0.
  `bl` is written only on the biped path (`0x27961f`) and before the record
  (`0x27964f`), which suggests a `bool` local on the biped path and plain
  `return false` elsewhere, with the record shared.
- **The biped test** is `mov al, [ebx + 0xaa]; test al, al` (`0x2795c4`), a
  direct test of the byte for 0, where `src/damage.cpp:3160` writes
  `(1 << object->type) & 1` on the same field.

## Callees without source

None: both callees are `matched`.
- `function_2792c0` (`0x2792c0`, 336 bytes): `contact` in `esi` (`[esi + 8]`,
  `0x2792c9`; `[esi + 0x14]`, `0x279357`), the result in `al`, plain `ret`
  (`0x279400`, `0x27940f`). It returns false only when one body's shape is of type `0x18`
  (a triangle) and the absolute dot product of the triangle's unit normal with
  `plane[1]` is below 0.577 (`src/unknown_2777d0.cpp:557`), that is, the two
  directions are more than about 54.7° apart: an edge contact.
- `function_1cfd10` (`0x1cfd10`): `contact` in `edi` (`[edi + 8]`,
  `0x1cfd24`), the component and the scale on the stack, `ret 8`. It scales
  the impact by the square root of the product of one material value per body
  (see [Findings](#findings-about-existing-names) for its name), and stores
  it in `component->unknown14` when it is larger than the stored value
  or 0.35 seconds have passed; it always sets `component->unknown10` to the
  game time (`src/unknown_1cec30.cpp:1083`–`1117`).

## Existing declarations

| Where | Declaration | Retail |
| --- | --- | --- |
| `src/unknown_1cec30.cpp:1083` | `void function_1cfd10(s_component_contact_pair const *contact, s_havok_component *component, real scale)` | The same; `contact` in `edi` |
| `src/unknown_2777d0.cpp:531` | `bool function_2792c0(s_2792c0_context *context)` | The same; `context` in `esi` |

`function_279410` itself has no declaration or stub anywhere in the repo.

## Findings about existing names

- **`s_2792c0_context` and `s_component_contact_pair` are one structure.**
  This function passes the one `contact` pointer to both
  `function_2792c0` (`s_2792c0_context`, `src/unknown_2777d0.cpp:515`) and
  `function_1cfd10` (`s_component_contact_pair`, `src/unknown_1cec30.cpp:1070`).
  Both put the two bodies at `+8` and `+0xc`; `s_2792c0_context` also names
  `+0x14` (`plane`).
- **`s_2792c0_contact` and `s_component_contact_body` are one structure**
  (`src/unknown_2777d0.cpp:508`, `src/unknown_1cec30.cpp:1058`): the shape at
  `+0`; `s_2792c0_contact::transform` at `+8` is
  `s_component_contact_body::unknown08`. The caller reads the body's `+0xc`
  (`parent`) and `+0x20` (`entity`), as `s_component_contact_body` names them.
- **The contact's `+0x18` and `+0x1c`** are not named: the caller passes
  `+0x1c` as `normal_speed` and zeroes `[+0x18] + 0xc` after an impact.
- **`function_1cfd10`'s local `friction`** (`src/unknown_1cec30.cpp:1087`)
  receives what `function_182b90` returns through its `restitution`
  parameter: the call passes `&restitution, &friction[i]`
  (`src/unknown_1cec30.cpp:1097`) to `(…, real *friction, real *restitution,
  …)` (`src/unknown_182020.cpp:124`), and retail `0x182b90` fills that pointer
  from the fields the repo names `restitution` (`0x182c0e`–`0x182c13`,
  `0x182c4e`–`0x182c53`; `src/unknown_182020.cpp:63`, `:85`). One of the two
  names is wrong.
- **`s_biped` `+0x3dd`** (`unknown3dd[0]`) is unnamed and has no other reader
  in the repo.
- **The shape of type `0x17`** carries two reals at `+0x24` and `+0x28` that
  the caller reads; nothing in the repo names them.

## Evidence

- `tools/fncfg.py 0x279410`: 41 blocks, two `ret`s, every join at a
  consistent depth. Its one problem line is the virtual call at `0x27954e`
  (unknown stack effect, assumed 0; `kind_code` takes no stack arguments).
- `tools/xrefs.py 279410`: two calls, at `0x2dabbe` and `0x2dac78`, both in
  `0x2daae0`.
- `0x1cfd10` ends with `ret 8` (`0x1cfdd0`, `0x1cfdde`); `0x2792c0` with `ret`
  (`0x279400`).
- Constants read from the retail image: `0x45dbc0` = 1.0.
- Line numbers are at `928e3ddf`.
