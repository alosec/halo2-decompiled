/* CHARACTER_TAG_ACCESSORS.H: the character tag an actor is made from (the tag
   index at +0x54 of the actor, s_actor_datum::character_index). A character names
   the character it inherits from; every block below is inherited: when a
   character's block is empty, its parent's is used. Read through the
   accessors in src/character_tag_accessors.cpp. */

#ifndef CHARACTER_TAG_ACCESSORS_H
#define CHARACTER_TAG_ACCESSORS_H

#include "unknown_11c920.h"
#include "slot_handler.h"
#include <stddef.h>

/* a block of a character tag: its element count and its elements */
struct s_character_block
{
	long count;
	byte *address;
};
typedef char assert_character_block_size[(sizeof(s_character_block) == 8) ? 1 : -1];

/* an element (0xcc bytes) of the weapons block, keyed by the weapon's tag */
struct s_character_weapon_settings
{
	byte unknown00[8];
	long weapon_tag_index;
	byte unknown0c[0xcc - 0xc];
};
typedef char assert_character_weapon_settings_size[(sizeof(s_character_weapon_settings) == 0xcc) ? 1 : -1];

/* an element (0x10 bytes) of the second block keyed by a weapon's tag */
struct s_character_weapon_extra
{
	long unknown00;
	long weapon_tag_index;
	byte unknown08[8];
};
typedef char assert_character_weapon_extra_size[(sizeof(s_character_weapon_extra) == 0x10) ? 1 : -1];

/* an element (0x3c bytes) of the grenades block, keyed by the grenade type
   (the unit's char at +0x23c) */
struct s_character_grenade_settings
{
	byte unknown00[4];
	short grenade_type;
	byte unknown06[0x3c - 0x6];
};
typedef char assert_character_grenade_settings_size[(sizeof(s_character_grenade_settings) == 0x3c) ? 1 : -1];

/* the variant-chosen blocks (variant_blocks[]): the element size of each and
   the index of s_character_variant::block_indices[] that picks its element */
enum
{
	k_character_variant_block_count = 10,
	k_character_situation_entry_size = 0x34
};

/* a character tag */
struct s_character_definition
{
	byte unknown00[8];
	long parent_index;                       /* the character it inherits from, or NONE */
	byte unknown0c[0x34 - 0xc];
	s_character_block block34;               /* starts with flags */
	s_character_block block3c;               /* read by the damage code */
	s_character_block difficulty_values;     /* values per difficulty */
	s_character_block situation_entries;     /* 0x34 bytes each: default, has a target, ... */
	s_character_block looking_settings;
	s_character_block block5c;               /* read by the path transitions */
	/* 0: 0x28 bytes (picked by variant index 10), 1: 0x8 (7), 2: 0x10 (0),
	   3: 0x40 (6), 4: 0x14 (1), 5: 0x40 (2), 6: 0x4c (5), 7: 0x14 (3),
	   8: 0x24 (4), 9: 0xc (8) */
	s_character_block variant_blocks[k_character_variant_block_count];
	s_character_block blockb4;               /* two durations in seconds */
	s_character_block vehicle_settings;
	s_character_block blockc4;
	s_character_block weapons;               /* s_character_weapon_settings */
	s_character_block weapon_extras;         /* s_character_weapon_extra */
	s_character_block grenades;              /* s_character_grenade_settings */
	s_character_block object_entries;        /* s_tag_element, keyed by an object's definition */
};
typedef char assert_character_definition_variant_blocks[(offsetof(s_character_definition, variant_blocks) == 0x64) ? 1 : -1];
typedef char assert_character_definition_weapons[(offsetof(s_character_definition, weapons) == 0xcc) ? 1 : -1];
typedef char assert_character_definition_size[(sizeof(s_character_definition) == 0xec) ? 1 : -1];

/* the character tag of a tag index */
inline s_character_definition *character_definition_get(long character_index)
{
	return (s_character_definition *)g_4e3b44[character_index & 0xffff].bytes;
}

#endif
