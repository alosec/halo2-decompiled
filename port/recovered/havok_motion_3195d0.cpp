// @flags /O2 /GL-
// Havok (third-party physics library): the keyframed motion's own slots
// (vtable 0x415130, the motion 0x3193c0 builds; see
// port/recovered/havok_motion_3193c0.cpp). It has no inverse mass, so the
// world step moves it along its velocities without any solver work:
//   slot 1  0x3195d0  integrate: the step's move (as 0x319f70's, but the
//                     velocities are not saved anywhere and the transform's
//                     position comes from the quaternion through 0x2db7a0)
//   slot 3  0x319430  the solver's view of the body: kind 2, flag 0xc set,
//                     its velocities and centre, no inverse inertia, the
//                     identity for the axes; the next record (0x80 on)
//   slot 4  0x319490  the same move as slot 1 for the solver's own step
//                     (step, scale, out), giving back the next record
// Slot numbers are those of 0x415130 (slot 0 the deleting destructor); the
// slots' signatures are those of the rigid motion's slots 1, 3 and 4
// (0x31b060, src/unknown_31ae70.cpp's 0x31b300, src/unknown_31a8c0.cpp).
#include "motion_step_319f70.h"
#include <stddef.h>

// the record slot 3 fills (src/unknown_31ae70.cpp's solver_state_31b300)
struct solver_state_319430
{
	char kind;
	char unknown1[0xb];
	char flag_c;
	char unknownd[3];
	__m128 linear_velocity;
	__m128 local_angular_velocity;
	__m128 inverse_inertia;
	__m128 centre;
	__m128 axes_transposed[3];
};

typedef char assert_solver_state_319430_size[sizeof(solver_state_319430) == 0x80 ? 1 : -1];

class motion_3195d0 : public class_3114d0
{
public:
	void integrate(float const *step);
	solver_state_319430 *solver_state(float const *step, __m128 const *gravity, solver_state_319430 *out);
	solver_state_319430 *solver_step(float const *step, float scale, solver_state_319430 *out);

private:
	__forceinline void move(float const *step)
	{
		field_60 = field_70;
		as_vector_319f70(field_70).add_mul(broadcast_319f70(*step), field_40);
		as_vector_319f70(field_60)(3) = field_c * *step * as_vector_319f70(field_50).xyz_length();
		quaternion_319300 spin;
		as_vector_319f70(spin.vector).set_mul(broadcast_319f70(0.5f), field_50);
		spin.vector = _mm_and_ps(spin.vector, *(__m128 const *)g_415120);
		quaternion_319300 change;
		change.function_319300(spin, *(quaternion_319300 const *)&rotation);
		rotation = _mm_add_ps(rotation, _mm_mul_ps(broadcast_319f70(*step), change.vector));
		((s_1d8d60 *)&rotation)->function_1d8d60();
		transform_rotation.function_372500(&rotation);
		vector_2db7a0 centre;
		centre.function_2db7a0(*(rotation_2db630 const *)&rotation, *(vector_2db7a0 const *)&field_20);
		transform_translation = _mm_sub_ps(field_70, centre.v);
		as_vector_319f70(field_70)(3) = 1.0f;
	}
};

typedef char assert_motion_3195d0_centre[offsetof(motion_3195d0, field_70) == 0x70 ? 1 : -1];
typedef char assert_motion_3195d0_translation[offsetof(motion_3195d0, transform_translation) == 0xb0 ? 1 : -1];

// @retail 0x3195d0
void motion_3195d0::integrate(float const *step)
{
	move(step);
}

// @retail 0x319430
solver_state_319430 *motion_3195d0::solver_state(float const *step, __m128 const *gravity, solver_state_319430 *out)
{
	out->kind = 2;
	__m128 const zero = _mm_setzero_ps();
	out->inverse_inertia = zero;
	out->centre = field_70;
	out->local_angular_velocity = field_50;
	out->linear_velocity = field_40;
	out->axes_transposed[0] = zero;
	out->axes_transposed[1] = zero;
	out->axes_transposed[2] = zero;
	out->axes_transposed[0].m128_f32[0] = 1.0f;
	out->axes_transposed[1].m128_f32[1] = 1.0f;
	out->axes_transposed[2].m128_f32[2] = 1.0f;
	out->flag_c = 1;
	return out + 1;
}

// @retail 0x319490
solver_state_319430 *motion_3195d0::solver_step(float const *step, float scale, solver_state_319430 *out)
{
	move(step);
	return out + 1;
}

// ---------------------------------------------------------------------------
// slot 4 of the rigid motion of vtable 0x4152b0 (0x31b4c0): the solver's
// summed velocities scaled (the angular one turned into the world's frame)
// make the step's velocities, which the step keeps at 0xc0 and 0xd0; after
// it the body takes the solver's own last velocities (unscaled), their last
// lanes cleared. The step's transform position comes from the rotation
// matrix (include/motion_step_319f70.h's step_319f70).

// one body's record as the solver hands it back (0x80 bytes)
struct solver_result_31b4c0
{
	char unknown0[0x10];
	__m128 linear_velocity;
	__m128 angular_velocity;
	char unknown30[0x10];
	__m128 linear_sum;
	__m128 angular_sum;
	char unknown60[0x20];
};

typedef char assert_solver_result_31b4c0_size[sizeof(solver_result_31b4c0) == 0x80 ? 1 : -1];

class motion_31b4c0 : public class_3114d0
{
public:
	__m128 saved_40;
	__m128 saved_50;

	solver_result_31b4c0 const *read_back(float const *step, float scale, solver_result_31b4c0 const *result);

	MOTION_STEP_319F70
};

typedef char assert_motion_31b4c0_saved[offsetof(motion_31b4c0, saved_40) == 0xc0 ? 1 : -1];

// @retail 0x31b4c0
solver_result_31b4c0 const *motion_31b4c0::read_back(float const *step, float scale, solver_result_31b4c0 const *result)
{
	as_vector_319f70(field_40).set_mul(broadcast_319f70(scale), result->linear_sum);
	as_vector_319f70(field_50).set_mul(broadcast_319f70(scale), rotate_319f70(transform_rotation, result->angular_sum));
	step_319f70(step);
	field_40 = result->linear_velocity;
	field_50 = rotate_319f70(transform_rotation, result->angular_velocity);
	as_vector_319f70(field_40).set_and(*(__m128 const *)g_415120);
	as_vector_319f70(field_50).set_and(*(__m128 const *)g_415120);
	return result + 1;
}
