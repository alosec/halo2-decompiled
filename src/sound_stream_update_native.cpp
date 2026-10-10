// Exact dormant sound-stream packet pump isolated from the broader Xbox
// DirectSound implementation.  This preserves genuine stream status,
// submission, discontinuity, and codec state transitions.

#include "unknown_11c920.h"
#include "unknown_218850.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_stream;
struct s_sound_cache_usage;

struct XMEDIAPACKET
{
	void *pvBuffer;
	dword dwMaxSize;
	dword *pdwCompletedSize;
	dword *pdwStatus;
	void *hCompletionEvent;
	long long *prtTimestamp;
};

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
	byte unknown03;
	byte unknown04[0x18];
	s_sound_chunk *chunks[2];
	void *stream;
	long unknown28;
	long offset;
	c_sound_stream_codec *codec;
};

extern "C" long __stdcall IDirectSoundStream_GetStatus(void *stream,
	dword *status);
extern "C" long __stdcall IDirectSoundStream_Process(void *stream,
	XMEDIAPACKET *input, XMEDIAPACKET *output);
extern "C" long __stdcall IDirectSoundStream_Discontinuity(void *stream);

static const dword k_direct_sound_stream_status_ready = 1;

static_assert(sizeof(XMEDIAPACKET) == 0x18, "media packet size");
static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");
static_assert(offsetof(s_sound_stream, codec) == 0x30,
	"sound stream codec offset");

// @retail 0x2ae820
void sound_stream_update(s_sound_stream *stream)
{
	bool submitted = false;
	bool const *submitted_reference = &submitted;
	bool success;
	bool ready;
	bool const *ready_reference = &ready;

	do
	{
		dword status;
		XMEDIAPACKET packet;

		success = false;
		if (((1 << stream->state) & ((1 << 2) | (1 << 3))) &&
			IDirectSoundStream_GetStatus(stream->stream, &status) >= 0)
		{
			ready = (status & k_direct_sound_stream_status_ready) != 0;
			if (*ready_reference &&
				stream->codec->can_submit(stream, *submitted_reference) &&
				stream->codec->get_packet(stream, &packet))
			{
				success = IDirectSoundStream_Process(stream->stream,
					&packet, NULL) >= 0;
				if (success)
					stream->codec->packet_submitted(stream, &packet);
				else
					stream->codec->packet_failed(stream, &packet);
				if (success)
					submitted = true;
			}
		}
	}
	while (success);

	if (submitted)
		IDirectSoundStream_Discontinuity(stream->stream);
}

#endif
