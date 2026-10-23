/* UNKNOWN_122870.H: the map cache file (unknown_122870.cpp) */
#ifndef UNKNOWN_122870_H
#define UNKNOWN_122870_H

#include "unknown_11c920.h"

struct s_cache_tag_group;
struct s_cache_tag_instance;

struct s_cache_header
{
	long header_signature;
	long version;
	long size;
	dword unknown0c;
	dword tag_data_offset;
	dword tag_data_size;
	dword unknown18;
	long unknown1c;
	byte unknown20[0x100];
	char build_version[0x20];
	short type;
	byte unknown142[0x56];
	char name[32];
	byte unknown1b8[0x644];
	long footer_signature;
};

struct s_cache_tags_header
{
	s_cache_tag_group *groups;
	long group_count;
	s_cache_tag_instance *instances;
	long scenario_index;
	long globals_index;
	dword unknown14;
	long instance_count;
	long signature;
};

struct s_structure_bsp_header
{
	dword unknown00;
	void *bsp_address;
	void *lightmap_address;
	long signature;
};

struct s_cache_file_location
{
	long file_index;
	dword offset;
};

/* a cache file location as the cache file reader takes it: one 64-bit value
   with the file index in the low half and the offset in the high half */
static inline unsigned __int64 cache_file_location_make(long file_index, dword offset)
{
	return ((unsigned __int64)offset << 32) | (dword)file_index;
}

/* 0x5476f8 */
struct s_cache_file_globals
{
	bool loaded;
	void *field_4_7;
	s_cache_header header;
	s_cache_tags_header *tags;
	s_structure_bsp_header *bsp;
};

extern s_cache_file_globals cache_file_globals;

#endif
