// @flags /O2 /GL-
// Havok (third-party physics library): how far an object with a radius
// reaches along a direction: its support vertex's projection plus the
// radius.
#include <xmmintrin.h>

class convex_2df0b0
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
	virtual void support(__m128 const *direction, __m128 *out) const {}

	char unknown4[8];
	float radius;

	float reach(__m128 const *direction) const;
};

// @retail 0x2df0b0
float convex_2df0b0::reach(__m128 const *direction) const
{
	__m128 vertex;
	support(direction, &vertex);
	__m128 d = _mm_mul_ps(vertex, *direction);
	float dot;
	_mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(d, d, 0xaa), _mm_add_ss(_mm_shuffle_ps(d, d, 0x55), d)));
	return dot + radius;
}
