// @flags /O2 /GL-
// Havok (third-party physics library): the support vertices of an object
// that offsets a child: the child's vertex, moved by the offset when the
// offset points along the direction (one direction), or always (a batch);
// the vertex's fourth lane (its id) is kept.
#include <xmmintrin.h>

class convex_2dbd40
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
	virtual void support_batch(__m128 const *directions, int count, __m128 *out) const {}
};

static inline void add_keeping_w_2dbd40(__m128 *vertex, __m128 const &offset)
{
	float w = vertex->m128_f32[3];
	*vertex = _mm_add_ps(*vertex, offset);
	vertex->m128_f32[3] = w;
}

class offset_2dbd40 : public convex_2dbd40
{
public:
	char unknown4[0xc];
	__m128 offset;
	char unknown20[0x10];
	convex_2dbd40 *child;

	void support(__m128 const *direction, __m128 *out) const;
	void support_batch(__m128 const *directions, int count, __m128 *out) const;
};

// @retail 0x2dbd40
void offset_2dbd40::support(__m128 const *direction, __m128 *out) const
{
	child->support(direction, out);
	__m128 d = _mm_mul_ps(offset, *direction);
	float dot;
	_mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(d, d, 0xaa), _mm_add_ss(_mm_shuffle_ps(d, d, 0x55), d)));
	if (dot > 0.0f)
	{
		add_keeping_w_2dbd40(out, offset);
	}
}

// @retail 0x2dbdb0
void offset_2dbd40::support_batch(__m128 const *directions, int count, __m128 *out) const
{
	child->support_batch(directions, count, out);
	for (int i = count - 1; i >= 0; i--)
	{
		add_keeping_w_2dbd40(out, offset);
		out++;
	}
}
