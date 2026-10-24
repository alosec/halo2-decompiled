# NV2A vertex program translator

`vertex_program.cpp` turns NV2A vertex program microcode (4 x 32-bit words per
instruction) into the MSL function `h2_vertex_program` described in
`shader_abi.hpp`. It generates straight-line code with one block per
instruction. Each block reads its operands, computes the MAC and/or ILU result,
then does the masked writes, so every read sees the registers as they were
before the instruction ran. Helpers (`h2vp_*`) implement the opcodes. They touch
vectors only through `.x/.y/.z/.w`, so the same text also compiles as C++, which
is how the tests run the generated code on the CPU.

## Sources

| Fact | Source |
|---|---|
| Field table (word, offset, size), opcode numbers, swizzle code, output register numbers, r12 = oPos, 192 constants, reset values | xboxdevwiki "NV2A/Vertex Shader" |
| Source mux 1 = temp, 2 = input, 3 = constant | Mesa `nv30_vertprog.h` (MIT), whose source layout is "matches NV20 exactly"; confirmed by game data |
| Write mask bit 3 = x … bit 0 = w | Mesa `nvfx_shader.h` `NVFX_VP_MASK_X = 8`; game data (oPos.xyz is mask 14, oPts.x is 8) |
| Opcode semantics | NV_vertex_program, NV_vertex_program1_1 (RCC) |
| Everything marked "game data" below | statistics over the ~530 programs a heuristic scan finds in the retail `shared.map` |

## Facts taken from game data

The public material does not document these, so they were derived from the
game's own programs:

- **Operand slots.** Unused slots always carry mux 2. The slot each opcode
  really reads is the one whose mux varies. MOV and ARL read A. MUL, DP3, DPH,
  DP4, DST, MIN, MAX, SLT and SGE read A and B. ADD reads A and C. MAD reads A,
  B and C. The ILU reads C. This matches Mesa's NV30 usage.
- **Destination select (word 3, bit 11).** A 1 goes with addresses 0 and 3..12,
  which are the output registers, so 1 = output. A 0 is rare, so 0 = constant
  register (state programs). Constant writes are dropped.
- **Destination mux (word 3, bit 2).** 1 = ILU result. It is always 1 when only
  the ILU runs.
- **ILU temporary destination.** When the MAC executes any operation, the ILU
  writes **r1**, not the instruction's temporary register. Only then do
  temporaries read across the game's programs always have a prior write.
  Instructions whose MAC and ILU masks overlap (for example both `.w`) also
  only make sense this way. When the MAC is NOP, the ILU writes the register
  field.
- **Constant index bias.** The hardware index already includes D3D's +96. The
  game's skinning matrices go to D3D c[-46], and the shaders read them as
  `c[50 + a0]`. So hardware index i reads buffer entry i, as the ABI's
  "c[0] = c[-96]" layout expects.

## Open points (not settled by any public source)

- **Scalar ILU component.** Scalar ILU ops read the *first* swizzled component
  of C, like the spec's `source0.c`. Game code always uses replicated swizzles
  for them, so the choice cannot be observed there.
- **Source mux 0.** It is undocumented, and the translator reads it as
  (0,0,0,0).
- **Relative addressing out of range.** `a0.x + index` is clamped to 0..191, as
  this task specified. NV_vertex_program says out-of-range reads return
  (0,0,0,0). What the hardware does is unknown.
- **ARL.** It uses the spec's `floor`, with no bias.
- **Overlapping writes.** If the MAC writes r1 and the ILU also writes r1 with
  overlapping masks, the ILU value wins. This order is our choice.
- **Spec rule not applied.** The spec's "0 * anything = 0" rule is not
  implemented, because MSL multiplication follows IEEE.
- **EXP and LOG precision.** EXP.z and LOG.z are computed at full precision,
  which the spec allows. LOG takes its exponent and mantissa from the float
  bits.
