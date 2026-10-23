// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "crc.h"
#include "unknown_123b30.h"
#include "globals.h"
#include <string.h>

struct s_object
{
	long definition_index;
	byte unknown04[0xb0];
	long manager_index;
};

struct s_object_header
{
	byte unknown00[8];
	s_object *object;
};

struct s_object_definition_data
{
	byte unknown00[0x38];
	long model_index;
};

struct s_model_data
{
	byte unknown00[0x24];
	long graph_index;
};

struct s_graph_record
{
	short value;
	byte unknown02[0x8e];
};

struct s_graph_data
{
	byte unknown00[0x3c];
	s_graph_record *records;
};

struct s_manager_node
{
	byte unknown00[0x48];
	signed char *record_indices;
	long record_index_count;
	byte unknown50[0x10];
};

/* an element (0xa0 bytes) of the havok components, g_51e9b8 */
struct s_manager_entry
{
	byte unknown00[0x70];
	s_manager_node *nodes;
	long node_count;
	byte unknown78[0x28];
};

real const k_real_zero = 0.0f;

struct s_node
{
	byte unknown00[0xc];
	s_node *next;
	s_node *function_965e0() const;
	s_node *get_last();
};

struct s_scale_definition
{
	byte unknown00[0x2c];
	real scale;
};

struct s_scale_owner
{
	byte unknown00[0x3c];
	s_scale_definition *definition;
	real get_inverse_scale() const;
};

struct s_scale_holder
{
	byte unknown00[0x2c];
	real scale;
	real get_inverse_scale() const;
};

struct s_flag_byte
{
	byte value;
	bool is_set() const;
	s_flag_byte *set(byte new_value);
};

// @retail 0x183d90
s_node *s_node::function_965e0() const
{
	return next;
}

// @retail 0x183df0
real s_scale_holder::get_inverse_scale() const
{
	real value = scale;

	if (value == k_real_zero)
	{
		return k_real_zero;
	}

	return 1.0f / value;
}

// @retail 0x183e20
bool s_flag_byte::is_set() const
{
	return value != 0;
}

// @retail 0x183e30
s_flag_byte *s_flag_byte::set(byte new_value)
{
	value = new_value;
	return this;
}

byte *g_4ed280;

// @retail 0x183e40
void function_183e40()
{
	long size = 0x4204;
	byte *base = game_state_globals.base_address + game_state_globals.cpu_allocation_size;

	game_state_globals.cpu_allocation_size += 0x4204;
	function_163ba0(&game_state_globals.allocation_size_checksum, &size, 4);
	g_4ed280 = base;
	g_4ea958.active_slot_count = 0;
	for (long i = 0; i < 8; i++)
	{
		g_4eca60[i] = NONE;
	}
}

// @retail 0x183ec0
void function_183ec0()
{
	memset(g_4ed280, 0xff, 0x4201);
}

// @retail 0x183c60
long function_183c60(
	long object_index,
	long maximum_value)
{
	long best_value = NONE;
	long best_node = NONE;

	if (maximum_value != NONE)
	{
		s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xFFFF].object;
		s_object_definition_data *definition = (s_object_definition_data *)g_4e3b44[object->definition_index & 0xFFFF].data;
		s_model_data *model = (s_model_data *)g_4e3b44[definition->model_index & 0xFFFF].data;
		s_graph_data *graph = (s_graph_data *)g_4e3b44[model->graph_index & 0xFFFF].data;
		s_manager_entry *entry = &((s_manager_entry *)g_51e9b8->data)[object->manager_index & 0xFFFF];

		for (long node_index = 0; node_index < entry->node_count; node_index++)
		{
			s_manager_node *node = &entry->nodes[node_index];

			for (long i = 0; i < node->record_index_count; i++)
			{
				s_graph_record *record = &graph->records[node->record_indices[i]];

				if (record->value != NONE && (best_value == NONE || (record->value <= maximum_value && record->value > best_value)))
				{
					best_value = record->value;
					best_node = node_index;
				}
			}
		}
	}

	return best_node;
}
