// Exact recovered old-map sound disposal isolated from the broad game-module
// callback unit.  The closure deletes live sound effects and looping sounds,
// preserves retained object-attached playback on partial switches, and uses
// the genuine playback, stream, controller, pool, and idle lifetimes.

#include "data_array.h"
#include "globals.h"
#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_4e6380
{
	byte unknown00[0x78];
	byte flag78;
	byte unknown79[2];
	byte flag7b;
	byte unknown7c;
	byte flag7d;
	byte unknown7e[2];
	long unknown80;
	long unknown84;
	byte unknown88[0x1f8 - 0x88];
	long unknown1f8;
	byte unknown1fc[4];
	long unknown200;
};

struct s_sound_playback_disposal_view
{
	byte unknown00[2];
	byte kind;
	byte unknown03[9];
	long object_index;
	byte unknown10[0x7c];
	long retained_marker;
	byte unknown90[0x2c];
};

struct s_sound_effect_disposal_view
{
	byte unknown00[8];
	long controller_index;
	byte unknown0c[8];
};

struct s_looping_sound_disposal_view
{
	byte unknown00[0x54];
	long controller_index;
	byte unknown58[0x7c];
};

struct s_unknown_5c;
extern s_4e6380 *g_4e6380;
extern s_record_pool *g_4e637c;
extern void *g_502110;
extern void *g_51ebe0;

void function_125d60(void);
void function_127320(long sound_index, long reason);
void function_21f290(void);
s_unknown_5c *function_221810(short type);
void looping_sound_controller_release(long controller_index);
extern "C" unsigned long __stdcall GetTickCount(void);

static_assert(sizeof(s_4e6380) == 0x204,
	"old-map sound-system view size");
static_assert(offsetof(s_4e6380, flag78) == 0x78,
	"old-map initialized flag offset");
static_assert(offsetof(s_4e6380, unknown80) == 0x80,
	"old-map sound tick offset");
static_assert(offsetof(s_4e6380, unknown1f8) == 0x1f8,
	"old-map ambience offset");
static_assert(offsetof(s_4e6380, unknown200) == 0x200,
	"old-map previous ambience offset");
static_assert(sizeof(s_sound_playback_disposal_view) == 0xbc,
	"old-map playback size");
static_assert(offsetof(s_sound_playback_disposal_view, object_index) == 0x0c,
	"old-map playback object offset");
static_assert(offsetof(s_sound_playback_disposal_view, retained_marker) == 0x8c,
	"old-map retained playback marker offset");
static_assert(sizeof(s_sound_effect_disposal_view) == 0x14,
	"old-map sound-effect size");
static_assert(offsetof(s_sound_effect_disposal_view, controller_index) == 0x08,
	"old-map sound-effect controller offset");
static_assert(sizeof(s_looping_sound_disposal_view) == 0xd4,
	"old-map looping-sound size");
static_assert(offsetof(s_looping_sound_disposal_view, controller_index) == 0x54,
	"old-map looping-sound controller offset");

static void sound_effect_delete_inline(long effect_index)
{
	s_record_pool *effects = reinterpret_cast<s_record_pool *>(g_51ebe0);
	s_sound_effect_disposal_view *effect =
		reinterpret_cast<s_sound_effect_disposal_view *>(effects->data) +
		(effect_index & 0xffff);
	if (effect->controller_index != NONE)
	{
		looping_sound_controller_release(effect->controller_index);
		effect->controller_index = NONE;
	}
	record_pool_release(effects, effect_index);
}

// @retail 0x21d4d0
void function_21d4d0(void)
{
	s_record_pool_iterator iterator = {
		reinterpret_cast<s_record_pool *>(g_51ebe0), NONE, NONE };
	while (data_iterator_next_inlined(&iterator))
		sound_effect_delete_inline(iterator.datum_index);
}

// @retail 0x21b070
void function_21b070(long looping_sound_index)
{
	s_record_pool *looping_sounds =
		reinterpret_cast<s_record_pool *>(g_502110);
	s_looping_sound_disposal_view *sound =
		reinterpret_cast<s_looping_sound_disposal_view *>(
			looping_sounds->data) + (looping_sound_index & 0xffff);
	looping_sound_controller_release(sound->controller_index);
	record_pool_release(looping_sounds, looping_sound_index);
}

// @retail 0x21a1e0
void function_21a1e0(void)
{
	s_record_pool *looping_sounds =
		reinterpret_cast<s_record_pool *>(g_502110);
	if (looping_sounds)
	{
		s_record_pool_iterator iterator = { looping_sounds, NONE, NONE };
		while (data_iterator_next_inlined(&iterator))
			function_21b070(iterator.datum_index);
	}
}

// @retail 0x1264c0
void __stdcall function_1264c0(dword flags)
{
	bool preserve_object_sounds = ((flags >> 6) & 1) != 0;
	bool partial_switch = ((flags >> 5) & 1) != 0;
	s_4e6380 *sound_system = g_4e6380;

	if (!partial_switch && sound_system->flag7b)
	{
		sound_system->flag7d = 1;
		sound_system->flag7b = 0;
		sound_system->flag7d = 0;
	}

	if (sound_system->flag78)
	{
		function_21d4d0();
		s_record_pool *sounds = g_4e637c;
		long sound_index = data_datum_index(
			sounds, function_16bc00(sounds, 0));
		while (sound_index != NONE)
		{
			s_sound_playback_disposal_view *sound =
				reinterpret_cast<s_sound_playback_disposal_view *>(sounds->data) +
				(sound_index & 0xffff);
			bool remove = true;
			if (partial_switch)
			{
				short type = *(reinterpret_cast<char *>(
					g_4e3b44[sound->object_index & 0xffff].flags) + 2);
				if (type == 0x20 || type == 0x26)
					remove = type == 0x26 && !preserve_object_sounds;
				else if ((1 << sound->kind) & 0x1e)
				{
					remove = reinterpret_cast<byte *>(
						function_221810(type))[0x4c] != 1;
				}
			}

			if (remove)
			{
				function_127320(sound_index, 10);
				sounds = g_4e637c;
			}
			else
				sound->retained_marker = NONE;

			sound_index = data_datum_index(sounds,
				function_16bc00(sounds,
					(sound_index & 0xffff) + 1));
		}

		if (preserve_object_sounds)
			function_125d60();
		else
			function_21f290();
	}

	sound_system = g_4e6380;
	sound_system->unknown80 = static_cast<long>(GetTickCount());
	sound_system->unknown1f8 = NONE;
	sound_system->unknown200 = NONE;
	sound_system->unknown84 = 0;
	g_4ed288->value20 = 0;
	g_4ed288->value24 = 0;
	if (!partial_switch)
		function_21a1e0();
}

#endif
