// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_122870.CPP: the map cache file: its header (0x800 bytes between 'head'
   and 'foot'), the tag index read from it, tag lookups by group and the
   structure bsp it loads. */

#include "unknown_11c920.h"
#include "globals.h"
#include "async.h"
#include "unknown_122870.h"
#include <xtl.h>
#include <string.h>
#ifdef H2_NATIVE_PORT
#include "h2/host_abi.h"
#endif

struct s_cache_tag_instance
{
	long group_tag;
	long datum_index;
	void *address;
	long size;
};

struct s_cache_tag_group
{
	long group_tag;
	long parent_group_tags[2];
};

struct s_structure_bsp_reference
{
	dword offset;
	long size;
	s_structure_bsp_header *address;
	byte unknown0c[8];
	long bsp_tag_index;
	byte unknown18[4];
	long lightmap_tag_index;
};

struct s_tag_iterator
{
	long unknown00;
	long unknown04;
	long datum_index;
	long next_index;
	long group_tag;
};

long function_13ddd0(const void *key, const void *base, long count, long element_size, long (__stdcall *compare)(const void *, const void *, const void *), const void *context);
long function_213760(dword location, long size, void *buffer, dword *bytes_read, bool *done, long category, long priority);
void function_213890(void);
long cache_file_get_maximum_size(long type);

extern long g_55aca8;

#ifdef H2_NATIVE_PORT
extern s_cache_file_globals cache_file_globals;
#else
s_cache_file_globals cache_file_globals;
#endif

#define CACHE_TAG_INSTANCES ((s_cache_tag_instance *)g_4e3b44)

// @retail 0x122870
bool cache_header_verify(s_cache_header const *header)
{
	bool result = false;
	FILETIME time;

	GetSystemTimeAsFileTime(&time);
	if (header->header_signature == 'head' &&
		header->footer_signature == 'foot' &&
		header->size >= 0 && header->size <= 0x20800000 &&
		header->type >= 0 && header->type < 5 &&
		header->size <= cache_file_get_maximum_size(header->type) &&
		strlen(header->name) < sizeof(header->name) &&
		header->version == 8)
	{
		result = true;
	}
	return result;
}

/* the open cache files (0x804 bytes each, unknown_213760.cpp): a handle and
   the file's header */
struct s_cache_file
{
	HANDLE handle;
	byte unknown04[0x800];
};

extern s_cache_file g_557c90[3];

long cache_file_find(char const *map_name);
bool function_122d60(unsigned __int64 location, long size, void *buffer);
bool __stdcall version_is_compatible(char const *version);

/* set when a map fails to load (the main loop shows the error) */
#ifdef H2_NATIVE_PORT
extern long g_510a08;
#else
long g_510a08;
#endif
extern bool g_510819;

/* the scenario tag of the loaded map */
extern long g_4686c0;

/* takes size bytes, rounded up to whole pages, from the bottom of the
   current physical memory stage; NULL when it is full */
static __forceinline void *physical_memory_malloc_low(long size, dword protect)
{
#ifdef H2_NATIVE_PORT
	return host_h2_physical_memory_malloc_low(size, protect);
#else
	void *result = NULL;
	long stage = g_global_f9ae07.field_0;
	long *bottom = &g_global_f9ae07.field_c_6[stage];
	long aligned_size = (size + 0xfff) & 0xfffff000;
	long address = g_global_f9ae07.field_c_6[stage];
	long top = address + aligned_size;

	if (top <= g_global_f9ae07.field_20[stage])
	{
		*bottom = top;
		result = (void *)address;
		if (address)
		{
			result = (void *)(address | 0x80000000);
			if (result)
				XPhysicalProtect(result, aligned_size, protect);
		}
	}
	return result;
#endif
}

static inline long cache_file_sector_align(long size)
{
	if (size & 0x1ff)
		size = (size | 0x1ff) + 1;
	return size;
}

/* loads a map's cache file: its header, then its tag data (the tags header
   and the tag instances) into physical memory; points the tag instances, the
   scenario and the globals at what it read */
