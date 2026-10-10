// Exact dormant all-channel halt used by driver teardown.  Every live stream
// status, flush, codec stop, chunk release, and final stream normalization is
// retained; no backend operation is replaced with a no-op.

#include "unknown_11c920.h"
#include "unknown_218850.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_stream;
struct s_sound_cache_usage;
struct XMEDIAPACKET;

class c_sound_stream_codec
{
public:
	virtual void attach(s_sound_stream *stream) = 0;
	virtual void start(s_sound_stream *stream) = 0;
	virtual void mark_cache_usage(s_sound_stream *stream,
		s_sound_cache_usage *usage) = 0;
	virtual bool get_packet(s_sound_stream *stream, XMEDIAPACKET *packet) = 0;
	virtual void packet_submitted(s_sound_stream *stream,
		XMEDIAPACKET *packet) = 0;
	virtual void packet_failed(s_sound_stream *stream,
		XMEDIAPACKET *packet) = 0;
	virtual void chunk_finished(s_sound_stream *stream, s_sound_chunk *chunk,
		long unused) = 0;
	virtual void stop() = 0;
	virtual bool can_submit(s_sound_stream *stream, bool submitted) = 0;
};

struct s_sound_stream
{
	byte unknown00;
	char state : 3;
	char channel_count : 3;
	char unknown01_6 : 2;
	char chunk_count : 3;
	char started_count : 3;
	char unknown02_6 : 2;
	byte unknown03_0 : 2;
	byte flushing : 1;
	byte unknown03_3 : 1;
	byte unknown03_4 : 4;
	byte unknown04[0x18];
	s_sound_chunk *chunks[2];
	void *stream;
	long unknown28;
	long offset;
	c_sound_stream_codec *codec;
};

struct s_driver_channels_view
{
	byte unknown0000[4];
	short channel_count;
	short voice_count;
	short maximum_voice_count;
	byte unknown000a[2];
	s_sound_stream channels[0x80];
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

extern "C" long __stdcall IDirectSoundStream_GetStatus(
	void *stream, dword *status);
extern "C" long __stdcall IDirectSoundStream_Flush(void *stream);
void sound_stream_release_chunks(s_sound_stream *stream);
void sound_stream_stop(s_sound_stream *stream);

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, chunks) == 0x1c,
	"sound stream chunk-array offset");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");
static_assert(offsetof(s_sound_stream, codec) == 0x30,
	"sound stream codec offset");
static_assert(offsetof(s_driver_channels_view, channels) == 0x0c,
	"driver channel-array offset");

static inline void sound_driver_channel_halt(s_sound_stream *stream)
{
	dword status;
	stream->flushing = 1;
	IDirectSoundStream_GetStatus(stream->stream, &status);
	IDirectSoundStream_Flush(stream->stream);
	stream->codec->stop();
	stream->state = 0;
	sound_stream_release_chunks(stream);
	stream->flushing = 0;
}

// @retail 0x21f290
void function_21f290(void)
{
	s_driver_channels_view *globals =
		reinterpret_cast<s_driver_channels_view *>(g_51ebe4);
	for (long i = 0; i < globals->channel_count; i++)
	{
		s_sound_stream *stream = &globals->channels[i];
		sound_driver_channel_halt(stream);
		*reinterpret_cast<dword *>(&stream->unknown04[8]) = 0xc2800000;
		sound_stream_stop(stream);
	}
}

#endif
