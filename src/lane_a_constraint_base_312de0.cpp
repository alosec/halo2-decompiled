// @flags /O2 /Ob1 /GL-
#include "lane_a_constraints.h"
// @retail 0x319280
lane_a_constraint_description::lane_a_constraint_description()
{
	body_a = 0;
	body_b = 0;
	priority = 0;
}
// @retail 0x31b970
void lane_a_constraint::export_base(lane_a_constraint_description *d) const
{
	d->body_a = body_a;
	d->body_b = body_b;
	d->priority = priority;
}
// @retail 0x31b960
void lane_a_constraint::import_base(lane_a_constraint_description const *d)
{
	priority = d->priority;
}
