// Exact asynchronous stream flush isolated from the Xbox DirectSound-heavy
// recovered unit.  Stream status, resume, envelope-aware flush, and codec
// stop remain genuine object operations.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSoundStream;
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
	virtual void chunk_finished(s_sound_stream *, void *, long) = 0;
	virtual void stop() = 0;
};

struct s_sound_stream
{
	byte unknown00;
	byte state_and_channels;
	byte chunks_and_started;
	byte flags;
	byte unknown04[0x20];
	IDirectSoundStream *stream;
	long unknown28;
	long offset;
	c_sound_stream_codec *codec;
};

extern "C" long __stdcall IDirectSoundStream_GetStatus(
	IDirectSoundStream *stream, dword *status);
extern "C" long __stdcall IDirectSoundStream_Pause(
	IDirectSoundStream *stream, dword pause);
extern "C" long __stdcall IDirectSoundStream_FlushEx(
	IDirectSoundStream *stream, long long timestamp, dword flags);

namespace
{
// Matching retail tests status bit 0x20000, resumes with argument zero, and
// submits combined asynchronous plus envelope flush flags as value three.
static const dword k_stream_status_paused = 0x00020000;
static const dword k_stream_pause_resume = 0;
static const dword k_stream_flush_async_envelope = 3;

static inline long stream_state(s_sound_stream const *stream)
{
	return stream->state_and_channels & 7;
}

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, state_and_channels) == 0x01,
	"sound stream state offset");
static_assert(offsetof(s_sound_stream, flags) == 0x03,
	"sound stream flags offset");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");
static_assert(offsetof(s_sound_stream, codec) == 0x30,
	"sound stream codec offset");
}

// @retail 0x2ae4b0
void sound_stream_flush(s_sound_stream *stream)
{
	if (stream_state(stream) != 0)
	{
		dword status;
		IDirectSoundStream_GetStatus(stream->stream, &status);
		if (status & k_stream_status_paused)
			IDirectSoundStream_Pause(stream->stream, k_stream_pause_resume);
		IDirectSoundStream_FlushEx(stream->stream, 0,
			k_stream_flush_async_envelope);
		stream->codec->stop();
		stream->flags |= 4;
	}
}

#endif
