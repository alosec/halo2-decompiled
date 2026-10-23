// @flags /O2 /GL-
// Havok (third-party physics library): a shape's closest point to a query,
// turned into world space by the body's rotation and handed to a collector
// when the shape finds one.
#include <xmmintrin.h>

struct found_2df110
{
	bool value;
};

struct hit_2df110
{
	__m128 point;
	long field_10;
	long field_14;
};

struct motion_2df110
{
	char pad_0[0x20];
	__m128 rotation[3];
};

struct body_2df110
{
	long field_0;
	long field_4;
	motion_2df110 *motion;
};

class collector_2df110
{
public:
	virtual void add(body_2df110 const *body, hit_2df110 const *hit) {}

	long field_4;
};

class shape_2df110
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
	virtual found_2df110 find_point(void const *query, __m128 *out) const { found_2df110 found = { false }; return found; }

	void function_2df110(void const *query, body_2df110 const *body, collector_2df110 *collector) const;
};

// @retail 0x2df110
void shape_2df110::function_2df110(void const *query, body_2df110 const *body, collector_2df110 *collector) const
{
	hit_2df110 hit;
	hit.field_14 = collector->field_4;
	if (find_point(query, &hit.point).value)
	{
		__m128 const point = hit.point;
		hit.field_10 = body->field_4;
		__m128 const *rotation = body->motion->rotation;
		hit.point = _mm_add_ps(_mm_add_ps(
			_mm_mul_ps(rotation[0], _mm_shuffle_ps(point, point, 0)),
			_mm_mul_ps(rotation[1], _mm_shuffle_ps(point, point, 0x55))),
			_mm_mul_ps(rotation[2], _mm_shuffle_ps(point, point, 0xaa)));
		collector->add(body, &hit);
	}
}
