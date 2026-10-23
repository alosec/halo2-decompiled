# Vehicle ground forces (unknown_206090)

Retail range covered: `0x206090`–`0x2078ed`. This is one inventory entry, 6,238
retail bytes, `todo` with no source: the function that adds up the forces and
torques a vehicle's physics points and contacts take from the ground in one
update, and keeps the contacts' friction impacts. **Analysis only:** this
document adds no source, and nothing in it has been built or checked against
retail with the original compiler. Names are provisional. The `function_<va>`
form is primary; the descriptions are offered for whoever decompiles it.

It uses the names of `src/vehicles.cpp` (the physics state, its points and
contacts, and the vehicle types), `src/unknown_181a10.cpp` (the ray and the
batch contact query), `include/unknown_1cec30.h` (havok components, rigid
bodies and motions), `src/impacts.cpp`, `include/impacts.h` and
`src/unknown_2055f0.cpp` (impacts), and `include/globals.h` (the game time
globals). No document in `docs/` describes it;
[object core part 1](unknown_0b7740.md) names its caller, `function_2056e0`,
only as a caller of `0xb77d0`. That caller and `function_205be0`, which fills
the state, have only stubs, so what is said of them here comes from their
retail code.

## Boundary

- `0x205be0`, just before, is `todo` with no source; its `@stub` is in
  `src/stubs/vehicles.cpp`. `0x2078f0`, just after, is `todo` with source in
  `src/unknown_2055f0.cpp`. Both are excluded.
