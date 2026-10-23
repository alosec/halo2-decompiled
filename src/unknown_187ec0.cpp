// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_187EC0.CPP: queries on the first local player, and the
   lookups of the globals' block at +0x150 of g_4e034c (0xb4 byte elements) */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_sources.h"
#include <math.h>

#define k_maximum_local_players 4

struct s_player_datum
{
	byte unknown00[0x24];
	long index24;
	byte unknown28[4];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};

struct s_player_record
{
	byte unknown00[0x1a];
	bool flag1a;
	byte unknown1b;
};

extern byte g_51ea18[];

struct s_pitch_object
{
	byte unknown00[0x18c];
	vector3f vector;
};

struct s_object_header
{
	byte unknown00[8];
	s_pitch_object *object;
};

static inline long local_player_first_index(void)
{
	for (long i = 0; i < k_maximum_local_players; i++)
	{
		if (g_4e8c20->entries[i] != NONE)
		{
			return i;
		}
	}
	return NONE;
}

static inline s_player_datum *player_get(long player_index)
{
	return (s_player_datum *)g_4e8c24->data + (player_index & 0xffff);
}

// score 3 (was 3): ours computes the record address in ecx and copies it into esi, retail computes it in esi
// directly; ours is padded with three int3s.
// @retail 0x187ec0
bool function_187ec0(void)
{
	bool result = false;
	long index = local_player_first_index();

	if (index != NONE)
	{
		long record_index = player_get(g_4e8c20->entries[index])->index24;
		if (record_index != NONE)
		{
			s_player_record const *local_0 = (s_player_record *)g_51ea18 + record_index;
			s_player_record record;
			record = *local_0;
			result = record.flag1a;
		}
	}
	return result;
}

// @retail 0x187f30
real function_187f30(void)
{
	real result = 0.0f;
	long index = local_player_first_index();

	if (index != NONE)
	{
		long player_index = g_4e8c20->entries[index];
		bool const &valid = player_index != NONE;
		if (valid)
		{
			long unit_index = player_get(player_index)->unit_index;
			if (unit_index != NONE)
			{
				vector3f vector = ((s_object_header *)g_4e0300->data)[unit_index & 0xffff].object->vector;
				real const &i = vector.i;
				real const &j = vector.j;
				real const &k = vector.k;
				result = (real)atan2(k, sqrt(i * i + j * j));
			}
		}
	}
	return result;
}

/* the 0xb4 byte elements of the block at +0x150 of g_4e034c */
struct s_globals_element_value
{
	long value;
	byte unknown04[4];
};

struct s_globals_element
{
	long key;
	byte unknown04[4];
	short parent_index;
	byte unknown0a[0x3c - 0xa];
	s_globals_element_value first_values[7];
	s_globals_element_value second_values[7];
	byte unknownac[4];
	long tag_index;
};

struct s_globals_element_block_view
{
	byte unknown00[0x150];
	long count;
	s_globals_element *elements;
};

/* an index into a tag block, returned through memory */
class c_block_index
{
public:
	c_block_index(short index) : m_index(index) {}
	short m_index;
};

// @retail 0x1885f0
c_block_index function_1885f0(s_globals_element_block_view const *globals, long key)
{
	for (long i = 0; i < globals->count; i++)
	{
		if (key == globals->elements[i].key)
		{
			return c_block_index((short)i);
		}
	}
	return c_block_index(NONE);
}

static inline s_globals_element *globals_element_get(s_globals_element_block_view const *globals, short index)
{
	s_globals_element *result = NULL;

	if (index != NONE && index >= 0 && index < globals->count)
	{
		result = &globals->elements[index];
	}
	return result;
}

// @retail 0x188640
s_globals_element *function_188640(long key)
{
	s_globals_element_block_view *globals = (s_globals_element_block_view *)g_4e034c;

	long const *local_0 = &key;
	c_block_index local_1 = function_1885f0(globals, *local_0);
	return globals_element_get(globals, local_1.m_index);
}

// @retail 0x188690
s_globals_element *function_188690(short index)
{
	short const *local_0 = &index;
	long local_1 = *(long const volatile *)local_0;
	return globals_element_get((s_globals_element_block_view *)g_4e034c, (short)local_1);
}
/* the entries of a seat's animations, by element of the globals block */
struct s_seat_animation_entry
{
	byte unknown00[4];
	long value04;
	byte unknown08[4];
	long value0c;
	byte unknown10[4];
	short element_index;
	byte type;
	byte unknown17;
};

struct s_seat_animation
{
	byte unknown00[8];
	long first_count;
	s_seat_animation_entry *first_entries;
	long second_count;
	s_seat_animation_entry *second_entries;
};

struct s_seat_animations
{
	long count;
	s_seat_animation *animations;
};

