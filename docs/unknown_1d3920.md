# Node matrices from a Havok component's rigid bodies (unknown_1d3920)

Retail range covered: `0x1d3920`–`0x1d3d1e`. This is one inventory entry,
1,023 retail bytes, `todo` with no source: `function_1d3920`, which writes an
object's node matrices for the nodes its Havok component's rigid bodies drive.
**Analysis only:** this document adds no source, and nothing in it has been
built or checked against retail with the original compiler. Names are
provisional. The `function_<va>` form is primary; the descriptions are offered
for whoever decompiles it.

**What it does.**

1. It returns at once when the component's `flag9` is set or it has no rigid
   bodies.
2. It finds the object's physics model and render model, and the object's
   orientation block.
3. For each rigid body, it walks the body's `nodes` entries (`node_count` of
   them). Each entry indexes the physics model's `bodies`, a link whose `node`
   is a node that the body drives. It skips an entry when:
   - the link has no node;
   - the body is keyframed (the test of `havok_component_rigid_body_keyframed`);
   - the node is already set in the caller's mask;
   - the node lies below the body's first node in the node tree.
4. For each node it keeps, it writes the node's matrix:
   - the body's matrix (`havok_component_rigid_body_matrix_get`);
   - plus the third argument on `position.z`;
   - unless the node is the body's first, composed with the body shape's frame
     for that entry (`c_component_child_frames::get_frame`).
5. It sets the node in the caller's mask, and the node and all its ancestors in
   a local mask.
6. If it wrote any node and the render model has nodes, it calls
   `function_1d3d20` from node 0 with both masks and the orientation block.

It uses the names of:

- `include/unknown_1cec30.h`:
  - `s_havok_component` (`flag9`, `transformed`, `object_index`, `transform`,
    `rigid_bodies`);
  - `s_havok_component_rigid_body` (`rigid_body`, `nodes`, `node_count`);
  - `hkRigidBody` (`m_motion`), `c_havok_body_motion_state` (`kind_code`), `hkTransform` and
    `hkVector4`.
- `src/unknown_1cec30.cpp`:
  - `s_component_transform_definition` (`model_index`),
    `s_component_transform_model` (`render_model_index`) and
    `s_component_collision_model_link` (`physics_model_index`);
  - `s_component_node_object` (`definition_index`, `matrices_offset`) and
    `component_node_matrices`;
  - `s_component_node_body_model` (`bodies`, `nodes`),
    `s_component_node_body_link` (`node`), `s_component_node_link` (`parent`,
    `sibling`, `child`) and `s_component_node_render_model`;
  - `c_component_child_frames` (`get_frame`), `s_component_child_frame`
    (`transform`) and `component_frame_matrix`;
  - `rigid_transform_scaled`, `s_havok_node_states`, `s_component_node_parent`
    and `s_component_node_physics`;
  - `c_component_body_state`;
  - `havok_component_rigid_body_keyframed`,
    `havok_component_rigid_body_matrix_get`, `havok_component_node_states_get`
    and `_set`, `function_1d3d20`, `function_1d83e0` and `function_1d43d0`.
- `include/slot_handler.h`: `node_matrices_offset` and `object_node_matrix`.
- `include/unknown_0259d0.h`: `transform4x3f`.
- `include/globals.h`: `g_4e0300` and `g_4e3b44`.
- `src/unknown_182d90.cpp`: `c_extent_shape` (`child`).
- `src/unknown_16d280.cpp`: `s_render_model_definition` (`node_count`).
- `src/unknown_0b8ca0.cpp`: `s_object_view` (`unknown110`, `unknown112`),
  `s_object_node_orientation` and `function_ba350`.
- `src/unknown_0bbf40.cpp`: `s_object_query_bbf40` (`orientations_offset`) and
  `function_bf5d0`.
- `src/unknown_1d9240.cpp`: `s_blend_orientation`.
- `src/unknown_20aa70.cpp` and `src/unknown_0b9fc0.cpp`: `s_type_1a7926`
  (`node_indices`) and `function_20a9a0`.
- `src/unknown_0b7740.cpp`: `function_bfa40`.
- `src/unknown_054fe0.cpp` and `src/unknown_14b560.cpp`:
  `voice_update_local_properties` and `function_14c320`.