- Lane U's row of the Active claims table (issue #9, `0x200000`–`0x217fff`, now
  run on machine 2) covers the function, and lane U's round 11 deferred it
  (issue #104). This document makes no claim and is offered to lane U.
- `0x206090` has no `@retail` or `@stub` marker.

## Conventions

"Callers" counts the functions that call an entry directly, from outside the
range and then from inside it.

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x206090` | 6238 | stack: physics state, force sum out, torque sum out (`ret 0xc`) | bool | 1 + 0 | Adds up the ground forces and torques of a vehicle's points and contacts, and keeps the contacts' friction impacts |

No register carries an input. The frame is `0x58c` bytes (`0x206090`), and
`ebx`, `ebp`, `esi` and `edi` are saved after it (`0x20609f`–`0x2060a2`) and
restored before the return (`0x2078a1`–`0x2078a4`). Below, S is `esp` after
those four pushes: the arguments are at `S+0x5a0`, `S+0x5a4` and `S+0x5a8`, and
the locals lie in `S+0x10`–`S+0x59b`. The result is a bool in `eax`: 1 at
`0x2078d5`, 0 at `0x2078e3`.

Its only caller is `function_2056e0`, which calls it at `0x205741` with its own
state argument and two local vectors for the sums. When the result is false,
`function_2056e0` sets its braking argument to 0.0 (`0x205746`–`0x20574d`). It
then adds its optional force and torque arguments to the sums and divides the
force sum by state `+0x6c` (`0x205752`–`0x2057fb`). In the repo's source of the
vehicle update, `function_efde0`, `function_2056e0` runs at most once per
update: from the physics function of vehicle types 0, 1, 3, 4 and 5
(`function_f1cd0`, `function_f1f80`, `function_f2360`, `function_f3010` and
`function_f4800`), or directly for type 6 (`src/vehicles.cpp:1032`–1052). In
retail, too, those six functions are its callers (`0xf00e4`, `0xf1f5c`,
`0xf2237`, `0xf2ffa`, `0xf4161` and `0xf5bd4`).

## Data

**The state** is `s_vehicle_physics_state` (`src/vehicles.cpp:863`). The
vehicle update keeps it on its stack and `function_205be0` fills it. The table
gives what that function's retail code stores in the fields used here.

| Field | Filled by `function_205be0` | Used here |
| --- | --- | --- |
| `+0x00` (unknown000) | the vehicle's index, its second argument (`0x205cc4`) | the object and its velocities |
| `+0x04` definition | the vehicle definition plus `0x2ac` (`0x205c2b`), so its point_count is the vehicle definition's `+0x2e8` that `src/vehicles.cpp:2790` reads | the physics definition |
| `+0x08` matrix | the object's matrix from `function_ba160` (`0x205ce6`) | its up.k (`+0x2c`) in the result |
| `+0x60`–`+0x68` (in unknown060) | rigid body 0's motion `+0x70`–`+0x78` (`0x205c93`–`0x205cbb`) | the origin of every lever arm (inferred: the centre of mass) |
| `+0x6c` unknown6c | `havok_component_rigid_body_mass_get(0, component)` (`0x205cc9`) | a factor of every spring force, and the bounds of a pushed body's mass |
| `+0x70` unknown70 | 0.0 (`0x205ce1`) | takes the powered contacts' forward friction |
| `+0x74`, `+0x78` (in unknown074) | the vehicle's throttle and steering, `+0x1b0` and `+0x1b4` (`0x205da7`–`0x205dcb`) | the bank factor |
| `+0x8c` unknown8c | 0.0 (`0x205db3`); `function_f3010` then sets it to its steering (`src/vehicles.cpp:2807`) | the bank factor |
| `+0x90` points, `+0xb10` contacts | zeroed for point_count and contact_count (`0x205dce`–`0x205e12`) | 0xa8 and 0xd8 bytes each |

**A point** is `s_vehicle_physics_point` (`src/vehicles.cpp:812`); P is
points[i]. Its definition, pdef, is the i-th 0x4c-byte entry at the pointer in
physics definition `+0x40` (unknown40). The source has no struct for it;
`src/vehicles.cpp:2792` reads the same table through the vehicle definition's
`+0x2ec`.

| Field | Meaning |
| --- | --- |
| P `+0x00`–`+0x33` | a copy of the state's matrix (`0x205e90`–`0x205ea8`), so `+0x24` is its up.k |
| P `+0x34`–`+0x3c` | the point's marker position (`0x205eaa`–`0x205eca`): the ray's origin |
| P `+0x40`, `+0x4c` | written here: the lever arm r and the velocity v |
| P `+0x58`, `+0x64` | the force, added to here, and r × force |
| P `+0x80` unknown80 | set on a hit; `function_f3010` counts these (`src/vehicles.cpp:2820`) |
| P `+0x81` | 1 when the marker's node index is 0 (`0x205e9b`, `0x205ee8`); the ray needs it |
| P `+0x88` | the y of the marker's node-matrix position (`0x205ecd`–`0x205ee1`) |
| P `+0x90`–`+0x98` | the ray's force, added to here |
| P `+0x9c` unknown9c | a factor of the ray's force; `function_f3010` and `function_f4800` set it to the vehicle's unknown25c, which `function_f3010` calls mass (`src/vehicles.cpp:2476`, 2795, 4392) |
| P `+0xa0` unknowna0 | the wobble's (inferred name) amplitude; `function_f3010` takes it from a table at pdef `+0x38`, by a permutation byte (`src/vehicles.cpp:2796`–2804) |
| P `+0xa4` value | the compression ratio of a hit; the vehicle update averages it (`src/vehicles.cpp:1063`) |
| pdef `+0x08`, `+0x14` | factors of the ray's force and of its v · n term (inferred: strength and damping) |
| pdef `+0x10`, `+0x20` | the ray's two lengths, B and A, before the bank factor |
| pdef `+0x18`, `+0x1c` | the ends k1 and k0 of the tilt ramp (inferred name) |

In the repo's source the vehicle types write only a point's unknown9c and
unknowna0, so `+0x24` still holds the up.k that `function_205be0` copied
(inferred for retail).

**A contact** is `s_vehicle_contact` (`src/vehicles.cpp:823`); C is
contacts[i], and cdef its `s_vehicle_contact_definition` (line 844) in the
array at physics definition `+0x48` (contacts).

| Field | Meaning |
| --- | --- |
| C `+0x04`–`+0x0c` matrix.forward | the forward f: copied with the state's matrix (`0x205f74`–`0x205f7b`) and turned for steered contacts by `vehicle_contact_steer` (`src/vehicles.cpp:2114`) |
| C `+0x34`–`+0x3c` (in unknown34) | the contact's marker position (`0x205f6a`–`0x205f96`) |
| C `+0x40`, `+0x4c` | written here: r and v |
| C `+0x58`, `+0x64` | the force, added to here, and r × force |
| C `+0x70` contact_type, `+0x74` contact_velocity | the result's moving-surface flag and velocity, from `function_207ab0` |
| C `+0x80` unknown80 | set for every contact with compression above 0.0001; `function_f1c80` returns at the first contact that has it set, and clears the vehicle's unknown387 only when none has (`src/vehicles.cpp:2018`–2025) |
| C `+0x81` | set in the kinetic branch |
| C `+0x82` | cdef flags bit 4 and the vehicle's flags348 bit 5 (`0x205f99`–`0x205fc3`); when the vehicle definition's `+0x1ec` has bit 17, `function_ef4f0` sets or clears that bit from control_flags bit 11 (`src/vehicles.cpp:605`–611) |
| C `+0x83` unknown83 | set when braking (`src/vehicles.cpp:2183`) |
| C `+0x84` compression | from `function_207ab0`, 0 unless the result is valid |
| C `+0x88` | written here: the normal force F |
| C `+0x8c` torque | the vehicle type's drive value (`src/vehicles.cpp:2085`, 2180); overwritten here for an unpowered contact |
| C `+0x90`, `+0x94`–`+0x9c` | written here: the slip's length, and F × n |
| C `+0xa0` contact_normal, `+0xb0` unknownb0 | the normal n and the material, from `function_207ab0` |
| C `+0xb3` | skips the contact (below) |
| cdef `+0x04` flags | bit 1: the contact is powered (inferred: the bit selects the slip and the state `+0x70` feed) |
| cdef `+0x08` | a factor of the friction |
| cdef `+0x0c` distance | the impact gate, and the impact's position |
| cdef `+0x18`, `+0x24` | the speed threshold, without and with C `+0x82` |
| cdef `+0x1c`, `+0x20` | the moving and static friction with C `+0x82` |
| cdef `+0x40` | the impact's material_a (a word) |

`function_205be0` sets C `+0xb3` when cdef flags bit 5 is set, cdef `+0x48` is
not NONE, and the signed byte at `+1` of the vehicle's 8-byte permutation entry
cdef `+0x48` is at least the short at cdef `+0x42` (`0x205fc6`–`0x205fe9`).
`src/vehicles.cpp:2787`–2788 builds the same permutations pointer (inferred:
the byte is a damage state).

**The physics definition** is `s_vehicle_physics_definition`
(`src/vehicles.cpp:854`), which names point_count (`+0x3c`), unknown40,
contact_count (`+0x44`) and contacts (`+0x48`). It is kept at `S+0x20`. The
other fields read here:

| Field | Use |
| --- | --- |
| `+0x04`, `+0x10` | the static and moving friction without C `+0x82` |
| `+0x08`, `+0x0c` | the divisor of gravity and the factor of v · n in a contact's spring |
| `+0x14`, `+0x18` | the ends of the traction ramp on n.k. `+0x18` is also passed to `function_1821d0`, gates the impacts and bounds up.k in the result (inferred: the cosine of the steepest slope) |
| `+0x2c`, `+0x30` | the bank factor's scale, and the factor of state `+0x8c` |

**Other data.** The 16 query results at `S+0x1dc` are 0x3c bytes each.
`s_query_batch_result` (`src/unknown_181a10.cpp:140`) and
`s_vehicle_contact_result` (`src/vehicles.cpp:882`) describe the same layout,
with flag2c and type at `+0x2c`. This function itself reads only index
(`+0x24`), the component under the contact, and other_index (`+0x28`), its
rigid body. The ray at `S+0xe8` is an `s_vehicle_ray` (`src/vehicles.cpp:3346`)
that `function_182800` reads as an `s_query_result`
(`src/unknown_181a10.cpp:77`), whose normal follows t, at `S+0x104`. The impact
data at `S+0x11c` is an `s_impact_data` (`include/impacts.h:40`), and `S+0x15c`
holds a list of up to 32 impact indices, counted at `S+0x2c`.

**The globals** are `g_4e0300`, the object headers; `g_51e9b8`, the havok
components (0xa0 bytes, `s_havok_component`, `include/unknown_1cec30.h:381`);
`g_510c54`, the `s_game_time_globals` (`include/globals.h:12`); `g_51e9c4`,
whose first real is gravity, as `src/projectiles.cpp:206` uses it; `g_51ebfc`,
the impacts (0xa0 bytes, `s_impact`, `src/impacts.cpp:26`); and `g_51ec00`, the
impact lists (0x40 bytes, `s_impact_array`, line 112). Of the game time it
reads field_2_3 (`+2`), the ticks per second of `game_seconds_to_ticks_round`
(`src/impacts.cpp:149`); rate (`+4`), which `src/vehicles.cpp:341` calls dt;
and game_time (`+8`). Through `.data` pointers it reads (0, 0, 0) (`g_4687a4`),
(0, 0, 1) (`g_4687b0`) and (0, 0, −1) (`g_4687bc`).

## The function

It works in three phases: the points, which cast rays straight down; the
contacts, which use a batch query and then add springs, friction and impacts;
and the havok component's impact list. There is no jump table and no indirect
jump; the one indirect call is the virtual kind_code at `0x206a8d`. There is no
early return: both `ret 0xc`, returning true at `0x2078e0` and false at
`0x2078eb`, follow the third phase. Fillers at `0x2067ca`, `0x2073d9`,
`0x20763e` and `0x207809` pad to an aligned jump target; those at `0x207417`,
`0x20741e` and `0x2077ea` follow an unconditional jump and never run.

### Setup (`0x206090`–`0x206153`)

1. The object is the state's `+0x00` index looked up in `g_4e0300`
   (`0x206096`–`0x2060ba`), as `havok_object_get` does
   (`include/unknown_1cec30.h:242`). Its havok component is found from object
   `+0xb4` (havok_component_index) as `havok_component_get` does (line 430),
   with no NONE check, and kept at `S+0xcc`; only the third phase uses it
   (`0x207661`). The object is kept at `S+0x50`.
2. `function_ba1d0`, with the index in `eax`, `S+0xb4` in `ecx` and `S+0xc0` on
   the stack (`0x2060f9`), gives the linear velocity lin at `S+0xb4` and the
   angular velocity ang at `S+0xc0`; in the source, those of the root object's
   main rigid body (`src/unknown_0b8bd0.cpp:97`).
3. Both sums are set to (0, 0, 0) from `g_4687a4` (`0x2060fe`–`0x206130`).
4. When point_count is not positive, it goes straight to the contacts
   (`0x20613e`).

### Each point (`0x206154`–`0x20676b`)

For point i, P is points[i] and pdef its definition. The index i is at
`S+0x1c`, `edi` holds P + 0x50 and `S+0x24` the offset of pdef
(`0x20674b`–`0x206762`).

1. r = P `+0x34` − state `+0x60`, into P `+0x40`, and v = ang × r + lin (the
   cross product, then the sum), into P `+0x4c` (`0x206154`–`0x20622f`).
2. The bank factor (inferred name; `0x206234`–`0x206331`). x = PIN(definition
   `+0x30` × state `+0x8c` × 0.31830987, −1, 1), the last factor being 1/π, and
   s = x, negated when state `+0x74` is at least 0. With a = −(state `+0x78` ×
   P `+0x88`) and b = s × s × sign(s) × P `+0x88`, c is a when |a| > |b|, else
   b. Then t = PIN(c − 0.1, 0, 1) when c > 0, else PIN(c + 0.1, −1, 0), and
   bank = definition `+0x2c` × t + 1.
3. The ray (`0x206335`–`0x2063a0`): A = bank × pdef `+0x20` (`S+0x3c`) and B =
   bank × pdef `+0x10` (`S+0x38`). The ray at `S+0xe8` starts at P `+0x34` and
   runs A + B (`S+0x30`) along (0, 0, −1).
4. It is cast only when P `+0x81` is set, object `+0xb4` is not NONE, and the
   component has flag5 and a non-zero unknown9c: `function_182800` with the ray
   in `eax`, NONE in `edx` and unknown9c in `esi` (`0x2063fe`).
   `function_f6670` makes the same component tests before the same call
   (`src/vehicles.cpp:3440`–3445), and it too casts only when the marker's node
   index is 0 (`src/vehicles.cpp:3426`–3429), which is what P `+0x81` records;
   it also reuses the last value on three ticks out of four
   (`src/vehicles.cpp:3430`–3435). Without the cast, or on a miss (`0x206405`),
   the point goes to step 10.
5. On a hit, d = (A + B) × t − A, with t at `S+0x100` (`0x20640b`–`0x20642d`),
   and the compression ratio c is 1 − d / B when d > 0, else 1
   (`0x2064c7`–`0x2064fa`). The bank factor thus changes the ray's length and
   c, not the force's factors.
6. The tilt ramp (inferred name) of u = P `+0x24` runs from k0 = pdef `+0x1c`
   (ramp 0) to k1 = pdef `+0x18` (ramp 1): PIN((u − k0) / (k1 − k0), 0, 1),
   coded for either order of k0 and k1, or u > k0 ? 1 : 0 when |k1 − k0| <
   0.001 (`0x206420`–`0x2064bf`).
7. F = (gravity × c × c − pdef `+0x14` × (v · n)) × P `+0x9c` × pdef `+0x08` ×
   state `+0x6c` × ramp, where n is the hit normal at `S+0x104`
   (`0x2064fd`–`0x20657e`).
8. When P `+0xa0` is positive (`0x206584`), the wobble (`0x20658a`–`0x206614`):
   phase = (q × 0.100000024 + 0.9) × game_time × rate × 0.3 + i × 42, computed
   in x87, where q = (i + 1) / point_count is an integer division (`idiv` at
   `0x206590`), 1 for the last point and 0 for the others. With w =
   `function_17c900(8, phase)` (8 in `eax`; `0x2065cd`), F becomes F × (1 − P
   `+0xa0` × (w × 0.55 + 0.45)).
9. P `+0x90` += F × n, P `+0x80` = 1 and P `+0xa4` = c (`0x206626`–`0x206654`).
10. For every point (`0x20665f`–`0x206746`): P `+0x58` += P `+0x90`, P `+0x64`
    becomes r × P `+0x58`, and the two sums add P `+0x58` and P `+0x64`.

### Between the loops (`0x20676c`–`0x2067ca`)

It clears the static count (`S+0x90`), the contact count (`S+0x34`), the list's
count (`S+0x2c`) and the moving-surface flag (`S+0x1b`). The 16 results at
`S+0x1dc` get material 0xffff (`0x206786`–`0x20679b`). When contact_count is
positive, `function_1821d0` fills them (`0x2067b8`), given object `+0xb4`,
definition `+0x18`, the results and contact_count.

### Each contact (`0x2067d0`–`0x206a29`)

For contact i (`S+0x1c`), C is contacts[i], in `edi`, and cdef its definition,
kept at `S+0x24`.

1. A contact with C `+0xb3` set goes to `0x207634` (`0x2067f2`).
2. powered = cdef flags bit 1 (`S+0x13`); the friction (`S+0x54`) = (0, 0, 0);
   r and v as for a point, into C `+0x40` and C `+0x4c`; and the contact count
   rises by 1 (`0x206889`).
3. `function_207ab0`, with i in `eax`, the state in `edx` and results[i] in
   `ecx` (`0x206912`), sets the contact from its result. In the source it
   resets contact_normal, unknownb0 and compression; for a valid result it
   copies the material and normal, sets compression to PIN(cdef distance −
   result distance, 0.001, cdef distance), contact_handle to NONE, and
   contact_type and contact_velocity from the result
   (`src/vehicles.cpp:906`–919).
4. When contact_type is set, v −= contact_velocity and the moving-surface flag
   is set (`0x206917`–`0x206949`). In the repo's
   `c_query_batch_collector::collect` (`0x1822f0`), `function_181db0` gives
   that flag and velocity from the surface's motion entry
   (`src/unknown_181a10.cpp:528`).
5. When compression is at most 0.0001, it goes to `0x207401` (`0x20695d`).
6. F = state `+0x6c` × (gravity / definition `+0x08` × compression − definition
   `+0x0c` × (v · n)), with n = contact_normal, into C `+0x88`. C `+0x94` = F ×
   n, and the impact direction (`S+0x44`) is set to (0, 0, 1) from `g_4687b0`
   (`0x206963`–`0x206a29`).

### The body under the contact (`0x206a31`–`0x206bce`)

When the result's index (`+0x24`) is not NONE and F > 0 (`0x206a31`,
`0x206a46`), the contact pushes the rigid body under it.

1. The component is index's, in `esi`, and the body is its rigid body
   other_index (`+0x28`, not checked for NONE), as
   `havok_component_rigid_body_get` finds it (`include/unknown_1cec30.h:435`).
2. Nothing is pushed when the body's m_fixed is set (`0x206a80`) or its
   motion's kind_code returns 6 (`0x206a8d`–`0x206a93`);
   `src/unknown_1c25a0.cpp:202` makes the same test.
3. M is the body's mass, at least 1, inlined as
   `havok_component_rigid_body_mass_get` computes it (`0x206abd`–`0x206afa`;
   `src/unknown_1cec30.cpp:730`), and M' = PIN(M, state `+0x6c` × 0.0625, state
   `+0x6c` × 16).
4. x = ((n.k + 1) × 0.5 / M' × F)², squared by `mulss xmm0, xmm0` at
   `0x206b5b`, and x is capped at max(8, field_2_3 × (v · n))
   (`0x206b48`–`0x206b74`).
5. `havok_component_rigid_body_linear_velocity_change` gets other_index in
   `ecx`, the component in `esi` and, in `edx`, dv = n × (−rate × x) at
   `S+0x110` (`0x206bc9`).

### Friction (`0x206bd2`–`0x207156`)

1. f is C `+0x04` (matrix.forward) without its part along n, normalized
   (`0x206bd2`–`0x206cb5`). Its length is computed in x87; below 0.0001 it
   counts as 0, and a zero length skips the friction (`0x206cc5`, to
   `0x207156`).
2. The tangential velocity is v_t = v − (v · n) n (`S+0x84`).
3. A powered contact (`S+0x13`) slips by v_t − torque × f, with torque at C
   `+0x8c` (`0x206d8d`–`0x206dcf`).
4. An unpowered one slips by v_t − rf, with rf = (v_t · f) f. C `+0x8c` is then
   overwritten with |rf|, made negative unless rf · f > 0
   (`0x206dda`–`0x206ec1`).
5. The slip's length goes to C `+0x90`. (The slip, the moving and static
   friction, the speed threshold and the spring are inferred names for unnamed
   fields and terms.) With C `+0x82` set, the moving friction, static friction
   and speed threshold are cdef `+0x1c`, `+0x20` and `+0x24`; without it,
   definition `+0x10`, definition `+0x04` and cdef `+0x18`
   (`0x206ec7`–`0x206f41`).
6. The friction vector V is v_t when C `+0x82` or C `+0x83` is set, else the
   slip; its length is the speed (`0x206f47`–`0x206f92`).
7. The traction (inferred name) is 1 − PIN((definition `+0x14` − n.k) /
   (definition `+0x14` − definition `+0x18`), 0, 1): 1 at n.k = `+0x14` and 0
   at n.k = `+0x18`. The argument is computed up to three times
   (`0x206f96`–`0x206ffa`), as from a PIN macro (inferred).
8. When the speed is above the threshold (`0x207011`), the kinetic branch
   (inferred name): μ = sqrt(moving friction × 0.9); u is V normalized, or V
   itself when its length is below 0.0001; the friction is −(cdef `+0x08` × μ ×
   traction × F) × u; and C `+0x81` = 1 (`0x207017`–`0x2070df`).
9. Otherwise, the static branch (inferred name): the friction is −(sqrt(static
   friction × 0.9) × cdef `+0x08` × rate × traction × F) × V, with V not
   normalized, and the static count rises by 1 (`0x2070e8`–`0x207138`).
10. Both branches set the impact direction (`S+0x44`) to the slip
    (`0x20713c`–`0x207150`).

### Friction impacts (`0x20715c`–`0x2073ff`)

1. C `+0x80` = 1 (`0x20715c`), for every contact that reached the friction,
   even one without a forward axis.
2. When cdef distance is at most 0.0001, or n.k is at most definition `+0x18`,
   the contact goes on to the sums (`0x20716f`, `0x207185`, to `0x207420`).
   Otherwise it makes two impacts, as follows.
3. The scale (`S+0x30`) is max(0, C `+0x90` − cdef `+0x18`). The direction is
   normalized, or (0, 0, 1) when its length is below 0.0001. The position
   (`S+0xdc`) is C `+0x34` − n × (cdef distance − compression − 0.0001)
   (`0x20718b`–`0x2072bc`).
4. The impact data gets, inline, what `impact_data_set`
   (`src/unknown_2055f0.cpp:22`) writes for a NULL shape
   (`0x20729d`–`0x207355`): unknown00 0, component_a object `+0xb4`, unknown08
   0, material_a cdef `+0x40`, component_b NONE, unknown14 NONE, material_b C
   `+0xb0`, the position, normal n, type i, unknown38 0 and shape (NONE, NONE).
5. With flag = C `+0x82` or C `+0x83`, it calls
   `function_2078f0(state, &data, &direction, scale, flag)` (`0x20738c`) and
   ignores the result. In the source that function finds or makes the impact
   and, as unknown38 is 0, sets its contact with `impact_set_contact`
   (`src/unknown_2055f0.cpp:111`–137).
6. It sets unknown38 to 1 (`0x207399`) and calls `function_2078f0` again
   (`0x2073c5`). A result that is not NONE joins the list at `S+0x15c`, unless
   it is there already (`0x2073e0`–`0x2073ec`) or the list holds 32
   (`0x2073ee`).

### The sums (`0x207401`–`0x207657`)

1. A contact with compression at most 0.0001 calls `function_2079f0`, with the
   state in `eax` and i and false on the stack (`0x20740a`). In the source that
   releases the component's impacts whose type is i
   (`src/unknown_2055f0.cpp:141`–159). The impact block's exits join this path
   at `0x20740f`, which reloads the friction's x (`S+0x54`): zero on this path,
   and the friction just computed after the impact block.
2. C `+0x58` += C `+0x94`, then += the friction; C `+0x64` = r × C `+0x58`; and
   the two sums add both (`0x207420`–`0x207541`). On the `0x207401` path this
   function does not write C `+0x94`; `function_205be0` zeroed it and nothing
   in the repo's source writes it, so it adds 0 (inferred).
3. For a powered contact with C `+0x80` set (`0x207546`–`0x207632`), with f the
   unprojected C `+0x04` and p = (f · friction) f, state `+0x70` −= |p| when f
   · friction > 0, else += |p|. `function_f1c80` copies state `+0x70` to the
   vehicle's unknown37c (`src/vehicles.cpp:2016`), which its comment calls the
   engine's load torque (line 2007).
4. A contact skipped for C `+0xb3` also calls `function_2079f0` with the state,
   i and false (`0x207634`–`0x207639`).
5. Then i rises, and the loop repeats while i < contact_count, read again
   through `S+0x20` (`0x207640`–`0x207652`).

### The impact list (`0x207658`–`0x207896`)

The component's unknown20 (`+0x20`) indexes its impact list in `g_51ec00`, as
in `havok_component_impact_add` (`src/unknown_2055f0.cpp:59`); NONE counts as
an empty list. For each listed impact j (`S+0x38`), with the impact in `edi`
and its index at `S+0x40`:

1. An impact whose unknowne (`+0xe`) is 0 is passed over (`0x2076da`).
2. Otherwise it is found when an entry of the collected list equals its index
   (`0x2077f0`) and the same impact list holds an impact with unknownd (`+0xd`)
   not −1, unknowne 0, and the same material_a (`+0x2c`) and material_b
   (`+0x2e`) (`0x207810`–`0x207860`). After a match the scan of the component's
   impact list stops and sets the found flag (`S+0x13`); the outer scan over
   the collected list still runs to its end before the flag is tested.
3. A found impact is refreshed by `impact_set_contact_from_component`, with the
   impact in `edi` (`0x20788c`).
4. Any other, including every one when the collected list is empty
   (`0x2076ef`), is released while its reference_count (`+8`) is positive
   (`0x2076f5`–`0x2077cd`). Each pass is the body of `impact_release`
   (`src/impacts.cpp:1116`–1129): when reference_count is 1 and
   `PIN(unknown10, 1, 1) == unknown10`, it calls
   `impact_rigid_body_indices_get` (`0x207738`), runs `impact_set_peak` inline
   (`0x20773d`–`0x2077ad`, with game_time and field_2_3 × 0.2 rounded by
   `fistp`) and calls `function_2294a0` (`0x2077bf`); then reference_count
   falls by 1. Retail's `impact_release` (`0x2277b0`) lowers it once; this loop
   takes it to 0.

`impact_new` stores the impact data's type in unknownd and its unknown38 in
unknowne (`src/impacts.cpp:1222`–1223). So an impact from the second
`function_2078f0` call (unknowne 1) lives on only while this update returned it
and the list holds an impact made for a contact (unknownd not −1, unknowne 0)
with the same materials (inferred).

### The result (`0x20789b`–`0x2078eb`)

It returns true when all of these hold:

- no contact had a contact_type (`S+0x1b`, read at `0x20789b` before the pops;
  `0x2078a5`);
- contact_count is not 0 (`0x2078b0`);
- the static count is greater than the contact count shifted right by 1
  (`0x2078b2`–`0x2078c1`): more than half of the contacts not skipped for C
  `+0xb3` took the static branch;
- up.k (state `+0x2c`) is greater than definition `+0x18`
  (`0x2078c3`–`0x2078d3`).

Otherwise it returns false, and `function_2056e0` then clears its braking
(inferred: braking holds only a vehicle at rest on firm ground).

## Callees without source

None. Every function it calls has source with an `@retail` marker (see
[Existing declarations](#existing-declarations)), and its one virtual call is
`c_havok_body_motion_state::kind_code`, the seventh slot of `c_havok_body_motion_state`
(`include/unknown_1cec30.h:127`).

## Existing declarations

Nothing in `src/` or `include/` declares, defines or stubs `0x206090`; only its
inventory row and `0x2056e0`'s calls list name it. A declaration that fits
retail is `__stdcall`, returns bool and takes an `s_vehicle_physics_state *`
and two `vector3f *` outputs (inferred).

Its caller and callees are declared as below, at `9feed5f`. Every parameter
count agrees with retail; the registers are what LTCG chose. All are `todo`
except `0x17c900` (`near`) and `0x1d1010` and `0x22a4b0` (`matched`).

| Retail | Declared as | Where | What retail shows |
| --- | --- | --- | --- |
| `0x2056e0` | `void __stdcall function_2056e0(long vehicle_index, s_vehicle_physics_state *state, real braking, vector3f const *force, vector3f const *torque)` | `src/stubs/vehicles.cpp`, `src/vehicles.cpp:945` | Five stack arguments (`ret 0x14`). It passes state on, and clears braking when this function returns false |
| `0xba1d0` | `void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity)` | `src/unknown_0b8bd0.cpp:97` | `eax` object_index, `ecx` linear_velocity, the stack angular_velocity (`ret 4`) |
| `0x182800` | `bool function_182800(s_vehicle_ray *ray_data, long excluded_component, void *world_data)` | `src/unknown_181a10.cpp:252` | `eax` ray_data, `edx` excluded_component, `esi` world_data; plain `ret`. `src/vehicles.cpp:3353` declares it again with other names |
| `0x17c900` | `real function_17c900(short function_type, real input)` | `src/unknown_17ca10.cpp:423` | `ax` function_type, the stack input, the result in `xmm0` (`ret 4`) |
| `0x1821d0` | `void __stdcall function_1821d0(long component_index, real scale, s_query_batch_result *results, long count)` | `src/unknown_181a10.cpp:197` | Four stack arguments (`ret 0x10`). scale receives definition `+0x18`; in the source the collector keeps a contact only when its normal.k > scale (`src/unknown_181a10.cpp:566`, not checked in retail here) |
| `0x207ab0` | `void function_207ab0(long index, s_vehicle_physics_state *state, s_vehicle_contact_result const *result)` | `src/vehicles.cpp:906` | `eax` index, `edx` state, `ecx` result; plain `ret` |
| `0x1d1010` | `void havok_component_rigid_body_linear_velocity_change(long rigid_body_index, s_havok_component *component, vector3f const *change)` | `src/unknown_1cec30.cpp:886` | `ecx` rigid_body_index, `esi` component, `edx` change; plain `ret` |
| `0x2078f0` | `long function_2078f0(long const *object_index, s_impact_data const *data, vector3f const *vector, real scale, bool flag)` | `src/unknown_2055f0.cpp:111` | Five stack arguments (`ret 0x14`). object_index receives the state, whose first dword is the vehicle's index |
| `0x2079f0` | `void function_2079f0(long const *object_index, long type, bool preserve)` | `src/unknown_2055f0.cpp:141` | `eax` object_index (the state again), then type (i) and preserve (false) on the stack (`ret 8`) |
| `0x227990` | `void impact_rigid_body_indices_get(s_impact const *impact, long impact_index, long *rigid_body_index_a, long *rigid_body_index_b)` | `src/impacts.cpp:670` | Four stack arguments (`ret 0x10`) |
| `0x2294a0` | `void function_2294a0(long impact_index, long rigid_body_index_a, long rigid_body_index_b, s_impact *impact)` | `src/impacts.cpp:1404` | `ebx` impact and three stack arguments (`ret 0xc`) |
| `0x22a4b0` | `void impact_set_contact_from_component(s_impact *impact)` | `src/impacts.cpp:522` | `edi` impact; plain `ret` |

Two fields have different names in different files. The result's `+0x2c` is
type in `s_vehicle_contact_result` and flag2c in `s_query_batch_result`. Impact
`+0xd` is unknownd in `s_impact` and type in `s_component_impact_view`
(`src/unknown_2055f0.cpp:93`); here it holds the contact's index.

## Evidence

- The function, its callees, its caller and `function_205be0` were disassembled
  from the retail XBE with capstone 5.0.9, through `tools/xbe.py`. Conventions,
  stack slots, offsets, flags and constants were read from retail code and
  data, and the callees with source were read from it. A reader analysed the
  function, and a verifier checked each claim against its own disassembly, with
  a register dataflow pass for the conventions: 34 confirmed and 6 corrected,
  with the corrections applied here. A third agent wrote this text from that
  work, and a fourth checked the finished document against retail and the
  repository: 245 claims confirmed and 10 corrected or reworded, with the
  changes applied here.
- Callers come from an image-wide scan of every section for `E8` and `E9` rel32
  calls and jumps, `0F 80`–`8F` rel32 jumps and absolute dwords. The call at
  `0x205741` is the only reference to `0x206090`.
- Constants are read from retail `.rdata`: `0x45df80` (0.31830987), `0x45dc68`
  (0.1), `0x45dc70` (0.001), `0x45e5d0` (0.100000024), `0x45dc84` (0.9),
  `0x44ae90` (0.3), `0x45e3b0` (42.0), `0x45dd24` (0.55), `0x45de20` (0.45),
  `0x45dbdc` (0.0001), `0x45df78` (0.0625), `0x44541c` (16.0), `0x45dbbc`
  (0.5), `0x45dc18` (8.0), `0x45dc5c` (0.2), `0x45dff0` (1.3), `0x45dbc0`
  (1.0), `0x45dbcc` (-1.0) and `0x45dbd8` (0.0); and in `.data`, the pointers
  `0x4687a4` to (0, 0, 0) at `0x440af4`, `0x4687b0` to (0, 0, 1) at `0x440b48`
  and `0x4687bc` to (0, 0, −1) at `0x440b7c`.
- Left open, and marked as inferred above: what state `+0x60` and definition
  `+0x18` stand for, and the meaning of the bank factor, the wobble and the
  push's squared term, which is unexplained.
- No document covered the function before this one. Source line numbers are at
  `9feed5f`.
- No SDK or outside dataset was used. Names are the repository's own, or
  describe behaviour.
