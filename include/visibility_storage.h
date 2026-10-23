#ifndef VISIBILITY_STORAGE_H
#define VISIBILITY_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "object_queries.h"

class c_entry_list;
struct s_masked_list;
struct s_133f70;
struct s_bit_vector_pool_sizes
{
	short unknown0;
	short list_sizes[4];
	short record_count;
};

struct s_bit_vector_pool
{
	void *context;
	s_location location;
	c_entry_list *lists[4];
	long indices[0x80];
	dword flags[0x10];
	dword pool[0x200];
	word pool_used;
	word entry_count;
	dword entries[0x200][4];
	dword flags2a60;
	short frustum_count;
	byte unknown2a66[2];
	point3f center;
	real radius;
	long mode;
	long query_type;
	byte unknown2a80[4];
	real plane_offset;
	plane3f plane;
	long counters[13];
	byte *records;
	void *filter;
	s_bit_vector_pool_sizes sizes;

	void function_133ab0(s_133f70 *entry, bool arg_3, bool arg_4, bool arg_5);
};

struct s_visibility_pool_storage
{
    union
    {
        s_bit_vector_pool field_0;
        struct
        {
            byte unknown_0[0xc];
            s_masked_list *visible;
            s_masked_list *filtered;
        } legacy_lists;
    };
};

extern s_visibility_pool_storage g_visibility_pool_storage;
#define g_547f88 (g_visibility_pool_storage.field_0)
typedef char assert_visibility_offset_0[(offsetof(s_visibility_pool_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_visibility_size[(sizeof(s_visibility_pool_storage) == 0x2ae0) ? 1 : -1];

#define g_547f94 (g_visibility_pool_storage.legacy_lists.visible)
#define g_547f98 (g_visibility_pool_storage.legacy_lists.filtered)
typedef char assert_visibility_visible[(offsetof(s_visibility_pool_storage, legacy_lists.visible) == 0xc) ? 1 : -1];
typedef char assert_visibility_filtered[(offsetof(s_visibility_pool_storage, legacy_lists.filtered) == 0x10) ? 1 : -1];
#endif