// score 63 (was 63): retail uses ebp as a frame pointer and keeps a word index in di, ours uses stack slots and
// 32-bit registers.
// @retail 0x187fe0
s_seat_animation_entry *function_187fe0(long type, s_seat_animation const *animation, short element_index)
{
	bool first = type == 0;
	s_seat_animation_entry *result = NULL;
	long count = first ? animation->first_count : animation->second_count;
	short index = element_index;
	s_globals_element *element;

	while ((element = globals_element_get((s_globals_element_block_view *)g_4e034c, index)) != NULL)
	{
		for (long i = 0; i < count && !result; i++)
		{
			s_seat_animation_entry *entries = first ? animation->first_entries : animation->second_entries;
			if (entries[i].element_index == index)
			{
				result = &entries[i];
				break;
			}
		}
		index = element->parent_index;
		if (result)
			break;
	}
	return result;
}

// @retail 0x188090
long function_188090(long tag_index, long index, long type)
{
	long result = NONE;

	if (index != NONE)
	{
		s_seat_animations *animations = (s_seat_animations *)g_4e3b44[tag_index & 0xffff].bytes;
		do
		{
			if (index < animations->count)
			{
				s_seat_animation *animation = &animations->animations[index];
				if (type == 0)
				{
					if (animation->first_count > 0)
					{
						goto local_0;
					}
				}
				else
				{
					if (animation->second_count > 0)
					{
						goto local_0;
					}
				}
				goto local_1;
local_0:
				result = index;
				break;
local_1:;
			}

			long next = NONE;
			switch (index)
			{
			case 16:
				next = 15;
				break;
			case 17:
				next = 15;
				break;
			case 19:
				next = 16;
				break;
			case 20:
				next = 19;
				break;
			case 13:
				next = 14;
				break;
			}
			index = next;
		} while (index != NONE);
	}
	return result;
}

static inline s_seat_animation_entry *seat_animation_entry_find(long tag_index, long index, long type, short element_index)
{
	long animation_index = function_188090(tag_index, index, type);
	s_seat_animation_entry *result = NULL;

	if (animation_index != NONE)
	{
		s_seat_animations *animations = (s_seat_animations *)g_4e3b44[tag_index & 0xffff].bytes;
		result = function_187fe0(type, &animations->animations[animation_index], element_index);
	}
	return result;
}

// @retail 0x188130
s_seat_animation_entry *function_188130(long tag_index, long index, long type, short element_index)
{
	return seat_animation_entry_find(tag_index, index, type, element_index);
}

// @retail 0x1882d0
s_seat_animation_entry *function_1882d0(long type, short element_index, long index, short entry_element_index)
{
	short const *element_reference = &element_index;
	short local_0 = *element_reference;
	s_seat_animation_entry *result = NULL;
	s_globals_element *element;

	while ((element = globals_element_get((s_globals_element_block_view *)g_4e034c, local_0)) != NULL)
	{
		long tag_index = element->tag_index;
		if (tag_index != NONE)
		{
			result = seat_animation_entry_find(tag_index, index, type, entry_element_index);
		}
		local_0 = element->parent_index;
		if (result)
		{
			break;
		}
	}
	return result;
}

// @retail 0x188370
real function_188370(long type)
{
	real result = 10.0f;

	switch (type)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		result = 10.0f;
		break;
	case 11:
		result = 20.0f;
		break;
	case 12:
		result = 40.0f;
		break;
	case 13:
		result = 15.0f;
		break;
	case 14:
		result = 15.0f;
		break;
	case 15:
		result = 50.0f;
		break;
	case 16:
		result = 20.0f;
		break;
	case 17:
		result = 20.0f;
		break;
	case 18:
		result = 20.0f;
		break;
	case 19:
		result = 20.0f;
		break;
	case 20:
		result = 20.0f;
		break;
	}
	return result;
}

bool function_172750(long mode, point3f const *point, real radius);

/* element_index starts the parent walk of 0x1882d0's fallback lookup, and
   entry_element_index (the fifth argument, which retail reads into esi) is
   the element whose values and seat entries are looked up */
