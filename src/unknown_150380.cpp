// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"

struct s_player_unit_state
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};

struct s_unit_motion_state
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0x3dc - 0x18];
	byte state;
};

struct s_unit_motion_header
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	s_unit_motion_state *unit;
};

bool function_e4050(long object_index);

// @retail 0x150380
byte function_150380(long player_index)
{
	s_player_unit_state *player = (s_player_unit_state *)g_4e8c24->data + (player_index & 0xffff);
	byte result = 0;

	if (player->unit_index != NONE)
	{
		s_unit_motion_header *header = (s_unit_motion_header *)g_4e0300->data + (player->unit_index & 0xffff);
		if ((1 << header->type) & 1)
		{
			s_unit_motion_state *unit = header->unit;
			result = unit->parent_index == NONE && unit->state == 1 && !function_e4050(player->unit_index);
		}
	}
	return result;
}
