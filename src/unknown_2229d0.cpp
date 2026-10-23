// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2229D0.CPP: the gamepad rumble of the local players (g_502120:
   per player 8 playing rumble effects, their timers and a constant level)
   and the lifecycle callbacks of entry 48 */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "globals.h"
#include <string.h>

void rumble_clear_all(void);

s_speed_table *g_502120;

#define PIN(n, floor, ceiling) ((n) < (floor) ? (floor) : ((n) > (ceiling) ? (ceiling) : (n)))

/* the two motor levels of a gamepad */
struct s_rumble_state
{
	word left;
	word right;
};

/* the input globals (unknown_1248b0.cpp): the rumble levels at +0x19c */
struct s_input_globals;
extern s_input_globals g_4e61b8;

struct s_input_globals_rumble_view
{
	byte unknown000[0x19c];
	s_rumble_state rumble[4];
};

/* a rumble effect definition: 2 motors */
struct s_rumble_motor
{
	real duration;
	byte function[8];
};

struct s_rumble_effect
{
	byte unknown00[0x24];
	s_rumble_motor motors[2];
	byte unknown3c[0x4c - 0x3c];
};

struct s_rumble_definition
{
	byte unknown00[0x6c];
	long effect_count;
	s_rumble_effect *effects;
};

struct s_rumble_player
{
	byte unknown00[0x24];
	long gamepad_index;
};

bool g_509340;

real function_13b390(void const *function, real input, real scale);
void function_124a40(short gamepad_index, word left, word right);

/* function_124a40 (unknown_1248b0.cpp, built /Ob1), as retail
   inlines it here */
PRIVATE inline void input_set_gamepad_rumbler_state_inline(short gamepad_index, word left, word right)
{
	bool enabled = true;

	if (TEST_FIELD_BIT(g_54e8e0[gamepad_index].flag4))
		enabled = !TEST_FIELD_BIT(g_54e8e0[gamepad_index].settings.vibration_disabled);

	if (enabled)
	{
		((s_input_globals_rumble_view *)&g_4e61b8)->rumble[gamepad_index].left = left;
		((s_input_globals_rumble_view *)&g_4e61b8)->rumble[gamepad_index].right = right;
	}
	else
	{
		((s_input_globals_rumble_view *)&g_4e61b8)->rumble[gamepad_index].left = 0;
		((s_input_globals_rumble_view *)&g_4e61b8)->rumble[gamepad_index].right = 0;
	}
}

/* function_68290 (unknown_067e10.cpp, built /Ob1), as retail inlines it
   here */
PRIVATE inline bool simulation_world_is_remote(void)
{
	bool result = false;

	if (g_4cf770 && !g_4cf772)
	{
		long *world = (long *)g_4cf77c;

		if (world[2])
			result = world[6] != 4;
	}
	return result;
}

PRIVATE __forceinline word real_to_word_round(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return (word)result;
}

// @retail 0x2229d0
void function_2229d0(void)
{
	g_502120 = (s_speed_table *)function_123d40("unknown", "unknown", sizeof(s_speed_table));
}

// @retail 0x222a10
void function_222a10(void)
{
	s_speed_table *data = g_502120;

	memset(data, 0, sizeof(*data));
	for (long i = 0; i < 4; i++)
	{
		for (long j = 0; j < 8; j++)
		{
			data->entries[i].items[j].a = NONE;
			data->entries[i].items[j].b = NONE;
			data->entries[i].items[j].scale = 1.0f;
		}
	}
}

// @retail 0x222a60
void function_222a60(void)
{
	rumble_clear_all();
}

// @retail 0x222d80
void rumble_clear_all(void)
{
	s_speed_table *data = g_502120;
	long gamepad_index;

	memset(data, 0, sizeof(*data));
	for (long i = 0; i < 4; i++)
	{
		for (long j = 0; j < 8; j++)
		{
			data->entries[i].items[j].a = NONE;
			data->entries[i].items[j].b = NONE;
			data->entries[i].items[j].scale = 1.0f;
		}
	}
	for (gamepad_index = 0; gamepad_index < 4; gamepad_index++)
		input_set_gamepad_rumbler_state_inline(gamepad_index, 0, 0);
}

