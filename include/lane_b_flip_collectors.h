#ifndef LANE_B_FLIP_COLLECTORS_H
#define LANE_B_FLIP_COLLECTORS_H
// Havok (third-party physics library): the collectors a query reports to,
// and the "flip" collectors that pass on what a query of two bodies taken the
// other way round finds, seen from the original side.
#include "reference_counted_2d88a0.h"
#include <float.h>
#include <xmmintrin.h>

struct body_2ea940;

// a point found between two bodies: its position (w = distance), its normal
// (pointing from the second body to the first) and the two bodies
struct lane_b_cd_point
{
	__m128 position;
	__m128 normal;
	body_2ea940 const *body_a;
	body_2ea940 const *body_b;
};

// collects pairs of bodies found overlapping (retail table 0x455580)
class lane_b_cd_body_pair_collector
{
public:
	static void operator delete(void *block)
	{
		g_memory_2d88a0->release_block(block, sizeof(lane_b_cd_body_pair_collector), 0x1a);
	}
	lane_b_cd_body_pair_collector() : early_out(false) {}
	virtual ~lane_b_cd_body_pair_collector() {}
	virtual void add(body_2ea940 const *a, body_2ea940 const *b) = 0;

	bool early_out;
};

// collects points found between two bodies (retail table 0x453b70)
class lane_b_cd_point_collector
{
public:
	static void operator delete(void *block)
	{
		g_memory_2d88a0->release_block(block, sizeof(lane_b_cd_point_collector), 0x1a);
	}
	lane_b_cd_point_collector() : early_out(FLT_MAX) {}
	virtual ~lane_b_cd_point_collector() {}
	virtual void add(lane_b_cd_point const *point) = 0;

	float early_out;
};

// passes a pair on with its two bodies swapped (retail table 0x412980)
class lane_b_flip_body_pair_collector : public lane_b_cd_body_pair_collector
{
public:
	lane_b_flip_body_pair_collector(lane_b_cd_body_pair_collector *collector_) : collector(collector_) {}
	virtual void add(body_2ea940 const *a, body_2ea940 const *b);

	lane_b_cd_body_pair_collector *collector;
};

// passes a point on seen from the other body (retail table 0x412970)
class lane_b_flip_point_collector : public lane_b_cd_point_collector
{
public:
	lane_b_flip_point_collector(lane_b_cd_point_collector *collector_) : collector(collector_) {}
	virtual void add(lane_b_cd_point const *point);

	lane_b_cd_point_collector *collector;
};

// the same for a point found casting along a direction (retail table 0x412978)
class lane_b_flip_cast_collector : public lane_b_cd_point_collector
{
public:
	lane_b_flip_cast_collector(__m128 const &direction_, lane_b_cd_point_collector *collector_)
		: direction(direction_), collector(collector_)
	{
	}
	virtual void add(lane_b_cd_point const *point);

	__m128 direction;
	lane_b_cd_point_collector *collector;
};
#endif
