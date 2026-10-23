// @flags /O2 /GL-
// Havok (third-party physics library): a rigid motion's step from the
// velocities the solver hands back, its angular velocity given in the
// body's own frame (include/motion_step_319f70.h has the step).
#include "motion_step_319f70.h"

// one body's velocities as the solver hands them back, 0x80 bytes each
struct solver_result_31a8c0
{
	char unknown0[0x40];
	__m128 linear_velocity;
	__m128 angular_velocity;
	char unknown60[0x20];
};

// a motion that keeps its velocities right after the transform
class motion_31a8c0 : public class_3114d0
{
public:
	__m128 saved_40;
	__m128 saved_50;

	solver_result_31a8c0 const *function_31a8c0(float const *step, float scale, solver_result_31a8c0 const *result);

	MOTION_STEP_319F70
};

// the velocities taken from the solver's result, scaled, the angular one
// turned into the world's frame, their last lanes cleared, then a step with
// the transform's position from the quaternion; the next result is returned
// @retail 0x31a8c0
solver_result_31a8c0 const *motion_31a8c0::function_31a8c0(float const *step, float scale, solver_result_31a8c0 const *result)
{
	as_vector_319f70(field_40).set_mul(broadcast_319f70(scale), result->linear_velocity);
	as_vector_319f70(field_50).set_mul(broadcast_319f70(scale), rotate_319f70(transform_rotation, result->angular_velocity));
	as_vector_319f70(field_40).set_and(*(__m128 const *)g_415120);
	as_vector_319f70(field_50).set_and(*(__m128 const *)g_415120);
	step_2db7a0(step);
	return result + 1;
}
