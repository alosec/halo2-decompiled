// @flags /O2 /GL-
// Havok (third-party physics library): a ray cast against every body in a
// list: the ray taken into each body's frame, then cast against its shape,
// hits going to a callback.
#include <xmmintrin.h>

class c_query_callback;

// three axes and a position
struct transform_30b130
{
	__m128 axes[3];
	__m128 position;
};

struct motion_30b130
{
	char unknown0[0x20];
	transform_30b130 transform;
};

struct body_30b130;

// the ray as a shape receives it, in the shape's frame: its two ends, the
// value passed along and the optional filter data
struct local_ray_30b130
{
	local_ray_30b130() : filter_data(0) {}

	__m128 from;
	__m128 to;
	long field_20;
	void const *filter_data;
};

class shape_30b130
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
	// a ray cast against the shape, hits going to a callback
	virtual void cast_ray(local_ray_30b130 const *ray, body_30b130 const *body, c_query_callback *callback) const {}
};

struct body_30b130
{
	shape_30b130 const *shape;
	long key;
	motion_30b130 const *motion;
};

// the ray as the game passes it (src/unknown_181a10.cpp): its two ends,
// whether to pass the filter data along, and a value passed along
struct s_bounded_ray
{
	__m128 from;
	__m128 to;
	bool flag;
	long field_24;
};

struct filter_owner_30b130
{
	char unknown0[0xd0];
	char *filter;
};

class c_bounded_query_world
{
public:
	char unknown0[8];
	filter_owner_30b130 *filter_owner;
	char unknown0c[0x74];
	body_30b130 **bodies;
	long body_count;

	void cast_ray(s_bounded_ray const *ray, c_query_callback *callback);
};

// a point taken into a transform's frame: the position subtracted, then the
// transposed axes applied
static __forceinline __m128 inverse_transform_30b130(transform_30b130 const &transform, __m128 const &point)
{
	__m128 c0 = transform.axes[0];
	__m128 c1 = transform.axes[1];
	__m128 c2 = transform.axes[2];
	__m128 offset = _mm_sub_ps(point, transform.position);
	__m128 t0 = _mm_shuffle_ps(c0, c1, 0x44);
	__m128 t2 = _mm_shuffle_ps(c0, c1, 0xee);
	__m128 t1 = _mm_shuffle_ps(c2, c2, 0x44);
	__m128 t3 = _mm_shuffle_ps(c2, c2, 0xee);
	__m128 row_0 = _mm_shuffle_ps(t0, t1, 0x88);
	__m128 row_1 = _mm_shuffle_ps(t0, t1, 0xdd);
	__m128 row_2 = _mm_shuffle_ps(t2, t3, 0x88);
	return _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row_0, _mm_shuffle_ps(offset, offset, 0x00)),
		_mm_mul_ps(row_1, _mm_shuffle_ps(offset, offset, 0x55))),
		_mm_mul_ps(row_2, _mm_shuffle_ps(offset, offset, 0xaa)));
}

// @retail 0x30b130
void c_bounded_query_world::cast_ray(s_bounded_ray const *ray, c_query_callback *callback)
{
	local_ray_30b130 local;
	local.field_20 = ray->field_24;
	if (ray->flag && filter_owner->filter != 0)
	{
		local.filter_data = filter_owner->filter + 0x10;
	}
	else
	{
		local.filter_data = 0;
	}
	body_30b130 **body = bodies;
	for (int index = body_count - 1; index >= 0; index--, body++)
	{
		shape_30b130 const *shape = (*body)->shape;
		if (shape != 0)
		{
			transform_30b130 const &transform = (*body)->motion->transform;
			local.from = inverse_transform_30b130(transform, ray->from);
			local.to = inverse_transform_30b130(transform, ray->to);
			shape->cast_ray(&local, *body, callback);
		}
	}
}
