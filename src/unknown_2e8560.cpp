// @flags /O2 /GL- /GX
// Penetration query, stopping when the collector requests early out.
#include "lane_a_multisphere_query.h"

// @retail 0x2e8560
void lane_a_multisphere_agent::function_2e8560(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_a_multisphere const *shape = (lane_a_multisphere const *)a->shape;
	placement_2ed170 placement(*(placement_2ed170 const *)a->placement);
	c_2def90 sphere(0.0f);
	lane_a_sphere_body child(a, &placement);
	lane_a_sphere_entry const *entry = agents.entries;
	long const count = agents.entry_count;
	long index = count - 1;
	for (; index >= 0; ++entry, --index)
	{
		__m128 const &point = shape->spheres[entry->sphere_index];
		__m128 rotated;
		lane_a_rotate(rotated, placement.transform, point);
		lane_a_set_add(placement.transform.rows[3],
			((placement_2ed170 const *)a->placement)->transform.rows[3], rotated);
		sphere.radius = ((float const *)&point)[3];
		child.shape = &sphere;
		child.key = index;
		entry->agent->slot_1(&child, b, input, collector);
		if (((lane_a_stop_collector *)collector)->get_early_out())
			break;
	}
}
