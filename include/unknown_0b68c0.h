/* UNKNOWN_0B68C0.H: the list of object type definitions the initialize runs
   through (g_4e0330, defined in unknown_0b68c0.cpp) */

#ifndef UNKNOWN_0B68C0_H
#define UNKNOWN_0B68C0_H

#include "unknown_11c920.h"

struct s_callback_node
{
	byte unknown00[0x14];
	void (*callback)(void);
	byte unknown18[0xac];
	s_callback_node *next;
};

extern s_callback_node *g_4e0330;

struct s_record_pool;

struct s_cluster_partition
{
	long *cluster_first_data_references;
	s_record_pool *data_references;
	s_record_pool *cluster_references;
};

extern s_cluster_partition g_4e0310_partition;

#define g_4e0310 (g_4e0310_partition.cluster_first_data_references)
#define g_4e0314 (g_4e0310_partition.data_references)
#define g_4e0318 (g_4e0310_partition.cluster_references)

#endif
