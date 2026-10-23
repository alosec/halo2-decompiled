# Decal placement recovery: `0x17dd80`–`0x17fd1f`

This batch supplies independently validated implementations of placement,
chain traversal and preparation, plus corrections to neighboring-face
intersection, registration radius accumulation and initial record linking.
Upstream `413fea15` also implements these three routines; this contribution
retains the implementations exercised by the reproducible comparisons below.
The shared predicate at `0x11e5e0` and its eight callers are audited below.

| Retail entry | Bytes | Current evidence |
| --- | ---: | --- |
| `0x17e670` | 1,787 | Placement body recovered; behavior checked, byte matching pending |
| `0x17ee20` | 240 | Iterates a decal chain, preparing and submitting projection work |
| `0x17ef10` | 3,591 | Preparation body recovered; behavior checked, byte matching pending |

The intervening placement-copy helper `0x17ed70` already has matching source
in `src/unknown_17d9a0.cpp`; preserve it. The three bodies total 5,618 retail
bytes. No new exact matches are claimed. Historical observations below name
their original tested revisions; current validation must use the image and
map from the contributor's own build.

## Reproduce the synthetic comparison

The comparisons below were made with an optional emulator runner (Unicorn) that builds synthetic fixtures and
checks our build against retail: a 648-case full synthetic path by default, plus physical-allocator, portal and
command-generation suites. The runner is not kept in this repository; its source and README are in the history of
[pull request #323](https://github.com/kirklandsig/halo2-decompiled/pull/323).
The historical exploratory sweeps below have narrower or different fixtures.

## Independently observed flow at `0x17ee20`

The entry aligns its stack to eight bytes and reserves `0x5938` bytes through
the compiler's stack-probe helper. An initial handle arrives in ECX, a
record pointer in EBX, and seven arguments on the stack (`ret 0x1c`).

A handle of `-1` returns false. Otherwise the loop stops if the signed short
at record `+0x20` equals `-1`. Each iteration clears a `0x110`-byte local
region and calls `0x17ef10`. A false return ends the loop. Success leads to
calls to `0x17fd20` and then `0x180d80`. The latter returns the next handle.
A positive signed short in the projection scratch region sets an accumulated
success byte; later iterations do not clear it. The loop ends when the next
handle is `-1` or an earlier exit condition applies.

The four calls in `0x17e670` establish the seven stack arguments, in order:
mesh pointer, optional transform, direction pointer, radius, a flag, a sequence
selection, and a final option whose meaning is not yet fully recovered.
The initial tag and placement record arrive in ECX and EBX. `0x17ef10` receives
the transform in ECX and nine stack arguments: tag, placement, direction,
radius, flag, sequence selection, final option, preparation, persistent state.

The local buffers are distinguishable by their accesses, not just the frame
size. The persistent state occupies 28 bytes, the cleared preparation occupies
272 bytes, and the output occupies `0x5804` bytes. The output's polygon count
is the signed short at `+0x5802`, initialized before each projection call.
Only preparation is cleared each iteration; persistent state survives calls.

Preparation starts with the existing `s_decal_projection_17fd20` layout
(`0xec` bytes). Its remaining independently observed fields are:

| Offset | Access and evidence |
| --- | --- |
| `0xec` | Four floats passed as texture bounds to `0x17cb00` |
| `0xfc` | Signed short sequence index, selected and clamped at `0x17fa14`–`0x17fa94` |
| `0xfe` | Signed short frame index, initialized to zero at `0x17faaf` |
| `0x100` | Signed short bitmap index, loaded from the selected sprite frame at `0x17fb87` |
| `0x104` | Bitmap-group pointer obtained from the tag table at `0x17f9fc`–`0x17fa0e` |
| `0x108` | Selected sequence pointer, using a `0x3c`-byte stride |
| `0x10c` | Selected sprite-frame pointer, using a `0x20`-byte stride |

The persistent state's first byte controls reuse of random choices. Preparation
uses float `+4` for radius interpolation, float `+0x14` as an angle passed to
sine/cosine, and dword `+0x18` for a sequence choice. The existing commit helper
uses floats `+8`, `+0xc`, and `+0x10` and updates the first byte. These accesses
explain the full 28-byte allocation despite the commit helper's shorter type.
The traversal and preparation now share explicit layouts for these fields.

## Preparation behavior

The preparation routine transforms the placement point for its nearby-decal
check, but uses the original point when constructing the projection matrix.
When the definition's nearby limit is nonzero and the final option is zero,
it scans 24 records at decal globals `+0x380c`, each `0x2c` bytes. It rejects
placements that reach the limit of overlapping, nonexpired records with the
same tag. Otherwise it records the placement in the first empty or expired
slot, or selects the oldest frame value. A zero lifetime keeps expiry zero.
At the exact expiry time, a record is neither counted as active nor immediately
reused: counting uses `expiry > now`, replacement uses `now > expiry`.

Orientation follows the definition flags and incoming direction. It either
uses a persistent random angle, projects the direction against the plane, or
selects from eight directions in the plane basis. Both resulting tangent vectors
are normalized. The random angle retains intermediate precision through both
multiplications before its single-precision store, as retail does; rounding the
random fraction first changed matrix bytes in the initial candidate.

Preparation then selects the sequence and first frame, samples the radius,
obtains sprite bounds or builds rectangular fallback bounds, and optionally
checks texture availability. A failed lookup and failed fallback reject the
placement. Successful preparation calls the existing quad helper and initializes
the orientation bounds. Persistent random choices are reused when the state
flag is set. The matrix's scale field is not written here; it retains its
incoming value, including the zero supplied by the traversal's cleared buffer.

## Placement entry

The entry respects the enable byte at `0x46dd48`. It normalizes a copy of the
incoming direction and offsets the collision-query origin by `0.01` along it.
A supplied collision record is reused unless its type is 4; otherwise it queries
for a hit. Type 1 uses the world mesh, while type 3 converts point, plane and
direction into the hit instance's local coordinates. Successful instance
placement can transform four caller-supplied output points back to world space.

The low byte of `unknown0` selects deterministic random seeding from the point's
three float bit patterns XOR `0xdeadc0de`. This mode restores the previous seed
and skips the additional searches. For sufficiently large decals, ordinary
placement searches nearby instance bounds, checks that ray hits belong to each
candidate, and places in their local coordinates. An initial instance hit also
tries six world-axis rays, stopping after successful world placement. These
four submission paths set the mode at `0x46dd4c` to 0, 1, 2, or 3.

## Scope and validation

The compiled preparation passes 2,048 differential cases against retail,
comparing return value, the entire synthetic input/output memory region,
random state, recent-placement cache, and helper-call order. The stronger run
executes the actual plane-basis, sprite-bound and quad-preparation helpers on
both sides. Texture lookup and fallback are mocked in this run. Coverage
includes 1,221 successful preparations, 827 failures, 163 plane-basis calls,
639 sprite-bound calls, 528 texture lookups, 263 fallbacks, and 702 cache updates.
Inputs vary flags, retained random state, transformed points, normals, radius,
sequence counts and selection, sprite type, aspect handling, cache expiry,
texture residency, and lookup/fallback results. A separate run with geometry
helpers also mocked passes the same 2,048 cases. Neither establishes GPU,
streamed texture, or gameplay behavior.

Placement passes 1,024 differential cases using actual normalization, point,
vector and plane transforms, placement copying, and output-point transformation.
Collision, nearby-bound search and chain execution are mocked in this probe.
It compares helper order/arguments, input memory, random seed, output points and
placement globals. Coverage includes 1,060 collision queries, 133 nearby-bound
searches, and 711 chain submissions across all four modes (278/283/13/137).
The chain flag is compared as a byte, consistent with the callee's bool argument;
unused high bytes in its stack slot are not semantic inputs.

The compiled traversal still passes its 216 helper-boundary cases against the
retail contract. That separate probe mocks preparation, projection, and commit;
it checks forwarding, zeroed preparation, persistent state, signed polygon
counts, sticky success, early exits, stack balance, and saved registers.

After merging main `588746a4`, the full byte check preserves all 7,500 existing
game matches,
including placement copy `0x17ed70`, with no gains or losses. All three probe
suites above pass on this merged image. None of the three new bodies is exact: traversal has a 240-byte checker extent versus 240 retail (including alignment
padding in the compiled extent); preparation is
3,453 versus 3,591. Placement is 1,752 compiled bytes versus 1,787 retail.
Explicit whole-buffer clearing reproduces the retail preparation clear of 68
dwords. The remaining traversal differences include register/instruction order
and the existing projection helper's stack argument convention; equal extents
do not mean an exact or near match. Both temporary stubs have been removed. The shared predicate contract and
caller changes are described below; no per-file compiler flags changed.

## Neighboring-face correction and shared predicate contract

The actual traversal, preparation, projection (`0x17fd20`), face clipping
(`0x17dd80`) and math helpers are compared on synthetic mesh data. Final commit
is mocked; texture availability is bypassed by the caller flag. A unit square
split at x=0, decal centered at the origin, radius 0.25, flags zero, and one
chain link exposed a defect: retail emitted two polygons/eight vertices while
the previous source emitted one polygon/four vertices. Preparation of the first
polygon and random state agreed; the missing work was the neighboring face.

At retail calls `0x17e087` and `0x17e3ad`, EAX contains the sphere center,
ECX the edge start, and EDX the full segment displacement. The predicate at
`0x11e5e0` computes start minus center. Supplying the two points in reverse
changes the sign of the direction/delta dot product and can reject a real
intersection. All eight retail callers were inspected to establish these roles:

| Caller | Sphere center | Segment start |
| --- | --- | --- |
| `0x11ea30` | endpoint of one segment | start of the other segment |
| `0x1516c0` | object position | eye position |
| `0x1691a0` | instance center | query point |
| `0x169510` | object center at +0x40 | query point |
| `0x1697c0` | reference center | query point |
| `0x17dd80` | decal quad center | edge start |
| `0x1c8d20` | capsule center | query origin |
| `0x1de2c0` | supplied sphere center | polygon edge start |

`include/segment_sphere.h` now declares the explicit contract
`(segment_start, direction, arg_28f920, radius)`, where `arg_28f920` is
the sphere's center. The definition and every call site use that order, with duplicate local declarations removed. This
corrects reversed point roles in `0x11ea30`, `0x1691a0`, `0x17dd80`, and
`0x1c8d20`; the other callers retain their established geometric inputs.
The predicate's arithmetic and branch conditions are unchanged. This is a
consistent shared-contract correction, not an isolated argument permutation
used to claim a match to an unmatched callee.

The unmodified compiled image now agrees with retail on all 648 single/two-face
scenarios: 720 polygons and 3,208 vertices across 1,296 commit boundaries,
including 324 two-face cases. No emulator argument intervention is used.
The local direct predicate probe also passes 10,504 finite random and boundary
cases (1,500 hits and 9,004 misses), including zero-length segments, tangencies,
endpoint intersections and translated cases, through the generated stand-in.
The 216 traversal, 2,048 real-geometry preparation and 1,024 placement tests
also pass on this same image.

Angled fixtures add 2,916 two-face and 2,916 three-face cases at bends of
-80, -41, -39, -15, 15, 39, 41, 80 and 111 degrees. The right-hand surface
rotates around its shared edge; its normalized plane and vertices are updated
together. These tests use projection mode 0 and vary size, placement offset,
radius, orientation flags, chain length and winding. Every case agrees with
retail byte for byte at the commit boundary. The two-face sweep produces
3,672 polygons/16,176 vertices; the three-face sweep produces 5,400
polygons/23,388 vertices, each across 5,832 commit boundaries.

The three-face fixture divides the angled surface into two adjacent polygons.
Helper-call traces also agree. It executes 1,312 additional quad constructions
in 656 cases, establishing that the secondary rotated-projection path runs;
432 of those cases emit at least three polygons. Thus the check includes
multi-face output after rotated projection, not just angle rejection or empty
results. This remains a bounded synthetic fixture rather than evidence for
arbitrary mesh topology or every projection mode.

Full checking against main `588746a4` preserves all 7,500 baseline matches,
including caller `0x1516c0`, with no gains or losses. The predicate remains
unmatched (271/271 bytes); equal sizes are not an exact-match claim.
Validated image SHA-256:
`bb565a283c842da5579964ceaf2c1e80427acffe2b8d5d878d870b752378f7d2`.
These local differential probes establish synthetic geometry behavior only;
real assets, GPU output and complete gameplay remain unverified. Actual commit
is tested separately below. Angled-surface coverage is limited to the fixtures described above.

## Actual commit and packed geometry

A local differential probe now executes `0x180d80` and its actual packing,
color and transformation helpers. Allocation, record creation, buffer lookup,
lighting and spatial registration are controlled at independently disassembled
call boundaries. The test compares return value, full synthetic memory, packed
output and helper arguments, including the squared registration radius.

The initial 1,024-case sweep found 131 radius mismatches while packed vertices
and persistent memory agreed. Retail computes `(z*z + y*y) + x*x`; the compiled
source reassociated this as `(x*x + z*z) + y*y`. A `volatile real` partial sum
in `src/unknown_180d80.cpp` forces the intermediate single-precision rounding
before the final term. No compiler flags change. The fix prioritizes the
observed arithmetic contract; it is not a byte-match claim.

All 1,024 cases then agree: 557 successful commits, 467 early or dependency
failures, 334 transformed successes, 226 allocation-cleanup calls and 371
lighting calls. Cases vary polygon sizes, texture coordinates, transforms,
retained random state, color modes and allocation/record/buffer failure stages.

The traversal/preparation/projection integration probe also runs with actual
commit enabled. The 648 flat cases agree, with 900 commit calls and 504
registered records. The 2,916 angled three-face cases agree, with 3,888 commit
calls, 1,944 registered records, 5,400 polygons and 23,383 generated vertices.
It compares final packed buffers, record and persistent state, seed, helper
traces, and a hash of record/buffer bytes at every registration boundary.
Secondary rotated projection executes in 656 cases, including 432 with at
least three polygons. Unlike the earlier mock, actual empty commits terminate
the chain and actual commits control random-state reuse; counts therefore
differ from the mocked-commit sweeps.

The full checker still preserves all 7,500 matches against `588746a4`, with no
gains or losses. Commit remains unmatched at 2,690/2,683 compiled/retail bytes.
The image used for all actual-commit tests has SHA-256
`3c0520f37c68bf5ee68e2c74e7bf23a249e2597a00692a9791ac3c8bbe575db7`.
This covers generated and packed synthetic geometry, not the actual allocator,
lighting, spatial indexing, asset streaming, GPU submission or gameplay.

## Spatial references and initial record creation

A spatial-registration probe executes `0x180b60` with controlled cluster search
and allocation. The 256-case boundary sweep agrees with retail. Enabling actual
reference copying (`0x17d100`) and cell linking (`0x17ce00`) passes 1,024 cases
with 384 queries and 16,212 allocation attempts. Comparisons include copied
records and salts, cell heads and links, fading/permanent counts, and query
state. Cases cover zero radius, missing clusters, skipping the original cluster,
512-entry clamping and allocation failure at several positions. BSP traversal
and record-pool allocation remain controlled.

Testing initial record creation exposed a separate discrepancy in `0x17cfa0`.
For cluster 0/type 1, retail writes record offsets +3/+0xa as 1/0 and selects
cell-head offset +0x800. The previous source wrote 0/1 and selected +4. Retail's
call to `decal_link` receives EBX=cluster and EDX=type; the compiled call had
those roles reversed. The definition now names its third/fourth arguments
`cluster_index`/`decal_type` and calls
`decal_link(cluster_index, decal_type, decal_index)`. The sole source caller in
`0x180d80` retains its argument order; its forward declaration is named
consistently. The matching `decal_link` body is unchanged.

All 40 direct initial-record cases now agree, varying five cluster indices,
four types and permanent/nonpermanent records. Only record-pool allocation is
mocked; low fading counts avoid the eviction path. The integrated 648 flat
cases also agree with actual initial record creation, commit, spatial
registration, reference copying and cell linking enabled. They issue 504
spatial queries and 1,512 pool allocations: 504 initial records and 1,008
secondary references. Final records, cell lists and counters, packed buffers,
state, seed, helper traces and per-registration snapshots are compared.

Full checking preserves all 7,500 baseline matches with zero gains/losses.
Record creation remains unmatched at 355/349 compiled/retail bytes. These tests
use image SHA-256
`cef349f727f83f7295b1d4f698fe004e632ef2098a26ef5f05f2926c11a34857`.
Geometry allocation, buffer lookup, lighting, BSP cluster search and record-pool
allocation are still controlled. This does not establish actual resource
management, eviction, asset streaming, GPU submission or gameplay.

## Actual record-pool allocation

The allocator and release routines execute without helper mocks in 24
lifecycle sequences. All 2,715 allocation attempts and 2,189 releases agree
with retail, including 340 full-pool returns. Capacities 1, 2, 31, 32, 33 and
64 cover bitmap boundaries; starting salts 0x8000 and 0xfffd cover salt
wraparound. The sweep compares handles, pool metadata, bitmap and all record
bytes after every operation, with released-record poisoning both enabled and
disabled. Pool backing memory is synthetic; no process or platform allocation
is established by this test.

The integrated flat-surface probe now also executes actual record-pool
allocation. All 648 cases pass with a 10-record pool, producing 504 registered
records through 1,512 pool allocation attempts. Another 648 cases pass with a
2-record pool, producing 252 registered records through 882 allocation
attempts. The small pool exercises failed secondary-reference allocation and
failed initial-record allocation on later chain links. Metadata, bitmaps,
record storage, cell lists, per-definition lookup, packed geometry and state
are compared; both builds terminate the affected chains consistently.

These runs use the same `cef349f7...` image identified above. No production
source change was needed. Remaining controlled dependencies are geometry
allocation, geometry-buffer lookup, lighting and BSP cluster search; texture
availability is bypassed. Actual asset streaming, GPU submission and complete
gameplay remain unverified.

## Actual portal cluster search

A separate probe executes `0x14a5b0` and its actual portal-plane, sphere and
polygon intersection helpers on synthetic chain, cycle and diamond graphs.
All 384 cases agree with retail, including 152 cases reaching multiple clusters
and 164 cases whose total exceeds output capacity. Comparisons cover the
returned total, bounded output and untouched canaries, visited markers, and
fixture memory. Cases vary sphere position/radius and output capacity; no
portal helper is mocked.

The integrated 648 flat decal cases also pass with actual portal traversal
enabled and a real 16-record pool. There are 504 committed records, 1,372 query
invocations including recursion, and 1,372 pool allocation attempts. Pool
metadata/bitmap, cell lists and counters, visited markers/pass counter,
per-definition lookup, all records, packed output and persistent state agree.
No production change was needed; the image remains `cef349f7...` above.

Remaining controlled dependencies in this integrated configuration are geometry
allocation, geometry-buffer lookup and lighting. Texture availability remains
bypassed. This demonstrates behavior on the specified synthetic portal graphs,
not arbitrary real-map traversal, asset streaming, GPU submission or gameplay.

## Buffer lookup, command bytes and lighting wrapper

Actual geometry-buffer lookup (`0x439d0`) agrees with retail in 54 cases varying
block index, page offset, page shift and usage clock. It updates the selected
block's timestamp, computes the resource address with its high address bit,
and emits the vertex-cache command words `0x41710, 0` into a synthetic
nonexhausted command buffer. Both the bytes and the command cursor are checked.
No flush helper is mocked; command-buffer refill and GPU execution are not
covered.

The integrated 648-case run now uses this actual lookup, high-address buffer
storage, command generation and actual lighting wrapper (`0xd47d0`), alongside
the previously enabled portal/record/pool logic. It passes with 504 committed
records and 1,372 recursive query invocations/pool allocation attempts. Resource
metadata, command storage and cursor are compared in addition to packed
geometry and persistent state. The image remains `cef349f7...` above; no
production source changes were needed.

The lighting wrapper is already byte-matching and always returns false. These
fixtures provide a valid fallback color and no lightmap (`g_4e0344 == 0`), so
the wrapper runs without exercising lightmap lookup/sampling. Earlier mocked
lighting-success cases were forced caller-branch tests, not evidence that the
actual wrapper returns true. The commit therefore keeps its default brightness
in this actual-wrapper configuration.

Only geometry allocation remains mocked at a game-routine boundary in this
configuration. Backing memory, resource metadata and command storage are still
synthetic; texture availability is bypassed. Full lightmap sampling, actual
assets, command-buffer refill, GPU execution and complete gameplay remain
unverified.

## Physical geometry allocation

Actual physical allocation/deletion and their record-pool helpers pass nine
lifecycle sequences: 990 allocation attempts, 162 explicit deletions, 345
failed allocations and 450 evicted blocks. Synthetic arenas vary page capacity,
allocation policy, requested size, type and time. Comparisons include returned
handles, manager state, block records and allocation bitmaps after every step.
Busy/delete callbacks are absent in these fixtures, so callback effects are not
covered by this allocator result.

The integrated 648 flat decal cases also pass with actual physical allocation
and cleanup enabled. This configuration no longer replaces any reached game
routine with a mock. It executes allocation, preparation, projection, packing,
buffer lookup, initial record creation, portal search, reference copying and
cell linking, using real generated handles and block offsets. There are 504
committed records and 1,372 recursive cluster-query invocations/decal-pool
allocation attempts. Physical manager/block/bitmap state, high-address geometry
storage, command storage/cursor, decal records/cells, lookup, seed and persistent
state agree with retail. No production changes were required; the image
remains `cef349f7...` above.

This is a bounded synthetic scenario: the physical arena has ample space for
the short chains, callbacks are absent, lighting uses the no-lightmap path,
texture availability is bypassed, and command storage does not refill. It
verifies vertex-cache command generation, not GPU execution or draw output.
Real-map/assets, rendering and complete gameplay remain unverified.

Shared changes are limited to the new header, the predicate definition and
callers in `src/unknown_11cb00.cpp`, `src/unknown_1502e0.cpp`,
`src/unknown_1689b0.cpp`, `src/unknown_17d9a0.cpp`, `src/ai.cpp`, and
`src/unknown_1ddda0.cpp`, the precision correction in `src/unknown_180d80.cpp`, record-creation parameter
semantics in `src/unknown_17d2a0.cpp`, and the previously removed placement stub in
`src/stubs/lane_r.cpp`. No compiler flags or generated inventory are included.

Sources: independent disassembly of the project's SHA-256-pinned retail
executable and the existing CC0 repository source and inventory. No leaked
symbols or proprietary SDK implementation is used. AI assistance: Codex.
This analysis is released under CC0 1.0.

## Texture availability through actual cache lookup

The runner's `decal-textures` suite runs the full synthetic allocation/projection/commit path with availability
checks enabled. It crosses the 648 geometry combinations with three routes:
a texture remembered for a future frame, a raw texture, and a resident
level-zero cache entry. The latter two execute actual `0x12ccf0`; the first
bypasses the helper through the actual preparation branch. Unexpected fallback
is rejected instead of replaced by a successful stand-in.

All 1,944 cases pass on the previously checked image
`cef349f727f83f7295b1d4f698fe004e632ef2098a26ef5f05f2926c11a34857`:
2,700 commits, 2,160 polygons, 9,577 vertices, 1,512 registered records, and
4,116 portal-query invocations (including recursion). The test compares full
bitmap and synthetic cache state plus normalized helper traces, alongside the
existing geometry/record comparisons. Independent assertions check the texture
pointer, resident frame update to current+2, physical block timestamp, and
lookup count: none for a frame hit, one per commit for a raw texture, and one
initial lookup for a resident texture subsequently remembered by the bitmap.

The check-enabling zero argument also propagates to commit, as in retail; it
is not forced back to the default suite's nonzero value. The default suite
continues to cover its original flag and bypassed texture path. Texture
resources are synthetic, lighting has no lightmap, and command storage does
not refill. This verifies neither missing-texture fallback nor shared textures,
streamed loading, GPU texture sampling, real assets, or gameplay. No C++ or
compiler flags changed for this validation extension.
