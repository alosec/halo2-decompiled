/* Independently recovered segment/sphere predicate at retail 0x11e5e0. */
#ifndef SEGMENT_SPHERE_H
#define SEGMENT_SPHERE_H

#include "unknown_0259d0.h"

/* direction is the full segment displacement, not a unit vector.
   Retail subtracts the sphere's center from segment_start before the quadratic test. */
bool function_11e5e0(point3f const *segment_start, vector3f const *direction,
    point3f const *arg_28f920, real radius);

#endif
