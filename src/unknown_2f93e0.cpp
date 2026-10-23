// @flags /O2 /GL- /GX
// Havok (third-party physics library): makes a new object for a pair of
// shapes whose first holds a number of parts (0x2f8ff0).
#include "parts_agent_2f8ff0.h"

// @retail 0x2f93e0
sized_2ed170 *__cdecl function_2f93e0(parts_description_2f8ff0 const *description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new parts_agent_2f8ff0(description, other, dispatcher, arg_3);
}

struct body_2f89b0;
class collector_2f89b0;

// a collector that keeps the nearest result, starting from the largest float
class nearest_collector_2f9440
{
public:
	nearest_collector_2f9440() : distance(3.40282347e+38f) {}
	virtual ~nearest_collector_2f9440() {}

	float distance;
};

// passes each hit on to another collector as seen from the other side
class flipped_hit_collector_2f9440 : public nearest_collector_2f9440
{
public:
	flipped_hit_collector_2f9440(collector_2f89b0 *target) : target(target) {}
	virtual ~flipped_hit_collector_2f9440() {}
	virtual void add_hit(void const *hit) {}

	collector_2f89b0 *target;
};

class rays_convex_2f89b0
{
public:
	void function_2f89b0(body_2f89b0 const *a, body_2f89b0 const *b, void const *input, collector_2f89b0 *collector);
	void function_2f9440(body_2f89b0 const *b, body_2f89b0 const *a, void const *input, collector_2f89b0 *collector);
};

// the rays query with the two sides the other way round, its hits passed on
// as seen from the other side
// @retail 0x2f9440
void rays_convex_2f89b0::function_2f9440(body_2f89b0 const *b, body_2f89b0 const *a, void const *input, collector_2f89b0 *collector)
{
	flipped_hit_collector_2f9440 flipped(collector);
	function_2f89b0(a, b, input, (collector_2f89b0 *)&flipped);
}
