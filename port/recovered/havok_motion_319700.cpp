// @flags /O2 /GL-
// Havok (third-party physics library): the rigid motions' setters and force
// accumulators, reached through the motion vtables (0x414328, 0x4143a8,
// 0x414428, 0x415130, 0x4151b0, 0x415230, 0x4152b0). Slot numbers are those
// of 0x415230 (slot 0 its deleting destructor):
//    7 set_mass                  0x319700  (hkMotion::setMass)
//    8 set_inverse_mass          0x31ac80  (hkMotion::setMassInv)
//   11 set_inertia_local         0x31adf0
//   12 set_inertia_inverse_local 0x31ae40
//   15 set_centre_of_mass_local  0x319750
//   16 set_position              0x3197a0
//   17 set_rotation              0x3197f0
//   18 set_position_and_rotation 0x319840
//   20 set_linear_velocity       0x319890
//   21 set_angular_velocity      0x3198a0
//   26 apply_force_at_point      0x31acd0
//   27 apply_force               0x31aca0
//   28 apply_torque              0x31ad50
//   29 set_step_position         0x31b700  (hkMotion::setStepPosition)
// and the rotation matrix built from a quaternion (0x372500) they use.
//
// The motion keeps its centre of mass twice, at the step's start and end
// (0x60, 0x70); the start's last lane holds a value that each change of the
// transform pushes a million on (the motion is then seen as having jumped),
// and the end's last lane the step's time. Its local centre of mass shares
// its vector with the inverse mass (lane 3, 0x2c), and so does the inverse
// inertia (0xec).
#include <xmmintrin.h>

// ---------------------------------------------------------------------------
// recovered code this file calls, declared as src declares it

// a point transformed by a matrix (src/unknown_2da610.cpp)
struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};

// the product of two quaternions (src/unknown_319300.cpp)
class quaternion_319300
{
public:
	__m128 vector;

	// this = a * b
	void function_319300(quaternion_319300 const &a, quaternion_319300 const &b);
};

// a quaternion normalized in place (src/unknown_1d8d60.cpp)
struct s_1d8d60
{
	__m128 field_0;
	void function_1d8d60();
};

// a vector rotated by a quaternion (src/unknown_2db630.cpp)
struct rotation_2db630
{
	__m128 v;
};

struct vector_2db7a0
{
	__m128 v;

	void function_2db7a0(rotation_2db630 const &rotation, vector_2db7a0 const &vector);
};

// ---------------------------------------------------------------------------

// a vector of four copies of a float
inline __m128 splat_319700(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// the cross product of two vectors' first three terms
inline __m128 cross_319700(__m128 const &a, __m128 const &b)
{
	return _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9)));
}

// the first three lanes kept, the last cleared (retail 0x415120)
static __declspec(align(16)) const unsigned long xyz_mask_319700[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0x00000000};

// a rotation's three columns
class rotation_372500
{
public:
	float columns[3][4];

	// the rotation of a unit quaternion (x, y, z, w), column by column
	void set_from_quaternion(__m128 const *quaternion);
};

// @retail 0x372500
void rotation_372500::set_from_quaternion(__m128 const *quaternion)
{
	float const *q = (float const *)quaternion;
	float const x2 = q[0] + q[0];
	float const y2 = q[1] + q[1];
	float const z2 = q[2] + q[2];
	float const xx = x2 * q[0];
	float const xy = y2 * q[0];
	float const xz = z2 * q[0];
	float const yy = y2 * q[1];
	float const yz = z2 * q[1];
	float const zz = z2 * q[2];
	float const xw = x2 * q[3];
	float const yw = y2 * q[3];
	float const zw = z2 * q[3];

	columns[0][0] = 1.0f - (zz + yy);
	columns[0][1] = xy + zw;
	columns[0][2] = xz - yw;
	columns[0][3] = 0.0f;
	columns[1][0] = xy - zw;
	columns[1][1] = 1.0f - (zz + xx);
	columns[1][2] = xw + yz;
	columns[1][3] = 0.0f;
	columns[2][0] = yw + xz;
	columns[2][1] = yz - xw;
	columns[2][2] = 1.0f - (yy + xx);
	columns[2][3] = 0.0f;
}

// three axes and a position
struct transform_319700
{
	__m128 axes[3];
	__m128 position;
};

