// @flags /O2 /Ob1 /Gr
/* UNKNOWN_1130A0.CPP */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_107590.h"
#include <string.h>

/* the units (a local view of the object data) */
struct s_unit_1130a0
{
	long definition_index;
	byte unknown004[0x10a - 4];
	word : 2;
	word frozen : 1;
	word : 13;
	byte unknown10c[0x134 - 0x10c];
	dword unit_flags;
	byte unknown138[0x1f6 - 0x138];
	char index1f6;
	char index1f7;
	byte unknown1f8[0x342 - 0x1f8];
	short state_offset;
};

struct s_object_header_1130a0
{
	byte unknown00[8];
	s_unit_1130a0 *object;
};

// @retail 0x1130a0
void function_1130a0(long unit_index, bool flag)
{
	s_unit_1130a0 *unit = ((s_object_header_1130a0 *)g_4e0300->data)[unit_index & 0xffff].object;
	if (flag && (unit->index1f6 == NONE || unit->index1f7 == NONE))
		flag = false;
	dword *flags = &unit->unit_flags;
	flag ? (*flags |= 0x1000000) : (*flags &= ~0x1000000);
}

// @retail 0x114480
long function_114480(long unit_index)
{
	s_unit_1130a0 *unit = ((s_object_header_1130a0 *)g_4e0300->data)[unit_index & 0xffff].object;
	return *(long *)((byte *)unit + unit->state_offset);
}

PRIVATE __forceinline long *unit_state_handle_114440(s_unit_1130a0 *unit)
{
	return (long *)((byte *)unit + unit->state_offset);
}

// @retail 0x114440
long function_114440(long unit_index)
{
	long result = 0;
	s_unit_1130a0 *unit = ((s_object_header_1130a0 *)g_4e0300->data)[unit_index & 0xffff].object;
	long const &index = *unit_state_handle_114440(unit);
	if (index != NONE)
		result = *(long *)(g_4e3b44[index & 0xffff].bytes + 0x14);
	return result;
}

struct s_state_entry_1144a0
{
	byte unknown00[0xc];
	long tag_index;
};

struct s_state_tag_1144a0
{
	byte unknown00[0xc];
	long count;
	s_state_entry_1144a0 *entries;
};

bool function_20b5c0(long definition_index);

// @retail 0x1144a0
bool function_1144a0(long unit_index, short entry_index, long *tag_index)
{
	s_unit_1130a0 *unit = ((s_object_header_1130a0 *)g_4e0300->data)[unit_index & 0xffff].object;
	long const &index = *unit_state_handle_114440(unit);
	long found = NONE;
	bool result = false;
	if (index != NONE)
	{
		s_state_tag_1144a0 *tag = (s_state_tag_1144a0 *)g_4e3b44[index & 0xffff].bytes;
		if (entry_index >= 0 && entry_index < tag->count)
		{
			found = tag->entries[entry_index].tag_index;
			if (found != NONE)
				result = !function_20b5c0(found);
		}
	}
	if (*tag_index)
		*tag_index = found;
	return result;
}

struct s_unit_state_114ec0
{
	long tag_index;
	long value4;
	long value8;
};

struct s_model_variant_114ec0
{
	long name;
	byte unknown04[0x2c - 4];
	long value;
	byte unknown30[4];
	long tag_index;
};

struct s_model_114ec0
{
	byte unknown00[0x50];
	long variant_count;
	s_model_variant_114ec0 *variants;
	byte unknown58[0x90 - 0x58];
	long tag_index;
	byte unknown94[0xa0 - 0x94];
	long value;
};

PRIVATE __forceinline void unit_state_tag_set_114ec0(long unit_index, long const &index)
{
	long value = *(long *)(g_4e3b44[index & 0xffff].bytes + 0x14);
	s_unit_1130a0 *unit = ((s_object_header_1130a0 *)g_4e0300->data)[unit_index & 0xffff].object;
	s_unit_state_114ec0 *state = (s_unit_state_114ec0 *)((byte *)unit + unit->state_offset);
	state->tag_index = index;
	state->value4 = value;
}

