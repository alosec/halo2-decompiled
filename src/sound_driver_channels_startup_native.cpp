// Exact dormant channel/voice construction phase isolated from the broader
// sound-driver startup.  Stream and voice creation remain genuine backend
// operations, and a partial failure preserves the recovered range/count state
// for ordered teardown.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;

struct s_sound_driver_counts
{
	short channels[3];
	byte unknown06[6];
	short voices;
};

struct s_47f0d0
{
	void *vtable;
	long field04;
	long field08;
	long field0c;
	long field10;
	void *field14;
};

struct s_driver_channels_view
{
	byte unknown0000[4];
	short channel_count;
	short voice_count;
	short unknown0008;
	byte unknown000a[0x1a0c - 0x000a];
	short channel_first[3];
	short channel_last[3];
	byte unknown1a18[0x2ab0 - 0x1a18];
	IDirectSound *direct_sound;
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
extern s_47f0d0 g_47f0d0;

void function_12c040(void);
bool function_21f4e0(long channel_index, long type);
bool sound_driver_voice_create_buffer(long voice_index);
void function_191420(IDirectSound *direct_sound);

static_assert(sizeof(s_sound_driver_counts) == 0x0e,
	"sound driver counts size");
static_assert(offsetof(s_sound_driver_counts, voices) == 0x0c,
	"sound driver voice-count input offset");
static_assert(sizeof(s_47f0d0) == 0x18, "codec allocator view size");
static_assert(offsetof(s_47f0d0, field14) == 0x14,
	"codec allocator owner offset");
static_assert(offsetof(s_driver_channels_view, channel_count) == 0x04,
	"driver channel-count offset");
static_assert(offsetof(s_driver_channels_view, unknown0008) == 0x08,
	"driver active-voice-count offset");
static_assert(offsetof(s_driver_channels_view, channel_first) == 0x1a0c,
	"driver channel-range start offset");
static_assert(offsetof(s_driver_channels_view, channel_last) == 0x1a12,
	"driver channel-range end offset");
static_assert(offsetof(s_driver_channels_view, direct_sound) == 0x2ab0,
	"driver DirectSound offset");

// Source-local exact phase of retail function_21e4b0.
bool sound_driver_channels_startup(s_sound_driver_counts const *counts)
{
	s_driver_channels_view *globals =
		reinterpret_cast<s_driver_channels_view *>(g_51ebe4);
	bool result = true;

	function_12c040();
	g_47f0d0.field04 = 0;
	g_47f0d0.field14 = NULL;
	g_47f0d0.field10 = 0x10000;
	for (long type = 0; type < 3; type++)
	{
		long first = globals->channel_count;
		globals->channel_first[type] = globals->channel_count;
		globals->channel_count += counts->channels[type];
		for (long i = 0; result && i < counts->channels[type]; i++)
			result = result && function_21f4e0(first + i, type);
		globals->channel_last[type] = globals->channel_count - 1;
	}

	short voices = counts->voices;
	globals->voice_count = voices;
	globals->unknown0008 = voices;
	for (long voice = 0; result && voice < counts->voices; voice++)
		result = result && sound_driver_voice_create_buffer(voice);
	function_191420(globals->direct_sound);
	return result;
}

#endif
