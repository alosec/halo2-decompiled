#pragma once

/* a unit direction from a yaw and a pitch, as the game's callers inline it */

#include "unknown_0259d0.h"

__forceinline void direction_from_angles(vector3f *direction, real yaw, real pitch)
{
	real cosine = (real)cos(pitch);
	direction->i = (real)cos(yaw) * cosine;
	direction->j = (real)sin(yaw) * cosine;
	direction->k = (real)sin(pitch);
}