// the slots the setters call through the motion's own vtable
class motion_slots_319700
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual void slot_5() = 0;
	virtual void slot_6() = 0;
	virtual void slot_7(float mass) = 0;
	virtual void set_inverse_mass(float inverse_mass) = 0;
};

// the rigid motion (the class's vtable is its retail one)
class motion_319700
{
public:
	void const *vtable;
	char unknown04[0xc - 0x4];
	// scales the angular speed into the start's last lane in
	// set_step_position
	float field_0c;
	char unknown10[0x20 - 0x10];
	// the local centre of mass; lane 3 the inverse mass
	__m128 local_centre;
	// the orientation, a quaternion
	__m128 rotation;
	__m128 linear_velocity;
	__m128 angular_velocity;
	// the centre of mass at the step's start and end
	__m128 centre_start;
	c_transformed_point centre_end;
	transform_319700 transform;
	// the velocities set_step_position integrates with
	__m128 step_linear_velocity;
	__m128 step_angular_velocity;
	// the inverse inertia's diagonal; lane 3 the inverse mass again
	__m128 inverse_inertia;
	__m128 force;
	__m128 torque;

	void set_mass(float mass);
	void set_inverse_mass(float inverse_mass);
	void set_inertia_local(__m128 const *inertia);
	void set_inertia_inverse_local(__m128 const *inertia_inverse);
	void set_centre_of_mass_local(__m128 const *centre);
	void set_position(__m128 const *position);
	void set_rotation(__m128 const *rotation);
	void set_position_and_rotation(__m128 const *position, __m128 const *rotation);
	void set_linear_velocity(__m128 const *velocity);
	void set_angular_velocity(__m128 const *velocity);
	void apply_force_at_point(__m128 const *force, __m128 const *point);
	void apply_force(__m128 const *force);
	void apply_torque(__m128 const *torque);
	void set_step_position(float position, float delta_time);

private:
	void step_by(float time);
};

typedef char assert_motion_319700_size[sizeof(motion_319700) == 0x110 ? 1 : -1];

// @retail 0x319700
void motion_319700::set_mass(float mass)
{
	float inverse_mass = mass == 0.0f ? 0.0f : 1.0f / mass;
	((motion_slots_319700 *)this)->set_inverse_mass(inverse_mass);
}

// @retail 0x31ac80
void motion_319700::set_inverse_mass(float inverse_mass)
{
	local_centre.m128_f32[3] = inverse_mass;
	inverse_inertia.m128_f32[3] = inverse_mass;
}

// the inverse of the diagonal of the inertia matrix (given by columns)
// @retail 0x31adf0
void motion_319700::set_inertia_local(__m128 const *inertia)
{
	float const inverse_mass = inverse_inertia.m128_f32[3];
	inverse_inertia.m128_f32[0] = 1.0f / inertia[0].m128_f32[0];
	inverse_inertia.m128_f32[1] = 1.0f / inertia[1].m128_f32[1];
	inverse_inertia.m128_f32[2] = 1.0f / inertia[2].m128_f32[2];
	inverse_inertia.m128_f32[3] = inverse_mass;
}

// @retail 0x31ae40
void motion_319700::set_inertia_inverse_local(__m128 const *inertia_inverse)
{
	float const inverse_mass = inverse_inertia.m128_f32[3];
	inverse_inertia.m128_f32[0] = inertia_inverse[0].m128_f32[0];
	inverse_inertia.m128_f32[1] = inertia_inverse[1].m128_f32[1];
	inverse_inertia.m128_f32[2] = inertia_inverse[2].m128_f32[2];
	inverse_inertia.m128_f32[3] = inverse_mass;
}

// @retail 0x319750
void motion_319700::set_centre_of_mass_local(__m128 const *centre)
{
	centre_start.m128_f32[3] += 1000000.0f;
	float const inverse_mass = local_centre.m128_f32[3];
	local_centre = *centre;
	local_centre.m128_f32[3] = inverse_mass;
	float const time = centre_end.value.m128_f32[3];
	centre_end.transform(&transform, &local_centre);
	centre_end.value.m128_f32[3] = time;
}