// @retail 0x222cb0
void rumble_player_play_effect(
	long player_index,
	long definition_index,
	long effect_index,
	real scale)
{
	if (definition_index != NONE && effect_index != NONE)
	{
		s_speed_table_entry *entry = &g_502120->entries[player_index];
		long oldest = 0;
		real oldest_time = entry->timers[0];
		long i;

		for (i = 1; i < 8; i++)
		{
			if (entry->timers[i] > oldest_time)
			{
				oldest = i;
				oldest_time = entry->timers[i];
			}
		}
		entry->items[oldest].scale = scale;
		entry->items[oldest].a = definition_index;
		entry->items[oldest].b = effect_index;
		entry->timers[oldest] = 0.0f;
	}
}

PRIVATE __forceinline void function_222e91(real *arg_0, real arg_1)
{
	if (0.0f > arg_1)
		*(real volatile *)arg_0 = 0.0f;
	else
	{
		*(real volatile *)arg_0 = 1.0f;
		if (!(arg_1 > 1.0f))
			*(real volatile *)arg_0 = arg_1;
	}
}

// @retail 0x222e90
s_rumble_state rumble_player_evaluate(
	s_speed_table_entry *entry)
{
	s_rumble_state result;
	real motors[2];
	long i;

	motors[0] = entry->value80;
	motors[1] = entry->value84;
	for (i = 0; i < 8; i++)
	{
		s_speed_table_item *item = &entry->items[i];

		if (item->a != NONE && item->b != NONE)
		{
			s_rumble_definition *definition = (s_rumble_definition *)g_4e3b44[item->a & 0xffff].bytes;
			real time = entry->timers[i];

			if (item->b >= 0 && item->b < definition->effect_count)
			{
				s_rumble_effect *effect = &definition->effects[item->b];
				long motor_index;

				for (motor_index = 0; motor_index < 2; motor_index++)
				{
					s_rumble_motor *motor = &effect->motors[motor_index];

					if (motor->duration > time)
					{
						real fraction = time / motor->duration;

						function_222e91(&fraction, fraction);
						motors[motor_index] += function_13b390(motor->function, fraction, 1.0f) * item->scale;
					}
				}
			}
		}
	}
	if (g_502120->value228 != 0.0f)
	{
		motors[0] += g_502120->value220 * g_502120->value228;
		motors[1] += g_502120->value224 * g_502120->value228;
	}
	motors[0] *= 65535.0f;
	result.left = real_to_word_round(PIN(motors[0], 0.0f, 65535.0f));
	motors[1] *= 65535.0f;
	result.right = real_to_word_round(PIN(motors[1], 0.0f, 65535.0f));
	return result;
}

// @retail 0x222a70
void function_222a70(
	real seconds)
{
	bool updated[4];
	memset(updated, 0, sizeof(updated));
	long gamepad_index;

	if (!g_509340 && !simulation_world_is_remote())
	{
		long i = 0;
		real *local_0 = g_502120->entries[0].timers + 1;
		long const *local_1 = g_4e8c20->entries;

		for (; i < 4; i++, local_1++, local_0 += sizeof(s_speed_table_entry) / sizeof(real))
		{
			s_speed_table_entry *entry = (s_speed_table_entry *)((byte *)local_0 - 0x64);
			s_rumble_state state = rumble_player_evaluate(entry);
			long player_index = NONE;
			if (i != NONE)
				player_index = *local_1;
			long j;

			for (j = 0; j < 8; j++)
				local_0[j - 1] = j == 1 ? seconds + local_0[j - 1] : local_0[j - 1] + seconds;

			if (player_index != NONE)
			{
				s_rumble_player *player = (s_rumble_player *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);

				if (player->gamepad_index != NONE)
				{
					bool enabled = true;

					if (TEST_FIELD_BIT(g_54e8e0[player->gamepad_index].flag4))
						enabled = !TEST_FIELD_BIT(g_54e8e0[player->gamepad_index].settings.vibration_disabled);
					if (enabled)
					{
						function_124a40((short)player->gamepad_index, state.left, state.right);
						updated[player->gamepad_index] = true;
					}
				}
			}
		}
	}
	for (gamepad_index = 0; gamepad_index < 4; gamepad_index++)
	{
		if (!updated[gamepad_index])
			input_set_gamepad_rumbler_state_inline(gamepad_index, 0, 0);
	}
}
