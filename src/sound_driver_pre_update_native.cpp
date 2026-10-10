// Exact dormant sound-driver pre-update isolated from the broader Xbox
// driver source.  Processor recovery, DirectSound work, stopped-stream
// detection, and immediate stream teardown remain genuine operations.

#include "unknown_11c920.h"
#include "unknown_218850.h"
#include <stddef.h>
#include <string.h>

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

struct s_driver_pre_update_view
{
	byte unknown0000[4];
	short channel_count;
	byte unknown0006[6];
	s_sound_stream channels[0x80];
	byte unknown1a0c[0xffc];
	bool reverb_dirty[2];
	byte unknown2a0a[0xca];
	dword panic_count;
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
extern "C" dword g_dwDirectSoundDeltaPanicCount;

void function_1915f0(void);
extern "C" void __stdcall DirectSoundDoWork(void);
extern "C" long __stdcall IDirectSoundStream_GetStatus(
	void *stream, dword *status);
extern "C" long __stdcall IDirectSoundStream_Flush(void *stream);
void sound_stream_release_chunks(s_sound_stream *stream);

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");
static_assert(offsetof(s_sound_stream, codec) == 0x30,
	"sound stream codec offset");
static_assert(sizeof(s_driver_pre_update_view) == 0x2ad8,
	"sound driver globals size");
static_assert(offsetof(s_driver_pre_update_view, channels) == 0x0c,
	"driver channel-array offset");
static_assert(offsetof(s_driver_pre_update_view, reverb_dirty) == 0x2a08,
	"driver reverb-dirty offset");
static_assert(offsetof(s_driver_pre_update_view, panic_count) == 0x2ad4,
	"driver panic-count offset");

static inline void sound_driver_flush_stopped_stream(s_sound_stream *stream)
{
	IDirectSoundStream_Flush(stream->stream);
	stream->codec->stop();
	stream->state = 0;
	sound_stream_release_chunks(stream);
	stream->flushing = 0;
}

// @retail 0x21ec00
void function_21ec00(void)
{
	s_driver_pre_update_view *globals =
		reinterpret_cast<s_driver_pre_update_view *>(g_51ebe4);
	if (globals->panic_count != g_dwDirectSoundDeltaPanicCount)
	{
		function_1915f0();
		memset(globals->reverb_dirty, true, sizeof(globals->reverb_dirty));
		globals->panic_count = g_dwDirectSoundDeltaPanicCount;
	}

	DirectSoundDoWork();

	for (long i = 0; i < globals->channel_count; i++)
	{
		s_sound_stream *stream = &globals->channels[i];
		if (stream->state == 1)
		{
			dword status;
			IDirectSoundStream_GetStatus(stream->stream, &status);
			volatile bool stopped = !((status >> 16) & 1);
			if (stopped)
				sound_driver_flush_stopped_stream(stream);
		}

		// Retail's trailing four-way switch has four empty cases.  Retain
		// its volatile state observation without assigning invented work.
		switch (reinterpret_cast<s_sound_stream const volatile *>(stream)->state)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			break;
		}
	}
}

#endif
