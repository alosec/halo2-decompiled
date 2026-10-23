// @flags /O2 /GL-
// Havok (third-party physics library): a list of shapes' bounding box: the
// union of every child's box.
#include <xmmintrin.h>

struct aabb_2fc120
{
	__m128 minimum;
	__m128 maximum;
};

class shape_2fc120
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void box_around(void const *transform, float tolerance, aabb_2fc120 *out) const {}
};

struct child_2fc120
{
	shape_2fc120 *shape;
	long field_4;
};

class list_shape_2fc120
{
public:
	char pad_0[0xc];
	child_2fc120 *children;
	long child_count;

	void box_around(void const *transform, float tolerance, aabb_2fc120 *out) const;
};

// @retail 0x2fc120
void list_shape_2fc120::box_around(void const *transform, float tolerance, aabb_2fc120 *out) const
{
	children[0].shape->box_around(transform, tolerance, out);
	for (long i = 1; i < child_count; i++)
	{
		aabb_2fc120 child;
		children[i].shape->box_around(transform, tolerance, &child);
		out->minimum = _mm_min_ps(out->minimum, child.minimum);
		out->maximum = _mm_max_ps(out->maximum, child.maximum);
	}
}
