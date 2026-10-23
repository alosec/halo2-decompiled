// @flags /O2 /arch:SSE /Gr
/* SCENERY.CPP: the scenery object type (its definition is at 0x467ff0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1428b0.h"
#include "unknown_1dacb0.h"
#include "unknown_1c62f0.h"
#include "unknown_1cafc0.h"
#include "unknown_184250.h"

/* the scenery definition (the tag data) */
struct s_scenery_definition
{
	byte unknown000[0x38];
	long model_index;
	byte unknown03c[0xbc - 0x3c];
	short pathfinding_policy;
	byte unknown0be[2];
	short value_c0;
};

struct s_model_definition_view
{
	byte unknown00[4];
	long collision_model_index;
};

struct s_collision_model_view
{
	byte unknown00[8];
	long value_08;
};

/* where a scenery is placed */
struct s_scenery_location
{
	long value_00;
	short value_04;
	byte value_06;
	byte value_07;
};

struct s_scenery_placement
{
	byte unknown00[0x4c];
	short pathfinding_policy;
	byte unknown4e[2];
	long entry_count;
	struct s_scenery_bsp_entry *entries;
};

struct s_scenery_bsp_entry
{
	short field_0;
	short value;
};

/* the scenery (the object data) */
struct s_scenery
{
	long definition_index;
	union
	{
		dword object_flags;
		struct { dword : 22; dword flag22 : 1; dword flag23 : 1; };
	};
	byte unknown008[0x1a - 8];
	short placement_index;
	byte unknown01c[0xa4 - 0x1c];
	s_scenery_location location;
	byte unknown0ac[0x116 - 0xac];
	short node_matrices_offset;
	byte unknown118[0x12a - 0x118];
	short animation_state_offset;
	dword flags;
	short value_130;
	short value_132;
	long value_134;
	long attached_object_index;
};

struct s_scenery_header
{
	short identifier;
	byte flags;
	byte type;
	short cluster_index;
	byte unknown06[2];
	s_scenery *scenery;
};

/* the animation state of an object (a view of unknown_1cafc0.cpp's) */
struct s_animation_view
{
	byte unknown00[0x14];
	short frame_count;
};

struct s_scenery_animation_state
{
	c_animation_channel channels[3];
	byte unknown60[8];
	long graph_tag_index;
};

static inline long real_to_long(real value)
{
	long result;
	__asm
	{
		fld value
		fistp result
	}
	return result;
}

/* g_4e0344: the structure bsp's entries (the first of the array at +0x84
   has a count at +0x58 and 12 byte entries at +0x5c) */
struct s_bsp_location_entry
{
	long value_00;
	short value_04;
	byte value_06;
	byte value_07;
	long value_08;
};

struct s_bsp_locations
{
	byte unknown00[0x58];
	long count;
	s_bsp_location_entry *entries;
};

s_structure_bsp_globals *g_4e0344;

#define SCENERY_GET(index) (((s_scenery_header *)g_4e0300->data)[(index) & 0xffff].scenery)
#define TAG_DATA(type, index) ((type *)g_4e3b44[(index) & 0xffff].bytes)

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

/* 0x10a8b0 is in unknown_10a8b0.cpp: retail calls it out of line, which
   needs an /Ob1 file */
long function_10a8b0(s_scenery_location *location, long value);

short function_10a0b0(long scenery_index);

// @retail 0x10a820
void function_10a820(long scenery_index)
{
	s_scenery *scenery = SCENERY_GET(scenery_index);
	s_scenery_definition *definition = TAG_DATA(s_scenery_definition, scenery->definition_index);
	long result = NONE;

	if (definition->model_index != NONE && function_10a0b0(scenery_index) != 2)
	{
		long collision_model_index = TAG_DATA(s_model_definition_view, definition->model_index)->collision_model_index;
		if (collision_model_index != NONE)
			result = function_10a8b0(&scenery->location, TAG_DATA(s_collision_model_view, collision_model_index)->value_08);
	}
	scenery->value_134 = result;
}

// @retail 0x10a030
void __stdcall function_10a030(long scenery_index)
{
	function_10a820(scenery_index);
}

#if !H2_NATIVE_PORT
// @retail 0x10a040
short __fastcall function_10a040(long definition_index, s_scenery_placement *placement)
{
	s_scenery_placement *const *placement_reference = &placement;
	short result = 3;

	if (definition_index != NONE)
	{
		switch ((*placement_reference)->pathfinding_policy)
		{
		case 0:
			result = TAG_DATA(s_scenery_definition, definition_index)->pathfinding_policy;
			break;
		case 1:
			result = 2;
			break;
		case 2:
			result = 0;
			break;
		case 3:
			result = 1;
			break;
		default:
			result = 3;
			break;
		}
	}
	return result;
}
#endif

// @retail 0x10a0b0
short function_10a0b0(long scenery_index)
{
	return TAG_DATA(s_scenery_definition, SCENERY_GET(scenery_index)->definition_index)->value_c0;
}