- `src/unknown_1428b0.cpp`: `function_142a60`.
- `src/unknown_0b9fc0.cpp`: `machine_keyframe_rigid_bodies` and
  `machine_release_rigid_bodies`.
- `src/bipeds.cpp`: `function_e1670` and `function_e0d70`.
- For the callers: `src/unknown_0bbf40.cpp` (the draft of `0xbc820`),
  `src/unknown_0b7740.cpp` (the disabled draft of `0xbd090`) and
  [part 2](unknown_0bc190.md) of the object documents.

## Boundary

- **Before:** `0x1d3880` (159 bytes, `todo`, `src/unknown_1cec30.cpp`) is
  excluded. It ends with `ret 8` at `0x1d391c`, and `0x1d391f` is `int3`
  padding.
- **After:** `0x1d3d1f` is `int3` padding. The next entry is `0x1d3d20`:
  `function_1d3d20`, `todo`, with a draft in `src/unknown_1cec30.cpp`, which
  this function calls.
- **Lanes:** lane C's row of the Active claims table covers the function (issue
  #9: `0x1c0000`–`0x1dffff`, on the second machine). Both callers, `0xbc820`
  and `0xbd090`, are in lane AB's range (`0x0b3d30`–`0x0c42df`). No `@user`
  row covers it. This document makes no claim and is offered to lane C.
- **Open pull requests:** none changes the function. #255's diff shows
  `function_1d3920` only in unchanged context and in a copy of the disabled
  `0xbd090` draft that main already has.