// @retail 0x1883e0
void function_1883e0(long tag_index, bool ignore_distance, point3f const *point, short element_index, long entry_element_index, long index, long variant,
	long *first_value04, long *second_value04, long *first_value, long *second_value, long *first_value0c, long *second_value0c)
{
	s_globals_element *element = globals_element_get((s_globals_element_block_view *)g_4e034c, (short)entry_element_index);
	long first_index = index == 13 ? 14 : index;
	long first04 = NONE;
	long second04 = NONE;
	long first = NONE;
	long second = NONE;
	long first0c = NONE;
	long second0c = NONE;

	if (ignore_distance || function_172750(0, point, function_188370(index)))
	{
		s_seat_animation_entry *first_entry = NULL;
		s_seat_animation_entry *second_entry = NULL;

		if (tag_index != NONE)
		{
			first_entry = function_188130(tag_index, first_index, 0, (short)entry_element_index);
			second_entry = function_188130(tag_index, index, 1, (short)entry_element_index);
		}
		if (!first_entry)
		{
			first_entry = function_1882d0(0, element_index, first_index, (short)entry_element_index);
		}
		if (!second_entry)
		{
			second_entry = function_1882d0(1, element_index, index, (short)entry_element_index);
		}

		if (first_entry)
		{
			if (first_entry->type == 1 && variant != NONE)
			{
				if (index >= 13 && index <= 14)
				{
					first = element->first_values[3].value;
				}
				else
				{
					first = element->first_values[variant].value;
				}
			}
			first04 = first_entry->value04;
			first0c = first_entry->value0c;
		}
		if (second_entry)
		{
			if (second_entry->type == 1 && variant != NONE)
			{
				if (index >= 13 && index <= 14)
				{
					switch (index)
					{
					case 13:
						second = element->second_values[4].value;
						break;
					case 14:
						second = element->second_values[3].value;
						break;
					}
				}
				else
				{
					second = element->second_values[variant].value;
				}
			}
			second04 = second_entry->value04;
			second0c = second_entry->value0c;
		}
	}

	if (first_value04)
	{
		*first_value04 = first04;
	}
	if (second_value04)
	{
		*second_value04 = second04;
	}
	if (first_value)
	{
		*first_value = first;
	}
	if (second_value)
	{
		*second_value = second;
	}
	if (first_value0c)
	{
		*first_value0c = first0c;
	}
	if (second_value0c)
	{
		*second_value0c = second0c;
	}
}

/* a block index of NONE, read where a function takes a block index */
short g_47d8e0 = NONE;

dword vector3d_compress(vector3f const *vector);
#include "unknown_1765e0.h"
long function_189400(s_sound_position const *position, long object_index, long tag_index, real scale);

// @retail 0x188180
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index, long variant,
	long entry_element_index, long effect_value, s_location const *location, real scale)
{
	long first_values[3];
	long second_values[3];
	point3f effect_point;
	s_sound_position position;

	effect_point.x = forward->i * 0.01f + point->x;
	effect_point.y = forward->j * 0.01f + point->y;
	effect_point.z = forward->k * 0.01f + point->z;
	second_values[0] = NONE;
	second_values[1] = NONE;
	second_values[2] = NONE;
	first_values[0] = NONE;
	first_values[1] = NONE;
	first_values[2] = NONE;
	function_1883e0(tag_index, false, point, g_47d8e0, entry_element_index, index, variant,
		&first_values[0], &second_values[0], &first_values[1], &second_values[1], &first_values[2], &second_values[2]);

	for (dword i = 0; i < sizeof(second_values) / sizeof(second_values[0]); i++)
	{
		if (second_values[i] != NONE)
		{
			function_1765e0(&effect_point, (vector3f const *)effect_value, forward, second_values[i], 0, 0);
		}
	}

	position.position = effect_point;
	position.compressed_forward = vector3d_compress(forward);
	position.velocity = *g_4687a4;
	position.location = *location;
	for (dword j = 0; j < sizeof(first_values) / sizeof(first_values[0]); j++)
	{
		if (first_values[j] != NONE)
		{
			function_189400(&position, object_index, first_values[j], scale);
		}
	}
}

struct s_unit_material_object
{
	long definition_index;
	byte unknown04[0x10a - 4];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word unknown10a : 13;
};

struct s_unit_material_object_header
{
	byte unknown00[8];
	s_unit_material_object *object;
};

struct s_unit_material_definition
{
	byte unknown00[0x38];
	long model_index;
	byte unknown3c[0x280 - 0x3c];
	short material_type;
	short alternate_material_type;
};

struct s_unit_material_model
{
	byte unknown00[0x60];
	long count;
	struct
	{
		byte unknown00[0xce];
		short material_type;
	} *entries;
};

// @retail 0x1886d0
short *function_1886d0(long object_index, short *material_type)
{
	struct s_1886d4 { signed short field_0 : 16; };
	s_1886d4 local_1;
	local_1.field_0 = g_47d8e0;
	s_unit_material_object *object = ((s_unit_material_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_unit_material_definition *definition = (s_unit_material_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;

	if (function_badc0(object_index, 1))
	{
		if (TEST_FIELD_BIT(object->flag2))
		{
			local_1.field_0 = definition->alternate_material_type;
		}
		else
		{
			local_1.field_0 = definition->material_type;
		}
		if (local_1.field_0 != g_47d8e0)
		{
			goto local_0;
		}
	}
	if (definition->model_index != NONE)
	{
		s_unit_material_model *model = (s_unit_material_model *)g_4e3b44[definition->model_index & 0xffff].bytes;
		if (model->count)
		{
			local_1.field_0 = model->entries->material_type;
		}
	}
local_0:
	*material_type = local_1.field_0;
	return material_type;
}