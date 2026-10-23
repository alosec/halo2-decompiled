// @flags /O2 /GL-
// Havok (third-party physics library): a box's half extents, its corner
// furthest along a direction (w tags which corner), the corners for a list
// of tags, and all eight corners (w holding the radius).
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// the eight corners' signs, by tag (bit 0 x, bit 1 y, bit 2 z), w zero
static __declspec(align(16)) const float g_413460[8][4] = {
	{1.0f, 1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 1.0f, 0.0f}, {-1.0f, -1.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, -1.0f, 0.0f}, {-1.0f, -1.0f, -1.0f, 0.0f},
};

// the same signs with w one
static __declspec(align(16)) const float g_4134e0[8][4] = {
	{1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, 1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f, 1.0f},
};

void __cdecl function_2fe350(void const *query, const __m128 *half_extents, float tolerance, void *out);

class class_2fe410
{
public:
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	__m128 half_extents;

	void get_half_extents(__m128 *out) const;
	void function_2fe420(void const *query, float tolerance, void *out) const;
	void get_support(const __m128 *direction, __m128 *out) const;
	void get_corners(const unsigned short *tags, int count, __m128 *out) const;
	__m128 *get_all_corners(__m128 *out) const;
};

// @retail 0x2fe410
void class_2fe410::get_half_extents(__m128 *out) const
{
	*out = half_extents;
}

// the query against the box, its tolerance grown by the box's radius
// @retail 0x2fe420
void class_2fe410::function_2fe420(void const *query, float tolerance, void *out) const
{
	function_2fe350(query, &half_extents, tolerance + *(const float *)&field_c, out);
}

// @retail 0x2fe450
void class_2fe410::get_support(const __m128 *direction, __m128 *out) const
{
	__m128 corner = half_extents;
	*out = corner;
	corner = _mm_xor_ps(corner, _mm_and_ps(*direction, *(const __m128 *)g_4124a0));
	*out = corner;
	((unsigned long *)out)[3] = (_mm_movemask_ps(corner) & 7) | 0x3f000000;
}

// @retail 0x2fe490
void class_2fe410::get_corners(const unsigned short *tags, int count, __m128 *out) const
{
	for (int i = count - 1; i >= 0; i--)
	{
		*out = _mm_mul_ps(half_extents, *(const __m128 *)g_413460[*tags]);
		((unsigned long *)out)[3] = *tags | 0x3f000000;
		out++;
		tags++;
	}
}

// @retail 0x2fe4f0
__m128 *class_2fe410::get_all_corners(__m128 *out) const
{
	__m128 extents = half_extents;
	extents.m128_f32[3] = *(const float *)&field_c;
	__m128 *odd = out + 1;
	int i = 0;
	for (int count = 4; count != 0; count--)
	{
		__m128 second = _mm_mul_ps(extents, *(const __m128 *)g_4134e0[i + 1]);
		out[i] = _mm_mul_ps(extents, *(const __m128 *)g_4134e0[i]);
		*odd = second;
		i += 2;
		odd += 2;
	}
	return out;
}