// @retail 0x114ec0
void function_114ec0(long unit_index, long name)
{
	s_unit_1130a0 *unit = ((s_object_header_1130a0 *)g_4e0300->data)[unit_index & 0xffff].object;
	s_unit_state_114ec0 *state = (s_unit_state_114ec0 *)((byte *)unit + unit->state_offset);
	long model_index = *(long *)(g_4e3b44[unit->definition_index & 0xffff].bytes + 0x38);
	s_model_114ec0 *model = (s_model_114ec0 *)g_4e3b44[model_index & 0xffff].bytes;
	state->tag_index = model->tag_index;
	state->value8 = model->value;
	for (short i = 0; i < model->variant_count; i++)
	{
		s_model_variant_114ec0 *variant = &model->variants[i];
		if (variant->name == name && variant->tag_index != NONE)
		{
			unit_state_tag_set_114ec0(unit_index, variant->tag_index);
			if (variant->value && variant->value != NONE)
				state->value8 = variant->value;
			break;
		}
		if (variant->tag_index != NONE && state->tag_index == NONE)
		{
			unit_state_tag_set_114ec0(unit_index, variant->tag_index);
			if (variant->value && variant->value != NONE)
				state->value8 = variant->value;
		}
	}
}

// @retail 0x114520
bool function_114520(long unit_index, short entry_index, short priority, long *previous, long *tag_index)
{
	s_unit_1130a0 *unit = ((s_object_header_1130a0 *)g_4e0300->data)[unit_index & 0xffff].object;
	byte *state = (byte *)unit + unit->state_offset;
	bool result = false;
	long tag = NONE;
	if (tag_index)
		tag = *tag_index;
	if (tag == NONE && entry_index != NONE)
	{
		function_1144a0(unit_index, entry_index, &tag);
		if (tag == NONE)
			goto done;
	}
	if (!TEST_FIELD_BIT(unit->frozen) || priority == 15)
	{
		short current = *(short *)(state + 0xc);
		if (!current)
			result = true;
		else
		{
			switch (priority)
			{
			case 12:
				if (current >= 10 && !state[0x49])
					result = false;
				else
					result = priority > current;
				break;
			case 13:
			case 14:
				result = priority >= current;
				break;
			default:
				result = priority > current;
				break;
			}
		}
	}
done:
	if (tag_index)
		*tag_index = tag;
	if (previous)
		*previous = *(long *)(state + 0x44);
	return result;
}

void function_20fec0(long object_index, long sound_index);
short function_1145f0(long definition_index, s_sound_permutation_reference const *reference, short type);

// @retail 0x114e80
void function_114e80(long unit_index)
{
	s_unit_1130a0 *unit = ((s_object_header_1130a0 *)g_4e0300->data)[unit_index & 0xffff].object;
	byte *state = (byte *)unit + unit->state_offset;
	long sound_index = *(long *)(state + 0x1c);
	if (sound_index != NONE)
		function_20fec0(unit_index, sound_index);
	*(short *)(state + 0xc) = 0;
}

// @retail 0x114680
void function_114680(long unit_index, void const *request)
{
	s_unit_1130a0 *unit = ((s_object_header_1130a0 *)g_4e0300->data)[unit_index & 0xffff].object;
	byte *state = (byte *)unit + unit->state_offset;
	if (!TEST_FIELD_BIT(unit->frozen) || *(short const *)request == 15)
	{
		if (*(short *)(state + 0xc) > 0)
			function_114e80(unit_index);
		memcpy(state + 0xc, request, 0x30);
		*(short *)(state + 0x50) = *(short *)(state + 0x16);
		*(short *)(state + 0x4e) = *(short *)(state + 0x14);
		state[0x48] = 0;
		state[0x49] = 0;
		state[0x4a] = 0;
		*(long *)(state + 0x54) = NONE;
		*(short *)(state + 0x4c) = function_1145f0(*(long *)(state + 0x10),
			(s_sound_permutation_reference const *)((byte const *)request + 0xc), *(short const *)request);
	}
}
