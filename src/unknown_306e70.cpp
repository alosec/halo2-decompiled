// @flags /O2 /GL-
// Havok (third-party physics library): the normal of an edge-edge pair of
// two boxes: one box's unit axis crossed with an axis of the other, turned to
// face along a reference direction, and taken into the other box's frame.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long sign_mask_306e70[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

struct edge_pair_306e70
{
	__m128 normal;
	__m128 local_normal;
	char unknown20[0x18];
	unsigned short own_axis;
	unsigned short other_axis;
};

class box_pair_306e70
{
public:
	char unknown0[0x70];
	__m128 axes[3];
	__m128 position;
	__m128 direction;

	void function_306e70(edge_pair_306e70 *out) const;
};

// @retail 0x306e70
void box_pair_306e70::function_306e70(edge_pair_306e70 *out) const
{
	__m128 unit = _mm_setzero_ps();
	unit.m128_f32[out->own_axis] = 1.0f;
	__m128 const axis = axes[out->other_axis];
	__m128 normal = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(unit, unit, 0xc9), _mm_shuffle_ps(axis, axis, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(unit, unit, 0xd2), _mm_shuffle_ps(axis, axis, 0xc9)));
	__m128 const product = _mm_mul_ps(normal, direction);
	float dot;
	_mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	if (dot < 0.0f)
	{
		normal = _mm_xor_ps(normal, *(__m128 const *)sign_mask_306e70);
	}
	__m128 row0 = axes[0];
	__m128 row1 = axes[1];
	__m128 row2 = axes[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	out->normal = normal;
	out->local_normal = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(normal, normal, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(normal, normal, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(normal, normal, 0xaa)));
}
