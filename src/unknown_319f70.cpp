// @flags /O2 /GL-
// Havok (third-party physics library): a rigid motion's step: the velocities
// kept, the position moved by the linear velocity, the rotation turned by the
// angular velocity (the angle turned kept in the old position's last lane)
// and normalized, and the transform rebuilt from them.
#include "motion_step_319f70.h"

// @retail 0x319f70
void motion_319f70::function_319f70(float const *step)
{
	step_319f70(step);
}
