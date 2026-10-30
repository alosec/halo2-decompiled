/* UNKNOWN_1946F0.CPP: the bit stream module's codecs that retail compiled
   with the small helpers (a boolean write, vector normalization, the
   direction decode) expanded inline. The stream primitives they call are in
   unknown_195720.cpp */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "bitstream.h"
#include "unknown_1946f0.h"
#include "normalize3d.h"
#include "slot_handler.h"
#include <math.h>

static __forceinline long real_to_long(real value);
#include <string.h>

// @flags /O2 /Gr /arch:SSE

s_direction_face g_475480[32];

/* the decode path's normalization: the length of the stored components,
   then each component scaled by its inverse */
static __forceinline real normalize_decoded_direction(vector3f *v)
{
	real m = (real)sqrt(v->i * v->i + v->k * v->k + v->j * v->j);
	if (!(fabs(m) < k_real_epsilon))
	{
		real inv = 1.f / m;
		v->i = inv * v->i;
		v->j = inv * v->j;
		v->k = v->k * inv;
		return m;
	}
	return 0.f;
}

// @retail 0x194830
void function_194830(s_bitstream *stream, bool value)
{
	if (stream->size_in_bytes * 8 - stream->bit_position >= 1 && value)
		stream->data[stream->bit_position / 8] |= (byte)(1 << (stream->bit_position % 8));
	stream->bit_position++;
}

// @retail 0x194c10
void function_194c10(s_bitstream *stream, vector3f const *vector, real lo, real hi, long bits)
{
	vector3f direction = *vector;
	real magnitude = normalize3d(&direction);
	if (magnitude > hi)
		magnitude = hi;
	if (magnitude >= lo)
	{
		function_194830(stream, false);
		function_194bc0(stream, &direction);
		function_194b60(stream, magnitude, lo, hi, bits);
	}
	else
		function_194830(stream, true);
}

/* the direction an index names: a 64 x 64 grid point on one face of the
   direction table */
static __forceinline void decode_direction_194d30(long index, vector3f *direction)
{
	real x = (real)(index & 0x3f) * (1.f / 63.f);
	real y = (real)((index >> 6) & 0x3f) * (1.f / 63.f);
	x = x * 2.f;
	y = y * 2.f;
	real z = 0.f;
	s_direction_face const *face = &g_475480[index >> 12];
	if (face->scale != 1.f)
	{
		x = face->scale * x;
		y = face->scale * y;
		z = face->scale * z;
	}
	direction->i = z * face->axes[2].i + y * face->axes[1].i + x * face->axes[0].i + face->origin.i;
	direction->j = z * face->axes[2].j + y * face->axes[1].j + x * face->axes[0].j + face->origin.j;
	direction->k = z * face->axes[2].k + y * face->axes[1].k + x * face->axes[0].k + face->origin.k;
}

// score 61 (was 61): two causes. (1) The decoder's constants: retail makes the zero first (xmm3)
// and loads 2.0 and 1.0 into xmm2 at their uses; ours loads 2.0 early into xmm3 and makes the zero
// late. Retail's decoder acts as if a block ends after the two 1/63 products: a one-pass loop around
// them (diagnostic only, not kept) gives retail's schedule, 51. (2) Ours keeps the decoded k on x87
// (fst) and squares it first; retail pops it and sums j*j, k*k, then i*i from memory. A block boundary
// before the normalization (diagnostic branch) makes ours reload in retail's order. These sum orders
// follow VC's tuple numbering, not the source order.
// Round 78: the rounding through the existing real_to_long (forceinline declaration, as ai.cpp does)
// in place of the hand-written fld/fistp block 62 -> 61 (kept: the sum is now k, j, i and j scales as
// retail's j * inv); on it: a two-pass grid loop 124 (VC keeps the loop); normalize_inline 61;
// normalize_decoded_direction 61; compound doubling 61; the angle call inside the rounding 101;
// decoder parameters (output, index) 61; before it: normalize_inline 62; grid doubling written first in
// one expression 62; named face index 62.
// Tried: shared face/grid scales 96; initialized grid 133; shared scale value 96; complete output
// aggregate/memcpy 155; default memcpy and direct angle 62; coordinate stages 62; selected scale
// results 62; plane offset first 100; axis table 62; origin snapshot 129; table subobjects 129;
// coordinate names 62; existing normalizer call 187; face scale value 82; integer grid values 62;
// complete shared decoder 503; decoder output reference 62; native component loop 173; complete
// private decode consumer 62; ordinary decoder inline 394; initial direction value 63; workspace
// names, formatter scope, unsigned index, complete bounds, shared rounding, shared/existing decoded
// normalization, completed decode output and decoder pointer/vector results 62, 62, 62, 122, 72,
// 62, 62, 88, 62, 155; sum orders j+k+i, i+(j+k), k+j+i, i+k+j 62 each; decoder stores through the
// n[] view 62 (with i+(j+k) 62); zero declared first in the decoder 84; grid divided by 63 62; the
// existing real_to_long for the angle (plain inline) 73; grid point passed to an inline point
// transform 62; step count as a decoder parameter 62; point2f grid scaled in place 68; a compiled-out
// `do {} while (false)` between the decode and the normalization 62 (identical code: no join); zero
// after y / between x and y / after the face pointer 73/81/62; the decoder returning its output
// pointer into the normalization 62. Round 20: constant-first doubling 61; `x *= face->scale` 61;
// face pointer before the grid values 81; the equal-direction arm first 61; zero between the grid
// products and the doubling 72; the rows written in both scale arms 224 (no tail merge).
// pointer into the normalization 62.
// Round 119: the 0x195070 decoder (same decode, retail hoists 2.0 there and makes the zero late, as
// ours does here) shows the block after the 1/63 products is particular to this caller. A two-pass
// grid loop shifting the index by six, the zero first 130; the same with `index >> (6 * n)` 134; the
// face transform as a three-component loop 172 (not unrolled); the grid through an inline
// `(real)value / 63.f` helper 114 (599 bytes); the zero first and the face pointer before the
// doubling 87.
// pointer into the normalization 62. Round 80: the file's decoded-direction normalizer with its sum in
// i, j, k order (here and/or in 0x195070) 61 (the reduced build keeps the decoded k on x87 either way;
// an LTCG harness of the file alone popped it); the face pointer before the grid 81; y before x 75;
// compound face scaling 61.
// Round 34: face pointer and zero before the grid values 87; the three rows into locals, then the
// stores 61; constant-first grid products and doubling 61; y before x 71; `1.f != face->scale` 61.
// pointer into the normalization 62. Round 20: constant-first doubling 61; `x *= face->scale` 61;
// face pointer before the grid values 81; the equal-direction arm first 61; zero between the grid
// products and the doubling 72; the rows written in both scale arms 224 (no tail merge).
// @retail 0x194d30
void function_194d30(s_bitstream *stream, vector3f const *up, vector3f const *forward)
{
	vector3f direction;
	if (!(fabs(forward->i - g_4687b0->i) < k_real_epsilon && fabs(forward->j - g_4687b0->j) < k_real_epsilon && fabs(forward->k - g_4687b0->k) < k_real_epsilon))
	{
		long index = function_24f590(forward);
		function_194830(stream, false);
		if ((dword)index >= k_direction_limit)
		{
			char message[256];
			message[0] = 0;
			csprintf_256(message, "%u exceeds max value of %u", index, k_direction_limit);
		}
		function_195720(stream, index, k_direction_bits);
		decode_direction_194d30(index, &direction);
		normalize3d(&direction);
	}
	else
	{
		function_194830(stream, true);
		direction = *g_4687b0;
	}
	real angle = function_1949b0(&direction, up);
	function_195720(stream, real_to_long((angle - -k_pi) * 40.42535400390625f), 8);
}

