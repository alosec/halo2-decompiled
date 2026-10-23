#ifndef MOTION_STEP_319F70_H
#define MOTION_STEP_319F70_H
// Havok (third-party physics library): a rigid motion's step: the velocities
// kept, the position moved by the linear velocity, the rotation turned by the
// angular velocity (the angle turned kept in the old position's last lane)
// and normalized, and the transform rebuilt from them. 0x319f70 is the step
// itself; the motions' other methods (0x31a0d0, 0x31a500, 0x31a8c0) inline it.
#include "derived_311920.h"

// the game's quaternion normalization (src/unknown_1d8d60.cpp)
struct s_1d8d60
{
	__m128 field_0;
	void function_1d8d60();
};

class quaternion_319300
{
public:
	__m128 vector;

	// this = a * b
	void function_319300(quaternion_319300 const &a, quaternion_319300 const &b);
};

// a quaternion, and a vector it turns (src/unknown_2db630.cpp)
struct rotation_2db630
{
	__m128 v;
};

struct vector_2db7a0
{
	__m128 v;

	void function_2db7a0(rotation_2db630 const &rotation, vector_2db7a0 const &vector);
};

// the first three lanes
static __declspec(align(16)) const unsigned long g_415120[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0};

// all four lanes set to one value
inline __m128 broadcast_319f70(float value)
{
	__m128 single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// a vector turned by a rotation matrix
inline __m128 rotate_319f70(rotation_372500 const &rotation, __m128 const &vector)
{
	__m128 z = _mm_mul_ps(rotation.column[2], _mm_shuffle_ps(vector, vector, 0xaa));
	__m128 y = _mm_mul_ps(rotation.column[1], _mm_shuffle_ps(vector, vector, 0x55));
	__m128 x = _mm_mul_ps(rotation.column[0], _mm_shuffle_ps(vector, vector, 0));
	return _mm_add_ps(_mm_add_ps(x, y), z);
}

// a vector worked on in place; its lanes are reached by reference
struct vector4_319f70
{
	__m128 v;

	void set_mul(__m128 const &a, __m128 const &b) { v = _mm_mul_ps(a, b); }
	void add_mul(__m128 const &a, __m128 const &b) { v = _mm_add_ps(v, _mm_mul_ps(a, b)); }
	void set_and(__m128 const &mask) { v = _mm_and_ps(v, mask); }
	float &operator()(int i) { return v.m128_f32[i]; }
	float xyz_length() const
	{
		__m128 squares = _mm_mul_ps(v, v);
		__m128 sum = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa),
			_mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
		float length;
		_mm_store_ss(&length, _mm_sqrt_ss(sum));
		return length;
	}
};

inline vector4_319f70 &as_vector_319f70(__m128 &v)
{
	return *(vector4_319f70 *)&v;
}

// the step's methods, for a class with the fields of 0x3114d0 and the
// velocities kept in saved_40 and saved_50: turn_319f70 keeps the
// velocities, moves the position and turns and normalizes the rotation,
// rebuilding its matrix; step_319f70 then takes the transform's position from
// the rotation matrix, step_2db7a0 from the quaternion
#define MOTION_STEP_319F70 \
	__forceinline void turn_319f70(float const *step) \
	{ \
		saved_40 = field_40; \
		saved_50 = field_50; \
		field_60 = field_70; \
		as_vector_319f70(field_70).add_mul(broadcast_319f70(*step), field_40); \
		as_vector_319f70(field_60)(3) = field_c * *step * as_vector_319f70(field_50).xyz_length(); \
		quaternion_319300 spin; \
		as_vector_319f70(spin.vector).set_mul(broadcast_319f70(0.5f), field_50); \
		spin.vector = _mm_and_ps(spin.vector, *(__m128 const *)g_415120); \
		quaternion_319300 change; \
		change.function_319300(spin, *(quaternion_319300 const *)&rotation); \
		rotation = _mm_add_ps(rotation, _mm_mul_ps(broadcast_319f70(*step), change.vector)); \
		((s_1d8d60 *)&rotation)->function_1d8d60(); \
		transform_rotation.function_372500(&rotation); \
	} \
	__forceinline void step_319f70(float const *step) \
	{ \
		turn_319f70(step); \
		transform_translation = _mm_sub_ps(field_70, rotate_319f70(transform_rotation, field_20)); \
		as_vector_319f70(field_70)(3) = 1.0f; \
	} \
	__forceinline void step_2db7a0(float const *step) \
	{ \
		turn_319f70(step); \
		{ \
			vector_2db7a0 centre; \
			centre.function_2db7a0(*(rotation_2db630 const *)&rotation, *(vector_2db7a0 const *)&field_20); \
			transform_translation = _mm_sub_ps(field_70, centre.v); \
		} \
		as_vector_319f70(field_70)(3) = 1.0f; \
	}

// a motion that keeps its velocities 0x20 bytes after the transform
class motion_319f70 : public class_3114d0
{
public:
	char unknownc0[0xe0 - 0xc0];
	__m128 saved_40;
	__m128 saved_50;

	void function_319f70(float const *step);

	MOTION_STEP_319F70
};

#endif
