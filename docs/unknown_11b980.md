# Device controls recovery

Retail range claimed: `0x11b980`–`0x11bb3f`.

## Evidence

At base `d3baec5`, the range contains four untouched inventory entries,
covering 429 bytes including the toggle routine's 16-byte jump table:

| Retail | Function | Inventory bytes |
| --- | --- | --- |
| `0x11b980` | `function_11b980` | 78 |
| `0x11b9d0` | `function_11b9d0` | 62 |
| `0x11ba10` | `function_11ba10` (definition-field getter) | 49 |
| `0x11ba50` | `function_11ba50` | 240 |

Retail's named `control` object-type definition at `0x4683d8` stores
`0x11b980` in its placement callback at offset `0x30`. The callback reads
scenario flags and a one-based HUD override index. The next routine checks
the definition's trigger and calls the toggle routine. The getter reads the
32-bit definition field at `0x124`; its original name is not established.

The [Halo CE reference](https://github.com/punpckhdq/halo) (CC0 when this was written; AGPL-3.0 since 2026-10-17),
`source/devices/unknown_11b980.c`, corroborates placement, touch handling,
and the four toggle modes. Halo 2's placement callback has no call to the
older reference's device scenario helper. The retail toggle jump table at
`0x11bb30` has four slots: toggle, on, off, and the definition's call value.
Retail reads the device group index at object offset `0x13c` and its value
at group offset `4`; these offsets are followed directly, since upstream
labels the `0x13c` device field as the power group rather than the older
reference's position group.

The preceding `0x11b930` is existing upstream code and is excluded.
The next function at `0x11bb40` is the garbage object's creation callback
(type definition `0x467e60`, slot `0x2c`), and is also excluded. Issue #9 and
all open PR descriptions were checked; the claimed range has no overlap.

## Implementation and validation

All four functions are implemented in `src/unknown_11b980.cpp`:

| Retail | Result |
| --- | --- |
| `0x11b980`, `function_11b980` | Exact match, 78 bytes |
| `0x11b9d0`, `function_11b9d0` | Exact match, 62 bytes |
| `0x11ba10`, `function_11ba10` | Exact match, 49 bytes |
| `0x11ba50`, `function_11ba50` | 244 / 240 bytes, including jump-table data |

The toggle differs in register allocation, instruction scheduling, an extra
switch bounds check, and moving the device-group lookup into the toggle case.
The four valid modes, sentinel handling, group update, and on/off/denied effect
selection follow retail. No behavior is defined for an invalid control type;
retail dispatches through its four-slot table without a range check.
The original source's unused unit argument is retained on `function_11b9d0`.
The getter's original name and the precise meaning of definition field `0x124`
remain unknown; the source leaves both neutral.

The source includes the actual control-type definition prefix at `0x4683d8`,
through its placement callback. Its name, tag, metadata, zero slots, and linked
callback target agree with retail. The later fields and parent-type list are
outside this partial view. The callback uses `__stdcall`, supported by this
retail table and its `ret 8`. `/Ob1` keeps the touch-to-toggle call out of line,
as in retail, without changing any other source file's flags.

Full XDK 5849 check1 against `d3baec5`: **4,085 game matches / 4,087 total**,
three above baseline, with no upstream matches lost. Thirty original-compiler
layout assertions pass. The four linked switch cases and all three absolute
floating-point references agree with retail (`0.5f`, `1.0f`, `0.5f`).
Validation is build/disassembly based; no game runtime tests were run.

No new stubs, shared-header edits, other files' flags, or inventory changes.
Existing `function_1071e0(long, real)` keeps its upstream `__stdcall`
convention; `function_107980(long, long)` also remains unchanged.
The draft claim was published before source. One implementation/build pass
was used; the remaining byte differences are recorded for later matching work.

## Sources

Source was reconstructed from retail disassembly with the CC0 reference; game and SDK files
remain outside the contribution.
