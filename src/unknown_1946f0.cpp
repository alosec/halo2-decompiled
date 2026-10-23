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
#include <math.h>
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
	real v = (angle - -k_pi) * 40.42535400390625f;
	long q;
	__asm
	{
		fld v
		fistp q
	}
	function_195720(stream, q, 8);
}

// @retail 0x195070
// score 96 (was 96): retail multiplies the two scale factors in the other
// order, by two first, and keeps them in xmm2 and xmm3 with the two components
// in xmm0 and xmm1; ours keeps the first factor in xmm0 and the components in
// xmm1 and xmm2
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
