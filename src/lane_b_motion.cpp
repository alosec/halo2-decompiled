// @flags /O2 /GL-
#include "motion_step_319f70.h"
#include <string.h>
struct lane_b_solver_result
{
	char unknown_00[0x10];
	__m128 next_linear;
	__m128 next_angular;
	char unknown_30[0x10];
	__m128 linear;
	__m128 angular;
	char unknown_60[0x20];
};
inline void lane_b_clear_w(__m128 &value)
{
	__m128 mask;
	memcpy(&mask, g_415120, sizeof(mask));
	value = _mm_and_ps(value, mask);
}
class lane_b_motion : public class_3114d0
{
public:
	__m128 saved_40;
	__m128 saved_50;
	const lane_b_solver_result *integrate(const float *step, float scale, const lane_b_solver_result *result);
};
// @retail 0x31b4c0
const lane_b_solver_result *lane_b_motion::integrate(const float *step, float scale, const lane_b_solver_result *result)
{
	as_vector_319f70(field_40).set_mul(broadcast_319f70(scale), result->linear);
	as_vector_319f70(field_50).set_mul(broadcast_319f70(scale), rotate_319f70(transform_rotation, result->angular));
	saved_40 = field_40;
	saved_50 = field_50;
	field_60 = field_70;
	as_vector_319f70(field_70).add_mul(broadcast_319f70(*step), field_40);
	as_vector_319f70(field_60)(3) = field_c * *step * as_vector_319f70(field_50).xyz_length();
	quaternion_319300 spin;
	as_vector_319f70(spin.vector).set_mul(broadcast_319f70(0.5f), field_50);
	lane_b_clear_w(spin.vector);
	quaternion_319300 change;
	change.function_319300(spin, *(quaternion_319300 const *)&rotation);
	rotation = _mm_add_ps(rotation, _mm_mul_ps(broadcast_319f70(*step), change.vector));
	((s_1d8d60 *)&rotation)->function_1d8d60();
	transform_rotation.function_372500(&rotation);
	transform_translation = _mm_sub_ps(field_70, rotate_319f70(transform_rotation, field_20));
	as_vector_319f70(field_70)(3) = 1.0f;
	field_40 = result->next_linear;
	field_50 = rotate_319f70(transform_rotation, result->next_angular);
	lane_b_clear_w(field_40);
	lane_b_clear_w(field_50);
	return result + 1;
}
