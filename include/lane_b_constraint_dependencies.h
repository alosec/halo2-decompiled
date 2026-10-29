#ifndef LANE_B_CONSTRAINT_DEPENDENCIES_H
#define LANE_B_CONSTRAINT_DEPENDENCIES_H
#include "lane_b_constraint_leaves.h"
// The six pointer/count arguments are independently visible in 31bdf0 and
// consumed by 31f620; opaque until the solver structures are reconstructed.
void __cdecl lane_b_contact_build(const void *points, long count,
	const void *properties, const void *solver_data, const void *input, void *output);
#endif
