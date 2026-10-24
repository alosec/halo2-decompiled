// @flags /O1 /Gr
/* UNKNOWN_165CC3.CPP: a caller optimized for size (/O1), which calls
function_123d40 rather than inlining it. */

#include "unknown_11c920.h"
#include "unknown_123b30.h"

/* the per player first-person groups (unknown_16658d.cpp) */
struct s_16658d_group;
extern s_16658d_group *g_4e9bc8;
void *g_4e9bc4;

// @retail 0x165cc3
void game_state_initialize_165cc3(void)
{
	g_4e9bc8 = (s_16658d_group *)function_123d40("unknown", "unknown", 0x8330);
	g_4e9bc4 = game_state_malloc_aligned("unknown", "unknown", 0x8000, 4);
}
