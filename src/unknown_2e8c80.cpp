// @flags /O2 /GL- /GX
// Havok (third-party physics library): destroying and creating the
// multisphere agent, and its kind used with the two bodies the other way round.
#include "lane_b_multisphere_agents.h"

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// Shared collection-agent cleanup (retail table slot 0x2ebc50).
void lane_a_multisphere_agent::cleanup()
{
	for (int i = 0; i < agents.entry_count; i++)
		agents.entries[i].agent->slot_5();
	delete this;
}

// @retail 0x2e8c80 destructor lane_a_multisphere_agent

// @retail 0x2e8e60 deleting lane_a_multisphere_agent

// @retail 0x2e8ce0
void lane_b_symmetric_multisphere_agent::function_2e8560(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_b_flip_body_pair_collector flipped((lane_b_cd_body_pair_collector *)collector);
	lane_a_multisphere_agent::function_2e8560((body_2ea940 const *)b, a, input, &flipped);
}

// @retail 0x2e8d40
void lane_b_symmetric_multisphere_agent::function_2e7f30(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_b_flip_point_collector flipped((lane_b_cd_point_collector *)collector);
	lane_a_multisphere_agent::function_2e7f30((body_2ea940 const *)b, a, input, &flipped);
}

// @retail 0x2e8da0
void lane_b_symmetric_multisphere_agent::function_2e7dc0(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_b_collision_output *output = (lane_b_collision_output *)collector;
	long i = output->points->count;
	lane_a_multisphere_agent::function_2e7dc0((body_2ea940 const *)b, a, input, collector);
	for (; i < output->points->count; ++i)
	{
		__m128 &normal = output->points->data[i].normal;
		normal = _mm_xor_ps(normal, *(const __m128 *)g_4124a0);
	}
}

// @retail 0x2e8e00
lane_a_multisphere_agent *__cdecl function_2e8e00(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager)
{
	return new lane_a_multisphere_agent(a, b, input, manager);
}

// @retail 0x2e8e90
void lane_b_symmetric_multisphere_agent::function_2e8240(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector)
{
	lane_b_linear_cast_input const *cast_input = (lane_b_linear_cast_input const *)input;
	lane_b_linear_cast_input flipped_input;
	flipped_input.collision = cast_input->collision;
	flipped_input.unknown20 = cast_input->unknown20;
	flipped_input.unknown24 = cast_input->unknown24;
	flipped_input.path = _mm_xor_ps(cast_input->path, *(const __m128 *)g_4124a0);
	lane_b_flip_cast_collector flipped(cast_input->path, (lane_b_cd_point_collector *)collector);
	if (start_collector)
	{
		lane_b_flip_cast_collector flipped_start(cast_input->path, (lane_b_cd_point_collector *)start_collector);
		lane_a_multisphere_agent::function_2e8240((body_2ea940 const *)b, a, &flipped_input, &flipped, &flipped_start);
	}
	else
	{
		lane_a_multisphere_agent::function_2e8240((body_2ea940 const *)b, a, &flipped_input, &flipped, 0);
	}
}

// @retail 0x2e8fb0 destructor lane_b_symmetric_multisphere_agent

// @retail 0x2e8f80 deleting lane_b_symmetric_multisphere_agent

// @retail 0x2e8fc0
lane_a_multisphere_agent *__cdecl function_2e8fc0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager)
{
	return new lane_b_symmetric_multisphere_agent(a, b, input, manager);
}
