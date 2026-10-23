// @flags /O2 /GL-
// Havok (third-party physics library): a motion step with the saved
// velocities immediately after the transform.
#include "motion_step_319f70.h"
#include <stddef.h>

// Local view of the accessed prefix, not the complete allocation or table.
// Keep the base's layout and inherited virtual destructor unchanged.
class motion_31b060 : public class_3114d0
{
public:
	__m128 saved_40;
	__m128 saved_50;

	void function_31b060(float const *step);

	MOTION_STEP_319F70
};

typedef char assert_31b060_base_extent[(sizeof(class_3114d0) == 0xc0) ? 1 : -1];
typedef char assert_31b060_scalar[(offsetof(motion_31b060, field_c) == 0xc) ? 1 : -1];
typedef char assert_31b060_centre[(offsetof(motion_31b060, field_20) == 0x20) ? 1 : -1];
typedef char assert_31b060_rotation[(offsetof(motion_31b060, rotation) == 0x30) ? 1 : -1];
typedef char assert_31b060_linear[(offsetof(motion_31b060, field_40) == 0x40) ? 1 : -1];
typedef char assert_31b060_angular[(offsetof(motion_31b060, field_50) == 0x50) ? 1 : -1];
typedef char assert_31b060_previous[(offsetof(motion_31b060, field_60) == 0x60) ? 1 : -1];
typedef char assert_31b060_position[(offsetof(motion_31b060, field_70) == 0x70) ? 1 : -1];
typedef char assert_31b060_axes[(offsetof(motion_31b060, transform_rotation) == 0x80) ? 1 : -1];
typedef char assert_31b060_translation[(offsetof(motion_31b060, transform_translation) == 0xb0) ? 1 : -1];
typedef char assert_31b060_saved_linear[(offsetof(motion_31b060, saved_40) == 0xc0) ? 1 : -1];
typedef char assert_31b060_saved_angular[(offsetof(motion_31b060, saved_50) == 0xd0) ? 1 : -1];
// This size is only the accessed extent, not evidence of the retail allocation.
typedef char assert_31b060_view_extent[(sizeof(motion_31b060) == 0xe0) ? 1 : -1];

// Save both velocities, advance and normalize the pose, then rebuild its transform.
// @retail 0x31b060
void motion_31b060::function_31b060(float const *step)
{
	step_319f70(step);
}