// @retail 0x1228f0
bool cache_files_load_map(char const *map_name)
{
	long scenario_index = NONE;
	bool result = false;

	g_55aca8 = cache_file_find(map_name);
	s_cache_file *file = &g_557c90[g_55aca8];
	cache_file_globals.header = *(s_cache_header *)file->unknown04;
	if (cache_header_verify(&cache_file_globals.header) && version_is_compatible(cache_file_globals.header.build_version))
	{
		cache_file_globals.field_4_7 = physical_memory_malloc_low(cache_file_globals.header.unknown1c, PAGE_READWRITE);
		result = cache_file_globals.field_4_7 != NULL;
		if (result)
		{
			result = function_122d60(cache_file_location_make(NONE, cache_file_globals.header.tag_data_offset),
				cache_file_sector_align(cache_file_globals.header.tag_data_size), cache_file_globals.field_4_7);
			if (!result)
			{
				g_510a08 = 0;
				g_510819 = true;
			}
			else
			{
				result = function_122d60(cache_file_location_make(NONE, cache_file_globals.header.tag_data_offset + cache_file_globals.header.tag_data_size),
					cache_file_sector_align(cache_file_globals.header.unknown18),
					(byte *)cache_file_globals.field_4_7 - cache_file_globals.header.unknown18 + cache_file_globals.header.unknown1c);
				if (!result)
				{
					g_510a08 = 0;
					g_510819 = true;
				}
				else
				{
					s_cache_tags_header *tags = (s_cache_tags_header *)cache_file_globals.field_4_7;

					if (!tags->instances || tags->instance_count <= 0 || tags->signature != 'tags')
						result = false;
					if (result)
					{
						cache_file_globals.tags = tags;
						g_4e3b44 = (s_tag_instance *)tags->instances;
						result = true;
						cache_file_globals.loaded = result;
						scenario_index = tags->scenario_index;
					}
					else
					{
						g_510a08 = 0;
						g_510819 = true;
					}
				}
			}
		}
	}
	if (!result)
	{
		if (cache_file_globals.field_4_7)
			cache_file_globals.field_4_7 = NULL;
		if (g_55aca8 != NONE)
		{
			function_213890();
			g_55aca8 = NONE;
		}
		/* a statement the release build compiles out: retail keeps the
		   close's join as its own block (the instance table is reloaded there) */
		do {} while (false);
	}
	g_4686c0 = scenario_index;
	if (scenario_index != NONE)
	{
		g_4e0350 = (s_palette_source_globals *)CACHE_TAG_INSTANCES[scenario_index & 0xffff].address;
		g_4e034c = (s_tag_header_globals *)CACHE_TAG_INSTANCES[cache_file_globals.tags->globals_index & 0xffff].address;
		return true;
	}
	return result;
}

// @retail 0x122af0
void cache_files_dispose_map(void)
{
	if (g_55aca8 != NONE)
	{
		function_213890();
		g_55aca8 = NONE;
	}
	cache_file_globals.loaded = false;
	cache_file_globals.field_4_7 = NULL;
	memset(&cache_file_globals.header, 0, sizeof(cache_file_globals.header));
	g_4e3b44 = NULL;
	cache_file_globals.tags = NULL;
	cache_file_globals.bsp = NULL;
}

bool function_122d60(unsigned __int64 location, long size, void *buffer);

// @retail 0x122b40
bool cache_files_load_structure_bsp(s_structure_bsp_reference *bsp)
{
	bool result = false;
	long size = bsp->size;

	if (size & 0x1ff)
		size = (size | 0x1ff) + 1;
	if (function_122d60(cache_file_location_make(NONE, bsp->offset), size, bsp->address) && bsp->address->signature == 'sbsp')
	{
		cache_file_globals.bsp = bsp->address;
		CACHE_TAG_INSTANCES[(short)bsp->bsp_tag_index].address = cache_file_globals.bsp->bsp_address;
		long lightmap_tag_index = bsp->lightmap_tag_index;
		if (lightmap_tag_index != NONE)
			CACHE_TAG_INSTANCES[(short)lightmap_tag_index].address = cache_file_globals.bsp->lightmap_address;
		result = true;
	}
	return result;
}

/* geometry_cache: forgets the streamed blocks of a tag (unknown_12de70.cpp) */
void function_12e150(long tag_index);

static inline void cache_files_unload_tag(long const &tag_index)
{
	long index = tag_index;
	s_cache_tag_instance *instance = &CACHE_TAG_INSTANCES[(short)index];

	function_12e150(tag_index);
	instance->address = NULL;
}

// @retail 0x122bc0
void cache_files_unload_structure_bsp(s_structure_bsp_reference *bsp)
{
	cache_files_unload_tag(bsp->bsp_tag_index);
	if (bsp->lightmap_tag_index != NONE)
	{
		cache_files_unload_tag(bsp->lightmap_tag_index);
	}
	cache_file_globals.bsp = NULL;
}
s_cache_tag_group *cache_tag_group_get(long group_tag);

s_cache_tag_instance *cache_tag_instance_get(long tag_index);

static inline bool cache_tag_group_is(s_cache_tag_group const *group, long group_tag)
{
	return group->group_tag == group_tag || group->parent_group_tags[0] == group_tag || group->parent_group_tags[1] == group_tag;
}

__declspec(noinline) void *function_122c10(long group_tag, long tag_index);

