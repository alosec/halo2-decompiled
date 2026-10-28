// @flags /O2 /Gr /arch:SSE
/* the render state constants of a projected source: built without /Op, which
   keeps sqrt and tan inline (src/unknown_0494b0.cpp is /Op) */

#include "unknown_11c920.h"
#include "globals.h"
#include "render_source_storage.h"
#include "texture_stage_storage.h"
#include "unknown_0494b0.h"
#include <xtl.h>
#include <math.h>

void function_1cf50(void);
void function_01ddd0(long index, dword width, dword height);
long function_48e70(real value);
extern real g_485a34, g_485a40;
extern byte g_485a47;
extern real g_485a0c[8];
extern real g_4c9798[36];

// score 295 (was 295): frame 0x20 versus 0x28; input and rounding scalar homes differ.
// Earlier form: frame 0x20 versus 0x28; input registers and rounding-call scalar reloads differ.
// Branch-defined extent 321; two-vector copy loop 311; frame pointer 372; size policy 497; inline depth one 311.
// Global optimization off 1009; expansion depth zero 349; late mode 350;
// near normal first 309; height clamp first 311; sample y/z first 313, plus normal first 311.
// Earlier form: Frame 0x20 versus 0x28 and input/rounding homes remain. Near normal input 313; far normal copy 295 kept (plain, retail length); integer maximum reuse 311; far point before scale 313; height rounding first 311 (fast).
// Earlier form: Frame 0x20 versus 0x28 and input/rounding homes remain.
// Frame 0x20 versus 0x28; state/matrix and protected rounding/dimension carriers remain.
// frame 0x20 versus 0x28; input registers and rounding-call scalar reloads remain.
// frame 0x20/0x28 and state/matrix/rounding homes remain.
// Radius/scaled-radius/padded-radius parts 295/295/295; size accumulation 295;
// width/height/both producers 295/295/295; consecutive clamp bounds 301.
// The one-use radius call result is restored; matched storage-provider root resolves all trials.
// frame 0x20 versus 0x28; input registers and rounding-call scalar reloads differ.
// Frame 0x20 versus 0x28 and input/rounding homes remain. Near normal input 313; far normal copy 295 kept (plain, retail length); integer maximum reuse 311; far point before scale 313; height rounding first 311 (fast).
// Frame 0x20 versus 0x28 and input/rounding homes remain.
// Distinct normals 295; scoped near normal 295; local near-normal dot 301; point setters 305;
// late pixel area 295; origin pointer 295; clamp result 295; fallback-first far scale 296 (plain).
// Frame 0x20 versus 0x28, input registers and rounding reloads remain. Tried texture row
// memset 300; whole table memcpy 361; two vector memcpy calls 295; row clear and whole copy 366; row clear and
// vector copies 300; near normal memcpy 295; far normal memcpy 295; both normal memcpy calls 295.
// Distance before aspect 296; aspect after offset 296; reciprocal before aspect 300;
// conditional/branch offset 309 each; branch reciprocal 295; complete size result 295; combined entry/results 313.
// Five consistent formal permutations 295 each; native byte mode 332; unsigned mode 295;
// signed-byte mode 331.
// Direction/origin/both value snapshots 376/329/411; far output dot 311;
// point component loops 303; normal loop 295; plane scaling loops 295; combined loops 300.
// Native width/height/both result arms 295/295/295; shared sizing predicate 331;
// height limit first 303; complete mode result 295; initial projection/opposite loops 295/295.
// Shared near/far/both/projection/all scaling 295/295/295/295/295; near/far/both point accumulation 295/295/295.
// Round 188 complete sizing scope, unsigned-word mode, depth-bound scope, their
// pairs/all, and signed-word mode with sizing scope: 300/291/295/297/300/291/297/297.
// All original scalar lifetimes, native mode, geometry and protected interfaces retained.
// Round 167: Individual or mixed bounded dimension carriers:
// width short 296; width word 296; height short 324; height word 324;
// short/word 325; word/short 325; short/int 296; int/word 324.
// Original native limits, far-normal copy and all protected call interfaces restored.
// Round 213: direct lower/upper/both depth fields 294/295/294; each with distinct far normal
// 294/295/294; both with distinct far point 302, with distinct far point and normal 294.
// One-point gains rejected; original field snapshots and captured far normal restored.
// Round 194: Native projection/dimension/border-identity row combinations 297/295/297/295/297/295/297; all
// rows with projection scope 298. Original body restored.
// Round 375: Complete near/far/shared-pair/distinct-pair plane outputs on the
// retained far-normal graph 322/335/361/364; CRT copies 322/335/361/364.
// Frame 0x30 versus 0x28; all exact-only output-record forms restored.
// Round 139: Repeated ratio/angle inputs captured once separately/together 316/305/316; each with
// real shared pixel area 321/326/321; joint input home names without/with area 316/321. Frame
// 0x20/0x28, state/matrix and protected rounding homes remain. Original executable source, every
// float tree and accepted far-normal copy restored.
// Round 96: retail takes the camera-like record (read at +8/+0x64/+0x28) in ECX and keeps the source
// record (origin +0, direction +0x28) in EBX across the calls; ours takes the first in EAX and leaves the
// second on the stack. The two pointer parameters were named crossed against both callers (kept: now
// swapped, data flow as retail, 295). Direction/origin fields read at each use 328; tag test as a
// one-case switch 295; origin/direction references at entry 300.
// Round 60: maximum width/height written through pointers by an inline helper 295; flag test as
// a shifted bool, its negated-bool and its compare-with-zero spellings 311/337/295; offset as a
// ternary 309.
// @retail 0x48ee0
void function_48ee0(byte const *matrix, byte const *state, byte const *tag)
{
    real aspect = (real)sqrt(*(real const *)(state + 8));
    real distance = *(real const *)(state + 0x64);
    real offset = 0.0f;
    if (*(short const *)(tag + 0x8e) == 1)
        offset = *(real const *)(state + 0x28);
    real inverse = distance != 0.0f ? 1.0f / distance : -3.4028234663852886e38f;
    vector3f const &direction = *(vector3f const *)(matrix + 0x28);
    g_4c9798[0] = direction.i * inverse;
    g_4c9798[1] = direction.j * inverse;
    g_4c9798[2] = direction.k * inverse;
    g_4c9798[3] = 0.0f - inverse * offset;
    g_4c9798[4] = 0.0f - direction.i;
    g_4c9798[5] = 0.0f - direction.j;
    g_4c9798[6] = 0.0f - direction.k;
    g_4c9798[7] = 0.0f;
    long mode = !(g_4c1a80 & 0x400) ? 2 : g_485a40 > 0.0f && *(short const *)(tag + 0x14) == 0 ? 1 : 0;
    real extent = *(real const *)(state + 4);
    if (*(real const *)(state + 0xc) > 0.0f)
        extent += (real)tan(*(real const *)(state + 0xc) * 0.5f) * distance * 2.0f;
    long max_width = (bool)((g_4ba014 >> 4) & 1) && !(bool)((g_4ba014 >> 5) & 1) ? 128 : 512;
    long max_height = (bool)((g_4ba014 >> 4) & 1) && !(bool)((g_4ba014 >> 5) & 1) ? 128 : 512;
    real maximum = (real)max_width;
    real size = ((real)sqrt(extent * extent * *(real const *)(state + 8)) * 0.33333334f + *(real const *)(tag + 0x10) * 0.125f) *
        (maximum - 128.0f) * g_485a34 + 128.0f;
    if (size < 128.0f) size = 128.0f;
    else if (size > maximum) size = maximum;
    long width = (function_48e70(size / aspect) + 63) & ~63;
    long height = (function_48e70(size * aspect) + 63) & ~63;
    if (width * height > max_width * max_height)
    {
        if (width > height) width = (max_width * max_height / height) & ~63;
        else height = (max_width * max_height / width) & ~63;
    }
    g_485a47 = (byte)mode;
    g_51f3c8[1][0] = 0;
    g_51f3c8[1][1] = 0;
    g_51f3c8[1][2] = 0;
    g_51f3c8[1][3] = 0;
    function_1cf50();
    if ((bool)((g_4ba014 >> 4) & 1) && !(bool)((g_4ba014 >> 5) & 1))
    {
        g_4c9798[8] = maximum;
        g_4c9798[9] = (real)max_height;
        g_4c9798[10] = 16777215.0f;
        g_4c9798[11] = 1.0f;
    }
    else
    {
        function_01ddd0(9, width, height);
        g_4c9798[8] = (real)width;
        g_4c9798[9] = (real)height;
        g_4c9798[10] = 16777215.0f;
        g_4c9798[11] = 1.0f;
    }
    g_4c9798[32] = 1.0f;
    g_4c9798[33] = 1.0f;
    g_4c9798[34] = 0.0f;
    g_4c9798[35] = 0.0f;
    real border = mode > 0 ? (real)width * 0.75f / maximum : 0.0f;
    g_4c9798[12] = border + 0.5f;
    g_4c9798[13] = 0.5f - border;
    g_4c9798[15] = 0.0f;
    point3f const &origin = *(point3f const *)matrix;
    plane3f *plane = (plane3f *)&g_4c9798[16];
    vector3f normal;
    point3f point;
    point.x = direction.i * 0.01f + origin.x;
    point.y = direction.j * 0.01f + origin.y;
    point.z = direction.k * 0.01f + origin.z;
    normal.i = 0.0f - direction.i;
    normal.j = 0.0f - direction.j;
    normal.k = 0.0f - direction.k;
    plane->n = normal;
    plane->d = dot3f(&plane->n, (vector3f const *)&point);
    plane->i *= 100.0f;
    plane->j *= 100.0f;
    plane->k *= 100.0f;
    plane->d *= -100.0f;
    real lower = *(real const *)(tag + 0x2c);
    real upper = *(real const *)(tag + 0x30);
    real scale = upper > lower ? 1.0f / (upper - lower) : 1000.0f;
    plane = (plane3f *)&g_4c9798[20];
    point.x = direction.i * lower + origin.x;
    point.y = direction.j * lower + origin.y;
    point.z = direction.k * lower + origin.z;
    normal = direction;
    plane->n = normal;
    plane->d = dot3f(&normal, (vector3f const *)&point);
    plane->i *= scale;
    plane->j *= scale;
    plane->k *= scale;
    plane->d = 0.0f - plane->d * scale;
    ((vector4f *)&g_4c9798[24])[0] = ((vector4f const *)g_485a0c)[0];
    ((vector4f *)&g_4c9798[24])[1] = ((vector4f const *)g_485a0c)[1];
    D3DDevice_SetVertexShaderConstantFast(-56, g_4c9798, 9);
}
