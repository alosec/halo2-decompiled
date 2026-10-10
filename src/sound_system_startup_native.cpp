#include "data_array.h"
#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

struct s_sound_environment
{
	real transition_time;
	real fade;
	long index;
	real unknown0c;
	real unknown10;
	real unknown14;
	real unknown18;
};

struct s_sound_listener_startup
{
	byte data[0x48];
};

struct s_sound_system_startup
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
	s_sound_listener_startup listeners[4];
	s_sound_environment environments[2];
	real elapsed_time;
	real master_fade;
	long master_fade_delay;
	real master_fade_times[3];
	long ambience_index;
	real ambience_fade;
	long previous_ambience_index;
	real previous_ambience_fade;
	short voice_count;
	short maximum_voice_count;
};
static_assert(sizeof(s_sound_system_startup) == 0x20c, "sound system layout");
static_assert(offsetof(s_sound_system_startup, environments) == 0x1a8, "sound environment offset");
static_assert(offsetof(s_sound_system_startup, voice_count) == 0x208, "sound voice count offset");

struct s_sound_voice_startup
{
	long sound_index;
	long driver_voice_index;
	byte definition_type;
	bool stream_reset;
	byte unknown0a;
	byte unknown0b;
	short channel_index;
	short unknown0e;
	real unknown10;
	byte unknown14[4];
	short chunk_index;
	short next_chunk_index;
	void const *permutation;
	void const *next_permutation;
};
static_assert(sizeof(s_sound_voice_startup) == 0x24, "sound voice layout");

struct s_sound_driver_counts
{
	short channels[3];
	byte unknown06[6];
	short voices;
};
static_assert(sizeof(s_sound_driver_counts) == 0xe, "sound driver counts layout");

extern long g_4e6374;
struct s_sound_voice;
extern s_sound_voice *g_4e6378;
extern s_record_pool *g_4e637c;
struct s_4e6380;
extern s_4e6380 *g_4e6380;
extern dword g_510800_pool_base;
extern long g_510804_pool_size;
extern dword g_510808_pool_checksum;

void function_163ba0(dword *checksum, void const *data, long size);
s_record_pool *function_11cc20(long maximum_count, char const *name, long size);
void record_pool_release_all(s_record_pool *pool);
void sound_cache_initialize(void);
bool looping_sound_controllers_initialize(void);
bool function_21a0c0(void);
bool sound_effects_initialize(void);
struct s_sound_voice_settings;
bool sound_voices_initialize(s_sound_voice_settings const *settings);
bool function_21e4b0(s_sound_driver_counts const *counts);

static void *sound_static_allocate(long size)
{
	byte *top = reinterpret_cast<byte *>(g_510800_pool_base + g_510804_pool_size);
	byte *memory = reinterpret_cast<byte *>((reinterpret_cast<dword>(top) + 3) & ~3u);
	long allocated_size = static_cast<long>(memory - top) + size;
	g_510804_pool_size += allocated_size;
	function_163ba0(&g_510808_pool_checksum, &allocated_size, sizeof(allocated_size));
	return memory;
}

static void bit_vector_fill(dword *vector, long count, byte value)
{
	memset(vector, value, ((count + 31) >> 5) * sizeof(dword));
}

// @retail 0x1253d0
void function_1253d0(void)
{
	s_sound_driver_counts counts;
	counts.channels[0] = 40;
	counts.channels[1] = 41;
	counts.channels[2] = 1;
	counts.unknown06[0] = 36;
	counts.unknown06[1] = 0;
	counts.unknown06[2] = 36;
	counts.unknown06[3] = 0;
	counts.unknown06[4] = 1;
	counts.unknown06[5] = 0;
	counts.voices = 51;
	if (g_4e6374 == 0)
	{
		s_sound_system_startup *system = static_cast<s_sound_system_startup *>(sound_static_allocate(0x20c));
		memset(system, 0, 0x20c);
		g_4e6380 = reinterpret_cast<s_4e6380 *>(system);
		system->hardware_available = true;
		system->changing_pause = false;
		sound_cache_initialize();
		system->environments[0].index = NONE;
		system->environments[0].unknown0c = 0.0f;
		system->environments[0].unknown10 = 6.283185482025146484375f;
		system->environments[1].index = NONE;
		system->environments[1].unknown0c = 0.0f;
		system->environments[1].unknown10 = 0.0f;
		g_4e637c = function_11cc20(0x180, "sound sources", 0xbc);
		s_sound_voice_startup *voices = static_cast<s_sound_voice_startup *>(sound_static_allocate(0x1200));
		g_4e6378 = reinterpret_cast<s_sound_voice *>(voices);
		if (g_4e637c && voices)
		{
			g_4e637c->valid = true;
			record_pool_release_all(g_4e637c);
			memset(voices, 0, 0x1200);
			if (looping_sound_controllers_initialize() && function_21a0c0() &&
				sound_effects_initialize() &&
				sound_voices_initialize(reinterpret_cast<s_sound_voice_settings const *>(&counts)) &&
				function_21e4b0(&counts))
			{
				bit_vector_fill(reinterpret_cast<dword *>(reinterpret_cast<byte *>(system) + 0x60), 0x80, 0);
				bit_vector_fill(system->channel_bits, 0x40, 0);
				short voice_index = 0;
				for (short type = 0; type < 3; ++type)
				{
					short count = reinterpret_cast<short const *>(counts.unknown06)[type];
					system->voice_count += count;
					for (short i = 0; i < count; ++i)
					{
						s_sound_voice_startup *voice = &voices[voice_index++];
						voice->sound_index = NONE;
						voice->definition_type = static_cast<byte>(type);
						voice->unknown0a = 0;
						voice->unknown0b = 0;
						voice->channel_index = NONE;
						voice->unknown0e = NONE;
						voice->driver_voice_index = NONE;
						voice->permutation = NULL;
						voice->next_permutation = NULL;
						voice->stream_reset = false;
					}
				}
				system->maximum_voice_count = 0x33;
				system->initialized = true;
			}
		}
	}
	g_4e6374++;
}
