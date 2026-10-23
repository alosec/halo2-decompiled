// @flags /O2 /GL-
// Havok (third-party physics library): copies a body's nine motion vectors
// (from +0x30 to +0xb0) into another object at the same offsets.
#include <xmmintrin.h>

struct motion_state_319950
{
	__m128 vectors[9];
};

class body_319950
{
public:
	char unknown0[0x30];
	motion_state_319950 state;

	void snapshot_into(body_319950 *out) const;
};

// @retail 0x319950
void body_319950::snapshot_into(body_319950 *out) const
{
	out->state.vectors[0] = state.vectors[0];
	out->state.vectors[1] = state.vectors[1];
	out->state.vectors[2] = state.vectors[2];
	out->state.vectors[3] = state.vectors[3];
	out->state.vectors[4] = state.vectors[4];
	out->state.vectors[5] = state.vectors[5];
	out->state.vectors[6] = state.vectors[6];
	out->state.vectors[7] = state.vectors[7];
	out->state.vectors[8] = state.vectors[8];
}

// the 3x3 rotation part of a transform, as the matched setter declares it
struct hkRotation;

// a rotation held as four floats, set from a rotation matrix (0x2da2d0)
class c_component_rotation
{
public:
	__m128 value;
	void set(hkRotation const &rotation);
};

// three axes and a position
struct c_query_transform
{
	__m128 axes[3];
	__m128 position;

	void copy_from(c_query_transform const &other)
	{
		axes[0] = other.axes[0];
		axes[1] = other.axes[1];
		axes[2] = other.axes[2];
		position = other.position;
	}
};

// a point transformed by a matrix (0x2da610)
struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};

class moving_body_3199c0
{
public:
	char unknown0[0x20];
	__m128 local_point;
	c_component_rotation rotation;
	char unknown40[0x2c];
	float elapsed_6c;
	c_transformed_point point_in_world;
	c_query_transform transform;

	void function_3199c0(c_query_transform const *given_transform);
};

// takes a new transform: pushes the time at +0x6c a million on, and derives
// the rotation and the point's world position from the transform
// @retail 0x3199c0
void moving_body_3199c0::function_3199c0(c_query_transform const *given_transform)
{
	elapsed_6c += 1000000.0f;
	transform.copy_from(*given_transform);
	rotation.set(*(hkRotation const *)given_transform);
	point_in_world.transform(&transform, &local_point);
	((long *)&point_in_world.value)[3] = 0;
}
