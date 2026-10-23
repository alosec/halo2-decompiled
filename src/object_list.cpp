// @flags /O2 /Gr
/* OBJECT_LIST.CPP: the object list (object_list.h): the objects whose
   definition marks them as carrying other objects, kept in a game state
   block of up to 32 */

#include "object_dispatch.h"
#include "crc.h"
#include "unknown_123b30.h"
#include "object_list.h"
#include <string.h>

s_object_list_state *g_5107f0;

/* allocates the list's block from the game state */
// @retail 0x109300
void object_list_initialize(void)
{
	long size = sizeof(s_object_list_state);
	s_object_list_state *state = (s_object_list_state *)(game_state_globals.base_address + game_state_globals.cpu_allocation_size);

	game_state_globals.cpu_allocation_size += size;
	function_163ba0(&game_state_globals.allocation_size_checksum, &size, sizeof(size));
	memset(state, 0, sizeof(s_object_list_state));
	g_5107f0 = state;
}

// @retail 0x109350
void object_list_dispose(void)
{
	g_5107f0 = 0;
}

/* empties the list for a new map */
// @retail 0x109360
void object_list_initialize_for_new_map(void)
{
	memset(g_5107f0, 0, sizeof(s_object_list_state));
	g_5107f0->locked = true;
}

// @retail 0x109380
void object_list_dispose_from_old_map(void)
{
	g_5107f0->locked = false;
}

/* adds an object whose definition carries objects to the list, while there
   is room */
// @retail 0x109390
void object_list_insert(long object_index)
{
	s_object_common_datum *object = OBJECT_DATUM_GET(object_index);

	if (TEST_FIELD_BIT(((s_object_definition *)g_4e3b44[object->definition_index & 0xFFFF].bytes)->carries_objects))
	{
		if (g_5107f0->object_count < 32)
		{
			object->in_object_list = true;
			g_5107f0->object_indices[g_5107f0->object_count] = object_index;
			g_5107f0->object_count++;
		}
	}
}

void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_e4c10(long object_index);

/* takes an object out of the list and lets go of the objects it carried */
// @retail 0x109400
void __stdcall object_list_erase(long object_index)
{
    s_object_common_datum *object = OBJECT_DATUM_GET(object_index);
    if (TEST_FIELD_BIT(((s_object_definition *)g_4e3b44[object->definition_index & 0xffff].bytes)->carries_objects))
    {
        s_object_list_state *state = g_5107f0;
        for (long i = 0; i < state->object_count; ++i)
        {
            if (state->object_indices[i] == object_index)
            {
                object->in_object_list = false;
                state->object_indices[i] = state->object_indices[state->object_count - 1];
                --state->object_count;
                s_record_pool_iterator iterator;
                iterator.data = g_4e0300;
                iterator.index = NONE;
                s_object_header_datum *header;
                while ((header = (s_object_header_datum *)data_iterator_next_inlined(&iterator)) != 0)
                {
                    long carried_index = iterator.datum_index;
                    s_object_common_datum *carried = OBJECT_DATUM_GET(carried_index);
                    if ((header->flags & 1) && !(header->flags & 0x10) &&
                        (TEST_FIELD_BIT(carried->flag1) || TEST_FIELD_BIT(carried->carried_by_platform)) &&
                        carried->platform_index == object_index)
                    {
                        long type_mask = 1 << header->type;
                        if (type_mask & ((1 << _object_kind_weapon) | (1 << _object_kind_equipment) | (1 << _object_kind_garbage)))
                        {
                            function_b9b90(carried_index, false);
                            function_b7360(carried_index);
                        }
                        else if (type_mask & (1 << _object_kind_biped))
                        {
                            function_e4c10(carried_index);
                            function_b9b90(carried_index, false);
                            function_b7360(carried_index);
                        }
                    }
                }
                break;
            }
        }
    }
}
