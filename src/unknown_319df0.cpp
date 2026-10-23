// @flags /O2 /GL-
// Havok (third-party physics library): one time step of the velocities of a
// body whose inertia is the same about every axis, then its state written
// out for the solver.
#include <xmmintrin.h>

// a vector of four copies of a float
inline __m128 splat_319df0(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// a vector updated in place
struct vector_319df0
{
	__m128 value;

	void add_product(__m128 const &a, __m128 const &b) { value = _mm_add_ps(value, _mm_mul_ps(a, b)); }
	void scale(__m128 const &a) { value = _mm_mul_ps(a, value); }
};

// a 3x3 matrix in rows of four floats
struct matrix_319df0
{
	__m128 rows[3];

	void set_diagonal_one()
	{
		rows[0].m128_f32[0] = 1.0f;
		rows[1].m128_f32[1] = 1.0f;
		rows[2].m128_f32[2] = 1.0f;
	}
};

// the step's length in time
struct step_319df0
{
	float delta_time;
};

// a body's state as the solver takes it: a kind (2), a flag, the linear
// velocity, the angular velocity, the inverse inertia (its fourth term the
// inverse mass), the centre and the axes
struct solver_state_319df0
{
	char kind;
	char unknown1[0xb];
	char flag_c;
	char unknownd[3];
	__m128 linear_velocity;
	__m128 angular_velocity;
	__m128 inverse_inertia;
	__m128 centre;
	matrix_319df0 axes;
};

class body_319df0
{
public:
	char unknown0[0x10];
	float inverse_inertia;
	float linear_damping;
	float angular_damping;
	char unknown1c[0x10];
	float inverse_mass;
	char unknown30[0x10];
	vector_319df0 linear_velocity;
	vector_319df0 angular_velocity;
	char unknown60[0x10];
	__m128 centre;
	char unknown80[0x40];
	__m128 force;
	__m128 torque;

	solver_state_319df0 *function_319df0(step_319df0 const *step, __m128 const *gravity, solver_state_319df0 *out);
};

// @retail 0x319df0
solver_state_319df0 *body_319df0::function_319df0(step_319df0 const *step, __m128 const *gravity, solver_state_319df0 *out)
{
	float const delta_time = step->delta_time;
	linear_velocity.add_product(splat_319df0(delta_time * inverse_mass), force);
	linear_velocity.add_product(splat_319df0(delta_time), *gravity);
	float linear_scale = 1.0f - delta_time * linear_damping;
	if (0.0f > linear_scale)
	{
		linear_scale = 0.0f;
	}
	linear_velocity.scale(splat_319df0(linear_scale));
	__m128 const zero = _mm_setzero_ps();
	force = zero;
	angular_velocity.add_product(splat_319df0(delta_time * inverse_inertia), torque);
	torque = zero;
	float angular_scale = 1.0f - delta_time * angular_damping;
	if (0.0f > angular_scale)
	{
		angular_scale = 0.0f;
	}
	angular_velocity.scale(splat_319df0(angular_scale));
	out->kind = 2;
	out->inverse_inertia = _mm_set1_ps(inverse_inertia);
	out->inverse_inertia.m128_f32[3] = inverse_mass;
	out->centre = centre;
	out->angular_velocity = angular_velocity.value;
	out->linear_velocity = linear_velocity.value;
	out->axes.rows[0] = zero;
	out->axes.rows[1] = zero;
	out->axes.rows[2] = zero;
	out->axes.set_diagonal_one();
	out->flag_c = 1;
	return out + 1;
}
