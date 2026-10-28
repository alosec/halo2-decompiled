// @flags /O2 /GL- /GX
// Each child agent queries a temporary sphere at its world-space centre.
#include "lane_a_multisphere_query.h"

// @retail 0x2e7dc0
void lane_a_multisphere_agent::function_2e7dc0(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_a_multisphere const *shape = (lane_a_multisphere const *)a->shape;
	placement_2ed170 placement(*(placement_2ed170 const *)a->placement);
	c_2def90 sphere(0.0f);
	lane_a_sphere_body child(a, &placement);
	lane_a_sphere_entry const *entry = agents.entries;
	for (long i = agents.entry_count - 1; i >= 0; ++entry, --i)
	{
		__m128 const &point = shape->spheres[entry->sphere_index];
		__m128 rotated;
		lane_a_rotate(rotated, placement.transform, point);
		lane_a_set_add(placement.transform.rows[3],
			((placement_2ed170 const *)a->placement)->transform.rows[3], rotated);
		sphere.radius = ((float const *)&point)[3];
		child.shape = &sphere;
		child.key = i;
		entry->agent->slot_4(&child, b, input, collector);
	}
}
