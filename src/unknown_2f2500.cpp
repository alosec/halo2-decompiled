// @flags /O2 /GL- /GX
// Havok (third-party physics library): passing a sphere and box query on,
// and running it with the two sides swapped into a collector that only notes
// whether there was a hit.
#include "pair_agents_2eab70.h"
#include "side_swap_collectors_2eb1d0.h"
#include "swapped_query_2ed2a0.h"

struct body_2f23c0;
class collector_2f23c0;

void __cdecl function_2f23c0(body_2f23c0 const *a, body_2f23c0 const *b, void const *input, collector_2f23c0 *collector);

// a collector that starts with no hit
class hit_2f2520
{
public:
	hit_2f2520() : hit(false) {}
	virtual ~hit_2f2520() {}

	bool hit;
};

class hit_value_2f2520 : public hit_2f2520
{
public:
	hit_value_2f2520(long value_) : value(value_) {}

	long value;
};

class agent_2f2500
{
public:
	void function_2f2500(body_2f23c0 const *a, body_2f23c0 const *b, void const *input, collector_2f23c0 *collector);
	void function_2f2520(body_2f23c0 const *a, body_2f23c0 const *b, void const *input, long value);
};

// @retail 0x2f2500
void agent_2f2500::function_2f2500(body_2f23c0 const *a, body_2f23c0 const *b, void const *input, collector_2f23c0 *collector)
{
	function_2f23c0(a, b, input, collector);
}

// @retail 0x2f2520
void agent_2f2500::function_2f2520(body_2f23c0 const *a, body_2f23c0 const *b, void const *input, long value)
{
	hit_value_2f2520 collector(value);
	function_2f23c0(b, a, input, (collector_2f23c0 *)&collector);
}

// @retail 0x2f2580
void __cdecl function_2f2580(body_2f23c0 const *a, body_2f23c0 const *b, void const *input, long value)
{
	hit_value_2f2520 collector(value);
	function_2f23c0(b, a, input, (collector_2f23c0 *)&collector);
}

// the query with the two sides the other way round, its hits passed on as seen from the other side
// @retail 0x2f25e0
void class_2f1ea0::function_2f25e0(void const *a, void const *b, void const *input, void *collector)
{
	flipped_collector_2ed110 flipped(collector);
	function_2f1ea0(b, a, input, &flipped);
}
