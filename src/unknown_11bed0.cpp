// @flags /O2 /Gr
/* UNKNOWN_11BED0.CPP: the location (leaf and cluster of the structure bsp) of
   a point. Decompiled by lane F for 0x18c3b0. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "object_queries.h"

struct s_bsp3d;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

/* the structure bsp's collision bsp */
s_bsp3d *g_4e033c;

struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
extern long g_4686c0;
void cache_files_dispose_map(void);

// @retail 0x11be50
void function_11be50(void)
{
	cache_files_dispose_map();
	g_4686c0 = NONE;
	g_4e0350 = NULL;
	g_4e034c = NULL;
	g_4686c4 = NONE;
	g_4e0348 = NULL;
	g_4e0344 = NULL;
	g_4e0340 = NULL;
	g_4e033c = NULL;
}

struct s_structure_leaf
{
	short cluster_index;
	byte unknown02[6];
};

struct s_structure_bsp_leaves_view
{
	byte unknown00[0x30];
	s_structure_leaf *leaves;
};

// @retail 0x11be90
void function_11be90(s_location *location, long leaf_index)
{
	location->leaf_index = leaf_index;
	location->cluster_index = leaf_index != NONE ? ((s_structure_bsp_leaves_view *)g_4e0348)->leaves[leaf_index].cluster_index : NONE;
	location->bsp_index = g_4686c4;
}

// @retail 0x11bed0
void function_11bed0(s_location *location, point3f const *point)
{
	if (g_4686c4 == NONE)
	{
		location->cluster_index = g_4686c4;
		location->bsp_index = g_4686c4;
		location->leaf_index = NONE;
	}
	else
	{
		long leaf_index = function_14a280(g_4e033c, 0, (point3f *)point);
		location->leaf_index = leaf_index;
		long cluster_index = leaf_index != NONE ? ((s_structure_bsp_leaves_view *)g_4e0348)->leaves[leaf_index].cluster_index : NONE;
		location->bsp_index = g_4686c4;
		location->cluster_index = cluster_index;
	}
}
