// @flags /O2 /Gr
/* CHARACTER_TAG_ACCESSORS.CPP: the accessors of an actor's character tag
   (include/character_tag_accessors.h). Each walks the character and the
   characters it inherits from, and returns the first non-empty block's
   element: the first, the one the actor's variant picks, or the one keyed
   by a weapon, grenade type or object definition */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "ai_actor.h"
#include "unknown_2729b0.h"
#include "squads.h"
#include "character_tag_accessors.h"

void *character_grenade_settings_get(long character_index, short grenade_type);

/* the elements of the character's first block (+0x34; its first byte holds flags) */
// @retail 0x1e4990
long function_1e4990(long character_index)
{
	long result = 0;
	if (character_index != NONE)
	{
		do
		{
			s_character_definition *character = character_definition_get(character_index);
			if (character->block34.count > 0)
			{
				result = (long)character->block34.address;
				break;
			}
			character_index = character->parent_index;
		} while (character_index != NONE);
	}
	return result;
}

/* the character's values per difficulty */
// @retail 0x1e49d0
long character_get_difficulty_values(long character_index)
{
	long result = 0;
	if (character_index != NONE)
	{
		do
		{
			s_character_definition *character = character_definition_get(character_index);
			if (character->difficulty_values.count > 0)
			{
				result = (long)character->difficulty_values.address;
				break;
			}
			character_index = character->parent_index;
		} while (character_index != NONE);
	}
	return result;
}

/* the elements of the character's block at +0x3c (read by the damage code) */
// @retail 0x1e4a10
long function_1e4a10(long character_index)
{
	long result = 0;
	if (character_index != NONE)
	{
		do
		{
			s_character_definition *character = character_definition_get(character_index);
			if (character->block3c.count > 0)
			{
				result = (long)character->block3c.address;
				break;
			}
			character_index = character->parent_index;
		} while (character_index != NONE);
	}
	return result;
}

/* the elements of the character's block at +0x5c (read by the path transitions) */
// @retail 0x1e4a50
long function_1e4a50(long character_index)
{
	long result = 0;
	if (character_index != NONE)
	{
		do
		{
			s_character_definition *character = character_definition_get(character_index);
			if (character->block5c.count > 0)
			{
				result = (long)character->block5c.address;
				break;
			}
			character_index = character->parent_index;
		} while (character_index != NONE);
	}
	return result;
}

/* the elements of the character's block at +0xb4 (two durations in seconds) */
// @retail 0x1e4a90
long function_1e4a90(long character_index)
{
	long result = 0;
	if (character_index != NONE)
	{
		do
		{
			s_character_definition *character = character_definition_get(character_index);
			if (character->blockb4.count > 0)
			{
				result = (long)character->blockb4.address;
				break;
			}
			character_index = character->parent_index;
		} while (character_index != NONE);
	}
	return result;
}

/* the character's vehicle properties */
// @retail 0x1e4ad0
long character_vehicle_settings_get(long character_index)
{
	long result = 0;
	if (character_index != NONE)
	{
		do
		{
			s_character_definition *character = character_definition_get(character_index);
			if (character->vehicle_settings.count > 0)
			{
				result = (long)character->vehicle_settings.address;
				break;
			}
			character_index = character->parent_index;
		} while (character_index != NONE);
	}
	return result;
}

/* the elements of the character's block at +0xc4 */
// @retail 0x1e4b10
long function_1e4b10(long character_index)
{
	long result = 0;
	if (character_index != NONE)
	{
		do
		{
			s_character_definition *character = character_definition_get(character_index);
			if (character->blockc4.count > 0)
			{
				result = (long)character->blockc4.address;
				break;
			}
			character_index = character->parent_index;
		} while (character_index != NONE);
	}
	return result;
}

/* the actor's element of its character's variant block 0: the one its
   variant picks (block index 10), else the default one, else the first */