void function_b8b70(long object_index);

/* sets or clears one bit of a flags word */
static inline void flags_set_bit(dword *flags, long bit, bool value)
{
	if (value)
		*flags |= 1 << bit;
	else
		*flags &= ~(1 << bit);
}

// @retail 0x10a0f0
void __stdcall function_10a0f0(long scenery_index, s_scenery_placement *placement)
{
	s_scenery *scenery = SCENERY_GET(scenery_index);
	scenery->value_130 = NONE;
	flags_set_bit(&scenery->object_flags, 22, false);
	flags_set_bit(&scenery->object_flags, 23, false);
	scenery->value_132 = function_10a040(scenery->definition_index, placement);
	switch (scenery->value_132)
	{
	case 1:
		if (g_4686c4 >= 0 && g_4686c4 < placement->entry_count)
		{
			s_scenery_bsp_entry *entry = &placement->entries[g_4686c4];
			scenery->value_130 = entry->value;
			if (entry->value != NONE)
				flags_set_bit(&scenery->object_flags, 22, true);
		}
	case 0:
	case 3:
		flags_set_bit(&scenery->object_flags, 23, true);
		break;
	default:
		break;
	}
	function_b8b70(scenery_index);
}

// @retail 0x10a390
void __stdcall function_10a390(long scenery_index, transform4x3f *matrix)
{
	s_scenery *scenery = SCENERY_GET(scenery_index);

	if (scenery->attached_object_index != NONE && function_badc0(scenery->attached_object_index, NONE))
	{
		s_scenery *object = SCENERY_GET(scenery->attached_object_index);
		transform4x3f *nodes = (transform4x3f *)((byte *)object + object->node_matrices_offset);

		function_142a60(nodes, matrix, matrix);
	}
}

// @retail 0x10a460
long function_10a460(long object_index)
{
	long result = 0;

	if (object_index != NONE)
	{
		if (((s_scenery_header *)g_4e0300->data)[object_index & 0xffff].type == 6)
		{
			s_scenery *scenery = SCENERY_GET(object_index);

			if (SCENERY_GET(object_index)->animation_state_offset != NONE)
			{
				s_scenery_animation_state *state = (s_scenery_animation_state *)((byte *)scenery + scenery->animation_state_offset);
				c_animation_channel *channel = &state->channels[0];

				if (state->graph_tag_index != NONE && channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
				{
					s_animation_view *animation = (s_animation_view *)channel->function_1c6440();
					real time = 0.0f;
					real remaining;
					long frames;

					if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
						time = channel->frame_position * (1.0f / 30.0f);
					remaining = ((real)animation->frame_count * (1.0f / 30.0f) - time) * 30.0f;
					__asm
					{
						fld remaining
						fistp frames
					}
					result = ((short)frames - 2 > 0) ? (short)frames - 2 : 0;
				}
			}
		}
	}
	return result;
}

/* the scenery object type definition */
bool __stdcall function_10a1b0(long scenery_index, void *placement, bool *result);
bool __stdcall function_10a2f0(long scenery_index);
void __stdcall function_10a240(long scenery_index);

struct s_scenery_type_definition
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *unknown10[4];
	void (__stdcall *handler20)(long);
	void *unknown24[2];
	bool (__stdcall *handler2c)(long, void *, bool *);
	void (__stdcall *handler30)(long, s_scenery_placement *);
	void *unknown34[2];
	void (__stdcall *handler3c)(long);
	bool (__stdcall *handler40)(long);
	void *unknown44[11];
	void (__stdcall *handler70)(long, transform4x3f *);
};

s_scenery_type_definition g_467ff0 =
{
	"scenery",
	'scen',
	0x13c,
	0x50,
	0x58,
	0x5c,
	{ 0, 0, 0, 0 },
	function_10a030,
	{ 0 },
	function_10a1b0,
	function_10a0f0,
	{ 0 },
	function_10a240,
	function_10a2f0,
	{ 0 },
	function_10a390
};

bool scenery_animation_begin(long object_index, long animation_name, short frame, long attached_object_index, bool interpolate, bool loop, long animation_graph_index);

/* starts a scenery's default looping animation; flag 0 says it plays */
// @retail 0x10a3f0
void function_10a3f0(long scenery_index)
{
	s_scenery *scenery = SCENERY_GET(scenery_index);

	scenery->flags &= ~1;
	if (SCENERY_GET(scenery_index)->animation_state_offset != NONE)
	{
		long graph_tag_index = ((s_scenery_animation_state *)((byte *)scenery + scenery->animation_state_offset))->graph_tag_index;

		if (graph_tag_index != NONE &&
			scenery_animation_begin(scenery_index, 0x400000c, 0, NONE, false, true, graph_tag_index))
		{
			scenery->flags |= 1;
		}
	}
}

struct s_scenery_placement_entry
{
	byte unknown00[0x58];
	short name_index;
	byte unknown5a[2];
};

struct s_scenery_scenario_view
{
	byte unknown00[0x54];
	s_scenery_placement_entry *entries;
};

