#ifndef FLIP_COLLECTOR_FORWARDERS_H
#define FLIP_COLLECTOR_FORWARDERS_H
// Havok (third-party physics library): the slot a side-swapping collector
// reports through, for the stand-in classes that queries run with their two
// sides given the other way round declare locally. Retail builds these
// objects with the flip collectors' own tables (0x412970 a point, 0x412978 a
// point cast along a direction, 0x412980 a pair), whose slot 1 turns each
// result back for the collector at +8 (+0x20 for a cast). The stand-ins share
// those layouts but had no slot 1, or an empty one: a swapped query's
// contacts were dropped, or reported through past the end of the table.
#include "lane_b_flip_collectors.h"

inline void flip_point_forward(void *self, void const *point)
{
	((lane_b_flip_point_collector *)self)->lane_b_flip_point_collector::add((lane_b_cd_point const *)point);
}

inline void flip_cast_forward(void *self, void const *point)
{
	((lane_b_flip_cast_collector *)self)->lane_b_flip_cast_collector::add((lane_b_cd_point const *)point);
}

inline void flip_pair_forward(void *self, void const *a, void const *b)
{
	((lane_b_flip_body_pair_collector *)self)->lane_b_flip_body_pair_collector::add(
		(body_2ea940 const *)a, (body_2ea940 const *)b);
}
#endif
