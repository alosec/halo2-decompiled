/* ANIMATION_CACHE.H: the xbox animation cache (src/animation_cache.cpp,
   0x123180..0x123b00). Retail names its parts "xbox animation cache" (the
   physical memory g_4e3b54 the resources are loaded into), "xbox animation"
   (a data array of loads, one per block of that memory, g_4e3b48) and
   "xbox predicted animations" (a data array of requests waiting to be
   loaded, g_4e3b4c). The animation graphs' resources (0x14 bytes each) are
   what it loads. */

#ifndef ANIMATION_CACHE_H
#define ANIMATION_CACHE_H

#include "unknown_11c920.h"
#include <stddef.h>

/* a resource the cache streams: its size, where its data lies (passed to the
   async read as its location), and the block of the cache it sits in */
struct s_cache_resource
{
	long unknown00; /* only copied to a dead stack slot by the load */
	long size;
	dword location;
	long block_index;
	bool streamed;
	byte unknown11[3];
};
typedef char assert_cache_resource_size[(sizeof(s_cache_resource) == 0x14) ? 1 : -1];
typedef char assert_cache_resource_block_index[(offsetof(s_cache_resource, block_index) == 0xc) ? 1 : -1];

/* a load of a resource into its block ("xbox animation", g_4e3b48, one at
   each block's index): the async read's handle and whether it is done */
struct s_animation_cache_load
{
	short salt;
	short unknown02;
	bool done;
	byte unknown05[3];
	long handle;
	s_cache_resource *resource;
};
typedef char assert_animation_cache_load_size[(sizeof(s_animation_cache_load) == 0x10) ? 1 : -1];

/* a request to load a resource ("xbox predicted animations", g_4e3b4c) */
struct s_animation_cache_request
{
	short salt;
	short unknown02;
	long priority;
	s_cache_resource *resource;
};
typedef char assert_animation_cache_request_size[(sizeof(s_animation_cache_request) == 0xc) ? 1 : -1];

/* by request priority (0 is needed now, 1 urgent, 2 predicted): the async
   read's priority, how long ago a resource of that priority counts as used,
   and the limit (an index into the physical memory's limits) its block is
   allocated under */
struct s_animation_cache_priority
{
	long read_priority;
	long age;
	long allocation_limit;
};
typedef char assert_animation_cache_priority_size[(sizeof(s_animation_cache_priority) == 0xc) ? 1 : -1];

/* whether the animation cache exists (g_510c20) and whether it streams
   resources (g_510c21, only between activate and deactivate) */
extern bool g_510c20;
extern bool g_510c21;

long animation_cache_find_request(s_cache_resource *resource);
void animation_cache_request_resource(s_cache_resource *resource, bool urgent);
void *animation_cache_get_resource(s_cache_resource *resource, long name);

#endif
