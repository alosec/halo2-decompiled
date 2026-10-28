// @flags /O2 /Ob1 /GL-
#include "lane_b_constraint_dependencies.h"

// @retail 0x31bdf0
void lane_b_contact_constraint_prefix::build(const void *input, void *output)
{
	lane_b_contact_build(points.data, points.size, properties, &solver_data, input, output);
}

