// @flags /O2 /Gr
/* UNKNOWN_1F9240.CPP: the path settings of an actor */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "command_scripts.h"
#include "unknown_2729b0.h"
#include "unknown_1f9240.h"

long function_1e4a50(long index);

/* a tag whose block at +0x4c starts with the mask of what may be pathed
   through (the style tags) */
struct s_path_mask_tag
{
	byte unknown00[0x4c];
	long mask_count;
	long *masks;
};

/* the character tag: its style at +0x20 */
struct s_path_character_tag
{
	byte unknown00[0x20];
	long style_tag_index;
};

/* the movement properties of a character (function_1e4a50) */
struct s_path_movement_properties
{
	byte unknown00[0x10];
	short unknown10;
	short unknown12;
	short unknown14;
	short unknown16;
	byte unknown18[2];
	short unknown1a;
	long unknown1c;
};

/* a squad (g_51e9d8) and its scenario definition (0x7c bytes, its masks at
   +0x6c) */
struct s_path_squad_view
{
	byte unknown00[0x2a];
	short definition_index;
	byte unknown2c[0x98 - 0x2c];
};

struct s_path_squad_definition
{
	byte unknown00[0x6c];
	long mask_count;
	long *masks;
	byte unknown74[0x7c - 0x74];
};

struct s_path_scenario_view
{
	byte unknown000[0x240];
	long squad_definition_count;
	s_path_squad_definition *squad_definitions;
};

inline s_path_squad_definition *path_squad_definition_get(short index)
{
	s_path_squad_definition *result = NULL;

	if (index != NONE)
	{
		result = &((s_path_scenario_view *)g_4e0350)->squad_definitions[index];
	}
	else
	{
		result = NULL;
	}
	return result;
}

PRIVATE __forceinline dword function_1f924a(dword arg_0, long const *arg_1)
{
    dword local_0 = arg_0;
    local_0 &= arg_1[0] << 1;
    return local_0;
}

// @retail 0x1f9240
bool function_1f9240(long actor_index, s_path_settings *settings)
{
	s_actor_view *actor = actor_get(actor_index);
	s_path_character_tag *character = (s_path_character_tag *)g_4e3b44[actor->unknown054 & 0xffff].bytes;
	s_path_mask_tag *style = NULL;
	s_path_movement_properties *properties;
	s_command_script *script;
	s_path_mask_tag *script_style;

	if (character->style_tag_index != NONE)
	{
		style = (s_path_mask_tag *)g_4e3b44[character->style_tag_index & 0xffff].bytes;
	}
	if (style)
	{
		if (style->mask_count > 0)
		{
			settings->flags = style->masks[0] << 1;
		}
		else
		{
			settings->flags = 0;
		}
	}
	else
	{
		settings->flags = 0xff;
	}

	if (actor->unknown858 != NONE && (script = command_script_get(actor->unknown858))->flag86 &&
		(script_style = (s_path_mask_tag *)g_4e3b44[script->style88 & 0xffff].bytes)->mask_count > 0)
	{
		s_path_settings *local_0 = settings;
		*(dword volatile *)&local_0->flags = function_1f924a(settings->flags, script_style->masks);
	}
	else if (actor->unknown030 != NONE)
	{
		s_path_squad_view *squad = &((s_path_squad_view *)g_51e9d8->data)[actor->unknown030 & 0xffff];
		s_path_squad_definition *definition = path_squad_definition_get(*(short const volatile *)&squad->definition_index);

		if (definition)
		{
			long *masks;

			if (definition->mask_count > 0)
			{
				masks = definition->masks;
			}
			else
			{
				s_path_mask_tag *variant = (s_path_mask_tag *)function_2729b0((s_2729b0_starting_location *)definition);

				if (!variant || variant->mask_count <= 0)
				{
					goto done;
				}
				masks = variant->masks;
			}
			if (masks)
			{
				settings->flags &= masks[0] << 1;
			}
		}
	}
done:
	settings->flags |= 1;

	properties = (s_path_movement_properties *)function_1e4a50(actor->unknown054);
	settings->unknown0c = false;
	settings->unknown08 = 0;
	settings->unknown04 = 0;
	settings->unknown0e = 0;
	settings->unknown10 = 0;
	settings->unknown12 = 0;
	settings->unknown14 = 0;
	if (actor->unknown266)
	{
		s_tag_element *element = function_1e5450(actor_index, object_get(actor->unknown26c)->tag_index);

		if (element)
		{
			settings->unknown0e = element->unknownb0;
		}
		settings->unknown08 = 1;
	}
	else if (properties)
	{
		if (properties->unknown1a > 0)
		{
			settings->unknown08 = (1 << (properties->unknown1a > 6 ? 6 : properties->unknown1a)) - 1;
			if (properties->unknown1a >= 7)
			{
				settings->unknown0c = true;
			}
		}
		settings->unknown04 = properties->unknown1c;
		settings->unknown0e = properties->unknown14;
		settings->unknown10 = properties->unknown16;
		settings->unknown12 = properties->unknown10;
		settings->unknown14 = properties->unknown12;
	}
	settings->unknown18 = actor->unknown300;
	return true;
}
