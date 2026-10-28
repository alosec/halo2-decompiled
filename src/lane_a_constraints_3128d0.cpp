// @flags /O2 /Ob1 /GL-
#include "lane_a_constraints.h"

// Serialization callback from retail table 4145f8, not a virtual stub.
// @retail 0x3128e0
void __cdecl lane_a_import_ragdoll(lane_a_ragdoll *object, lane_a_ragdoll_description const *d)
{
	object->import_description(d);
}
// @retail 0x312a20
lane_a_solver_result *lane_a_ragdoll::runtime() { return results; }
// @retail 0x312a30
void lane_a_ragdoll::import_fields(lane_a_ragdoll_description const *d)
{
	pivot_a = d->pivot_a;
	pivot_b = d->pivot_b;
	plane_a = d->twist_b;
	plane_b = d->plane_b;
	twist_a = d->twist_a;
	twist_b = d->plane_a;
	cone_limit = d->cone_limit;
	plane_low = d->plane_low;
	plane_high = d->plane_high;
	twist_low = d->twist_low;
	twist_high = d->twist_high;
	friction = d->friction;
}
// @retail 0x312de0
void lane_a_ragdoll::import_description(lane_a_ragdoll_description const *d)
{
	import_base(d);
	import_fields(d);
}
// @retail 0x312b30
void lane_a_ragdoll::info(lane_a_constraint_info *o) const
{
	o->maximum = 0;
	o->result_bytes = 0x54;
	o->op_bytes = 0xf0;
	o->result_count = 6;
	if (friction != 0.0f)
	{
		o->result_bytes = 0x6c;
		o->op_bytes = 0x150;
		o->result_count = 9;
	}
}
// @retail 0x313410
lane_a_solver_result *lane_a_limited_hinge::runtime() { return results; }
// @retail 0x313520
void lane_a_limited_hinge::info(lane_a_constraint_info *o) const
{
	o->maximum = 0;
	o->result_bytes = 0x44;
	o->op_bytes = 0x110;
	o->result_count = 7;
}
// @retail 0x313c40
lane_a_solver_result *lane_a_hinge::runtime() { return results; }
// @retail 0x313c90
void lane_a_hinge::info(lane_a_constraint_info *o) const
{
	o->maximum = 0;
	o->result_bytes = 0x2c;
	o->op_bytes = 0xd0;
	o->result_count = 5;
}