extern long *g_51e9cc;
struct c_shape_global_owner;
extern c_shape_global_owner *g_51e9d0;
void function_1c5710(long object_index);
void __stdcall function_b9b90(long object_index, bool disable);
void function_bba20(long object_index);
void __stdcall function_bf600(long user, real frame, s_animation_frame_event const *event);

// @retail 0x10a1b0
bool __stdcall function_10a1b0(long scenery_index, void *placement, bool *result)
{
	s_scenery *scenery = SCENERY_GET(scenery_index);
	long name_index = NONE;
	if (scenery->placement_index != NONE)
		name_index = ((s_scenery_scenario_view *)g_4e0350)->entries[scenery->placement_index].name_index - 1;
	function_10a820(scenery_index);
	scenery->attached_object_index = NONE;
	scenery->value_132 = 3;
	function_b9b90(scenery_index, true);
	function_10a3f0(scenery_index);
	scenery->flags = 0;
	if (name_index != NONE)
		g_51e9cc[name_index] = scenery_index;
	return true;
}

#if !H2_NATIVE_PORT
// @retail 0x10a250
void __stdcall function_10a250(long scenery_index)
{
    s_scenery *scenery = SCENERY_GET(scenery_index);
    function_2e90a0(s_184251(false));
    if (!(scenery->flags & 2))
    {
        s_scenery *current = SCENERY_GET(scenery_index);
        if (current->placement_index != NONE)
        {
            long name_index = ((s_scenery_scenario_view *)g_4e0350)->entries[current->placement_index].name_index - 1;
            if (name_index != NONE)
            {
                if (g_51e9d0 != NULL)
                    function_1c5710(scenery_index);
                g_51e9cc[name_index] = NONE;
            }
        }
        scenery->flags |= 2;
    }
}
#endif

// @retail 0x10a2f0
bool __stdcall function_10a2f0(long scenery_index)
{
	bool result = false;
	s_scenery *scenery = SCENERY_GET(scenery_index);
	switch (SCENERY_GET(scenery_index)->animation_state_offset)
	{
	case NONE:
		break;
	default:
		{
			s_animation_state *state = (s_animation_state *)((byte *)scenery + scenery->animation_state_offset);
			state->update(function_bf600, scenery_index, 0, NULL, NULL);
			c_animation_channel *channel = &state->channels[0];
			if (state->graph_tag_index != NONE && channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
			{
				s_animation *animation = function_1daea0(graph_tag_get(channel->graph_tag_index), channel->animation_id);
				if (((s_animation_view *)animation)->frame_count > 1)
				{
					function_bba20(scenery_index);
					result = true;
				}
			}
		}
	}
	return result;
}

struct s_scenery_model_part
{
	byte unknown00[5];
	char index;
	byte unknown06[2];
};

struct s_scenery_model_region
{
	long name;
	char index;
	byte unknown05[3];
	long count;
	s_scenery_model_part *parts;
};

struct s_scenery_model_view
{
	byte unknown00[0xc];
	long graph_index;
	byte unknown10[0x70 - 0x10];
	long count;
	s_scenery_model_region *regions;
};

struct s_scenery_graph_part
{
	byte unknown00[0xc];
	long value;
	byte unknown10[4];
};

struct s_scenery_graph_region
{
	long name;
	long count;
	s_scenery_graph_part *parts;
};

struct s_scenery_graph_view
{
	byte unknown00[0x20];
	s_scenery_graph_region *regions;
};

struct s_type_1a7926;
bool function_20a9a0(s_type_1a7926 *matrices, long object_index);

// @retail 0x10a520
bool function_10a520(long object_index)
{
	s_scenery_header *header = &((s_scenery_header *)g_4e0300->data)[object_index & 0xffff];
	s_scenery_definition *definition = TAG_DATA(s_scenery_definition, header->scenery->definition_index);
	bool result = false;
	byte info[0x54];
	if (header->cluster_index != NONE)
	{
		if (function_20a9a0((s_type_1a7926 *)info, object_index))
			result = true;
		else if (definition->model_index != NONE)
		{
			s_scenery_model_view *model = TAG_DATA(s_scenery_model_view, definition->model_index);
			if (model->graph_index != NONE && model->count > 0)
			{
				s_scenery_graph_view *graph = TAG_DATA(s_scenery_graph_view, model->graph_index);
				for (long i = 0; i < model->count; i++)
				{
					s_scenery_model_region *region = &model->regions[i];
					if (region->index != NONE)
					{
						s_scenery_graph_region *graph_region = &graph->regions[region->index];
						if (region->count > 0)
						{
							long count = 0;
							for (long j = 0; j < region->count; j++)
							{
								char index = region->parts[j].index;
								if (index != NONE && graph_region->parts[index].value == 1)
									count++;
							}
							if (region->count != count)
							{
								result = false;
								break;
							}
							result = true;
						}
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x10a240
void __stdcall function_10a240(long scenery_index)
{
    function_10a250(scenery_index);
}
