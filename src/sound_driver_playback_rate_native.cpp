#include "unknown_11c920.h"
#include <math.h>

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact compact stream playback-rate query.  It reads genuine driver stream
 * state; no stream progress, DirectSound object, or audio backend is created. */

struct s_sound_stream_view
{
	byte unknown00[8];
	long pitch_cents;
	byte unknown0c[0x2c];
};

struct s_sound_driver_streams_view
{
	byte unknown00[0x0c];
	s_sound_stream_view streams[1];
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

static_assert(sizeof(s_sound_stream_view) == 0x38, "sound stream size");
static_assert(offsetof(s_sound_stream_view, pitch_cents) == 0x08,
	"sound stream pitch offset");
static_assert(offsetof(s_sound_driver_streams_view, streams) == 0x0c,
	"sound driver stream base offset");

static real sound_pitch_ratio(long cents)
{
	real const octaves = cents / 4096.0f;
	return static_cast<real>(exp(octaves * 0.693147182f));
}

// @retail 0x21f650
real function_21f650(long channel_index, long mode)
{
	s_sound_driver_streams_view *driver =
		reinterpret_cast<s_sound_driver_streams_view *>(g_51ebe4);
	long const pitch = driver->streams[channel_index].pitch_cents;
	if (mode == 0)
		return sound_pitch_ratio(pitch + 0x11f5);
	if (mode == 1)
		return sound_pitch_ratio(pitch + 0x1f5);
	return sound_pitch_ratio(pitch + 0x95c);
}

#endif
