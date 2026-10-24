// @flags /O2 /GL- /GX
// Havok (third-party physics library): two of the motions 0x311940 builds
// (src/unknown_311940.cpp), both on the object 0x3114d0 makes from a position
// and an orientation (include/derived_311920.h):
//   0x3193c0  the motion of vtable 0x415130 (the base of 0x31a6b0's, vtable
//             0x4151b0): no inverse mass, field 0x10 and 0xc0 cleared, the
//             end centre's last lane (the step's time) one
//   0x319740  slot 8 of both those vtables, set_inverse_mass: only stores the
//             value into the local centre's last lane (0x2c); unlike the
//             rigid motion's 0x31ac80 it leaves the inertia alone
//   0x31ad80  the rigid motion (vtable 0x4152b0, the class of
//             port/recovered/havok_motion_319700.cpp): unit inverse inertia
//             and inverse mass, no force, torque or step velocities, field
//             0x10 minus one
//
// The classes carry their own names so as not to clash with the different
// declarations src/ gives class_3193c0 and class_31ad80; the placeholders in
// src/stubs/lane_k1.cpp branch here by address. The vtables are the retail
// ones, stored by address.
//
// 0x319740 takes a float (retail stores the dword argument unchanged, and
// the motions' callers through slot 8 pass a mass; havok_motion_319700.cpp
// declares the slot as set_inverse_mass(float)). src/unknown_31a6a0.cpp
// declares it with an unsigned long and passes 0, which reaches this
// definition through the placeholder with the argument in the wrong
// register; that caller needs its declaration made float.
#include "derived_311920.h"

class motion_3193c0 : public class_3114d0
{
public:
	motion_3193c0(void const *position, void const *orientation);
	void set_inverse_mass(float inverse_mass);

	long field_c0;
	char unknownc4[0xd0 - 0xc4];
};

typedef char assert_motion_3193c0_size[sizeof(motion_3193c0) == 0xd0 ? 1 : -1];

// @retail 0x3193c0
motion_3193c0::motion_3193c0(void const *position, void const *orientation)
	: class_3114d0(position, orientation)
{
	*(unsigned long *)this = 0x415130;
	field_c0 = 0;
	field_10 = 0;
	set_inverse_mass(0.0f);
	field_70.m128_f32[3] = 1.0f;
}

// @retail 0x319740
void motion_3193c0::set_inverse_mass(float inverse_mass)
{
	field_20.m128_f32[3] = inverse_mass;
}

class motion_31ad80 : public class_3114d0
{
public:
	motion_31ad80(void const *position, void const *orientation);

	__m128 step_linear_velocity;
	__m128 step_angular_velocity;
	// the inverse inertia's diagonal; lane 3 the inverse mass
	__m128 inverse_inertia;
	__m128 force;
	__m128 torque;
};

typedef char assert_motion_31ad80_size[sizeof(motion_31ad80) == 0x110 ? 1 : -1];

// @retail 0x31ad80
motion_31ad80::motion_31ad80(void const *position, void const *orientation)
	: class_3114d0(position, orientation)
{
	*(unsigned long *)this = 0x4152b0;
	inverse_inertia.m128_f32[0] = 1.0f;
	inverse_inertia.m128_f32[1] = 1.0f;
	inverse_inertia.m128_f32[2] = 1.0f;
	inverse_inertia.m128_f32[3] = 1.0f;
	force = _mm_setzero_ps();
	torque = _mm_setzero_ps();
	step_linear_velocity = _mm_setzero_ps();
	step_angular_velocity = _mm_setzero_ps();
	// 0xbf800000
	*(float *)&field_10 = -1.0f;
}
