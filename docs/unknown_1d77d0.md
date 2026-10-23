# A Havok component's constraints (unknown_1d77d0)

Retail range covered: `0x1d77d0`–`0x1d7eeb`. This is one inventory entry, 1,820
retail bytes, `todo` with no source: `function_1d77d0`, which creates the
constraints of one object's Havok component from its physics model.
**Analysis only:** this document adds no source, and nothing in it has been
built or checked against retail with the original compiler. Names are
provisional. The `function_<va>` form is primary; the descriptions are offered
for whoever decompiles it.

**What it does.**

1. It maps the object's nodes to the component's rigid bodies.
2. It walks every constraint element of the physics model, in the six blocks
   that `physics_model_shape_block_get` selects. It skips an element when:
   - the object's damage section for that element is gone;
   - its first node is NONE;
   - its two nodes resolve to the same body, or its first node to no body;
   - its first body is fixed (`hkRigidBody::m_fixed`);
   - exactly one of its second node and its second body is NONE.
3. For each element it keeps, it computes the two frames and bodies with
   `function_1d7ef0`, and looks up an override strength.
4. It builds the constraint data of the element's kind in a local.
5. It constructs a Havok constraint from that local, in a block from Havok's
   thread memory (`g_480118`).
6. It adds the constraint to the component's `unknown7c` with
   `function_1d0150`.