// score 96 (was 96): retail keeps 1/63 in xmm2 and 2 in xmm3 and finishes x (both products) before
// converting y; ours keeps 1/63 in xmm0 and scales x and y by two late. Retail also pops the decoded k
// and sums i, k, j from memory (a block boundary before the normalization), and scales the normalized
// vector in place. Single-expression grid values 126 (1/63*2 folds into one constant); x finished
// before y 96; `vector3f_scale` with the magnitude call 96; the existing decode_direction_194d30
// helper 114 (aligned 34 -> 26: the final scale then matches retail and the hand-written decode copy
// goes; the decode registers and x87 reload remain); that helper as plain inline 344 (not inlined).
// Plain component loops (rows/normalization/output in seven subsets) 114/96/114/96/96/96/96; all
// with complete normalized value 155 at 489 bytes. Row/output forms are 440 bytes; no count gain or
// missing store/reload recovery. Original body and normalizer restored.
// DL190: Whole face value/native output stores, setter, aggregate 175/175/205; memcpy face with the same boundaries 175/175/205. Original face with setter/aggregate 96/136. Complete face snapshot adds spills; grid reciprocal, last-component reload and output carriers remain. All restored.
// @retail 0x195070
void function_195070(s_bitstream *stream, vector3f *out, real lo, real hi, long bits)
{
	if (!function_1957d0(stream))
	{
		long index = (long)function_1959c0(stream, k_direction_bits);
		vector3f direction;
		{
			real x = (real)(index & 0x3f) * (1.f / 63.f);
			real y = (real)((index >> 6) & 0x3f) * (1.f / 63.f);
			x = x * 2.f;
			y = y * 2.f;
			s_direction_face const *face = &g_475480[index >> 12];
			real z = 0.f;
			if (face->scale != 1.f)
			{
				x = face->scale * x;
				y = face->scale * y;
				z = face->scale * z;
			}
			direction.i = z * face->axes[2].i + y * face->axes[1].i + x * face->axes[0].i + face->origin.i;
			direction.j = z * face->axes[2].j + y * face->axes[1].j + x * face->axes[0].j + face->origin.j;
			direction.k = z * face->axes[2].k + y * face->axes[1].k + x * face->axes[0].k + face->origin.k;
		}
		normalize_decoded_direction(&direction);
		real magnitude = function_194ff0(stream, lo, hi, bits);
		out->i = magnitude * direction.i;
		out->j = magnitude * direction.j;
		out->k = magnitude * direction.k;
	}
	else
		*out = *g_4687a4;
}

// @retail 0x195370
bool __fastcall function_195370(vector3f const *a, vector3f const *b, real lo, real hi, long bits)
{
	vector3f direction_a = *a;
	vector3f direction_b = *b;
	real magnitude_a = normalize3d(&direction_a);
	real magnitude_b = normalize3d(&direction_b);
	bool short_a = lo > magnitude_a;
	bool short_b = lo > magnitude_b;
	bool near_a = !short_a && function_1952f0(magnitude_a, lo, lo, hi, bits);
	bool near_b = !short_b && function_1952f0(magnitude_b, lo, lo, hi, bits);
	if (!short_a || !short_b)
	{
		if (short_a || short_b)
		{
			bool other_near = short_a ? near_b : near_a;
			if (!other_near)
				return false;
		}
		if (!direction_a.quantized_equal(&direction_b) || !function_1952f0(magnitude_a, magnitude_b, lo, hi, bits))
			return false;
	}
	return true;
}
