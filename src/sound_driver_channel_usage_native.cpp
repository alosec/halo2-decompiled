#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

/* Exact recovered sound-driver channel/voice usage snapshot. */

struct s_sound_driver_channel_usage
{
	dword channels_by_state[4][4];
	long unknown40[3];
	long unknown4c[3];
	dword free_voices[2];
};

struct s_sound_stream_usage_view
{
	byte voice_index;
	char state : 3;
	char channel_count : 3;
	char unknown01_6 : 2;
	byte unknown02[0x32];
};

struct s_sound_driver_usage_view
{
	bool unknown0000;
	bool surround;
	byte unknown0002[2];
	short channel_count;
	short voice_count;
	short maximum_voice_count;
	byte unknown000a[2];
	s_sound_stream_usage_view channels[0x80];
	short unknown1a0c[3];
	short unknown1a12[3];
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

static_assert(sizeof(s_sound_driver_channel_usage) == 0x60,
	"sound driver channel usage size");
static_assert(offsetof(s_sound_driver_channel_usage, unknown40) == 0x40,
	"sound driver usage first counts offset");
static_assert(offsetof(s_sound_driver_channel_usage, free_voices) == 0x58,
	"sound driver usage free voice offset");
static_assert(sizeof(s_sound_stream_usage_view) == 0x34,
	"sound driver stream size");
static_assert(offsetof(s_sound_driver_usage_view, channels) == 0x0c,
	"sound driver channel offset");
static_assert(offsetof(s_sound_driver_usage_view, unknown1a0c) == 0x1a0c,
	"sound driver first count offset");
static_assert(offsetof(s_sound_driver_usage_view, unknown1a12) == 0x1a12,
	"sound driver second count offset");

// @retail 0x21f360
void __stdcall function_21f360(s_sound_driver_channel_usage *usage)
{
	s_sound_driver_usage_view *globals =
		reinterpret_cast<s_sound_driver_usage_view *>(g_51ebe4);
	memset(usage, 0, sizeof(*usage));
	memset(usage->free_voices, 0xff,
		((globals->maximum_voice_count + 31) >> 5) * sizeof(dword));
	for (long i = 0; i < globals->channel_count; i++)
	{
		s_sound_stream_usage_view *stream = &globals->channels[i];
		usage->channels_by_state[stream->state][i >> 5] |=
			1u << (i & 31);
		if (stream->voice_index != 0xff && stream->state)
		{
			char const voice_index = stream->voice_index;
			usage->free_voices[voice_index >> 5] &=
				~(1u << (voice_index & 31));
		}
	}
	long i = 0;
	do
	{
		usage->unknown40[i] = globals->unknown1a0c[i];
		usage->unknown4c[i] = globals->unknown1a12[i];
		i++;
	}
	while (i < 3);
}

#endif