- **Existing source:** `0x1d3920` has no `@retail` marker at `f0aa591`. It has
  a stub and two declarations; see [Existing declarations](#existing-declarations).
- **Likely home:** both neighbours have drafts in `src/unknown_1cec30.cpp`, and
  so do the helpers it shares with `function_1d83e0`. That file is the likely
  home (inferred).

## Conventions

"Callers" counts the functions that call an entry directly, from outside the
range and then from inside it.

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x1d3920` | 1023 | stack: the component, the node mask, a real (`ret 0xc`) | void (inferred) | 2 + 0 | Writes the node matrices that the component's rigid bodies drive |

**The frame.** It is an `ebp` frame aligned to 16 bytes:
`push ebp; mov ebp, esp; and esp, 0xfffffff0; sub esp, 0x184`
(`0x1d3920`–`0x1d3926`), then the pushes of `ebx` (`0x1d392c`), `esi`
(`0x1d3938`) and `edi` (`0x1d3939`). The one exit restores `esp` from `ebp`
(`0x1d3d19`) and returns with `ret 0xc` (`0x1d3d1c`).

- The component is at `[ebp+8]`, the mask at `[ebp+0xc]` and the real at
  `[ebp+0x10]`.
- Below, L is `esp` after the three pushes. The alignment pads by an unknown
  amount, so locals are given from L.
- **Register inputs:** there are none. `eax` is written at `0x1d3930` before
  it is read, and `ecx` and `edx` later.
- **Return value:** nothing is returned (inferred `void`). Neither caller
  reads `eax` after the call (`0xbc8e1`, `0xbd1ac`).

| Local | Type | Use |
| --- | --- | --- |
| `L+0x17` | bool | Set when any node was written (`0x1d3c3d`) |
| `L+0x18` | long | `j`, the entry's position in the body's `nodes` |
| `L+0x1c` | long | `i`, the rigid body's index |
| `L+0x20` | `s_component_node_body_model *` | The physics model; `edi` holds it too, except while it holds a parent |
| `L+0x24` | long | `i` × 0x60, the body's offset in `rigid_bodies.data` |
| `L+0x28` | long | The number of orientations, or 0 |
| `L+0x2c` | `s_component_node_body_link *` | The body's first link: `bodies[nodes[0]]` |
| `L+0x30` | `rigid_transform_scaled *` | The object's orientations, or null |
| `L+0x34` | `s_component_node_render_model *` | The render model |
| `L+0x38` | long | The first link's `node`, sign-extended. It is stored for each entry that reaches the descendant test (`0x1d3ac0`) and read back as a word (`0x1d3ae0`), likely a compiler spill |
| `L+0x3c` | `dword[8]` | The local mask: one bit per node, zeroed at `0x1d39b1`–`0x1d39cf` |
| `L+0x5c` | `transform4x3f` | The frame's matrix: `scale` 1.0 at `L+0x5c`, then `forward`, `left`, `up` and `position` at `L+0x60`, `L+0x6c`, `L+0x78` and `L+0x84` |
| `L+0x90` | 0x100 bytes, the rest of the frame | The buffer for `get_frame`. `function_1d83e0` passes an `hkVector4[16]`, which is the same size |

**Registers in the loops.**
- `ebx` holds the component, then each link (`0x1d3a71`). It reloads the
  component after the inner loop (`0x1d3cc9`).
- `edi` holds the physics model, and each node's parent from `0x1d3afa`. It
  reloads the model from `L+0x20` (`0x1d3ca6`).
- `esi` holds the node's matrix from `0x1d3b29` to the compose (`0x1d3c1d`);
  the masks then reuse it.

## Data

### The component and its bodies

`s_havok_component` is defined at `include/unknown_1cec30.h:381`,
`s_havok_component_rigid_body` at `:254`, and `hkRigidBody` at `:170`.

| Where | Name | Use here |
| --- | --- | --- |
| Component `+0x04` | `flag9` (bit 9) | When set, the function returns. Retail tests it with `shr eax, 9; test al, 1` (`0x1d3933`–`0x1d3936`), the form of a one-bit field. `0x1d6ad0` sets the bit when the component has exactly one rigid body and that body gives no node (no nodes, or a first link with node NONE), and clears it otherwise (`0x1d6b40`–`0x1d6b6b`) |
| Component `+0x08` | `object_index` | The object (`0x1d394b`, `0x1d3b02`) |
| Component `+0x70`, `+0x74` | `rigid_bodies.data`, `.size` | The outer loop (`0x1d3a20`, `0x1d3cd0`) |
| Body `+0x40` | `rigid_body` | Its `m_motion` (`+0x3c`) for `kind_code` (`0x1d3a7d`–`0x1d3a86`), and its shape at `+0xc` for `get_frame` (`0x1d3b5d`–`0x1d3b73`) |
| Body `+0x48`, `+0x4c` | `nodes` (chars), `node_count` | The inner loop (`0x1d3a29`, `0x1d3a3b`, `0x1d3a60`, `0x1d3cb8`) |

### The object and its tags

Retail reaches each through the object header (`g_4e0300`) and the tag
instances (`g_4e3b44`), at `0x1d394b`–`0x1d39ab`:

| Value | Path | Repository name |
| --- | --- | --- |
| The object | `[g_4e0300->data + index × 12 + 8]` | `s_component_node_object` (`src/unknown_1cec30.cpp:1619`) |
| The definition | the object's `+0` | `definition_index` |
| The model | the definition's `+0x38` | `s_component_transform_definition::model_index` (`:50`) |
| The physics model | the model's `+0x24` | `s_component_collision_model_link::physics_model_index` (`:1145`) |
| The render model | the model's `+4` | `s_component_transform_model::render_model_index` (`:56`) |

**The object's blocks.**
- **Orientations.**
  - A 16-bit size at `+0x110` and offset at `+0x112`.
    `s_object_view` (`src/unknown_0b8ca0.cpp:69`–`70`) names them
    `unknown110` and `unknown112`.
  - When the offset is not NONE, `L+0x30` = object + offset, and `L+0x28` =
    the size divided by 32 as unsigned (`movsx`, then `shr eax, 5`; a signed
    `short >> 5` would be `sar`) (`0x1d39d3`–`0x1d3a06`).
  - The elements are `rigid_transform_scaled` (`src/unknown_1cec30.cpp:1612`,
    32 bytes). The draft of `function_bfa40` writes the same count as
    `(dword)(long)*(short *)(object + 0x110) / sizeof(rigid_transform_scaled)`
    (`src/unknown_0b7740.cpp:752`).
- **Node matrices.** The 16-bit offset at `+0x116`
  (`s_component_node_object::matrices_offset`; `node_matrices_offset` in
  `include/slot_handler.h:431`) gives the 0x34-byte `transform4x3f` of each
  node (`include/unknown_0259d0.h:46`). Retail scales the node at `0x1d3af7`
  and adds the offset at `0x1d3b1b`–`0x1d3b27`. That is what
  `component_node_matrices` (`src/unknown_1cec30.cpp:1658`) and
  `object_node_matrix` (`include/slot_handler.h:460`) do.

**The physics model** (`s_component_node_body_model`,
`src/unknown_1cec30.cpp:2303`):
- `bodies` (`+0x3c`): 0x90-byte links (`s_component_node_body_link`,
  `:2290`), whose `node` (`+0`) is the node that a body drives.
- `nodes` (`+0xcc`): 12-byte records, whose `parent` (`+6`) gives the node
  tree. `s_component_node_link` (`:1626`) also names `sibling` (`+8`) and
  `child` (`+0xa`).

**The render model.** The function reads only its `+0x48`, which must be
above 0: `node_count` in `s_render_model_definition`
(`src/unknown_16d280.cpp:44`), just before `nodes` (`+0x4c`), which
`s_component_node_render_model` (`src/unknown_1cec30.cpp:1648`) also names.

## The function

**Control flow.**
- **Blocks.** It has 33 basic blocks and one `ret 0xc`.
  - Two blocks are alignment padding that never runs (`lea ecx, [ecx]` at
    `0x1d3a1d` and `0x1d3a5d`).
  - A third, `lea esp, [esp]` at `0x1d3c69`, runs: it falls into the ancestor
    loop.
- **Stack depth.** A stack-depth walk finds the same depth at every join and
  depth 0 at the `ret`, provided `kind_code` pops nothing and `get_frame` pops 8
  (its two arguments).
  - With `get_frame` popping 0, 4 or 12, the depths disagree at the join
    `0x1d3c22`.
  - With `kind_code` popping 4, they disagree at `0x1d3caa`.

### Setup (`0x1d3920`–`0x1d3a17`)

1. **Early exits.** It returns when `flag9` is set (`0x1d393a`) or
   `rigid_bodies.size` is not above 0 (`0x1d3945`).
2. **The tags.** It finds the object and its tags (see above) and keeps the
   physics model in `edi` and `L+0x20`, and the render model in `L+0x34`.
3. **The masks.** It zeroes the local mask and `L+0x17`.
4. **The orientations.** It reads the orientation block into `L+0x28` and
   `L+0x30`.
5. **A redundant test.** The outer loop's first test re-reads
   `rigid_bodies.size` (`0x1d3a0a`–`0x1d3a11`). After `0x1d3945` it cannot
   exit.

### The rigid bodies (`0x1d3a20`–`0x1d3ce1`)

For each body `i` (`L+0x1c`, with `L+0x24` = `i` × 0x60):
1. **The first link.** `L+0x2c` = `&bodies[nodes[0]]`, read before the node
   count is tested (`0x1d3a20`–`0x1d3a37`).
2. **The node count.** When `node_count` is not above 0, it goes to the next
   body. It tests the count twice: at `0x1d3a3b`–`0x1d3a40`, then again with
   `j` = 0 (`0x1d3a49`–`0x1d3a55`).
3. **The entries.** For each `j` below `node_count` (`L+0x18`; the count is
   re-read each time, `0x1d3cb8`), the steps below.

**The skips.** It goes to the next entry (`0x1d3caa`) when:
1. **No node.** The link `&bodies[nodes[j]]` (in `ebx`) has `node` NONE
   (`0x1d3a73`).
2. **A keyframed body.** `rigid_body->m_motion->kind_code()` returns 6
   (`0x1d3a86`–`0x1d3a8c`).
   - That is the body of the matched `havok_component_rigid_body_keyframed`
     (`0x1d0c00`, `src/unknown_1cec30.cpp:752`). Retail cannot tell whether
     the call was inlined or the test written out. The matched
     `machine_release_rigid_bodies` writes it out
     (`src/unknown_0b9fc0.cpp:640`) and compiles to the same `call`/`cmp`
     (`0x115ccc`–`0x115ccf`).
   - The test is inside the entry loop, though it depends only on the body.
   - The repository's machine code makes the same test for the same bodies:
     - `machine_keyframe_rigid_bodies` (`0x115b10`,
       `src/unknown_0b9fc0.cpp:591`) moves them to the machine's nodes;
     - `machine_release_rigid_bodies` (`0x115c70`, `:630`) stops them.
   - So for these bodies the nodes drive the body, and this function leaves
     their nodes alone (inferred).
3. **Already set.** The node's bit is set in the caller's mask: `mask[node >>
   5] & (1 << (node & 31))` (`0x1d3a92`–`0x1d3aac`).
4. **Below the first node.** The first link's `node` is a proper ancestor of
   this node (`0x1d3ab2`–`0x1d3ae8`).
   - It walks `parent` up from the node (`0x1d3ac7`–`0x1d3adc`). Reaching
     NONE keeps the entry.
   - Reaching the first link's node keeps it only when that is the node
     itself (`0x1d3ae0`–`0x1d3ae8`); otherwise the entry is skipped.
   - The test's own check of the node against NONE (`0x1d3ab4`) cannot
     succeed after skip 1.

**The matrix.**
1. **The parent.** `edi` = `nodes[node].parent` (`0x1d3aee`–`0x1d3afa`).
2. **The node's matrix.** `esi` = the object's node matrix for `node`
   (`0x1d3af7`, `0x1d3aff`–`0x1d3b29`). It looks the object up again from
   `component->object_index` for each entry, rather than reusing the setup's
   pointer (`0x1d3aff`–`0x1d3b17`).
3. **The body's matrix.** `havok_component_rigid_body_matrix_get(i,
   component, esi)` (`0x1d3b2f`).
   - It writes the body's transform, and a `scale` of 1.0.
   - When the component's `transformed` is set, it applies the component's
     `transform` first.
4. **The third argument.** `esi->position.z += value` (`0x1d3b34`–`0x1d3b42`).
5. **The frame.** When the link's `node` differs from the first link's
   (`0x1d3b47`–`0x1d3b4d`):
   1. **Get the frame** (`0x1d3b53`–`0x1d3b73`). The shape at
      `rigid_body+0xc` returns its frame for the entry:
      `get_frame(j, L+0x90)`.
   2. **Build its matrix** (`0x1d3b76`–`0x1d3c14`). `L+0x5c` is the frame's
      `transform` (`+0x10`) as a matrix, with `scale` 1.0 (`0x45dbc0`). That
      is what `component_frame_matrix` (`src/unknown_1cec30.cpp:2382`) does.
   3. **Compose** (`0x1d3c1d`): `function_142a60(esi, L+0x5c, esi)`.

   `function_1d83e0` (`0x1d83e0`, `todo`, `src/unknown_1cec30.cpp:2392`)
   makes the same `get_frame` call on the same shape, and builds the matrix
   with `component_frame_matrix`. It skips the frame when the entry's index is
   0, where this function compares the link's `node` with the first link's.

**The masks** (`0x1d3c22`–`0x1d3ca4`).
1. It sets the node's bit in the caller's mask and sets `L+0x17`.
2. It sets the node's bit in the local mask.
3. It sets the bit of every ancestor in the local mask, following `parent`
   from `edi` until NONE (`0x1d3c70`–`0x1d3ca4`).

### The rest of the tree (`0x1d3ce7`–`0x1d3d11`)

When `L+0x17` is set and the render model's `node_count` is above 0, it calls
`function_1d3d20`:
- the component in `ecx`;
- pushed, in parameter order: 0 (the node to start from), the caller's mask,
  the local mask, the orientations (`L+0x30`) and their count (`L+0x28`).

The draft of `function_1d3d20` (`src/unknown_1cec30.cpp:1665`) handles each
node that has a parent; the root it starts from is never changed:
- **A node in `updated`** (the caller's mask) gets its orientation relative to
  its parent stored, when there are orientations.
- **A node only in `restore`** (the local mask) is an ancestor of a written
  node, not written itself.
  - It is reset to the render model's default pose under its parent, and set
    in `updated`.
  - When there are orientations, it is also given that default orientation.

It then recurses over `child` and `sibling`. The count is only passed on to
the recursive calls.

## Callees

| Retail | Declared as | Where | Status | What retail shows here |
| --- | --- | --- | --- | --- |
| `0x1d08e0` | `void havok_component_rigid_body_matrix_get(long rigid_body_index, s_havok_component *component, transform4x3f *matrix)` | `src/unknown_1cec30.cpp:638` | `matched` | The index in `eax`, the component in `ecx`, the matrix in `esi` |
| `0x142a60` | `int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result)` | `src/unknown_1428b0.cpp:233` | `matched` | `a` in `ecx`, `b` in `edx`, `result` pushed |
| `0x1d3d20` | `void function_1d3d20(s_havok_component *component, long node_index, dword *updated, dword const *restore, rigid_transform_scaled *orientations, long unknown)` | `src/unknown_1cec30.cpp:1665` | `todo` | The component in `ecx`, the other five pushed; it ends with `ret 0x14` |
| `0x1d0c00`, inlined or written out | `bool havok_component_rigid_body_keyframed(long rigid_body_index, s_havok_component *component)` | `src/unknown_1cec30.cpp:752` | `matched` | Its body, at `0x1d3a7d`–`0x1d3a8c` |
| `c_havok_body_motion_state` slot 6 | `virtual long kind_code(void) const { return 0; }` | `include/unknown_1cec30.h:127` | | `this` in `ecx`, nothing pushed |
| shape slot 13 | `virtual s_component_child_frame *get_frame(long index, void *buffer) { return NULL; }`, in `c_component_child_frames` | `src/unknown_1cec30.cpp:2377` | | `this` in `ecx`, `index` and `buffer` pushed |

`c_extent_shape` (`src/unknown_182d90.cpp:26`) names the same slot
`child(long key, void *buffer)`.

## Existing declarations

| Kind | Where | Text |
| --- | --- | --- |
| Stub | `src/stubs/lane_s.cpp:23` | `void __stdcall function_1d3920(byte *component, dword *mask, real value) { }` |
| Declaration, in the draft of `0xbc820` | `src/unknown_0bbf40.cpp:868` | `void __stdcall function_1d3920(byte *component, dword *mask, real value);` |
| Declaration, in the disabled draft of `0xbd090` | `src/unknown_0b7740.cpp:1063` | `void __stdcall function_1d3920(void *component, dword *mask, real amount);` |

**The stub's file.** The stub is at `:22`–`23`. It follows `0xbd090`'s own stub
(`:10`–`11`, under "object callees of 0xb7680", `:8`) and two leftover
comments for other callees (`:14`, `:16`). The file holds "callees of lane S's
region" (`:5`), though both of this function's callers are lane AB's.

**The callers.**
- `0xbc820` (draft at `src/unknown_0bbf40.cpp:960`, call at `:977`) passes
  0.0.
- `0xbd090` (call at `src/unknown_0b7740.cpp:1092`) passes
  `function_e1670(object_index)` for a biped and 0.0 otherwise.
  - The source of `function_e1670` (`src/bipeds.cpp:1956`) describes it as
    "how fast a dead biped's ragdoll falls, while it has fallen over".
  - This function adds that value to each written node's `position.z`.
  - On the `flag9` path, the matched `function_1d43d0` (`0x1d43d0`,
    `src/unknown_1cec30.cpp:98`) likewise adds `function_e0d70`'s value to
    `position.z`.

In retail, both push all three arguments: the real, the mask's address and
the component (`0xbc8d4`–`0xbc8db`, `0xbd1a0`–`0xbd1a6`).

**The stack convention.** Retail keeps a stack convention with callee cleanup.
Yet both callers are LTCG code, and nothing else in retail's image refers to
the function. That is only the second of the three evidence items that
`docs/DECOMPILING.md` asks for before the last-resort `standard` marker
("Standard convention with no visible reason"); the other two need a build.

Some matched functions with an aligned `ebp` frame keep their stack arguments
by taking the parameters' addresses, one of the idioms that section names:
- **In `src/unknown_1cec30.cpp`:** the constructor of `c_component_body_state`
  (`0x1d86d0`, `:1257`, `and esp, 0xfffffff0`, `ret 0x10`).
- **Free functions:** `voice_update_local_properties` (`0x54fe0`,
  `src/unknown_054fe0.cpp:1386`) and `function_14c320`
  (`src/unknown_14b560.cpp:1363`), both with `and esp, 0xfffffff8`.

The draft of `function_1d3d20` uses the same idiom. A declaration from the
repository's own types would be the following; it is offered, not applied.

```
void __stdcall function_1d3920(s_havok_component *component, dword *mask,
    real value);
