// Exact initial looping-channel submission isolated from the broad Xbox
// DirectSound update unit.  Stream pause/output operations, effect parameter
// submission, the shared update path, and every object lifetime remain real.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSoundStream;
struct IDirectSoundBuffer;
struct s_looping_channel_properties;
struct s_looping_effect_playback;
struct s_sound_channel_parameters;

struct s_sound_stream
{
	byte unknown00;
	byte unknown01;
	byte unknown02;
	byte flags;
	byte unknown04[0x20];
	IDirectSoundStream *stream;
	long unknown28;
	long offset;
	void *codec;
};

namespace
{
struct s_channel_update_input
{
	dword field_0;
	long format;
	real pitch;
	real gain;
	byte field_10[4];
	real field_14;
	real field_18;
	real field_1c;
	real field_20;
	short voice_index;
	byte field_26[2];
	real field_28;
	long field_2c;
	dword field_30;
	byte field_34[4];
	real levels[4];
};

struct s_driver_voice
{
	byte unknown00[0x34];
	IDirectSoundBuffer *buffer;
	IDirectSoundBuffer *submix;
};

struct s_driver_channel_start_view
{
	byte unknown0000[0xc];
	s_sound_stream channels[0x80];
	byte unknown1a0c[0xc];
	s_driver_voice voices[0x40];
	byte unknown2918[0x1c0];
};

static_assert(sizeof(s_channel_update_input) == 0x48,
	"looping channel input size");
static_assert(offsetof(s_channel_update_input, voice_index) == 0x24,
	"looping channel voice offset");
static_assert(offsetof(s_channel_update_input, field_2c) == 0x2c,
	"looping channel seek offset");
static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, flags) == 0x3,
	"sound stream flags offset");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");
static_assert(offsetof(s_sound_stream, unknown28) == 0x28,
	"sound stream seek-state offset");
static_assert(sizeof(s_driver_voice) == 0x3c, "driver voice size");
static_assert(offsetof(s_driver_voice, buffer) == 0x34,
	"driver voice buffer offset");
static_assert(sizeof(s_driver_channel_start_view) == 0x2ad8,
	"sound driver globals size");
static_assert(offsetof(s_driver_channel_start_view, channels) == 0xc,
	"driver channels offset");
static_assert(offsetof(s_driver_channel_start_view, voices) == 0x1a18,
	"driver voices offset");
}

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

void sound_stream_set_envelope(s_sound_stream *stream,
	real attack, real release);
void function_2201f0(long channel_index,
	s_sound_channel_parameters const *parameters, bool initialize);
void function_21fa80(long channel_index,
	s_looping_channel_properties const *properties,
	s_looping_effect_playback const *effects, bool force);

extern "C" long __stdcall IDirectSoundStream_Pause(
	IDirectSoundStream *stream, dword pause);
extern "C" long __stdcall IDirectSoundStream_SetOutputBuffer(
	IDirectSoundStream *stream, IDirectSoundBuffer *buffer);

// @retail 0x21f8a0
void function_21f8a0(long channel_index,
	s_looping_channel_properties const *properties,
	s_looping_effect_playback const *effects)
{
	s_channel_update_input const *input =
		reinterpret_cast<s_channel_update_input const *>(properties);
	s_driver_channel_start_view *globals =
		reinterpret_cast<s_driver_channel_start_view *>(g_51ebe4);
	s_sound_stream *stream = &globals->channels[channel_index];
	stream->flags |= 0x10;
	stream->unknown28 = input->field_2c;
	stream->offset = 0;
	IDirectSoundStream_Pause(stream->stream,
		stream->unknown28 != NONE ? 1 : 0);
	sound_stream_set_envelope(stream, 0.0f, 0.1f);
	if (input->voice_index != NONE)
	{
		IDirectSoundStream_SetOutputBuffer(stream->stream,
			globals->voices[input->voice_index].buffer);
		stream->unknown00 = static_cast<byte>(input->voice_index);
	}
	else
	{
		IDirectSoundStream_SetOutputBuffer(stream->stream, NULL);
		stream->unknown00 = 0xff;
	}
	function_2201f0(channel_index,
		reinterpret_cast<s_sound_channel_parameters const *>(effects), true);
	stream->flags &= ~1;
	function_21fa80(channel_index, properties, NULL, false);
	stream->flags |= 1;
}

#endif
