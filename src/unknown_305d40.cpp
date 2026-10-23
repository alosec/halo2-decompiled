// @flags /O2 /GL-
// Havok (third-party physics library): a corner of one box from a feature
// key, in its own frame and taken into the frame of the other box.
#include "box_corners_413610.h"
#include <xmmintrin.h>

// a box's corner key: an id, then a word whose low byte's high nibble picks
// the corner and whose high byte's low nibble is kept in the vertex
struct corner_key_305d40
{
	unsigned short id;
	union
	{
		unsigned short word;
		struct
		{
			unsigned char corner;
			unsigned char flags;
		};
	};
};

struct corner_305d40
{
	__m128 own;
	__m128 other;
	char unknown20[0x10];
	float tagged_one;
	long field_34;
	unsigned short id;
};

class box_pair_305d40
{
public:
	char unknown0[0x20];
	__m128 half_extents;
	char unknown30[0x40];
	__m128 axes[3];
	__m128 position;

	void function_305d40(corner_305d40 *out, corner_key_305d40 const *key) const;
};

// out: the corner the key picks, in the box's frame, and taken into the
// other frame by the inverse of the transform; 1.0 with the key's high nibble
// in its top four bits; the id
// @retail 0x305d40
void box_pair_305d40::function_305d40(corner_305d40 *out, corner_key_305d40 const *key) const
{
	out->id = key->id;
	float tagged = 1.0f;
	*(long *)&tagged |= (key->word >> 8) << 28;
	out->tagged_one = tagged;
	out->own = _mm_mul_ps(half_extents, *(__m128 const *)((char const *)g_corner_signs_413610 + (key->corner & 0xf0)));
	__m128 const d = _mm_sub_ps(out->own, position);
	__m128 row0 = axes[0];
	__m128 row1 = axes[1];
	__m128 row2 = axes[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	out->other = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(d, d, 0xaa)));
}
