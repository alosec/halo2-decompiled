// @flags /O2 /GL-
// Havok (third-party physics library): impulses on a body: one at a point,
// changing both velocities, and a turning one, changing the angular velocity
// through the inverse inertia held in the body's frame.
#include <xmmintrin.h>

// a vector of four copies of a float
inline __m128 splat_31ae70(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// a vector updated in place
struct vector_31ae70
{
	__m128 value;

	void add_product(__m128 const &a, __m128 const &b) { value = _mm_add_ps(value, _mm_mul_ps(a, b)); }
	void add(__m128 const &a) { value = _mm_add_ps(value, a); }
	void scale(__m128 const &a) { value = _mm_mul_ps(a, value); }
};

// the step's length in time
struct step_31b1c0
{
	float delta_time;
};

// a 3x3 matrix set to the transpose of another (0x2da7a0)
class class_2da7a0
{
public:
	float rows[3][4];

	void set_transpose(const class_2da7a0 &other);
};

// a body's state as the solver takes it: a kind (2), a flag, the linear
// velocity, the angular velocity in the body's frame, the inverse inertia,
// the centre and the transposed axes
struct solver_state_31b300
{
	char kind;
	char unknown1[0xb];
	char flag_c;
	char unknownd[3];
	__m128 linear_velocity;
	__m128 local_angular_velocity;
	__m128 inverse_inertia;
	__m128 centre;
	class_2da7a0 axes_transposed;
};

class body_31ae70
{
public:
	char unknown0[0x14];
	float linear_damping;
	float angular_damping;
	char unknown1c[0x10];
	float inverse_mass;
	char unknown30[0x10];
	vector_31ae70 linear_velocity;
	vector_31ae70 angular_velocity;
	char unknown60[0x10];
	__m128 centre;
	__m128 axes[3];
	char unknownb0[0x30];
	__m128 inverse_inertia;
	__m128 force;
	__m128 torque;

	void function_31ae70(__m128 const *impulse, __m128 const *point);
	void function_31af90(__m128 const *impulse);
	void function_31b1c0(step_31b1c0 const *step, __m128 const *gravity);
	solver_state_31b300 *function_31b300(step_31b1c0 const *step, __m128 const *gravity, solver_state_31b300 *out);
};

// @retail 0x31ae70
void body_31ae70::function_31ae70(__m128 const *impulse, __m128 const *point)
{
	linear_velocity.add_product(splat_31ae70(inverse_mass), *impulse);
	__m128 const arm = _mm_sub_ps(*point, centre);
	__m128 const p = *impulse;
	__m128 const torque = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(arm, arm, 0xc9), _mm_shuffle_ps(p, p, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(arm, arm, 0xd2), _mm_shuffle_ps(p, p, 0xc9)));
	__m128 row0 = axes[0];
	__m128 row1 = axes[1];
	__m128 row2 = axes[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	__m128 const local = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(torque, torque, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(torque, torque, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(torque, torque, 0xaa)));
	__m128 const scaled = _mm_mul_ps(inverse_inertia, local);
	__m128 const turn = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(_mm_load_ps(axes[0].m128_f32), _mm_shuffle_ps(scaled, scaled, 0x00)),
		_mm_mul_ps(_mm_load_ps(axes[1].m128_f32), _mm_shuffle_ps(scaled, scaled, 0x55))),
		_mm_mul_ps(_mm_load_ps(axes[2].m128_f32), _mm_shuffle_ps(scaled, scaled, 0xaa)));
	angular_velocity.add(turn);
}
// @retail 0x31af90
void body_31ae70::function_31af90(__m128 const *impulse)
{
	__m128 row0 = axes[0];
	__m128 row1 = axes[1];
	__m128 row2 = axes[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	__m128 const torque = *impulse;
	__m128 const local = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(torque, torque, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(torque, torque, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(torque, torque, 0xaa)));
	__m128 const scaled = _mm_mul_ps(inverse_inertia, local);
	__m128 const turn = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(_mm_load_ps(axes[0].m128_f32), _mm_shuffle_ps(scaled, scaled, 0x00)),
		_mm_mul_ps(_mm_load_ps(axes[1].m128_f32), _mm_shuffle_ps(scaled, scaled, 0x55))),
		_mm_mul_ps(_mm_load_ps(axes[2].m128_f32), _mm_shuffle_ps(scaled, scaled, 0xaa)));
	angular_velocity.add(turn);
}

// one time step of the velocities: the gathered force and gravity added to
// the linear velocity, the gathered torque applied as a turning impulse, and
// both velocities damped (never below zero)
// @retail 0x31b1c0
void body_31ae70::function_31b1c0(step_31b1c0 const *step, __m128 const *gravity)
{
	float const delta_time = step->delta_time;
	linear_velocity.add_product(splat_31ae70(delta_time * inverse_mass), force);
	linear_velocity.add_product(splat_31ae70(delta_time), *gravity);
	float linear_scale = 1.0f - delta_time * linear_damping;
	if (0.0f > linear_scale)
	{
		linear_scale = 0.0f;
	}
	linear_velocity.scale(splat_31ae70(linear_scale));
	__m128 const zero = _mm_setzero_ps();
	force = zero;
	if (torque.m128_f32[3] != 0.0f)
	{
		__m128 const impulse = _mm_mul_ps(splat_31ae70(step->delta_time), torque);
		function_31af90(&impulse);
		torque = zero;
	}
	float angular_scale = 1.0f - delta_time * angular_damping;
	if (0.0f > angular_scale)
	{
		angular_scale = 0.0f;
	}
	angular_velocity.scale(splat_31ae70(angular_scale));
}
// the same time step, then the body's state written out for the solver;
// gives back the space after it
// @retail 0x31b300
solver_state_31b300 *body_31ae70::function_31b300(step_31b1c0 const *step, __m128 const *gravity, solver_state_31b300 *out)
{
	float const delta_time = step->delta_time;
	linear_velocity.add_product(splat_31ae70(delta_time * inverse_mass), force);
	linear_velocity.add_product(splat_31ae70(delta_time), *gravity);
	float linear_scale = 1.0f - delta_time * linear_damping;
	if (0.0f > linear_scale)
	{
		linear_scale = 0.0f;
	}
	linear_velocity.scale(splat_31ae70(linear_scale));
	__m128 const zero = _mm_setzero_ps();
	force = zero;
	if (torque.m128_f32[3] != 0.0f)
	{
		__m128 const impulse = _mm_mul_ps(splat_31ae70(step->delta_time), torque);
		function_31af90(&impulse);
		torque = zero;
	}
	float angular_scale = 1.0f - delta_time * angular_damping;
	if (0.0f > angular_scale)
	{
		angular_scale = 0.0f;
	}
	angular_velocity.scale(splat_31ae70(angular_scale));
	out->axes_transposed.set_transpose(*(class_2da7a0 const *)axes);
	out->kind = 2;
	out->inverse_inertia = inverse_inertia;
	out->linear_velocity = linear_velocity.value;
	out->centre = centre;
	__m128 const w = angular_velocity.value;
	__m128 const *rows = (__m128 const *)out->axes_transposed.rows;
	out->local_angular_velocity = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(rows[0], _mm_shuffle_ps(w, w, 0x00)),
		_mm_mul_ps(rows[1], _mm_shuffle_ps(w, w, 0x55))),
		_mm_mul_ps(rows[2], _mm_shuffle_ps(w, w, 0xaa)));
	out->flag_c = 0;
	return out + 1;
}
