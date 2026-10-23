// @flags /O2 /GL-
// Havok (third-party physics library): casting a segment against a box
// centred on the origin, clipping it against each face it crosses.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// the one-byte answer of a cast, returned through a hidden pointer
struct answer_2fe550
{
	char value;

	answer_2fe550(bool value_) : value((char)value_) {}
};

struct segment_2fe550
{
	__m128 start;
	__m128 end;
};

// where a cast hit: the surface normal, the part hit and the fraction of the
// segment travelled
struct hit_2fe550
{
	__m128 normal;
	long part;
	float fraction;
};

class box_2fe550
{
public:
	char unknown0[0x10];
	__m128 half_extents;

	answer_2fe550 function_2fe550(segment_2fe550 const *segment, hit_2fe550 *hit) const;
};

// a segment starting inside the box misses; otherwise the faces are taken
// first on the positive side, then on the negative side, each one the start
// lies outside of moving the entry and each other one moving the exit
// score 73 (was 141): retail walks the three axes by their byte offset, so the
// offset is the loop's one counter and every access is a plain displacement;
// ours still derives the axis from the offset and scales it, and our four
// masks and the frame's slots for the normal and the negated extents sit in
// other registers and other places than retail's.
// @retail 0x2fe550
answer_2fe550 box_2fe550::function_2fe550(segment_2fe550 const *segment, hit_2fe550 *hit) const
{
	__m128 const extents = half_extents;
	__m128 const negative_extents = _mm_xor_ps(extents, *(__m128 const *)g_4124a0);
	int const start_above = _mm_movemask_ps(_mm_cmple_ps(extents, segment->start)) & 7;
	int const start_below = _mm_movemask_ps(_mm_cmple_ps(segment->start, negative_extents)) & 7;
	int const end_above = _mm_movemask_ps(_mm_cmple_ps(extents, segment->end)) & 7;
	int const end_below = _mm_movemask_ps(_mm_cmple_ps(segment->end, negative_extents)) & 7;
	if ((start_above & end_above) != 0 || (start_below & end_below) != 0 || (start_below | start_above) == 0)
	{
		return answer_2fe550(false);
	}
	int const axis_bits[3] = {1, 2, 4};
	float enter = 0.0f;
	float leave = hit->fraction;
	__m128 normal;
	float side = 1.0f;
	int crossed = end_above | start_above;
	int outside = start_above;
	for (int pass = 1; pass >= 0; pass--)
	{
		for (int offset = 8; offset >= 0; offset -= 4)
		{
			int const axis = offset >> 2;
			if ((crossed & axis_bits[axis]) == 0)
			{
				continue;
			}
			float const start_distance = side * segment->start.m128_f32[axis] + negative_extents.m128_f32[axis];
			float const end_distance = side * segment->end.m128_f32[axis] + negative_extents.m128_f32[axis];
			float const fraction = start_distance / (start_distance - end_distance);
			if ((outside & axis_bits[axis]) != 0)
			{
				if (fraction >= enter)
				{
					enter = fraction;
					normal = _mm_setzero_ps();
					normal.m128_f32[axis] = side;
				}
			}
			else if (fraction <= leave)
			{
				leave = fraction;
			}
			if (leave < enter)
			{
				return answer_2fe550(false);
			}
		}
		side = -1.0f;
		crossed = end_below | start_below;
		outside = start_below;
	}
	hit->normal = normal;
	hit->fraction = enter;
	hit->part = -1;
	return answer_2fe550(true);
}
