// @flags /O2 /arch:SSE /Gr
// ARM-safe selective ownership of the recovered driver stream-format policy.
// DirectSound stream construction and codec execution remain genuine boundaries.

#include "unknown_11c920.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

class c_sound_stream_codec;
struct s_sound_stream;

#pragma pack(push, 1)
struct s_wave_format_ex
{
	word format_tag;
	word channels;
	dword samples_per_second;
	dword average_bytes_per_second;
	word block_align;
	word bits_per_sample;
	word extra_size;
};

struct XBOXADPCMWAVEFORMAT
{
	s_wave_format_ex wfx;
	word samples_per_block;
};
#pragma pack(pop)

typedef void (__stdcall *sound_stream_callback)(
	void *stream_context,
	void *packet_context,
	dword status);

struct DSSTREAMDESC
{
	dword flags;
	dword maximum_attached_packets;
	s_wave_format_ex *format;
	sound_stream_callback callback;
	void *context;
	void *mixbins;
};

struct s_47f0d0;
extern s_47f0d0 g_47f0d0;

struct s_47f0f0
{
	void *vtable;
};
extern s_47f0f0 g_47f0f0;

// @retail 0x21e410
void function_21e410(
	long type,
	XBOXADPCMWAVEFORMAT *format,
	c_sound_stream_codec **codec)
{
	if (type != 2)
	{
		word channels;

		format->wfx.format_tag = 0x0069; // WAVE_FORMAT_XBOX_ADPCM
		format->wfx.bits_per_sample = 4;
		channels = (word)(type == 1 ? 2.0f : 1.0f);
		format->wfx.channels = channels;
		format->wfx.samples_per_second = 44100;
		format->wfx.extra_size = 2;
		format->samples_per_block = 64;
		format->wfx.block_align = (word)(channels * 36);
		format->wfx.average_bytes_per_second = format->wfx.block_align * 689;
		*codec = (c_sound_stream_codec *)&g_47f0f0;
	}
	else
	{
		format->wfx.format_tag = 1; // WAVE_FORMAT_PCM
		format->wfx.channels = 2;
		format->wfx.samples_per_second = 44100;
		format->wfx.average_bytes_per_second = 176400;
		format->wfx.block_align = 4;
		format->wfx.bits_per_sample = 16;
		format->wfx.extra_size = 0;
		*codec = (c_sound_stream_codec *)&g_47f0d0;
	}
}

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

void __stdcall sound_driver_stream_callback(
	void *stream_context,
	void *packet_context,
	dword status);

bool sound_stream_create(
	s_sound_stream *stream,
	c_sound_stream_codec *codec,
	DSSTREAMDESC *description,
	short channel_count);

// @retail 0x21f4e0
bool function_21f4e0(long channel_index, long type)
{
	s_sound_stream *stream = (s_sound_stream *)((byte *)g_51ebe4 + 0x0c +
		(dword)channel_index * 0x34);
	c_sound_stream_codec *codec = NULL;
	DSSTREAMDESC description;
	XBOXADPCMWAVEFORMAT format;

	memset(&description, 0, sizeof(description));
	function_21e410(type, &format, &codec);
	description.format = &format.wfx;
	description.flags = 0;
	description.maximum_attached_packets = 2;
	description.callback = sound_driver_stream_callback;
	description.context = (void *)(uintptr_t)(dword)channel_index;
	return sound_stream_create(stream, codec, &description, (short)type);
}

static_assert(sizeof(s_wave_format_ex) == 0x12,
	"Microsoft ABI wave format layout changed");
static_assert(offsetof(s_wave_format_ex, extra_size) == 0x10,
	"Microsoft ABI wave format tail changed");
static_assert(sizeof(XBOXADPCMWAVEFORMAT) == 0x14,
	"Microsoft ABI Xbox ADPCM format layout changed");
static_assert(sizeof(DSSTREAMDESC) == 0x18,
	"Microsoft ABI DirectSound stream descriptor layout changed");
static_assert(offsetof(DSSTREAMDESC, callback) == 0x0c &&
	offsetof(DSSTREAMDESC, context) == 0x10,
	"Microsoft ABI DirectSound stream callback fields changed");
