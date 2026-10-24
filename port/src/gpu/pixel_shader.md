# Pixel shader translator (pixel_shader.cpp)

`translate_pixel_shader` turns a 60-word `D3DPIXELSHADERDEF` into the MSL
function `h2_pixel_shader` (contract: `shader_abi.hpp`). The definition words
are decoded as the NV2A register values they carry. Each encoding is cited in
the source: [RC]/[TS] are the NV_register_combiners / NV_texture_shader specs,
[XML] is nouveau `nv20_3d.xml.h` / envytools rnndb (MIT), [WIKI] is
xboxdevwiki, [GAME] is the game's own definitions, and [INF] marks an inference.

## Generated code shape

1. Texture stages: `t0..t3` (0 for none). Dot-product stages also keep
   `dp1..dp3`.
2. The registers: `v0 = saturate(diffuse)`, `v1 = saturate(specular)`,
   `fog = (fog_color.rgb, fog)`, `r0 = (0, 0, 0, t0.a)` and `r1 = 0`.
3. One block per general combiner stage. Every input is read first, then
   every write happens (clamped to [-1, 1]). Blue-to-alpha writes go last.
4. The final combiner computes `A*B + (1-A)*C + D` and `G`, then
   `saturate`s the result.

Literal inputs are constant-folded: the zero register under a mapping
becomes 0, 1, -1, 0.5 or -0.5. `max(x, 0)` is dropped for registers that
are known to be non-negative.

## What the backend must provide

- `k.c0[s]` and `k.c1[s]` hold stage s's constants. With the per-stage flags
  clear (PSCombinerCount bits 12 and 16), only slot 0 is read. The final
  combiner reads `k.final_c0` and `k.final_c1`. PSC0Mapping, PSC1Mapping and
  PSFinalCombinerConstants are not read here: resolving them into those slots
  is the backend's job.
- Signed texture formats must arrive in -1..1 (`PixelShaderContext::texture_signed`).
- The texture coordinates are assumed to be normalised, linear textures
  included.

## Fallbacks and inferences

- BRDF (0x08): sampled as a plain 2D texture.
- DOT_ZW (depth replace): the depth is not replaced, because the function
  returns colour only. The texture result is 0.
- DOT_RFLCT_SPEC_CONST: the eye vector is taken as (0, 0, 1), because the
  constants have no eye vector.
- PROJECT2D, PROJECT3D and CUBEMAP: when the context knows the bound
  texture's dimension, that dimension decides the sampler kind.
- [INF] RGB output bit 19 is AB blue-to-alpha and bit 18 is CD blue-to-alpha.
- [INF] PSCombinerCount bit 8 set means mux on the MSB of r0.a (>= 0.5).
  Clear means mux on the LSB of the 8-bit value.
- [INF] The values of PSDotMapping (4 bits per stage, as in
  TEX_SHADER_DOTMAPPING) are read as: 0 = as read, 1 = byte - 128, 2 = two's
  complement byte, 3 = 2x - 1, 4 = HILO (x, y, 1), 5..7 = HILO hemisphere.
- Not decoded: RC_FINAL1 bits 6:0, and the texture-stage states alpha kill
  and colour key (they are not part of the definition).
- A mode that is invalid for its stage (for example, bump env map in stage
  0) is treated as none.