// @retail 0x1e4b50
void *function_1e4b50(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[10])
				index = variant->block_indices[10] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[0].count > 0)
		{
			if (index < character->variant_blocks[0].count)
				result = character->variant_blocks[0].address + index * 0x28;
			else
				result = character->variant_blocks[0].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the actor's element of its character's variant block 2: the one its
   variant picks (block index 0), else the default one, else the first */
// @retail 0x1e4be0
void *function_1e4be0(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[0])
				index = variant->block_indices[0] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[2].count > 0)
		{
			if (index < character->variant_blocks[2].count)
				result = character->variant_blocks[2].address + index * 0x10;
			else
				result = character->variant_blocks[2].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the actor's element of its character's variant block 4: the one its
   variant picks (block index 1), else the default one, else the first */
// @retail 0x1e4c70
void *function_1e4c70(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[1])
				index = variant->block_indices[1] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[4].count > 0)
		{
			if (index < character->variant_blocks[4].count)
				result = character->variant_blocks[4].address + index * 0x14;
			else
				result = character->variant_blocks[4].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the actor's element of its character's variant block 5: the one its
   variant picks (block index 2), else the default one, else the first */
// @retail 0x1e4d10
void *function_1e4d10(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[2])
				index = variant->block_indices[2] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[5].count > 0)
		{
			if (index < character->variant_blocks[5].count)
				result = character->variant_blocks[5].address + index * 0x40;
			else
				result = character->variant_blocks[5].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the actor's element of its character's variant block 7: the one its
   variant picks (block index 3), else the default one, else the first */
// @retail 0x1e4db0
void *function_1e4db0(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[3])
				index = variant->block_indices[3] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[7].count > 0)
		{
			if (index < character->variant_blocks[7].count)
				result = character->variant_blocks[7].address + index * 0x14;
			else
				result = character->variant_blocks[7].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the actor's element of its character's variant block 8: the one its
   variant picks (block index 4), else the default one, else the first */
// @retail 0x1e4e50
void *function_1e4e50(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[4])
				index = variant->block_indices[4] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[8].count > 0)
		{
			if (index < character->variant_blocks[8].count)
				result = character->variant_blocks[8].address + index * 0x24;
			else
				result = character->variant_blocks[8].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the actor's element of its character's variant block 6: the one its
   variant picks (block index 5), else the default one, else the first */
// @retail 0x1e4ef0
void *function_1e4ef0(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[5])
				index = variant->block_indices[5] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[6].count > 0)
		{
			if (index < character->variant_blocks[6].count)
				result = character->variant_blocks[6].address + index * 0x4c;
			else
				result = character->variant_blocks[6].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the actor's element of its character's variant block 3: the one its
   variant picks (block index 6), else the default one, else the first */
// @retail 0x1e4f90
void *function_1e4f90(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[6])
				index = variant->block_indices[6] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[3].count > 0)
		{
			if (index < character->variant_blocks[3].count)
				result = character->variant_blocks[3].address + index * 0x40;
			else
				result = character->variant_blocks[3].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the actor's element of its character's variant block 1: the one its
   variant picks (block index 7), else the default one, else the first */
// @retail 0x1e5030
void *function_1e5030(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[7])
				index = variant->block_indices[7] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[1].count > 0)
		{
			if (index < character->variant_blocks[1].count)
				result = character->variant_blocks[1].address + index * 0x8;
			else
				result = character->variant_blocks[1].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the actor's element of its character's variant block 9: the one its
   variant picks (block index 8), else the default one, else the first */
// @retail 0x1e50c0
void *function_1e50c0(long actor_index)
{
	long character_index = actor_datum_get(actor_index)->character_index;
	void *result = 0;

	while (character_index != NONE)
	{
		short index = 0;
		s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
		if (variant)
		{
			if (variant->block_indices[8])
				index = variant->block_indices[8] - 1;
			else
				index = variant->default_index;
		}

		s_character_definition *character = character_definition_get(character_index);
		if (character->variant_blocks[9].count > 0)
		{
			if (index < character->variant_blocks[9].count)
				result = character->variant_blocks[9].address + index * 0xc;
			else
				result = character->variant_blocks[9].address;
			break;
		}
		character_index = character->parent_index;
	}

	return result;
}

/* the element of the actor's character's second weapon-keyed block for a
   weapon tag */
// @retail 0x1e5300
void *actor_get_character_weapon_extra(long actor_index, long weapon_tag_index)
{
	void *result = NULL;
	long character_index = actor_datum_get(actor_index)->character_index;
	while (character_index != NONE)
	{
		s_character_definition *character = character_definition_get(character_index);
		short i = 0;
		while (i < character->weapon_extras.count)
		{
			s_character_weapon_extra *entry = &((s_character_weapon_extra *)character->weapon_extras.address)[i];
			if (entry->weapon_tag_index == weapon_tag_index)
			{
				result = entry;
				goto done;
			}
			i++;
		}
		character_index = character->parent_index;
	}
done:
	return result;
}

/* the actor's character's properties for the weapon it holds */
// @retail 0x1e5240
void *function_1e5240(long actor_index)
{
	void *result = NULL;
	long weapon_index = function_1e1f20(actor_index);

	if (weapon_index != NONE)
		result = function_1e5280(actor_index, ai_object_get(weapon_index)->definition_index);
	return result;
}

/* the actor's character's properties for a weapon tag */
// @retail 0x1e5280
void *function_1e5280(long actor_index, long weapon_tag_index)
{
	void *result = NULL;
	long character_index = actor_datum_get(actor_index)->character_index;

	while (character_index != NONE)
	{
		s_character_definition *character = character_definition_get(character_index);
		short i = 0;

		while (i < character->weapons.count)
		{
			s_character_weapon_settings *entry = &((s_character_weapon_settings *)character->weapons.address)[i];
			if (entry->weapon_tag_index == weapon_tag_index)
			{
				result = entry;
				goto done;
			}
			i++;
		}
		character_index = character->parent_index;
	}
done:
	return result;
}

/* the actor's character's properties for its unit's grenade type */
// @retail 0x1e5380
void *function_1e5380(long actor_index)
{
	void *result = NULL;
	s_actor_datum *actor = actor_datum_get(actor_index);
	long unit_index = actor->unit_index;

	if (unit_index != NONE)
	{
		short grenade_type = ai_object_get(unit_index)->unknown23c;
		if (grenade_type != NONE)
			result = character_grenade_settings_get(actor->character_index, grenade_type);
	}
	return result;
}

/* a character's properties for a grenade type */
// @retail 0x1e53e0
void *character_grenade_settings_get(long character_index, short grenade_type)
{
	void *result = NULL;

	while (character_index != NONE)
	{
		s_character_definition *character = character_definition_get(character_index);
		short i = 0;

		while (i < character->grenades.count)
		{
			s_character_grenade_settings *entry = &((s_character_grenade_settings *)character->grenades.address)[i];
			if (entry->grenade_type == grenade_type)
			{
				result = entry;
				goto done;
			}
			i++;
		}
		character_index = character->parent_index;
	}
done:
	return result;
}

struct s_actor_looking_properties;

/* a character's looking properties */
// @retail 0x1e5160
s_actor_looking_properties *function_1e5160(long character_index)
{
	s_actor_looking_properties *result = NULL;
	if (character_index != NONE)
	{
		do
		{
			s_character_definition *character = character_definition_get(character_index);
			if (character->looking_settings.count > 0)
			{
				result = (s_actor_looking_properties *)character->looking_settings.address;
				break;
			}
			character_index = character->parent_index;
		} while (character_index != NONE);
	}
	return result;
}

/* the actor's situation entry of its character: the second when it has a
   target (+0x26c), the third at +0x86 level 4 or more, the fourth at level
   1 or less, else the first */
// @retail 0x1e51a0
void *actor_get_character_situation_entry(long actor_index)
{
	s_actor_datum *actor = actor_datum_get(actor_index);
	long character_index = actor->character_index;
	void *result = NULL;
	while (character_index != NONE)
	{
		s_character_definition *character = character_definition_get(character_index);
		if (character->situation_entries.count > 1 && actor->unknown26c != NONE)
		{
			result = character->situation_entries.address + 1 * k_character_situation_entry_size;
			break;
		}
		if (character->situation_entries.count > 2 && actor->value086 >= 4)
		{
			result = character->situation_entries.address + 2 * k_character_situation_entry_size;
			break;
		}
		if (character->situation_entries.count > 3 && actor->value086 <= 1)
		{
			result = character->situation_entries.address + 3 * k_character_situation_entry_size;
			break;
		}
		if (character->situation_entries.count > 0)
		{
			result = character->situation_entries.address;
			break;
		}
		character_index = character->parent_index;
	}
	return result;
}

/* the actor's character's element for an object definition (the tag index of
   the object's definition) */
// @retail 0x1e5450
s_tag_element *function_1e5450(long actor_index, long definition_index)
{
	s_tag_element *result = NULL;
	long character_index = actor_datum_get(actor_index)->character_index;

	while (character_index != NONE)
	{
		s_character_definition *character = character_definition_get(character_index);
		short i = 0;

		while (i < character->object_entries.count)
		{
			s_tag_element *element = &((s_tag_element *)character->object_entries.address)[i];
			if (element->key == definition_index)
			{
				result = element;
				goto done;
			}
			i++;
		}

		character_index = character->parent_index;
	}

done:
	return result;
}
