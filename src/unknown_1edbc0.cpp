// @flags /O2 /Ob1 /Gr
/* UNKNOWN_1EDBC0.CPP: a caller optimized for speed, which has
function_123d40 inlined. /Ob1 keeps this function out of line in its
caller function_1c25a0 (0x1c25a0), as retail does. */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "globals.h"
#include <string.h>

/* the scenery index of each object name (unknown_1c2670.cpp) */
extern long *g_51e9cc;

// @retail 0x1edbc0
void game_state_initialize_1edbc0(void)
{
	if (!g_51e9cc)
	{
		g_51e9cc = (long *)function_123d40("unknown", "unknown", 8000);
	}
}

struct s_mapping_object
{
	byte field_0[0x1a];
	short scenario_index;
};

struct s_mapping_header
{
	short salt;
	byte flags;
	byte type;
	short cluster;
	byte field_6[2];
	s_mapping_object *object;
};

struct s_mapping_definition
{
	byte field_0[0x58];
	short mapping_index;
	byte field_5a[2];
};

struct s_mapping_scenario
{
	byte field_0[0x54];
	s_mapping_definition *definitions;
};

struct s_type_4f0dcc;
bool function_d5910(const s_type_4f0dcc *datum, bool force);

// @retail 0x1edc10
void function_1edc10()
{
	s_record_pool *data = g_4e0300;
	memset(g_51e9cc, 0xff, 8000);
	long previous = NONE;
	for (;;)
	{
		long index = data_next_absolute_index_inlined(data, previous + 1);
		if (index == NONE) return;
		s_mapping_header *header = (s_mapping_header *)(data->data + data->size * index);
		long object_index = (header->salt << 16) | index;
		previous = index;
		if (header->cluster != NONE && ((1 << header->type) & 0x40) && !(header->flags & 0x10))
		{
			s_mapping_object *object = ((s_mapping_header *)(*(byte *volatile *)&data->data))[object_index & 0xffff].object;
			if (object->scenario_index != NONE)
			{
				s_mapping_definition *definition = &((s_mapping_scenario *)g_4e0350)->definitions[object->scenario_index];
				if (function_d5910((const s_type_4f0dcc *)definition, true))
				{
					long mapping_index = definition->mapping_index - 1;
					if (mapping_index != NONE)
						g_51e9cc[mapping_index] = object_index;
				}
			}
		}
	}
}
