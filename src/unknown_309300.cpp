// @flags /O2 /GL-
// Havok (third-party physics library): the supporting point of a pair of
// shapes (the second placed by a transform) along a direction: the first
// shape's point along it, the second's against it in its own frame, and that
// point placed in the first shape's frame.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long sign_mask_309300[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

class shape_309300
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	// the shape's point furthest along a direction
	virtual void support(__m128 const *direction, __m128 *out) const {}
};

// the first shape's point, the second's in its own frame, and that point in
// the first shape's frame
struct support_309300
{
	__m128 first;
	__m128 second;
	__m128 second_placed;
};

class pair_309300
{
public:
	__m128 axes[3];
	__m128 position;
	shape_309300 *first;
	shape_309300 *second;

	void function_309300(__m128 const *direction, support_309300 *out) const;
};

// @retail 0x309300
void pair_309300::function_309300(__m128 const *direction, support_309300 *out) const
{
	first->support(direction, &out->first);
	__m128 const d = _mm_xor_ps(*direction, *(__m128 const *)sign_mask_309300);
	__m128 row0 = axes[0];
	__m128 row1 = axes[1];
	__m128 row2 = axes[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	__m128 local = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(d, d, 0xaa)));
	second->support(&local, &out->second);
	__m128 const p = out->second;
	out->second_placed = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(axes[0], _mm_shuffle_ps(p, p, 0x00)),
		_mm_mul_ps(axes[1], _mm_shuffle_ps(p, p, 0x55))),
		_mm_add_ps(_mm_mul_ps(axes[2], _mm_shuffle_ps(p, p, 0xaa)), position));
}
