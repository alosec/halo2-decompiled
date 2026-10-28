// @flags /O2 /GL- /GX
// Havok (third-party physics library): the multisphere queries against
// another shape, run the other way round with the collector flipped so that
// what they find is seen from the original side.
#include "lane_b_multisphere_agents.h"

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// @retail 0x2e8880
void __cdecl function_2e8880(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_body_pair_collector *collector)
{
	lane_b_flip_body_pair_collector flipped(collector);
	function_2e86e0(b, a, input, &flipped);
}

// @retail 0x2e88e0
void __cdecl function_2e88e0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_point_collector *collector)
{
	lane_b_flip_point_collector flipped(collector);
	function_2e80a0(b, a, input, &flipped);
}

// @retail 0x2e8940
void __cdecl function_2e8940(body_2ea940 const *a, body_2ea940 const *b, lane_b_linear_cast_input const *input, lane_b_cd_point_collector *collector, lane_b_cd_point_collector *start_collector)
{
	lane_b_linear_cast_input flipped_input;
	flipped_input.collision = input->collision;
	flipped_input.unknown20 = input->unknown20;
	flipped_input.unknown24 = input->unknown24;
	flipped_input.path = _mm_xor_ps(input->path, *(const __m128 *)g_4124a0);
	lane_b_flip_cast_collector flipped(input->path, collector);
	if (start_collector)
	{
		lane_b_flip_cast_collector flipped_start(input->path, start_collector);
		function_2e83c0(b, a, &flipped_input.collision.dispatcher, &flipped, &flipped_start);
	}
	else
	{
		function_2e83c0(b, a, &flipped_input.collision.dispatcher, &flipped, 0);
	}
}
