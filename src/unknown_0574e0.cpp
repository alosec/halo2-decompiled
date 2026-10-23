// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0574E0.CPP: the voice observer (lane D): the radio sounds played
   when a player starts or stops talking, and the players' talking times */

#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <xonline.h>
#include <xhv.h>
#include "network_voice.h"
#include "unknown_0662e0.h"
#include "local_cameras.h"
#include "unknown_1428b0.h"
#include <math.h>

/* a sound to play by label (unknown_189010.cpp) */
struct s_sound_label_play
{
	long label;
	long tag_index;
	real scale;
	char const *variant;
};

long function_189760(s_sound_label_play const *play);
long function_1896c0(real scale, long tag_index);

/* network_voice.cpp: the controller of a session player, or NONE */
long voice_get_player_unknown14(long index);

/* unknown_190001.cpp */
bool function_1906da(long index);

/* the radio effects of the multiplayer globals (8 bytes each) */
struct s_voice_radio_effect
{
	long unknown0;
	long sound_index;
};

struct s_voice_radio_effects
{
	byte unknown00[0x90];
	long count;
	s_voice_radio_effect *effects;
};

class c_voice_observer
{
public:
	void play_sound(long player_index, long tag_index);
	void play_radio_effect(long player_index, long radio_effect_index);

	byte unknown000[0x44];
	dword local_masks[16];
	byte unknown084[0x148 - 0x84];
	long recent_times[16];
	long distance_volume;
	long directional_volume;
	bool linear_gain;
	byte unknown191[3];
	long talk_times[16];
	long last_talk_times[16];
};

static inline bool voice_available(void)
{
	return g_4c9878.initialized && g_476fc8.initialized;
}

// @retail 0x57530
void c_voice_observer::play_sound(long player_index, long tag_index)
{
	long controller_index = voice_get_player_unknown14(player_index);

	if (controller_index != NONE)
	{
		long state = 0;
		if (voice_available())
			state = g_4c9878.port_states[controller_index];
		long mask = 0;
		if (voice_available())
			mask = g_4c9878.unknownEE;
		if (state != 3 && function_1906da(controller_index))
		{
			if (state != 1 && (mask & (1 << player_index)))
			{
				long label;
				long index = voice_get_player_unknown14(player_index);
				if (index == NONE)
					return;
				switch (index)
				{
				case 0:
					label = 0x14000140;
					break;
				case 1:
					label = 0x14000141;
					break;
				case 2:
					label = 0x14000142;
					break;
				case 3:
					label = 0x14000143;
					break;
				default:
					__assume(0);
				}
				s_sound_label_play play;
				play.tag_index = tag_index;
				play.scale = 1.0f;
				play.variant = NULL;
				play.label = label;
				function_189760(&play);
			}
			else
			{
				function_1896c0(1.0f, tag_index);
			}
		}
	}
}

// @retail 0x574e0
void c_voice_observer::play_radio_effect(long player_index, long radio_effect_index)
{
	s_voice_radio_effects *effects = *(s_voice_radio_effects **)(g_4e3b44[g_4e034c->index & 0xffff].bytes + 0xc);

	if (radio_effect_index < effects->count)
	{
		long sound_index = effects->effects[radio_effect_index].sound_index;
		if (sound_index != NONE)
			play_sound(player_index, sound_index);
	}
}

/* whether the player talked within the configured time */
// @retail 0x57790
bool voice_observer_talked_recently(c_voice_observer *observer, long player_index)
{
	long time = observer->talk_times[player_index];

	if (time)
	{
		s_game_time_globals *game_time = g_510c54;
		real seconds = (real)(dword)(game_time->game_time - time) * game_time->rate;
		if (g_network_configuration.real16f8 > seconds)
			return true;
	}
	return false;
}

struct s_input_state
{
	byte unknown00[0x10];
	byte values[0x38];
};

extern byte g_4e61b9;
extern s_input_state g_4e61dc[3];
extern s_input_state g_4e630c;
bool function_589e0(long player_index);
bool function_53750(long player);

// @retail 0x57620
void function_57620(c_voice_observer *observer, long player, short controller)
{
	short const *controller_reference = &controller;
	s_input_state *input = 0;
	if (g_4e61cc[*controller_reference])
		input = g_4e61b9 ? &g_4e630c : &g_4e61dc[*controller_reference];
	if (input)
	{
		bool unavailable = !function_589e0(player);
		long start = observer->talk_times[player];
		long now = g_510c54->game_time;
		bool started = start != 0;
		bool pressed = input->values[5] > 0 || input->values[8] > 0;
		if (!started)
		{
			if (pressed && !unavailable)
			{
				observer->play_radio_effect(player, 0);
				observer->talk_times[player] = now;
				observer->last_talk_times[player] = now;
			}
		}
		else if ((real)(now - start) * g_510c54->rate >= g_network_configuration.real16f8 || unavailable)
		{
			if (observer->last_talk_times[player])
			{
				observer->play_radio_effect(player, 1);
				observer->last_talk_times[player] = 0;
			}
			if (!pressed)
				observer->talk_times[player] = 0;
		}
		else if (pressed || function_53750(player))
			observer->last_talk_times[player] = now;
		else if ((real)(now - observer->last_talk_times[player]) * g_510c54->rate >= g_network_configuration.real16f4)
		{
			observer->play_radio_effect(player, 1);
			observer->talk_times[player] = 0;
			observer->last_talk_times[player] = 0;
		}
	}
}

void *voice_get_membership(void);
dword voice_get_local_player_mask(void);
bool function_54df0(long player_index);
bool function_57270(long player, long other);
bool voice_test_unknownF0(long port, long bit);

