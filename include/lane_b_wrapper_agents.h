#ifndef LANE_B_WRAPPER_AGENTS_H
#define LANE_B_WRAPPER_AGENTS_H
// Havok (third-party physics library): the agent for a shape that wraps a
// child shape (shape type 0x14). It keeps one agent, made for the wrapped
// child against the other side, and passes every query on to it with the
// wrapped side replaced by its child at the same key and placement.
#include "lane_b_multisphere_agents.h"
#include "agent_table_2e5d50.h"

// retail table 0x412988
class lane_b_wrapper_agent : public lane_a_agent_base
{
public:
	lane_b_wrapper_agent(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager);
	virtual void get_penetrations(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void get_closest_points(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void linear_cast(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector);
	virtual void process_collision(body_2ea940 const *a, void const *b, void const *input, void *collector);
	// tells the child agent, then destroys itself (retail slot 0x2ecc60)
	virtual void cleanup();
	// whether this is the kind used with the two bodies the other way round
	// (retail slot 0x2fe820)
	virtual void is_symmetric(bool *result);
	virtual void update_filter(body_2ea940 const *a, void const *b, void const *input);

	lane_a_child_agent *agent;
};

// the same with the wrapping shape as the second body (retail table 0x4129a8)
class lane_b_symmetric_wrapper_agent : public lane_b_wrapper_agent
{
public:
	lane_b_symmetric_wrapper_agent(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager)
		: lane_b_wrapper_agent(b, a, input, manager)
	{
	}
	virtual void get_penetrations(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void get_closest_points(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void linear_cast(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector);
	virtual void process_collision(body_2ea940 const *a, void const *b, void const *input, void *collector);
	// retail slot 0x31bc80
	virtual void is_symmetric(bool *result);
	virtual void update_filter(body_2ea940 const *a, void const *b, void const *input);
};

void __cdecl function_2e64a0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector);
void __cdecl function_2e6560(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector);
void __cdecl function_2e6620(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector, void *start_collector);
lane_b_wrapper_agent *__cdecl function_2e66e0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager);
void __cdecl function_2e6780(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_body_pair_collector *collector);
void __cdecl function_2e67e0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_point_collector *collector);
void __cdecl function_2e6a20(body_2ea940 const *a, body_2ea940 const *b, lane_b_linear_cast_input const *input, lane_b_cd_point_collector *collector, lane_b_cd_point_collector *start_collector);
lane_b_wrapper_agent *__cdecl function_2e6c60(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager);
void __cdecl function_2e6cd0(s_1c3a40 *table);
#endif
