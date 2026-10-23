// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"

struct s_effect_player
{
    byte unknown00[0x2c];
    long unit_index;
    byte unknown30[0x198 - 0x30];
    short effect_timers[2];
    byte unknown19c[0x21c - 0x19c];
};

struct s_effect_unit
{
    byte unknown00[0x134];
    dword flags134;
};

struct s_effect_object_header
{
    byte unknown00[8];
    s_effect_unit *object;
};

static inline s_effect_player *effect_player_get(long player_index)
{
    return (s_effect_player *)g_4e8c24->data + (player_index & 0xffff);
}

static inline s_effect_unit *effect_unit_get(long object_index)
{
    return ((s_effect_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

void function_152240(long player_index, short effect);
void function_1522b0(long player_index, short effect);

// @retail 0x152000
bool function_152000(long player_index, short effect, short ticks)
{
	short const *ticks_reference = &ticks;
	s_effect_player *player = effect_player_get(player_index);
	if (effect == 0)
	{
		s_effect_unit *unit = (s_effect_unit *)effect_unit_get(player->unit_index);
		if ((bool)((unit->flags134 >> 3) & 1))
			return false;
	}
	short *timer = &player->effect_timers[effect];
	if (*timer == 0)
		function_152240(player_index, effect);
	else if (g_4e6948->state != 2)
		function_1522b0(player_index, effect);
	*timer += *ticks_reference;
	return true;
}

// @retail 0x1520a0
void function_1520a0(long player_index, short effect, short ticks)
{
	s_effect_player *player = effect_player_get(player_index);
	if (player->effect_timers[effect] == 0)
		function_152240(player_index, effect);
	player->effect_timers[effect] = player->effect_timers[effect] > ticks ? player->effect_timers[effect] : ticks;
}
