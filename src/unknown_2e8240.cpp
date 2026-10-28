// @flags /O2 /GL- /GX
#include "lane_a_multisphere_query.h"

// The sphere's virtual shape type, folded with the retail return-four body.
long c_2def90::type()
{
	return 4;
}

// @retail 0x2e8240
void lane_a_multisphere_agent::function_2e8240(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector)
{
	lane_a_multisphere const *shape = (lane_a_multisphere const *)a->shape;
	placement_2ed170 placement(*(placement_2ed170 const *)a->placement);
	c_2def90 sphere(0.0f);
	lane_a_sphere_body child(a, &placement);
	lane_a_sphere_entry const *entry = agents.entries;
	for (long i = agents.entry_count - 1; i >= 0; --i)
	{
		__m128 const &point = shape->spheres[i];
		__m128 rotated;
		lane_a_rotate(rotated, placement.transform, point);
		lane_a_set_add(placement.transform.rows[3],
			((placement_2ed170 const *)a->placement)->transform.rows[3], rotated);
		sphere.radius = ((float const *)&point)[3];
		child.shape = &sphere;
		child.key = i;
		entry->agent->slot_3(&child, b, input, collector, start_collector);
	}
}
