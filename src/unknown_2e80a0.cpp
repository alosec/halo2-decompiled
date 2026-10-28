// @flags /O2 /GL- /GX
#include "lane_a_multisphere_dispatch.h"

// @retail 0x2e80a0
void __cdecl function_2e80a0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector)
{
	lane_a_multisphere const *shape = (lane_a_multisphere const *)a->shape;
	placement_2ed170 placement(*(placement_2ed170 const *)a->placement);
	c_2def90 sphere(0.0f);
	lane_a_sphere_body child(a, &placement);
	long other_type = ((typed_2ed170 *)b->shape)->type();
	for (long i = 0; i < shape->sphere_count; ++i)
	{
		__m128 const *point = &shape->spheres[i];
		__m128 rotated;
		lane_a_rotate(rotated, placement.transform, *point);
		lane_a_set_add(placement.transform.rows[3],
			((placement_2ed170 const *)a->placement)->transform.rows[3], rotated);
		sphere.radius = ((float const *)point)[3];
		child.shape = &sphere;
		child.key = i;
		long type = sphere.type();
		lane_a_query_handler handler = (*input)->closest[type][other_type];
		handler(&child, b, input, collector);
	}
}