// @retail 0x122c10
void *function_122c10(long group_tag, long tag_index)
{
	s_cache_tag_instance *instance = NULL;
	if ((short)tag_index >= 0)
	{
		long count = cache_file_globals.tags->instance_count;
		long index = (short)tag_index;
		if (index < count)
		{
			if (tag_index == CACHE_TAG_INSTANCES[index].datum_index)
				instance = &CACHE_TAG_INSTANCES[index];
		}
	}
	void *result = NULL;

	if (instance && cache_tag_group_is(cache_tag_group_get(instance->group_tag), group_tag))
		result = instance->address;
	return result;
}

// @retail 0x122c70
long function_122c70(s_tag_iterator *iterator)
{

	long local_0 = NONE;
	if (iterator->next_index < cache_file_globals.tags->instance_count)
	{
		do
		{
			long local_1 = *(long volatile *)&iterator->next_index;
			s_cache_tag_instance *instance = &CACHE_TAG_INSTANCES[local_1];
			iterator->next_index = local_1 + 1;
			if (instance && instance->group_tag != NONE && instance->datum_index != NONE)
			{
				if (iterator->group_tag != NONE)
				{
					s_cache_tag_group *group = cache_tag_group_get(instance->group_tag);
					if (iterator->group_tag != group->group_tag && iterator->group_tag != group->parent_group_tags[0] && iterator->group_tag != group->parent_group_tags[1])
						continue;
				}
				iterator->datum_index = instance->datum_index;
				local_0 = instance->datum_index;
				goto local_2;
			}
		} while (iterator->next_index < cache_file_globals.tags->instance_count);
	}
local_2:
	return local_0;
}

// @retail 0x122cf0
long __stdcall cache_tag_group_compare(void const *a, void const *b, void const *context)
{
	return *(long const *)a - *(long const *)b;
}

// @retail 0x122d00
s_cache_tag_group *cache_tag_group_get(long group_tag)
{
	s_cache_tag_group *result = NULL;

	if (cache_file_globals.tags)
	{
		s_cache_tag_group key = { group_tag, NONE, NONE };
		long index = function_13ddd0(&key, cache_file_globals.tags->groups, cache_file_globals.tags->group_count, sizeof(s_cache_tag_group), cache_tag_group_compare, NULL);
		if (index != NONE)
			result = &cache_file_globals.tags->groups[index];
	}
	return result;
}

// @retail 0x122d60
bool function_122d60(unsigned __int64 location, long size, void *buffer)
{
#ifdef H2_NATIVE_PORT
	return host_h2_cache_read(((s_cache_file_location *)&location)->offset, size, buffer) != 0;
#else
	/* the read's completion flag lives in the offset's half once the offset
	   is read */
	s_cache_file_location *parts = (s_cache_file_location *)&location;
	bool *done = (bool *)&parts->offset;
	bool result = false;
	dword bytes_read;
	function_213760(parts->offset, size, buffer, &bytes_read, done, 2, 6);
	function_120d50(done, false);
	if (bytes_read == size)
		result = true;
	return result;
#endif
}

long map_location_get(char const *map_name);
bool map_names_equal(char const *map_name, char const *other_map_name);
bool function_2148b0(long progress);
extern char g_55bd21[0x103];
extern char const *g_4687fc;
extern char const *g_468800;
extern char const *g_468804;
extern char const *g_468808;

static __forceinline void cache_map_progress(char const *name, real *output)
{
	*output = 0.0f;
	long location = map_location_get(name);
	if (location == 3)
	{
		*output = 1.0f;
	}
	else if (location == 2)
	{
		if (map_names_equal(g_55bd21, name))
			function_2148b0((long)output);
	}
	else
	{
		*output = 0.0f;
	}
}

