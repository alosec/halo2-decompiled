#ifndef LANE_B_MULTISPHERE_AGENTS_H
#define LANE_B_MULTISPHERE_AGENTS_H
// Havok (third-party physics library): the multisphere agent used with the
// two bodies the other way round. It is built with the bodies swapped, and
// the contacts its collision step adds have their normals flipped back.
#include "lane_a_multisphere_dispatch.h"
#include "lane_b_flip_collectors.h"

// the input of a query: the dispatcher first
struct lane_b_collision_input
{
	lane_a_dispatch *dispatcher;
	long unknown04;
	long unknown08;
	long unknown0c;
};

// the input of a linear cast: a query's input, then the path cast along
struct lane_b_linear_cast_input
{
	lane_b_collision_input collision;
	__m128 path;
	long unknown20;
	long unknown24;
};

// a contact added by a collision step: a point, its normal and the rest
struct lane_b_agent_contact_point
{
	__m128 position;
	__m128 normal;
	long unknown20[4];
};

struct lane_b_agent_contact_points
{
	lane_b_agent_contact_point *data;
	long count;
};

// the output a collision step adds its contacts to
struct lane_b_collision_output
{
	lane_b_agent_contact_points *points;
};

class lane_b_symmetric_multisphere_agent : public lane_a_multisphere_agent
{
public:
	lane_b_symmetric_multisphere_agent(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager)
		: lane_a_multisphere_agent(b, a, input, manager)
	{
	}
	// the queries with the bodies swapped and the collector flipped
	// (retail 0x2e8ce0, 0x2e8d40, 0x2e8e90)
	virtual void function_2e8560(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void function_2e7f30(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void function_2e8240(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector);
	// the collision step with the bodies swapped (retail 0x2e8da0)
	virtual void function_2e7dc0(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void is_symmetric(bool *result) { *result = true; }
};

void __cdecl function_2e8880(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_body_pair_collector *collector);
void __cdecl function_2e88e0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_point_collector *collector);
void __cdecl function_2e8940(body_2ea940 const *a, body_2ea940 const *b, lane_b_linear_cast_input const *input, lane_b_cd_point_collector *collector, lane_b_cd_point_collector *start_collector);

lane_a_multisphere_agent *__cdecl function_2e8e00(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager);
lane_a_multisphere_agent *__cdecl function_2e8fc0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager);
#endif
