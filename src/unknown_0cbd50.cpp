// @flags /O2 /Gr
/* UNKNOWN_0CBD50.CPP: unit weapon queries (lane T decompiled them for
   unknown_165ce5.cpp, which calls them with register arguments) */

#include "unknown_11c920.h"
#include "globals.h"

/* the unit fields read here */
struct s_unit_weapons_view
{
	byte unknown000[0x212];
	char weapon_slots[2];
	byte unknown214[4];
	long weapon_object_indices[4];
};

struct s_unit_weapons_header
{
	byte unknown00[8];
	s_unit_weapons_view *unit;
};

static inline s_unit_weapons_view *unit_weapons_get(long unit_index)
{
	return ((s_unit_weapons_header *)g_4e0300->data)[unit_index & 0xffff].unit;
}

// @retail 0xcbd50
long function_cbd50(long unit_index, short weapon_index)
{
	long result = NONE;

	if (weapon_index != NONE)
	{
		result = unit_weapons_get(unit_index)->weapon_object_indices[weapon_index];
	}
	return result;
}

/* 0xcd660 is in unknown_0cd660.cpp: retail calls it out of line from
   0x101690, which needs an /Ob1 file */

/* the weapon fields read here */
struct s_weapon_flags_view
{
	byte unknown000[0x16c];
	word unknown_bit0 : 1;
	word unknown_bit1 : 1;
	word unknown_bit2 : 1;
	word unknown_bits3 : 13;
};

struct s_weapon_flags_header
{
	byte unknown00[8];
	s_weapon_flags_view *weapon;
};

// @retail 0xee8a0
bool function_ee8a0(long unit_index, long field_x11c898)
{
	bool result = false;
	s_unit_weapons_view *unit = unit_weapons_get(unit_index);
	short weapon_index = unit->weapon_slots[field_x11c898];

	if (weapon_index != NONE)
	{
		long weapon_object_index = unit->weapon_object_indices[weapon_index];

		if (weapon_object_index != NONE)
		{
			/* retail rereads the pool's data field for the weapon lookup */
			s_weapon_flags_view *weapon = ((s_weapon_flags_header *)((s_record_pool volatile *)g_4e0300)->data)[weapon_object_index & 0xffff].weapon;

			result = TEST_FIELD_BIT(weapon->unknown_bit1) && !TEST_FIELD_BIT(weapon->unknown_bit2);
		}
	}
	return result;
}