// @retail 0x3197a0
void motion_319700::set_position(__m128 const *position)
{
	transform.position = *position;
	centre_end.transform(&transform, &local_centre);
	centre_start = centre_end.value;
	centre_start.m128_f32[3] += 1000000.0f;
	centre_end.value.m128_f32[3] = 0.0f;
}

// @retail 0x3197f0
void motion_319700::set_rotation(__m128 const *rotation_)
{
	centre_start.m128_f32[3] += 1000000.0f;
	rotation = *rotation_;
	((rotation_372500 *)&transform)->set_from_quaternion(&rotation);
	// (retail stores the position back over itself here, as setting the
	// transform from its rotation and its own position does)
	centre_end.transform(&transform, &local_centre);
	centre_end.value.m128_f32[3] = 0.0f;
}

// @retail 0x319840
void motion_319700::set_position_and_rotation(__m128 const *position, __m128 const *rotation_)
{
	centre_start.m128_f32[3] += 1000000.0f;
	rotation = *rotation_;
	((rotation_372500 *)&transform)->set_from_quaternion(&rotation);
	transform.position = *position;
	centre_end.transform(&transform, &local_centre);
	centre_end.value.m128_f32[3] = 0.0f;
}

// @retail 0x319890
void motion_319700::set_linear_velocity(__m128 const *velocity)
{
	linear_velocity = *velocity;
}

// @retail 0x3198a0
void motion_319700::set_angular_velocity(__m128 const *velocity)
{
	angular_velocity = *velocity;
}

// the force, and its torque about the centre of mass; the torque's last lane
// set says a torque is pending
// @retail 0x31acd0
void motion_319700::apply_force_at_point(__m128 const *force_, __m128 const *point)
{
	force = _mm_add_ps(force, *force_);
	__m128 const arm = _mm_sub_ps(*point, centre_end.value);
	torque = _mm_add_ps(torque, cross_319700(arm, *force_));
	torque.m128_f32[3] = 1.0f;
}

// @retail 0x31aca0
void motion_319700::apply_force(__m128 const *force_)
{
	force = _mm_add_ps(force, *force_);
}

// @retail 0x31ad50
void motion_319700::apply_torque(__m128 const *torque_)
{
	torque = _mm_add_ps(torque, *torque_);
	torque.m128_f32[3] = 1.0f;
}

// moves the motion by `time` along its step velocities: the end centre
// becomes the start, the end moves on, the start's last lane takes the angle
// turned (the angular speed by field_0c by the time), and the orientation
// turns by the angular velocity (q += time * (w/2, 0) * q, renormalized),
// the transform following
void motion_319700::step_by(float time)
{
	centre_start = centre_end.value;
	centre_end.value = _mm_add_ps(centre_end.value, _mm_mul_ps(splat_319700(time), step_linear_velocity));

	__m128 const squares = _mm_mul_ps(step_angular_velocity, step_angular_velocity);
	float length_squared;
	_mm_store_ss(&length_squared, _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares)));
	float const speed = __builtin_sqrtf(length_squared);
	centre_start.m128_f32[3] = speed * field_0c * time;

	quaternion_319300 spin;
	spin.vector = _mm_and_ps(_mm_mul_ps(splat_319700(0.5f), step_angular_velocity),
		*(__m128 const *)xyz_mask_319700);
	quaternion_319300 turned;
	turned.function_319300(spin, *(quaternion_319300 const *)&rotation);
	rotation = _mm_add_ps(rotation, _mm_mul_ps(splat_319700(time), turned.vector));
	((s_1d8d60 *)&rotation)->function_1d8d60();
	((rotation_372500 *)&transform)->set_from_quaternion(&rotation);

	vector_2db7a0 offset;
	offset.function_2db7a0(*(rotation_2db630 const *)&rotation, *(vector_2db7a0 const *)&local_centre);
	transform.position = _mm_sub_ps(centre_end.value, offset.v);
}

// steps back over the time the last step covered (the end's last lane), then
// forward by the fraction `position` of `delta_time`, which becomes the end's
// last lane
// @retail 0x31b700
void motion_319700::set_step_position(float position, float delta_time)
{
	step_by(-(delta_time * centre_end.value.m128_f32[3]));
	centre_end.value.m128_f32[3] = 1.0f;
	step_by(position * delta_time);
	centre_end.value.m128_f32[3] = position;
}