// score 39 (was 39): frame/output-home reuse remains.
// Earlier allocation: frame 0x24 versus 0x18; independent query homes still differ.
// Frame 0x24 versus 0x18; opaque query homes still cannot reuse the incoming slot.
// Incoming name/mode/type names 39/39/39; name plus type 39; helper input/output/state names 39/39/39;
// all boundary names 39. Original operations, helper, parameters and query outputs retained.
// frame 0x24 versus 0x18; independent query homes still differ.
// Output-first helper 80; ordinary inline 80; depth one 80; not-loaded arm first 128;
// guarded stages 126; nested name guard 76 fast, 74 plain.
// Later-three scratch 194; menu/shared scratch 180; final-pair scratch 109;
// nested-guard value return 211; post-switch validity 76; scoped later scratch 194;
// nested-guard direct persistent outputs 203 (fast preview 76). Original body retained.
// Direct menu/shared/campaign outputs 157/148/134; paired direct menu/shared 207;
// menu/campaign 192; shared/campaign 179; early menu completion 232;
// per-query completion lifetimes 256. Independent query homes and incoming-slot reuse still differ.
// Current independent-output trials: loaded/readback names 76/76; delayed accumulation 256;
// delayed map weight 224; delayed accumulation plus names 256; completion selections 76;
// final result selection 76; query declarations together 76.
// Return-by-value helper 211; one shared scratch local 194; scoped same-name scratch 194;
// scoped distinct scratch 208 (a shared temp lets the compiler store 1.0f/0.0f straight to
// the homes and skip the scratch reload on the non-call paths, breaking the per-query block
// order retail shows with its reused incoming-slot scratch); param address reads removed 76;
// mode==0 divide arm first 67 KEPT; case-2 break 69; accumulate form 69.
// Earlier allocation: Frame 0x24 versus 0x18 remains. Initial bool/long/short state 258/287/73; private
// Frame 0x24 versus 0x18 remains. Initial bool/long/short state 258/287/73; private
// helper publication 206; private bool/long state 242/262; private late weight 258; private predicate and
// late weight 228 (fast).
// frame/output-home reuse remains; existing helper output plus
// scalar return for menu/shared/campaign 68/68/68, each pair 68/68/68,
// all later or all four return consumers 68/68. Original helper and readbacks retained.
// Earlier allocation: every instruction has retail's shape; the frame is 0x24 against 0x18, since
// every instruction has retail's shape; the frame is 0x24 against 0x18, since
// retail keeps the three later query outputs and the campaign progress in the dead map-name slot.
// Mode switch alone 75 (73 for the kept form with the mode if/else); product operands swapped 75;
// scoped query outputs 206; one shared query output 207; the existing out-of-line progress query
// called 499, inlined 214 (inline on its definition breaks 0x2c8a0).
// frame/output homes and high-clamp exit still differ.
// Separate final result: entry-one clamps 268/268; uninitialized 68/78;
// dispatch-one 92/93; normalized result 102/101 (lower/upper first).
// Late menu/shared/campaign readbacks 205/188/173; paired menu/shared 214;
// all late readbacks 231; reference output 39; reference plus late readbacks 231;
// completed query producers 223.
// Further trials: weight after entry/map/menu query 45/63/81;
// accumulation after entry/map/menu query 136/135/136;
// both scalars after map/menu query 148/148.
// Helper writing its own result local and copying it out (direct or kept caller copies) 206/206,
// frame 0x1c; one scratch for the three queries 206; one scratch or scoped scratches with the
// helper plain inline 206/206. Any shared scratch slot also drops the non-call-path scratch
// stores and the reload.
// @retail 0x122dd0
real __stdcall function_122dd0(byte *map_name, long mode, long type)
{
	(void)&mode;
	real map_weight = 100.0f;
	real progress = 0.0f;
	long location = map_location_get((char const *)map_name);
	real map_progress;
	cache_map_progress((char const *)map_name, &map_progress);
	real menu_query;
	cache_map_progress(g_468804, &menu_query);
	real menu_progress = menu_query;
	real shared_query;
	cache_map_progress(g_4687fc, &shared_query);
	real shared_progress = shared_query;
	real campaign_query;
	cache_map_progress(g_468800, &campaign_query);
	real campaign_progress = campaign_query;
	if (!strcmp((char const *)map_name, g_468808))
		map_weight = 90.0f;
	else if (type == 0)
		map_weight = 280.0f;
	else if (type == 1)
		map_weight = 100.0f;
	long menu_state = map_location_get(g_468804);
	long shared_state = map_location_get(g_4687fc);
	long campaign_state = map_location_get(g_468800);
	if (menu_state == 4) menu_progress = 1.0f;
	if (shared_state == 4) shared_progress = 1.0f;
	if (campaign_state == 4) campaign_progress = 1.0f;
	if (!strcmp((char const *)map_name, g_468808)) campaign_progress = 1.0f;
	real weight = 0.0f;
	switch (type)
	{
	case 0:
		weight = 500.0f;
		progress = campaign_progress * weight;
	case 1:
	case 4:
		progress = shared_progress * 200.0f + progress;
		weight += 200.0f;
	case 3:
		progress = menu_progress * 80.0f + progress;
		weight += 80.0f;
	case 2:
		progress = map_progress * map_weight + progress;
		weight += map_weight;
	}
	if (weight >= 0.0f)
	{
		switch (mode)
		{
		case 0:
			progress /= weight;
			if (progress < 0.0f) progress = 0.0f;
			else if (progress > 1.0f) progress = 1.0f;
			break;
		case 1:
			progress = weight - progress;
			break;
		default:
			__assume(0);
		}
	}
	else
	{
		progress = 1.0f;
	}
	if (location == 4) progress = 0.0f;
	return progress;
}

// @retail 0x122db0
long function_122db0(long type, char const *map_name)
{
	return ((long)function_122dd0((byte *)map_name, 1, type) / 4) * 1000;
}
