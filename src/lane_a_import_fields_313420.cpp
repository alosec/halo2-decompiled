// @flags /O2 /Ob1 /GL-
#include "lane_a_constraints.h"
// @retail 0x313420
void lane_a_limited_hinge::import_fields(lane_a_limited_hinge_description const *d)
{
	angle_min = d->angle_min;
	angle_max = d->angle_max;
	friction = d->friction;
	pivot_a = d->pivot_a;
	axle_a = d->axle_a;
	perp_a = d->perp_a;
	perp_b = d->perp_b;
	pivot_b = d->pivot_b;
	axle_b = d->axle_b;
	reference_b = d->reference_b;
	previous_angle = 0;
	valid();
}
// @retail 0x313ea0
void lane_a_hinge::import_description(lane_a_hinge_description const *d)
{
	import_base(d);
	pivot_a = d->pivot_a;
	axle_a = d->axle_a;
	perpendicular_a = d->perpendicular_a;
	pivot_b = d->pivot_b;
	axle_b = d->axle_b;
	valid();
}
