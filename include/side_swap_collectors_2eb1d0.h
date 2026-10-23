#ifndef SIDE_SWAP_COLLECTORS_2EB1D0_H
#define SIDE_SWAP_COLLECTORS_2EB1D0_H
// Havok (third-party physics library): the collectors a pair query uses when
// it runs with its two sides given the other way round: one passes each pair
// on with the sides swapped back (its table is at 0x412980), the other passes
// each hit on as seen from the other side, keeping the nearest distance (its
// table is at 0x412970).

// a collector that stops a query early once its flag is set
class stopping_collector_2eb1d0
{
public:
	stopping_collector_2eb1d0() : early_out(false) {}
	virtual ~stopping_collector_2eb1d0() {}

	bool early_out;
};

class swapped_collector_2eb1d0 : public stopping_collector_2eb1d0
{
public:
	swapped_collector_2eb1d0(void *target) : target(target) {}
	virtual ~swapped_collector_2eb1d0() {}
	virtual void add_pair(long first, long second) {}

	void *target;
};

// a collector that keeps the nearest result, starting from the largest float
class nearest_collector_2ed110
{
public:
	nearest_collector_2ed110() : distance(3.40282347e+38f) {}
	virtual ~nearest_collector_2ed110() {}

	float distance;
};

class flipped_collector_2ed110 : public nearest_collector_2ed110
{
public:
	flipped_collector_2ed110(void *target) : target(target) {}
	virtual ~flipped_collector_2ed110() {}
	virtual void add_hit(void const *hit) {}

	void *target;
};

#endif
