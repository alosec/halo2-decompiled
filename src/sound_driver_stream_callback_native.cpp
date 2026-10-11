// Exact dormant DirectSound stream completion callback isolated from the
// broader Xbox driver source.  Codec completion and packet pumping remain
// genuine stream lifetime operations; no completion is fabricated.

#include "unknown_11c920.h"
#include "unknown_218850.h"
#include <stddef.h>
#include <stdint.h>

#ifdef H2_NATIVE_PORT

struct s_sound_stream;

class c_sound_stream_codec
{
public:
	virtual void attach(s_sound_stream *) = 0;
	virtual void start(s_sound_stream *) = 0;
	virtual void mark_cache_usage(s_sound_stream *, void *) = 0;
	virtual bool get_packet(s_sound_stream *, void *) = 0;
	virtual void packet_submitted(s_sound_stream *, void *) = 0;
	virtual void packet_failed(s_sound_stream *, void *) = 0;
	virtual void chunk_finished(s_sound_stream *, s_sound_chunk *, long) = 0;
	virtual void stop() = 0;
};

struct s_sound_stream
{
	byte unknown00;
	byte state_and_channels;
	byte chunks_and_started;
	byte flags;
	byte unknown04[0x18];
	s_sound_chunk *chunks[2];
	void *stream;
	long unknown28;
	long offset;
	c_sound_stream_codec *codec;
};

struct s_driver_callback_view
{
	byte unknown0000[4];
	short channel_count;
	byte unknown0006[6];
	s_sound_stream channels[1];
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

void sound_stream_update(s_sound_stream *stream);

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, flags) == 0x03,
	"sound stream callback flags offset");
static_assert(offsetof(s_sound_stream, codec) == 0x30,
	"sound stream callback codec offset");
static_assert(offsetof(s_driver_callback_view, channel_count) == 0x04,
	"driver callback channel-count offset");
static_assert(offsetof(s_driver_callback_view, channels) == 0x0c,
	"driver callback channel-array offset");

// @retail 0x220730
void __stdcall sound_driver_stream_callback(
	void *stream_context,
	void *packet_context,
	dword status)
{
	static const dword k_packet_status_success = 0;
	static const byte k_stream_flush_or_transient = 0x0c;

	if (packet_context)
	{
		long channel_index = (long)(uintptr_t)stream_context;
		s_driver_callback_view *globals =
			reinterpret_cast<s_driver_callback_view *>(g_51ebe4);
		if (channel_index >= 0 && channel_index < globals->channel_count)
		{
			s_sound_stream *stream = &globals->channels[channel_index];
			stream->codec->chunk_finished(stream,
				reinterpret_cast<s_sound_chunk *>(packet_context), status);
			if (status == k_packet_status_success &&
				!(stream->flags & k_stream_flush_or_transient))
			{
				sound_stream_update(stream);
			}
		}
	}
}

#endif
