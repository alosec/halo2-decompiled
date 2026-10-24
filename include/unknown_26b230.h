/* UNKNOWN_26B230.H: clump structures (guessed layout) */

#ifndef UNKNOWN_26B230_H
#define UNKNOWN_26B230_H

#include "unknown_11c920.h"

struct s_clump
{
	byte unknown0[0x10];
	short divisor;
	short team;
	long first_prop;
	long first_object;
	byte unknown1c[0x0a];
	short state;
	long state_time;
	byte unknown2c[4];
	byte unknown30;
	byte unknown31[0x0d];
	word unknown3e;
	byte unknown40[0x10];
};

struct s_clump_prop
{
	byte unknown0[8];
	long type;
	byte unknownc[8];
	long next;
	long first_node;
	byte unknown1c[0x34 - 0x1c];
	bool unknown34;
	byte unknown35[0xc4 - 0x35];
};

/* the elements of g_502418 (0x3c bytes) */
struct s_clump_node
{
	byte unknown00[4];
	long actor_index;
	byte unknown08[0x14 - 0x8];
	long tracking_index;
	byte unknown18[0x24 - 0x18];
	short state;
	byte unknown26[0x34 - 0x26];
	long next;
	byte unknown38[0x3c - 0x38];
};
typedef char assert_clump_node_tracking_index[((long)&((s_clump_node *)0)->tracking_index == 0x14) ? 1 : -1];

/* the iterator of function_26bda0 (unknown_26bda0.cpp) */
struct s_iterator
{
	long index;
	long next;
};

long function_26b230(long clump_index, long prop_index);
void function_26bda0(long clump_index, s_iterator *iterator);
bool function_26ba60(long prop_index, long actor_index, long clump_index);

struct s_clump_object
{
	byte unknown0[0x80];
	long next;
	byte unknown84[0x2a4];
	short count;
	byte unknown32a[0x338 - 0x32a];
	long node_index;
	byte unknown33c[0x888 - 0x33c];
};


#endif
