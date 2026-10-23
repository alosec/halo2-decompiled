// @flags /O2 /GL-
// Havok (third-party physics library): a bounding sphere's radius grown to
// take in a shape's box: half the box's diagonal plus the distance from the
// sphere's centre to the box's centre.
#include <xmmintrin.h>
#include "derived_311920.h"

// the identity transform: three unit axes and a zero position
static __declspec(align(16)) const float g_identity_461ea0[16] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f,
};

struct transform_3117b0;

// a box given by its two corners
struct bounds_3117b0
{
	__m128 minimum;
	__m128 maximum;
};

class shape_3117b0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	// the shape's bounds under a transform, widened by a tolerance
	virtual void bounds(transform_3117b0 const *transform, float tolerance, bounds_3117b0 *out) const {}
};

struct sphere_3117b0
{
	char unknown0[0xc];
	float radius;
	char unknown10[0x10];
	__m128 centre;
};

class bounded_shape_3117b0
{
public:
	char unknown0[0xc];
	shape_3117b0 *shape;
	char unknown10[0x2c];
	sphere_3117b0 *bound;

	void function_3117b0();
};

// a vector's four terms set to one value
inline __m128 splat_3117b0(float value)
{
	__m128 single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// the length of a vector's first three terms
inline float length3_3117b0(__m128 const &v)
{
	__m128 const squares = _mm_mul_ps(v, v);
	float result;
	_mm_store_ss(&result, _mm_sqrt_ss(_mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares))));
	return result;
}

// @retail 0x3117b0
void bounded_shape_3117b0::function_3117b0()
{
	bounds_3117b0 box;
	shape->bounds((transform_3117b0 const *)g_identity_461ea0, 0.0f, &box);
	sphere_3117b0 *sphere = bound;
	float const diagonal = length3_3117b0(_mm_sub_ps(box.maximum, box.minimum));
	__m128 const box_centre = _mm_mul_ps(splat_3117b0(0.5f), _mm_add_ps(box.minimum, box.maximum));
	sphere->radius = diagonal * 0.5f + length3_3117b0(_mm_sub_ps(box_centre, sphere->centre));
}

// builds the object on the one 0x3114d0 builds, with its own sixteen words
// cleared by their constructors
// @retail 0x311890
class_311890::class_311890(void const *a, void const *b) : class_3114d0(a, b)
{
}
