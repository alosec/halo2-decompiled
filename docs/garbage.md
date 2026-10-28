# Garbage object recovery

Retail range claimed: `0x11bb40`–`0x11bbef`.

## Evidence

The range contains one untouched 169-byte entry at base `c1bcd3c`:
`function_11bb40` at `0x11bb40` (name inferred from callback role and reference).
Retail's named `garbage` type definition at `0x467e60` stores this address
in its creation slot at `+0x2c`. It is the only non-null code callback in that
type definition. The type's datum size is `0x170`.

The [Halo CE reference](https://github.com/punpckhdq/halo) (CC0 when this was written; AGPL-3.0 since 2026-10-17),
`source/items/garbage.c`, identifies the corresponding `function_11bb40` behavior:
garbage-list membership, shadowless/deactivation flags, and a randomized
lifetime. Halo 2's retail callback has a different lifetime calculation.

The retail callback calls the existing object-list helper `0xbb950`, sets
object flag `0x20000`, advances the deterministic random state at `0x4e7408`
once, and stores an absolute expiration tick at object offset `0x16c`.
The random step is `seed = 1664525 * seed + 1013904223`, with its high 16 bits
scaled by `1 / 65535`. The lifetime is `(sample + 1) * 10 * field_2_3`,
rounded by the x87 `fistp` instruction and added to current game time.
Finally, it sets the shadowless flag `0x10000` and returns true.

The preceding device-controls module belongs to PR #18 and ends at
`0x11bb3f`. The following `0x11bbf0` is the crate creation callback and is
excluded. Issue #9 and all open PR descriptions were checked: no overlap.

## Implementation and validation

The creation callback and actual type-definition prefix through `+0x2c`
are recovered in `src/garbage.cpp`. The two unused creation arguments remain
provisional `long` values; retail's `ret 12` and the generic creation slot
establish three stack arguments. The object view has flags at `+4` and an
expiration tick at `+0x16c`. The deterministic random seed is at offset zero
of `g_4e7408`, not the separate seed at `+4`.

`function_11bb40` is a near-match: 173 code bytes versus retail's 169, with the
first difference at `+0x79`. The compiler exchanges the multiply by ten and
integer tick-rate multiply. It also emits load/OR/store for the final flag,
changes the addition register, and schedules that flag store ahead of the
expiration store. The float store/reload followed by x87 `fistp` is preserved.
Floating-point rounding equivalence after the multiplication reorder has
not been established by a game runtime test. The checker labels this entry
`todo`; “near-match” here describes the remaining instruction differences.

This is one source implementation and one full check. Work stopped at
operand ordering and register/scheduling differences, per the project guide;
no attributes, artificial callers, or compiler-flag tuning were introduced.

Validation against upstream `c1bcd3c`:

- Full `tools/check.py`: **4,228 game / 4,228 total matches**, none lost.
- Twenty original-SDK compile-time assertions validate the object, header,
  random-state, time-global, and type-definition layouts.
- Linked type metadata, name, creation pointer, all three global references,
  existing `0xbb950` call, one random step, flag masks, floating constants,
  expiration offset, and x87 conversion instructions were inspected.
- No game runtime tests were run.

The draft range claim was published before source. Only `src/garbage.cpp`
and this document change. Upstream `function_bb950(long, bool, long)` is
reused unchanged; there are no new stubs, shared-header edits, changes to
other files' flags, or committed inventory changes.

## Sources

Source was reconstructed from retail disassembly with the CC0 reference;
game and SDK files remain outside the contribution.
