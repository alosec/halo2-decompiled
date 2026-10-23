#ifndef SWAPPED_QUERY_2ED2A0_H
#define SWAPPED_QUERY_2ED2A0_H
// Havok (third-party physics library): a query for a pair of shapes passed on
// with the two shapes the other way round (0x2ed2a0, 0x2ed4e0): the query's
// direction turned around, and each collector wrapped in one that turns its
// results back.
#include <xmmintrin.h>

struct query_header_2ed2a0
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

struct query_2ed2a0
{
	query_header_2ed2a0 header;
	__m128 direction;
	long field_20;
	long field_24;
};

// a collector; starts with no result closer than the largest float
class collector_2ed2a0
{
public:
	float closest;

	collector_2ed2a0() : closest(3.402823466e+38f) {}
	virtual ~collector_2ed2a0() {}
};

// wraps a collector
class swapped_collector_2ed2a0 : public collector_2ed2a0
{
public:
	__m128 direction;
	void *collector;

	swapped_collector_2ed2a0(__m128 const &direction_, void *collector_) :
		direction(direction_),
		collector(collector_)
	{
	}
};

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// the query with its direction turned around
inline void swap_query_2ed2a0(query_2ed2a0 *swapped, query_2ed2a0 const *query)
{
	swapped->header = query->header;
	swapped->field_20 = query->field_20;
	swapped->field_24 = query->field_24;
	swapped->direction = _mm_xor_ps(query->direction, *(__m128 const *)g_4124a0);
}

#endif