// @retail 0x56f60
void __stdcall function_56f60(c_voice_observer *observer)
{
	dword mask = 0;
	if (voice_available())
	{
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			mask = *(dword *)(membership + 0x10d0);
	}
	dword local_mask = voice_get_local_player_mask();
	for (long player = 0; player < 16; player++)
	{
		if ((mask & (1 << player)) && !(local_mask & (1 << player)) &&
			function_54df0(player) && !function_589e0(player) &&
			0.3f > (g_510c54->game_time - observer->recent_times[player]) * g_510c54->rate)
		{
			for (long local = 0; local < 16; local++)
			{
				if ((mask & (1 << local)) && (local_mask & (1 << local)) &&
					function_54df0(local) && function_57270(local, player) &&
					!voice_test_unknownF0(local, player) && !voice_test_unknownF0(player, local))
					observer->play_radio_effect(local, 2);
			}
			observer->recent_times[player] = 0;
		}
	}
}

#include <string.h>

byte *voice_get_world_player(long player_index);
bool function_147da5(long controller_id);

// @retail 0x57360
void __stdcall function_57360(c_voice_observer *observer)
{
	dword local_mask = voice_get_local_player_mask();
	byte *players = 0;
	if (voice_available())
	{
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			players = membership + 0x10d4;
	}
	memset(observer->local_masks, 0, sizeof(observer->local_masks));
	if (players)
	{
		dword mask = 0;
		if (voice_available())
		{
			byte *membership = (byte *)voice_get_membership();
			if (membership)
				mask = *(dword *)(membership + 0x10d0);
		}
		dword enabled = 0;
		if (voice_available())
			enabled = g_4c9878.unknownEE;
		for (long local = 0; local < 16; local++)
		{
			if ((local_mask & (1 << local)) && (enabled & (1 << local)) && function_54df0(local))
			{
				byte *player = voice_get_world_player(local);
				if (player)
				{
					long controller = *(long *)(player + 0x24);
					if (controller != NONE)
					{
					short slot = *(short *)(player + 0x28);
					if (slot != NONE && !function_147da5(slot))
					{
						function_57620(observer, local, (short)controller);
						if (voice_observer_talked_recently(observer, local))
						{
							for (long other = 0; other < 16; other++)
							{
								if ((mask & (1 << other)) && !(local_mask & (1 << other)) && function_54df0(other))
								{
									if (function_57270(local, other))
										observer->local_masks[local] |= 1 << other;
									else
										observer->local_masks[local] &= ~(1 << other);
								}
							}
						}
					}
					}
				}
			}
		}
	}
}


byte *voice_get_world_player(long player_index);
real function_30bf0(vector3f *vector);
void function_192d40(point3f const *direction, real *gains, bool linear);

static __forceinline real function_586f1(void)
{
 real result = 8.f;
 if (g_510c94)
  result = g_510c94->motion_sensor_range;
 return result;
}

static __forceinline point3f function_586f3(const point3f *source)
{
 point3f result = *source;
 return result;
}

static __forceinline real function_586f4(real z, real y, real x)
{
 return sqrtf(z * z + y * y + x * x);
}

// @retail 0x586f0
void function_586f0(long player, c_voice_observer *observer, long *count, DSMIXBINVOLUMEPAIR *bins, const point3f *position)
{
 byte *world_player = voice_get_world_player(player);
 if (world_player && *(short *)(world_player + 0x28) != NONE)
 {
  s_local_camera *camera = local_camera_get(*(short *)(world_player + 0x28));
  if (camera && camera->active)
  {
   long smallest = 0;
   long used = 0;
   point3f local;
   function_142700((const transform4x3f *)((const byte *)camera + 8), position, &local);
   point3f direction = function_586f3(&local);
   real distance = function_586f4(direction.z, direction.y, direction.x);
   real range = function_586f1();
   real attenuation;
   if (distance > range)
   {
    real beyond = distance - (function_586f1());
    real divisor = function_586f1();
    real extra_divisor = function_586f1();
    attenuation = ((real)observer->distance_volume * distance) / (divisor + 1.5f) + ((real)observer->distance_volume / extra_divisor) * beyond * 150.f;
   }
   else
    attenuation = ((real)observer->distance_volume * distance) / ((function_586f1()) + 1.5f);
   function_30bf0((vector3f *)&direction);
   real gains[5];
   function_192d40(&direction, gains, observer->linear_gain);
   real compact[4];
   real minimum = gains[0];
   for (long i = 0; i < 5; i++)
   {
    if (minimum > gains[i])
    {
     minimum = gains[i];
     smallest = i;
    }
   }
   if (smallest != 0) { compact[used] = gains[0]; bins[used++].dwMixBin = 0; }
   if (smallest != 1) { compact[used] = gains[1]; bins[used++].dwMixBin = 1; }
   if (smallest != 2) { compact[used] = gains[2]; bins[used++].dwMixBin = 4; }
   if (smallest != 3) { compact[used] = gains[3]; bins[used++].dwMixBin = 5; }
   if (smallest != 4) { compact[used] = gains[4]; bins[used++].dwMixBin = 2; }
   for (long j = 0; j < 4; j++)
   {
    long directional = (long)((1.f - compact[j]) * (real)observer->directional_volume);
    long volume = (long)((real)directional + attenuation);
    if (volume < -10000) volume = -10000;
    else volume &= (long)(volume > 0) - 1;
    bins[j].lVolume = volume;
   }
   *count = 4;
   return;
  }
 }
 bins[0].lVolume = 0;
 bins[0].dwMixBin = 2;
 *count = 1;
}
