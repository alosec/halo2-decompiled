// @flags /O1 /Oi /Gr
/* PLAYER_APPEARANCE_NAMES.CPP: the string ids of the names of a player
   profile's appearance choices: the player colors, the emblem list's
   entries, and the emblem foregrounds and backgrounds (the profile settings'
   bytes at +0x11d and +0x11e, as the multiplayer settings list shows them) */

#include "unknown_11c920.h"

/* a player color, passed by value */
struct s_player_color
{
	char index;
};

/* the name of one of the 18 player colors */
// @retail 0x14986f
long player_color_get_string_id(s_player_color color)
{
	long string_ids[18] =
	{
		0x100030a, 0x100030b, 0x100030c, 0x100030d, 0x100030e, 0x100030f, 0x1000310, 0x1000311, 0x1000312,
		0x1000313, 0x2000314, 0x2000315, 0x2000316, 0x2000317, 0x2000318, 0x2000319, 0x200031a, 0x200031b
	};
	long result = NONE;

	if (color.index != NONE && color.index >= NONE && color.index < 18)
	{
		result = string_ids[color.index];
	}
	return result;
}

/* the name of an entry of the emblem list (64; the first 18 are the player
   colors' names) */
// @retail 0x14990f
long player_emblem_list_get_string_id(byte index)
{
	long string_ids[64] =
	{
		0x100030a, 0x100030b, 0x100030c, 0x100030d, 0x100030e, 0x100030f, 0x1000310, 0x1000311,
		0x1000312, 0x1000313, 0x2000314, 0x2000315, 0x2000316, 0x2000317, 0x2000318, 0x2000319,
		0x200031a, 0x200031b, 0x200031c, 0x200031d, 0x200031e, 0x200031f, 0x2000320, 0x2000321,
		0x2000322, 0x2000323, 0x2000324, 0x2000325, 0x2000326, 0x2000327, 0x2000328, 0x2000329,
		0x200032a, 0x200032b, 0x200032c, 0x200032d, 0x200032e, 0x200032f, 0x2000330, 0x2000331,
		0x2000332, 0x2000333, 0x2000334, 0x2000335, 0x2000336, 0x2000337, 0x2000338, 0x2000339,
		0x200033a, 0x200033b, 0x200033c, 0x200033d, 0x200033e, 0x200033f, 0x2000340, 0x2000341,
		0x2000342, 0x2000343, 0x2000344, 0x2000345, 0x2000346, 0x2000347, 0x2000348, 0x2000349
	};
	byte const *index_reference = &index;
	long result = NONE;

	if ((*index_reference > 0x3f ? 0x3f : *index_reference) == *index_reference)
	{
		result = string_ids[*index_reference];
	}
	return result;
}

/* the name of one of the 64 emblem foregrounds */
// @retail 0x149b5a
long player_emblem_foreground_get_string_id(byte index)
{
	long string_ids[64] =
	{
		0xb00036f, 0xb000370, 0xb000371, 0xb000372, 0xb000373, 0xb000374, 0xb000375, 0xb000376,
		0xb000377, 0xb000378, 0xc000379, 0xc00037a, 0xc00037b, 0xc00037c, 0xc00037d, 0xc00037e,
		0xc00037f, 0xc000380, 0xc000381, 0xc000382, 0xc000383, 0xc000384, 0xc000385, 0xc000386,
		0xc000387, 0xc000388, 0xc000389, 0xc00038a, 0xc00038b, 0xc00038c, 0xc00038d, 0xc00038e,
		0xc00038f, 0xc000390, 0xc000391, 0xc000392, 0xc000393, 0xc000394, 0xc000395, 0xc000396,
		0xc000397, 0xc000398, 0xc000399, 0xc00039a, 0xc00039b, 0xc00039c, 0xc00039d, 0xc00039e,
		0xc00039f, 0xc0003a0, 0xc0003a1, 0xc0003a2, 0xc0003a3, 0xc0003a4, 0xc0003a5, 0xc0003a6,
		0xc0003a7, 0xc0003a8, 0xc0003a9, 0xc0003aa, 0xc0003ab, 0xc0003ac, 0xc0003ad, 0xc0003ae
	};
	byte const *index_reference = &index;
	long result = NONE;

	if ((*index_reference > 0x3f ? 0x3f : *index_reference) == *index_reference)
	{
		result = string_ids[*index_reference];
	}
	return result;
}

/* the name of one of the 32 emblem backgrounds (the index is checked
   against 64) */
// @retail 0x149da5
long player_emblem_background_get_string_id(byte index)
{
	long string_ids[32] =
	{
		0xb0003af, 0xb0003b0, 0xb0003b1, 0xb0003b2, 0xb0003b3, 0xb0003b4, 0xb0003b5, 0xb0003b6,
		0xb0003b7, 0xb0003b8, 0xc0003b9, 0xc0003ba, 0xc0003bb, 0xc0003bc, 0xc0003bd, 0xc0003be,
		0xc0003bf, 0xc0003c0, 0xc0003c1, 0xc0003c2, 0xc0003c3, 0xc0003c4, 0xc0003c5, 0xc0003c6,
		0xc0003c7, 0xc0003c8, 0xc0003c9, 0xc0003ca, 0xc0003cb, 0xc0003cc, 0xc0003cd, 0xc0003ce
	};
	byte const *index_reference = &index;
	long result = NONE;

	if ((*index_reference > 0x3f ? 0x3f : *index_reference) == *index_reference)
	{
		result = string_ids[*index_reference];
	}
	return result;
}