The six kinds are Havok's hinge, limited hinge, ragdoll, stiff spring, ball
and socket and prismatic constraints: retail's own class-name strings tie each
case to its name (see [Retail's own names](#retails-own-names)).

At the end it sets or clears the component's `flag10` (bit 10 of `+4`). It sets
it when one of the component's constraints has the physics world's body
`function_30c170(g_51e9a4)` as its second entity.

It uses the names of:

- `include/unknown_1cec30.h`:
  - `s_havok_component`, `s_havok_component_rigid_body` and `s_havok_array60`;
  - `s_havok_array0c`, `s_havok_component_element0c` and
    `s_havok_contact_entities` (`entity_a`, `entity_b`);
  - `hkRigidBody` (`m_fixed`), `hkTransform` and `hkRotation`.
- `src/unknown_1cec30.cpp`:
  - `s_component_frame_owner`, `s_component_frame_map`,
    `s_component_frame_pair` and `s_component_collision_model_link`;
  - `s_component_joint_model` and the records it points to
    (`s_component_joint_override_group`, `s_component_joint_override`,
    `s_component_joint_hinge` and `s_component_joint_angles`);
  - `c_component_joint_snapshot_0`, `_1`, `_2` and `_4`;
  - `function_1d0150`, `function_1d7ef0`, `function_30c170`,
    `function_1cff80` and `function_1d35d0`.
- `src/unknown_1eb1b0.cpp`: `s_physics_constraint_iterator`,
  `physics_model_shape_block_get` (which returns an `s_impact_tag_block`),
  `function_1eb110` and `function_1eb160`.
- `src/unknown_181a80.cpp`: `s_list_rigid_bodies` and `function_181bd0`.
- `src/unknown_1ce700.cpp` and `src/unknown_1ce8d0.cpp`: the five builders of
  the constraint data, and the placeholder structs they are members of.
- `src/damage.cpp`: `function_db4c0` and `function_db810`.
- `src/unknown_1c25a0.cpp`: `g_51e9a4`, the physics world.
- `include/globals.h`: `g_480118` (`c_allocator`), `g_4e3b44` and `g_45dbd8`.
- `include/unknown_146a20.h`: `c_havok_abstract_allocator` (`allocate16`).
- `include/havok_reference.h`: `c_havok_reference_counted` (`allocation_size`,
  `reference_count`).
- For the callers and the owner: `src/unknown_1d5940.cpp` (`s_1d5941`),
  `src/unknown_1d6560.cpp` (`s_physics_model_owner`), `src/unknown_20aa70.cpp`
  (`s_type_1a7926`, `function_20a9a0`) and `src/stubs/lane_c.cpp`.

## Boundary

- **Before:** `0x1d7400` (972 bytes, `todo`, `src/unknown_1d7400.cpp`) is
  excluded. It ends with `ret 0xc` at `0x1d77c9`, and `0x1d77cc`–`0x1d77cf`
  are `int3` padding.
- **After:** the function's jump table occupies `0x1d7ed4`–`0x1d7eeb`, the
  last 24 of its 1,820 bytes, and `0x1d7eec`–`0x1d7eef` are `int3` padding.
  The next entry is `0x1d7ef0`: `function_1d7ef0`, `todo`, with source in
  `src/unknown_1cec30.cpp`, which this function calls.
- **Lanes:** lane C's row of the Active claims table covers the function (issue
  #9: `0x1c0000`–`0x1dffff`, AI and actor behaviour and animation graphs, on
  the second machine). No other row and no open pull request touches it. This
  document makes no claim and is offered to lane C.
- **Existing source:** `0x1d77d0` has no `@retail` or `@stub` marker,
  declaration or stub at `5d1a893`; only `config/functions.csv` names it.
- **The caller:** its only caller, `0x1d6ad0` (165 bytes, `todo`), has a stub
  (`src/stubs/lane_c.cpp:636`) and a declaration
  (`src/unknown_1d5940.cpp:53`). The draft of `0x1d5940` calls it there
  (`:123`).
- **Likely home:** `s_havok_component` is in `include/unknown_1cec30.h`. The
  frame structures, `function_1d0150` and `function_1d7ef0` are in
  `src/unknown_1cec30.cpp`, which is the likely home for this function
  (inferred).

## Conventions

"Callers" counts the functions that call an entry directly, from outside the
range and then from inside it.

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x1d77d0` | 1820 | stack: the component, the frame owner (`ret 8`) | void (inferred) | 1 + 0 | Creates a component's constraints from its physics model |

**The frame.** It is an `ebp` frame aligned to 16 bytes:
`push ebp; mov ebp, esp; and esp, 0xfffffff0; sub esp, 0x454`
(`0x1d77d0`–`0x1d77d6`), then the pushes of `ebx` (`0x1d77dc`), `esi`
(`0x1d77dd`) and `edi` (`0x1d77e4`). Both exits restore `esp` from `ebp`
(`0x1d7e90`, `0x1d7ece`).

- The component is at `[ebp+8]` and the owner at `[ebp+0xc]`.
- Below, L is `esp` after the three pushes. The alignment pads by an unknown
  amount, so locals are given from L.
- **Register inputs:** there are none. `eax`, `ecx` and `edx` are written
  before they are read (`0x1d77e1`–`0x1d77f2`).
- **Return value:** nothing is returned (inferred `void`). `eax` holds whatever
  was last loaded at either `ret`. The caller returns straight after each call
  (`pop ecx; ret` at `0x1d6b5f` and `0x1d6b73`), and the repository declares
  the caller `void`.

| Local | Type | Use |
| --- | --- | --- |
| `L+0x14`, `L+0x18` | `hkRigidBody *` | `body_b` and `body_a`, from `function_1d7ef0` |
| `L+0x1c` | `s_physics_constraint_iterator` | `physics` at `L+0x1c`, `type` at `L+0x20`, `index` at `L+0x22` |
| `L+0x24` | real | The override strength (see [The override](#the-override)), or 0.0 |
| `L+0x28`, `L+0x30`, `L+0x34` | real | Kind 2's angles, before `fsin` and `fcos` |
| `L+0x2c` | long | The element's type, sign-extended (`0x1d785e`): the switch index, and `function_1d0150`'s third argument |
| `L+0x38` | long | The element size from `physics_model_shape_block_get` |
| `L+0x3c` | long | The element's index (`0x1d7838`) |
| `L+0x40`, `L+0x80` | `hkTransform` | `frame_a` and `frame_b` from `function_1d7ef0`: `m_rotation`'s `m_col0`, `m_col1` and `m_col2` (forward, left, up) at `+0`, `+0x10` and `+0x20`, and `m_translation` at `+0x30` |
| `L+0xc0`, `L+0xf0`, `L+0x180`, `L+0x1c0`, `L+0x250`, `L+0x2f0` | | The constraint data of kinds 4, 2, 3, 1, 5 and 0: 0x30, 0x90, 0x40, 0x90, 0xa0 and 0x60 bytes, which tile `L+0xc0`–`L+0x350`. The repository's `c_component_joint_snapshot_4`, `_2`, `_1` and `_0` have these sizes |
| `L+0x358` | `s_list_rigid_bodies` | The node map: `count`, then `lists` at `L+0x35c` and `unlisted` at `L+0x45c`. `function_1d7ef0` takes it as an `s_component_frame_map` (`bodies`, `fallback`), which has the same layout |

**Registers held across the loop.** `esi` holds the component at the top of
each iteration: `0x1d77de` at the start, and `0x1d7e39` when the loop
continues. `ebx` holds the element's index (`0x1d7831`); kind 3 reuses it and
reloads it from `L+0x3c` (`0x1d7cfb`).

**The caller.** `0x1d6ad0` calls this function twice: at `0x1d6b5a` and at
`0x1d6b6e`.
- Each time it pushes the owner in `edi` and then the component in `ecx`, so
  the component is the first argument.
- On one path it sets the component's `flag9` (`or eax, 0x200` at
  `0x1d6b51`), and on the other it clears it (`0x1d6b65`), just before the
  call.

**What `0x1d6ad0` does first** (from a review of this document by @mrv777;
checked against retail):
1. It takes the component in `ecx` and the owner in `edi`. `edi` is read at
   `0x1d6aff` without being set.
2. It sets `unknown18` and `unknown19` to NONE (`0x1d6ada`, `0x1d6ade`). The
   slot of its first push holds the smallest node so far, from `0x7fffffff`
   (`0x1d6ae2`).
3. For each rigid body with `node_count` above 0, it takes the `node` of the
   physics model's `bodies[nodes[0]]`:
   - **NONE:** the body becomes `unknown18`, `unknown19` gets NONE, and the
     scan stops (`0x1d6b37`–`0x1d6b3c`).
   - **Below the smallest so far:** the body becomes `unknown18`, and the
     node becomes the new smallest and `unknown19` (`0x1d6b14`–`0x1d6b27`).
4. It sets `flag9` when the component has exactly one rigid body and
   `unknown19` is still NONE, and clears it otherwise (`0x1d6b40`–`0x1d6b4b`).
- `0x1d6ad0` is called only by `0x1d5940` (`todo`, a draft in
  `src/unknown_1d5940.cpp`). It passes the component in `ecx` and the owner in
  `edi` (`0x1d5bb1`–`0x1d5bb7`).
- That owner is `0x1d5940`'s local `[esp+0x38]`. `0x1d5940` first passes the
  same local to the matched `function_20a9a0`, which fills it (`0x1d5993`).

The image-wide scan finds no other reference to `0x1d77d0` or `0x1d6ad0`.

## Data

### The component (`s_havok_component`, `include/unknown_1cec30.h:381`)

| Offset | Name | Use here |
| --- | --- | --- |
| `+0x04` | `unknown04` (`flag9`, `flag10`, …) | `flag10` (bit 10) is cleared before the final loop (`0x1d7e4d`) and set when a constraint holds the world's body (`0x1d7ec4`) |
| `+0x08` | `object_index` | Passed to `function_181bd0` (`0x1d77e1`) and `function_db4c0` (`0x1d7859`) |
| `+0x18`, `+0x19` | `unknown18`, `unknown19` (char) | `unknown18` is read as a body index (`0x1d7e96`); both are set to NONE (`0x1d7ebc`, `0x1d7ec0`) |
| `+0x70` | `rigid_bodies` (`s_havok_array60`) | `data[i].rigid_body` (`+0x40` of a 0x60-byte `s_havok_component_rigid_body`) is read at `0x1d78d4` and `0x1d7eac` |
| `+0x7c` | `unknown7c` (`s_havok_array0c`) | `function_1d0150` appends to it. The final loop reads `data` (`+0x7c`) and `size` (`+0x80`). Each 12-byte element (`s_havok_component_element0c`) holds the type in `unknown00`, the index in `unknown02`, and the constraint in `contact` (`+8`) |

### The owner

The repository has four names for the struct that `0x1d5940` fills and passes
down:
- `s_component_frame_owner` (`src/unknown_1cec30.cpp:2501`), which
  `function_1d7ef0` takes, with `physics` at `+0x48`;
- `s_physics_model_owner` (`src/unknown_1d6560.cpp:45`), which the stub of
  `0x1d6ad0` takes, with `model` at `+0x48`;
- `s_type_1a7926` (`src/unknown_20aa70.cpp:10`), which `function_20a9a0`
  fills (`src/unknown_20aa70.cpp:23`);
- `s_1d5941` (`src/unknown_1d5940.cpp:18`), with `field_48` (`byte *`) at
  `+0x48`.
  - It is the type of the draft's `local_3` (`:63`).
  - The draft passes that local to `function_20a9a0` (`:70`) and to
    `function_1d6ad0` (`:123`).

| Offset | What it holds | Use here |
| --- | --- | --- |
| `+0x44` | The model's tag data, which `function_20a9a0` stores there. Its `+0x24` is `physics_model_index` (`s_component_collision_model_link`, `src/unknown_1cec30.cpp:1145`) | The iterator's `physics` is that tag's data, through `g_4e3b44` (`0x1d77f5`–`0x1d7811`) |
| `+0x48` | The same physics model's tag data, which `function_20a9a0` also stores | The cases read their records through it (`0x1d79ef`, `0x1d7af1`, `0x1d7c94`, `0x1d7d6f`), and the override reads its `+0x34` (`0x1d790b`) |

### The physics model

`physics_model_shape_block_get` (`src/unknown_1eb1b0.cpp:8`) gives each type's
block (count, then address) and element size. `s_component_joint_model`
(`src/unknown_1cec30.cpp:1770`) names two of the blocks' addresses: `angles`
(`+0xbc`) and `hinges` (`+0xec`).

| Type | Block | Element | The kind it builds |
| --- | --- | --- | --- |
| 0 | `+0xb0` | 0x78 | A hinge |
| 1 | `+0xe8` | 0x84 (`s_component_joint_hinge`) | A limited hinge |
| 2 | `+0xb8` | 0x94 (`s_component_joint_angles`) | A ragdoll joint |
| 3 | `+0xf8` | 0x7c | A stiff spring |
| 4 | `+0xf0` | 0x78 | A ball and socket |
| 5 | `+0x100` | 0x84 | A prismatic joint |

**Element layout.** Every element starts with `s_component_frame_pair`
(`src/unknown_1cec30.cpp:2492`, 0x70 bytes):
- `node_a` at `+4` and `node_b` at `+6`;
- `frame_a` at `+8` and `frame_b` at `+0x3c`, each a 0x34-byte
  `transform4x3f`.

After the pair, a short at `+0x70` selects an override group (`0x1d78fb`). The
kind-specific values follow from `+0x78`.

### Retail's own names

The retail executable's strings include `hkHingeConstraint`,
`hkLimitedHingeConstraint`, `hkRagdollConstraint`, `hkStiffSpringConstraint`,
`hkBallAndSocketConstraint` and `hkPrismaticConstraint`.

**The chain from case to name.**
1. Each kind's constructor (see [Callees without source](#callees-without-source))
   stores a vtable at the block's `+0`.
2. Slot 3 (`+0xc`) of that vtable returns a static object.
3. A function in `0x379190`–`0x379310` constructs that object
   (`mov ecx, <object>; call 0x2d9750`), with the string pushed.

| Case | Constructor | Vtable (stored at) | Slot 3 | Object | Built by | String |
| --- | --- | --- | --- | --- | --- | --- |
| 0 | `0x313de0` | `0x41490c` (`0x313e08`) | `0x313b20` | `0x4806a8` | `0x379210` | `hkHingeConstraint` (`0x4148f8`) |
| 1 | `0x313550` | `0x414860` (`0x31357a`) | `0x313310` | `0x480680` | `0x3791d0` | `hkLimitedHingeConstraint` (`0x414840`) |
| 2 | `0x312d50` | `0x4146fc` (`0x312d60`) | `0x3128f0` | `0x480658` | `0x379190` | `hkRagdollConstraint` (`0x4146d0`) |
| 3 | `0x314b00` | `0x414b4c` (`0x314b0e`) | `0x314850` | `0x480720` | `0x3792d0` | `hkStiffSpringConstraint` (`0x414b34`) |
| 4 | `0x314270` | `0x41498c` (`0x31427e`) | `0x3140b0` | `0x4806d0` | `0x379250` | `hkBallAndSocketConstraint` (`0x414970`) |
| 5 | `0x314ef0` | `0x414ca4` (`0x314f1a`) | `0x314ba0` | `0x480748` | `0x379310` | `hkPrismaticConstraint` (`0x414c8c`) |

**Two cross-checks.**
- Each of those functions also pushes a constant equal to the size of the
  kind's local: 0x60, 0x90, 0x90, 0x40, 0x30 and 0xa0.
- The repository's names for the records of types 1 and 2 (a hinge's minimum
  and maximum; twist, cone and plane angles) fit the limited hinge and the
  ragdoll.

### Globals and constants

| Where | Use here |
| --- | --- |
| `g_480118` (`c_allocator`, `include/globals.h:504`) | Its fifth virtual slot (`+0x10`) is called with the size and `0x27` pushed, and returns the new block. It pops both (`ret 8`); the stack depth agrees at every join. `c_allocator` calls the slot `slot4`; the repository's `c_havok_abstract_allocator` names it `allocate16(long size, long memory_class)` (`include/unknown_146a20.h:39`) |
| `g_51e9a4` (`hkWorld *`, `src/unknown_1c25a0.cpp:520`) | The physics world. `function_30c170` (`third:havok`, 4 bytes) returns its `+0x38`: `mov eax, [ecx+0x38]`. `function_1d7ef0` makes that the second body when an element has no second body (`src/unknown_1cec30.cpp:2580`). This document calls it the world's body; that it is Havok's fixed body is inferred |
| `0x45dc54` | π/180, which converts the angle limits to radians |
| `g_45dbd8` (`include/globals.h:507`) | 0.0, compared with the override strength |

## The function

**Control flow.** It has 64 basic blocks and two `ret 8`, at `0x1d7e93` and
`0x1d7ed1`. A stack-depth walk with the allocator slot popping 8 finds the same
depth at every join and depth 0 at both `ret`s. The switch at `0x1d797e`
(`jmp dword ptr [ecx*4 + 0x1d7ed4]`) has no bounds check. Its six entries are
`0x1d7985`, `0x1d79ec`, `0x1d7aee`, `0x1d7c8e`, `0x1d7d04` and `0x1d7d6c`.

### Setup (`0x1d77d0`–`0x1d781a`)

1. It sets up the frame and loads the component into `esi` (`0x1d77de`).
2. It calls `function_181bd0` (`0x1d77ed`) with the component's
   `object_index` in `eax` and `L+0x358` pushed. That fills the node map.
3. It stores the physics tag's data in the iterator (`0x1d7811`) and calls
   `function_1eb110` with the iterator in `edi` (`0x1d7815`), which finds the
   first element.

### The loop (`0x1d7820`–`0x1d7e3c`)

**Each element.** Each iteration:
1. Calls `physics_model_shape_block_get` with the physics data in `eax`, the
   iterator's key (`L+0x20`) in `edx` and `L+0x38` in `ecx` (`0x1d782c`).
2. Reads `index` (`0x1d7831`). The loop ends when the index is not below the
   block's count (`0x1d783c`), or the element (`address + index × size`) is
   null (`0x1d784e`).

**The filters.** The element is skipped, going to `0x1d7e30`, when:
1. `function_db4c0` (`0x1d7862`) returns true, called with the object index
   in `eax` and the type and index pushed. Its source says this tests
   whether the damage section with those keys is in the object's first
   section mask. `function_db810` (`src/damage.cpp:3784`, `todo`) walks the
   same elements with the same iterator and test, and calls the ones it keeps
   "constraints that hold (those whose damage sections are intact)".
2. `node_a` (`+4`) is NONE (`0x1d786f`–`0x1d7877`).
3. The two bodies are equal, or the first is NONE (`0x1d78b4`–`0x1d78c2`).
   The bodies are `lists[node]` of the map, or `unlisted` for a NONE node
   (`0x1d787d`–`0x1d78b4`).
4. The first body's `rigid_body` has `m_fixed` (`+0x40`) set
   (`0x1d78c8`–`0x1d78dc`).
5. `node_b` is NONE without the second body being NONE, or the reverse
   (`0x1d78e2`–`0x1d78f5`).

**A branch that cannot run.** The first body's lookup keeps its branch for a
NONE node (`0x1d7885`), although filter 2 has already skipped that case.
`function_1d7ef0`'s source writes the same lookup as
`node_a == NONE ? mapping->fallback : mapping->bodies[node_a]`
(`src/unknown_1cec30.cpp:2532`).

**The frames** (`0x1d78fb`–`0x1d793b`). It calls `function_1d7ef0` with:
- `L+0x40` in `eax` (`frame_a`);
- pushed, in parameter order: the component, the owner, `L+0x358`,
  `L+0x80` (`frame_b`), `L+0x18` (`body_a`), `L+0x14` (`body_b`) and the
  element (`pair`).

It sets `L+0x24` to 0.0 at `0x1d7935`.

### The override

1. **The group** is `overrides[element +0x70]` of the
   `s_component_joint_model` at the owner's `+0x48`. The short is sign-extended
   and scaled by 24 (`0x1d78fb`–`0x1d790f`). Nothing checks it against NONE or
   `override_count`.
2. **The entries.** That `s_component_joint_override_group` holds `count`
   (`+8`) entries (`s_component_joint_override`, 12 bytes) at `entries`
   (`+0xc`) (`0x1d7940`–`0x1d7949`).
3. **The match.** The first entry whose `type` and `index` equal the
   iterator's gives its `strength` (`+8`) to `L+0x24` (`0x1d7950`–`0x1d7974`).

This runs for every element, but only kinds 1 and 2 read `L+0x24`.

`function_1d35d0` (`src/unknown_1cec30.cpp:1788`, `todo`) runs the same search
over every group. In retail its outer loop goes on after a match, so the last
matching group wins (`0x1d3647`–`0x1d367f`).

### The kinds

**The steps.** Each kind:
1. Initializes its local with a library routine (`ecx` = the local).
2. Fills it with a builder, except kind 4, which fills it inline.
3. Allocates a block of its size from `g_480118` (`allocate16`).
4. Stores the size in the block's word `+4`
   (`c_havok_reference_counted::allocation_size`, `include/havok_reference.h:16`).
5. Constructs the constraint in the block from the local (`ecx` = the block,
   the local pushed). The constructor returns the block.

The draft of `0x1d7400` (`src/unknown_1d7400.cpp:123`–`125`, `todo`) writes
the same allocate, size and construct sequence. Here retail has no null test
between the allocation and the constructor, in any of the six kinds.

**The builders' arguments** (kinds 0, 1, 2 and 5; kind 3's builder differs,
below). The local in `eax`, the first body in `ecx` (`L+0x18`), and the
second body (`L+0x14`) pushed. The vectors follow:
- the two pivots (`m_translation`), `L+0x70` and `L+0xb0`;
- then the two up axes (`m_col2`), `L+0x60` and `L+0xa0`;
- and, for kinds 1, 2 and 5, the two left axes (`m_col1`), `L+0x50` and
  `L+0x90`.

**The six kinds:**

0. **Type 0, a hinge** (`0x1d7985`–`0x1d79e7`).
   - Initialize `L+0x2f0` (`0x313b30`).
   - `function_1ce4f0` with four vectors (`0x1d79bb`).
   - Allocate 0xa0 (`0x1d79cf`); construct (`0x313de0`).
1. **Type 1, a limited hinge** (`0x1d79ec`–`0x1d7ae9`). The record is
   `hinges[index]`: `[physics+0xec]` + index × 0x84 (`0x1d79ef`–`0x1d7a07`).
   - Initialize `L+0x1c0` (`0x313320`).
   - `function_1ce2f0` with six vectors (`0x1d7a48`).
   - **The strength:** `L+0x248` = `L+0x24`, or the record's `strength`
     (`+0x78`) when `L+0x24` is 0.0 (`0x1d7a4d`–`0x1d7a64`).
   - **The limits** (`0x1d7a6d`–`0x1d7ac1`), from `minimum` (`+0x7c`) and
     `maximum` (`+0x80`):
     - `L+0x240` = min(−`minimum`, −`maximum`) × π/180;
     - `L+0x244` = max(−`minimum`, −`maximum`) × π/180.
   - Allocate 0xe0 (`0x1d7ad1`); construct (`0x313550`).
2. **Type 2, a ragdoll joint** (`0x1d7aee`–`0x1d7c89`). The record is
   `angles[index]`: `[physics+0xbc]` + index × 0x94.
   - Initialize `L+0xf0` (`0x312920`).
   - `function_1ce700` with six vectors (`0x1d7b4a`).
   - **The twist:** `L+0x16c` and `L+0x170` = the minimum and maximum of
     `twist_minimum` and `twist_maximum` (`+0x78`, `+0x7c`), × π/180.
   - **The plane:** `L+0x164` and `L+0x168` = the sines of the minimum and
     maximum of `plane_minimum` and `plane_maximum` (`+0x88`, `+0x8c`). Each
     is multiplied by π/180 first: `fmul` at `0x1d7bc7` and `0x1d7bfb`, before
     `fsin` at `0x1d7bcd` and `0x1d7c01`.
   - **The cone:** `L+0x160` = the cosine of the maximum of `cone_minimum`
     and `cone_maximum` (`+0x80`, `+0x84`), × π/180 first (`0x1d7c35`,
     `0x1d7c46`).
   - **The strength:** `L+0x174` = `L+0x24`, or `strength` (`+0x90`) when
     that is 0.0 (`0x1d7c2f`–`0x1d7c61`).
   - Allocate 0xe0 (`0x1d7c71`); construct (`0x312d50`).
3. **Type 3, a stiff spring** (`0x1d7c8e`–`0x1d7cff`). The record is from
   `[physics+0xfc]` + index × 0x7c, in `ebx`.
   - Initialize `L+0x180` (`0x314860`).
   - `function_1ce8d0` (`0x1d7cc4`).
     - It takes the local in `esi`, the first body in `ecx`, the second
       pushed, and the pivots in `eax` and `edi`.
     - Its source stores the distance between the two pivots, each
       transformed by its body's motion, at the local's `+0x30` (`L+0x1b0`).
   - `L+0x1b0` = the record's `+0x78`, which replaces that distance
     (`0x1d7cd6`).
   - Allocate 0x50 (`0x1d7ce3`); construct (`0x314b00`).
   - Reload `ebx` from `L+0x3c` (`0x1d7cfb`).
4. **Type 4, a ball and socket** (`0x1d7d04`–`0x1d7d67`).
   - Initialize `L+0xc0` (`0x3140c0`). It reads no record and calls no
     builder.
   - **Bodies:** `L+0xc0` = the first body and `L+0xc4` = the second
     (`0x1d7d35`, `0x1d7d3c`).
   - **Pivots:** `L+0xd0` = the first pivot and `L+0xe0` = the second
     (`0x1d7d10`–`0x1d7d43`).
   - Allocate 0x60 (`0x1d7d4f`); construct (`0x314270`).
5. **Type 5, a prismatic joint** (`0x1d7d6c`–`0x1d7e1c`). The record is from
   `[physics+0x104]` + index × 0x84, in `esi`.
   - Initialize `L+0x250` (`0x314bb0`).
   - `function_1ce970` with six vectors (`0x1d7dc8`).
   - **Values:** `L+0x2e0` = `+0x78`, `L+0x2e4` = `+0x7c` and `L+0x2e8` =
     `+0x80`, with no override. The initializer's defaults there are
     −FLT_MAX, FLT_MAX and 0, so they are a range and a strength (inferred).
   - Allocate 0xf0 (`0x1d7e09`); construct (`0x314ef0`).

**Adding the constraint** (`0x1d7e21`–`0x1d7e2b`). Every kind continues here.
It calls `function_1d0150`:
- the component in `edx`;
- pushed, in parameter order: the constructor's result (`eax`), the type
  (`L+0x2c`) and the index (`ebx`).

That appends `{type, index, NONE, constraint}` to the component's
`unknown7c`. Then `function_1eb160` advances the iterator (`edi` = `L+0x1c`,
`0x1d7e34`), and the loop continues.

### The world's body (`0x1d7e41`–`0x1d7ed1`)

1. It clears `flag10` (`0x1d7e41`–`0x1d7e53`).
2. If `unknown7c` is empty, it returns (`0x1d7e5a` to `0x1d7ecb`; the `ret` is
   at `0x1d7ed1`).
3. For each element (`0x1d7e60`–`0x1d7e8b`), it compares the constraint's
   `entity_b` (`+0x10`, `0x1d7e70`) with `function_30c170(g_51e9a4)`
   (`0x1d7e73`).
4. If none matches, it returns with `flag10` clear (`0x1d7e93`).
5. On the first match, it checks `unknown18`. If that is not NONE, and
   `rigid_bodies.data[unknown18].rigid_body` equals the constraint's
   `entity_a` (`+0xc`), it sets `unknown18` and `unknown19` to NONE
   (`0x1d7e96`–`0x1d7ec0`).
6. Either way it sets `flag10` (`0x1d7ec4`) and returns (`0x1d7ed1`).

## Callees without source

None of these has source. The initializers and constructors are
`other:library`, and `0x30c170` is `third:havok`.

| Retail | In the repository | What it does here |
| --- | --- | --- |
| `0x313b30`, `0x313320`, `0x312920`, `0x3140c0` | Stubs of the constructors of `c_component_joint_snapshot_0`, `_1`, `_2` and `_4` (`src/stubs/lane_c.cpp:340`, `:346`, `:352`, `:358`) | Initialize the locals of kinds 0, 1, 2 and 4 (`ecx` = the local) |
| `0x314860`, `0x314bb0` | No name | Initialize the locals of kinds 3 and 5 |
| `0x313de0`, `0x313550`, `0x312d50`, `0x314b00`, `0x314270`, `0x314ef0` | No name | Construct the constraints of kinds 0 to 5 in the block from the local (`ecx` = the block, the local pushed, `ret 4`), and return the block |
| `0x30c170` | `function_30c170` (stub at `src/stubs/lane_c.cpp:169`) | Returns `[ecx+0x38]` |

**The initializers.** Each calls `0x31b950`, then zeroes the local from `+0x10`
in 16-byte stores. For example, `0x313b30` zeroes the fields at `+0x10` to
`+0x50`. Their defaults agree with the fields this function writes:
- **`0x313320` (kind 1):** −π, π and 0 at `+0x80`, `+0x84` and `+0x88`: the
  limits and the strength.
- **`0x312920` (kind 2):**
  - 1.0 at `+0x70`: the cone's cosine;
  - −0.025 and 0.025 at `+0x74` and `+0x78`: the plane's sines;
  - −0.5236 and 0.5236 (about π/6) at `+0x7c` and `+0x80`: the twist;
  - 0 at `+0x84`: the strength.
- **`0x314860` (kind 3):** 0 at `+0x30`: the length.
- **`0x314bb0` (kind 5):** 1.0 at `+0x60`, `+0x74` and `+0x88`, and −FLT_MAX,
  FLT_MAX and 0 at `+0x90`, `+0x94` and `+0x98`.

**The constructors.** Each calls `0x31bbd0` on the block with the local. That
routine:
- sets up an exception frame (`0x31bbd7`);
- sets the block's word `+6` to 1 (`reference_count`,
  `include/havok_reference.h:17`);
- stores the local's `+0`, the first body, at `+0xc` (`entity_a`), through
  `0x31bac0`;
- stores the local's `+4`, the second body, at `+0x10` (`entity_b`), through
  `0x31bb30`;
- copies the local's `+8` to `+0x18`;
- returns the block.

Then each constructor stores its own vtable at `+0` and `0x4146ec` at `+8`
(for example `0x313e08` and `0x313e0e`), and fills the rest from the local.
Three also set up their own exception frame: `0x313de0` (`0x313de7`),
`0x313550` (`0x313557`) and `0x314ef0` (`0x314ef7`).

## Existing declarations

**The builders.** They are members of placeholder structs named after their
addresses:
- `s_1ce700`, `s_1ce2f0`, `s_1ce4f0` and `s_1ce970`
  (`src/unknown_1ce700.cpp:18`, `:53`, `:98`, `:154`);
- `s_1ce8d0` (`src/unknown_1ce8d0.cpp:11`).

**Their layout.** Each struct starts with two `hkRigidBody *` fields, then
vector fields. `s_1ce970` ends in an `hkRotation` (`s_2da8c0`), and `s_1ce8d0`
in a `real`.
- Together they give the leading part of each kind's local.
- `s_1ce700`, `s_1ce2f0` and `s_1ce970` stop before the fields that this
  function writes itself.

**Their conventions.** Retail passes `this` in `eax`, `arg_1` in `ecx`, and
the rest on the stack. `function_1ce8d0` instead takes `this` in `esi`, `arg_3` in
`eax` and `arg_4` in `edi`.

| Retail | Declared as | Where | Status | What retail shows at these calls |
| --- | --- | --- | --- | --- |
| `0x181bd0` | `s_list_rigid_bodies *function_181bd0(long object_index, s_list_rigid_bodies *rigid_bodies)` | `src/unknown_181a80.cpp:211` | `matched` | The object in `eax`, the list pushed |
| `0x1eb110` | `void function_1eb110(s_physics_constraint_iterator *iterator)` | `src/unknown_1eb1b0.cpp:55` | `todo` | The iterator in `edi` |
| `0x1eb160` | `void function_1eb160(s_physics_constraint_iterator *iterator)` | `src/unknown_1eb1b0.cpp:79` | `matched` | The iterator in `edi` |
| `0x1eb1b0` | `s_impact_tag_block *physics_model_shape_block_get(byte *physics_model, s_physics_model_shape_key const *key, long *element_size)` | `src/unknown_1eb1b0.cpp:8` | `matched` | The model in `eax`, the key in `edx`, the size in `ecx` |
| `0xdb4c0` | `bool function_db4c0(long object_index, long key_a, long key_b)` | `src/damage.cpp:3289` | `matched` | The object in `eax`, both keys pushed |
| `0x1d7ef0` | `void function_1d7ef0(hkTransform *frame_a, s_havok_component *component, s_component_frame_owner *owner, s_component_frame_map const *mapping, hkTransform *frame_b, hkRigidBody **body_a, hkRigidBody **body_b, s_component_frame_pair const *pair)` | `src/unknown_1cec30.cpp:2527` | `todo` | `frame_a` in `eax`, the other seven pushed |
| `0x1d0150` | `void function_1d0150(s_havok_component *component, s_havok_contact_entities *contact, short a, short b)` | `src/unknown_1cec30.cpp:20` | `matched` | The component in `edx`, the other three pushed |
| `0x1ce4f0` | `void s_1ce4f0::function_1ce4f0(hkRigidBody *arg_1, hkRigidBody *arg_2, …)` | `src/unknown_1ce700.cpp:112` | `todo` | Four vectors |
| `0x1ce2f0` | `void s_1ce2f0::function_1ce2f0(hkRigidBody *arg_1, hkRigidBody *arg_2, …)` | `src/unknown_1ce700.cpp:77` | `matched` | Six vectors |
| `0x1ce700` | `void s_1ce700::function_1ce700(hkRigidBody *arg_1, hkRigidBody *arg_2, …)` | `src/unknown_1ce700.cpp:34` | `matched` | Six vectors |
| `0x1ce970` | `void s_1ce970::function_1ce970(hkRigidBody *arg_1, hkRigidBody *arg_2, …)` | `src/unknown_1ce700.cpp:170` | `matched` | Six vectors |
| `0x1ce8d0` | `void s_1ce8d0::function_1ce8d0(hkRigidBody *arg_1, hkRigidBody *arg_2, const __m128 *arg_3, const __m128 *arg_4)` | `src/unknown_1ce8d0.cpp:22` | `matched` | `this` in `esi`, `arg_1` in `ecx`, `arg_2` pushed, `arg_3` in `eax`, `arg_4` in `edi` |
| `0x30c170` | `byte *__fastcall function_30c170(hkWorld *world)` | `src/unknown_1cec30.cpp:2514` | `todo` (stub) | The world in `ecx` (`0x1d7e6a`) |
| `0x313b30`, `0x313320`, `0x312920`, `0x3140c0` | `c_component_joint_snapshot_0()`, `_1()`, `_2()`, `_4()` | `src/unknown_1cec30.cpp:1845`, `:1855`, `:1865`, `:1875` | `todo` (stubs) | The local in `ecx` |
| `0x1d6ad0` (the caller) | `void function_1d6ad0(s_havok_component *arg_0, s_physics_model_owner *arg_1)` | `src/stubs/lane_c.cpp:636` | `todo` (stub) | From `0x1d5940`: the component in `ecx`, the owner in `edi` (`0x1d5bb1`–`0x1d5bb7`) |

**A declaration for `0x1d77d0`.** No declaration exists.

Retail keeps a stack convention with callee cleanup (`ret 8`). Yet nothing in
retail's image holds the function's address, and its one caller, LTCG code,
pushes both arguments. That is only the second of the three evidence items
that `docs/DECOMPILING.md` asks for before the last-resort `standard` marker
("Standard convention with no visible reason"); the other two need a build.
Under LTCG, `__stdcall` alone does not keep the stack convention.

A declaration from the repository's own types would be the following; it is
offered, not applied. `function_1d7ef0` takes the owner as an
`s_component_frame_owner`, and the stub of `0x1d6ad0` as an
`s_physics_model_owner`.

```
void __stdcall function_1d77d0(s_havok_component *component,
    s_component_frame_owner *owner);
```

## Findings about existing names

- **`unknown7c` holds constraints.**
  - The repository's comments call it something else.
    `include/unknown_1cec30.h:278` calls `s_havok_component_element0c` "a
    contact (unknown7c): its impact", and `:287` calls the element of
    `unknown88` "a constraint".
  - But this function adds every constraint it creates to `unknown7c`. The
    matched `function_1cff80` (`src/unknown_1cec30.cpp:1883`) reads
    `unknown7c` elements as constraints, by type (`unknown00`).
  - In those elements:
    - `unknown00` is the constraint's type and `unknown02` its index in the
      physics model;
    - `impact_index` starts as NONE;
    - `contact` is the Havok constraint, whose `entity_a` and `entity_b` are
      its two bodies.
- **One owner, four names.** `s_component_frame_owner`,
  `s_physics_model_owner`, `s_type_1a7926` and the draft's `s_1d5941` describe
  the same struct (see [The owner](#the-owner)). In `s_type_1a7926`, the
  member named `node_indices` (`+0x48`, `short *`) holds the physics model's
  tag data.
- **The map.** `function_181bd0` fills an `s_list_rigid_bodies`, which its
  source describes as "the rigid body that moves each list of sections"
  (`src/unknown_181a80.cpp:202`).
  - This function indexes it by the element's `node_a` and `node_b`.
  - `function_1d7ef0` reads it as `s_component_frame_map::bodies`.
  - `function_db810` reads the same two shorts as `list_a` and `list_b`
    (`src/damage.cpp:3825`–`3826`).

## Evidence

- **Disassembly.** The function was disassembled with capstone 5.0.7 through
  `tools/xbe.py`.
- **Stack and control flow.** A control-flow graph followed the jump table and
  walked the stack depth over every path from the 16-byte-aligned frame. It
  took the allocator slot as popping 8, and each direct callee's stack effect
  from its own `ret` in retail. With 0 or 4, the depths disagree at the join
  `0x1d7e30`.
- **What was read where.** Offsets, constants and conventions were read from
  retail code and data, and the callees with source from that source.
- **References.** An image-wide scan finds only the two calls in `0x1d6ad0`,
  and only `0x1d5940`'s call to `0x1d6ad0`.
- **Checks.** Three independent reviews checked the document: one against
  retail, one against the repository, and a final one of the finished
  document. Their corrections are applied.
- **A second map.** @mrv777 mapped the function independently from their own
  retail copy, and their map agrees wherever the two overlap. Their review
  added `0x1d6ad0`'s setup, the last-match rule of `function_1d35d0` and the
  `c_havok_abstract_allocator` and reference-count names. Each was checked against retail and
  the repository before it went in.
- **Prior coverage.** No document covered this range before. Source line
  numbers are at `5d1a893`.
- **Outside sources.** No emulator, runtime testing, SDK or outside dataset was
  used. Names are the repository's own, retail's strings, or describe
  behaviour.
