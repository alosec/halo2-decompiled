// Exact recovered per-map sound publication and clear lifetime isolated from
// the broader Xbox DirectSound translation unit.  The old-map disposal call
// remains a genuine recovered lifecycle boundary; no playback or backend
// completion is fabricated here.

#include "data_array.h"
#include "unknown_124f90.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_globals;

struct s_sound_listener_map_view
{
	long leaf_index;
	short cluster_index;
	bool active;
	byte unknown07;
	byte unknown08[0x40];
};

struct s_sound_environment_map_view
{
	real transition_time;
	real fade;
	long index;
	real unknown0c;
	real unknown10;
	real unknown14;
	real unknown18;
};

struct s_4e6380
{
	byte unknown00[0x70];
	dword channel_bits[2];
	bool initialized;
	bool hardware_available;
	bool enabled;
	bool unknown7b;
	byte unknown7c;
	bool changing_pause;
	byte unknown7e[2];
	dword field_14_2;
	long time;
	s_sound_listener_map_view listeners[4];
	s_sound_environment_map_view environments[2];
	real elapsed_time;
	real master_fade;
	long master_fade_delay;
	real master_fade_times[3];
	long ambience_index;
	real ambience_fade;
	long previous_ambience_index;
	real previous_ambience_fade;
};

struct s_globals_sound_globals_view
{
	byte unknown00[0x20];
	long sound_globals_index;
};

struct s_bink_sound_settings
{
	byte unknown0000[0x2a98];
	long impulse_ids[2];
};

static_assert(sizeof(s_sound_listener_map_view) == 0x48,
	"sound listener map size");
static_assert(sizeof(s_sound_environment_map_view) == 0x1c,
	"sound environment map size");
static_assert(offsetof(s_4e6380, initialized) == 0x78,
	"sound initialized offset");
static_assert(offsetof(s_4e6380, listeners) == 0x88,
	"sound listener map offset");
static_assert(offsetof(s_4e6380, environments) == 0x1a8,
	"sound environment map offset");
static_assert(offsetof(s_4e6380, master_fade) == 0x1e4,
	"sound master-fade offset");
static_assert(offsetof(s_4e6380, ambience_index) == 0x1f8,
	"sound ambience offset");
static_assert(offsetof(s_bink_sound_settings, impulse_ids) == 0x2a98,
	"sound impulse identifier offset");
static_assert(sizeof(s_sound_playback) == 0xbc,
	"sound playback map size");
static_assert(offsetof(s_sound_playback, fade_curve) == 0xae,
	"sound playback fade-curve offset");
static_assert(offsetof(s_sound_playback, fade_gain) == 0xb0,
	"sound playback fade-gain offset");
static_assert(offsetof(s_sound_playback, fade_start_time) == 0xb4,
	"sound playback fade-start offset");
static_assert(offsetof(s_sound_playback, fade_end_time) == 0xb8,
	"sound playback fade-end offset");

struct s_47f0d0
{
	void *vtable;
	void *stream;
	void *decoder;
	dword position;
	dword packet_size;
	byte *buffer;
	dword packet_status[2];
};

static_assert(sizeof(s_47f0d0) == 0x20, "WMA codec object size");
static_assert(offsetof(s_47f0d0, packet_size) == 0x10,
	"WMA codec packet-size alias offset");

extern s_4e6380 *g_4e6380;
extern s_record_pool *g_4e637c;
extern s_47f0d0 g_47f0d0;
extern void *g_502110;
extern void *g_51ebd8;
extern s_bink_sound_settings *g_51ebe4;
extern "C" unsigned long __stdcall GetTickCount(void);

void sound_mix_apply(void);
void function_191270(void);
long function_12a810(long sound_index);
void function_125d60(void);
void __stdcall function_1264c0(dword flags);

s_sound_globals *g_51ebd4;

// @retail 0x125690
void function_125690(void)
{
	s_tag_header_globals *globals = g_4e034c;
	s_globals_sound_globals_view *header =
		reinterpret_cast<s_globals_sound_globals_view *>(
			globals->header ? globals->header_alt : NULL);

	g_51ebd4 = reinterpret_cast<s_sound_globals *>(
		g_4e3b44[header->sound_globals_index & 0xffff].bytes);
	g_4e6380->master_fade = 1.0f;
	g_4e6380->enabled = true;
	g_4e6380->field_14_2 = GetTickCount();
	g_4e6380->time = 0;
	g_4e6380->ambience_index = NONE;
	g_4e6380->previous_ambience_index = NONE;
	g_4e6380->environments[0].fade = 0.0f;
	g_4e6380->environments[0].index = NONE;
	g_4e6380->environments[0].unknown0c = 0.0f;
	g_4e6380->environments[0].unknown10 = 2.0f * 3.14159265358979323846f;
	g_4e6380->environments[1].fade = 0.0f;
	g_4e6380->environments[1].index = NONE;
	g_4e6380->environments[1].unknown0c = 0.0f;
	g_4e6380->environments[1].unknown10 = 0.0f;
	for (long listener_index = 0; listener_index < 4; listener_index++)
		g_4e6380->listeners[listener_index].active = false;
	sound_mix_apply();
	function_191270();
	g_51ebe4->impulse_ids[0] = NONE;
	g_51ebe4->impulse_ids[1] = NONE;
	g_47f0d0.packet_size = g_4e6948->state == 3 ? 0x10000 : 0x4000;
}

// @retail 0x1257b0
void function_1257b0(void)
{
	if (!g_4e6380->unknown7b && g_4e6380->initialized &&
		g_4e6380->hardware_available && g_4e6380->enabled)
	{
		long start_tick = static_cast<long>(GetTickCount());
		s_record_pool *sounds = g_4e637c;
		long sound_index = data_datum_index(
			sounds, function_16bc00(sounds, 0));
		if (sound_index != NONE)
		{
			do
			{
				s_sound_playback *playback =
					reinterpret_cast<s_sound_playback *>(sounds->data) +
					(sound_index & 0xffff);
				playback->fade_gain = function_12a810(sound_index);
				playback->fade_curve = 0;
				playback->fade_start_time = 300;
				playback->fade_end_time = NONE;
				playback->fading = true;
				sound_index = data_datum_index(sounds,
					function_16bc00(sounds,
						(sound_index & 0xffff) + 1));
			}
			while (sound_index != NONE);

			real end_tick = static_cast<real>(start_tick) + 300.0f;
			for (;;)
			{
				if (!(GetTickCount() < end_tick))
					break;
				function_125d60();
			}
		}
	}
	function_1264c0(0);
	g_4e6380->enabled = false;
	if (g_502110)
		record_pool_release_all(reinterpret_cast<s_record_pool *>(g_502110));
	if (g_51ebd8)
		record_pool_release_all(reinterpret_cast<s_record_pool *>(g_51ebd8));
	g_51ebd4 = NULL;
	function_191270();
	g_51ebe4->impulse_ids[0] = NONE;
	g_51ebe4->impulse_ids[1] = NONE;
}

#endif