```

## Findings about existing names

- **`nodes` holds indices into the physics model's `bodies`.**
  `include/unknown_1cec30.h:264` describes the entries as "the object's nodes
  the body drives".
  - In retail, three functions read each entry as an index into the physics
    model's `bodies`, whose `node` is the node:
    - this function;
    - `function_1d83e0` (`src/unknown_1cec30.cpp:2404`; retail
      `0x1d846a`–`0x1d8481`);
    - `machine_keyframe_rigid_bodies` (`0x115b10`, `todo`), which loads
      `bodies` (`+0x3c`) and adds `nodes[0]` × 0x90 (`0x115ba9`–`0x115bc0`).
  - **The draft of `machine_keyframe_rigid_bodies` leaves out that load.**
    - It indexes `matrices.node_indices` directly
      (`src/unknown_0b9fc0.cpp:606`).
    - `function_20a9a0` sets `node_indices` to the physics model itself, not
      to its `bodies` (`src/unknown_20aa70.cpp:45`).
    - Retail adds the `bodies` pointer from `+0x3c` first (`0x115bb6`).
  - `havok_component_node_states_get` and `_set` (`0x1d1a20`, `0x1d1b60`, both
    `todo`, `src/unknown_1cec30.cpp:967` and `:991`) index the arrays of
    `s_havok_node_states` (`:947`) by `nodes[0]` directly, as if it were a
    node index.
- **One model, two names.** The physics model is `s_component_node_body_model`
  (`src/unknown_1cec30.cpp:2303`) and `s_component_node_physics` (`:1634`).
  The draft of `function_1d3d20` uses the second.
- **One slot, two names.** Slot 13 of the shape at `rigid_body+0xc` is
  `c_component_child_frames::get_frame` (`src/unknown_1cec30.cpp:2377`) and
  `c_extent_shape::child` (`src/unknown_182d90.cpp:26`).
- **One record, two names.** `s_component_node_parent`
  (`src/unknown_1cec30.cpp:2296`) and `s_component_node_link` (`:1626`) are
  the same 12-byte record of the physics model's `nodes` block.
- **The object's orientation block.** `s_object_view` leaves `+0x110` and
  `+0x112` as `unknown110` and `unknown112`. Other code already reads them as
  the size and offset of 32-byte orientations:
  - its own file's draft of `function_ba350` divides by
    `sizeof(s_object_node_orientation)` (`src/unknown_0b8ca0.cpp:277`–`280`);
  - `s_object_query_bbf40` names `+0x112` `orientations_offset`
    (`src/unknown_0bbf40.cpp:463`), which the matched `function_bf5d0` tests;
  - the draft of `function_bfa40` takes them as `rigid_transform_scaled`.

  This function reads them the same way. The 32-byte record has three names:
  `rigid_transform_scaled` (`src/unknown_1cec30.cpp:1612`),
  `s_object_node_orientation` (`src/unknown_0b8ca0.cpp:255`) and
  `s_blend_orientation` (`src/unknown_1d9240.cpp:187`).

## Evidence

- **Disassembly.** The function was disassembled with capstone 5.0.7 through
  `tools/xbe.py`. The XBE used has a header edited after signing; its 33
  section digests verify.
- **Stack and control flow.** A control-flow graph walked the stack depth over
  every path from the 16-byte-aligned frame, with each direct callee's stack
  effect from its own `ret` in retail.
- **What was read where.** Offsets, constants and conventions were read from
  retail code and data, and the callees with source from that source.
- **References.** An image-wide scan finds only the two calls, at `0xbc8dc`
  and `0xbd1a7`.
- **Checks.** Three independent reviews checked the document: one against
  retail, one against the repository, and a final one of the finished
  document. Their corrections are applied.
- **Prior coverage.** [Part 2](unknown_0bc190.md) of the object documents
  describes both callers. Source line numbers are at `f0aa591`.
- **Outside sources.** No emulator, runtime testing, SDK or outside dataset was
  used. Names are the repository's own, or describe behaviour.
