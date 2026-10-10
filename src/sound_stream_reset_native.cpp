// Exact dormant sound-stream reset policy isolated from the broader Xbox
// DirectSound implementation.  Halt and flush continue to perform genuine
// backend and codec operations.

#include "unknown_11c920.h"
#include "unknown_218850.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_stream;

class c_sound_stream_codec
{
public:
	virtual void attach(s_sound_stream *stream) = 0;
	virtual void start(s_sound_stream *stream) = 0;
	virtual void mark_cache_usage(s_sound_stream *stream, void *usage) = 0;
	virtual bool get_packet(s_sound_stream *stream, void *packet) = 0;
	virtual void packet_submitted(s_sound_stream *stream, void *packet) = 0;
	virtual void packet_failed(s_sound_stream *stream, void *packet) = 0;
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
	byte unknown04[0x20];
	void *stream;
	long unknown28;
	long offset;
	c_sound_stream_codec *codec;
};

extern "C" long __stdcall IDirectSoundStream_GetStatus(void *stream,
	dword *status);
extern "C" long __stdcall IDirectSoundStream_Flush(void *stream);
void sound_stream_release_chunks(s_sound_stream *stream);
void sound_stream_flush(s_sound_stream *stream);
void sound_stream_set_envelope(s_sound_stream *stream, real attack,
	real release);

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");
static_assert(offsetof(s_sound_stream, codec) == 0x30,
	"sound stream codec offset");

static inline void sound_stream_halt(s_sound_stream *stream)
{
	if (stream->state == 1)
	{
		dword status;
		IDirectSoundStream_GetStatus(stream->stream, &status);
		IDirectSoundStream_Flush(stream->stream);
		stream->codec->stop();
		stream->state = 0;
		sound_stream_release_chunks(stream);
		stream->flushing = 0;
	}
}

// @retail 0x2ae750
void sound_stream_reset(s_sound_stream *stream)
{
	sound_stream_set_envelope(stream, 0.0f, 0.25f);
	sound_stream_halt(stream);
	sound_stream_flush(stream);
}

#endif
