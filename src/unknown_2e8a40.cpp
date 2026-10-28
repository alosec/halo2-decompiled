// @flags /O2 /GL- /GX
#include "lane_a_multisphere_dispatch.h"
#include "growable_array_2d91d0.h"

// @retail 0x2e8a40
lane_a_multisphere_agent::lane_a_multisphere_agent(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager) : lane_a_agent_base(manager)
{
	lane_a_entry_storage &array = agents;
	lane_a_multisphere const *shape = (lane_a_multisphere const *)a->shape;
	long count = shape->sphere_count;
	long capacity = array.capacity_and_flags & 0x7fffffff;
	if (capacity < count)
		function_2d9160(&array, count >= capacity * 2 ? count : capacity * 2, sizeof(lane_a_sphere_entry));
	c_2def90 sphere(0.0f);
	placement_2ed170 placement(*(placement_2ed170 const *)a->placement);
	lane_a_sphere_body child(a, &placement);
	__m128 const *point = shape->spheres;
	for (long i = 0; i < count; ++point, ++i)
	{
		__m128 rotated;
		lane_a_rotate(rotated, placement.transform, *point);
		lane_a_set_add(placement.transform.rows[3],
			((placement_2ed170 const *)a->placement)->transform.rows[3], rotated);
		sphere.radius = ((float const *)point)[3];
		child.shape = &sphere;
		child.key = i;
		lane_a_dispatch *table = *input;
		long first = ((typed_2ed170 *)child.shape)->type();
		long second = ((typed_2ed170 *)b->shape)->type();
		lane_a_create_handler handler = table->create[first][second];
		lane_a_child_agent *agent = handler(&child, b, input, manager);
		if (agent)
		{
			if (array.entry_count == (array.capacity_and_flags & 0x7fffffff))
				function_2d91d0(&array, sizeof(lane_a_sphere_entry));
			lane_a_sphere_entry &entry = array.entries[array.entry_count++];
			entry.agent = agent;
			entry.sphere_index = i;
		}
	}
}
