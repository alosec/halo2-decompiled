// Exact dormant sound-stream construction policy isolated from the broader
// Xbox DirectSound and codec implementation.  This preserves genuine device
// and stream calls; it does not fabricate a DirectSound stream.

#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

struct DSSTREAMDESC
{
	dword dwFlags;
	void *lpwfxFormat;
	long dwMaxAttachedPackets;
	void *lpfnCallback;
	void *lpvContext;
	void *lpMixBins;
};

struct s_sound_stream;

class c_sound_stream_codec
{
public:
	virtual void attach(s_sound_stream *stream) = 0;
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
	byte unknown04[4];
	long unknown08;
	long unknown0c;
	byte unknown10[0xc];
	void *chunks[2];
	void *stream;
	long unknown28;
	long offset;
	c_sound_stream_codec *codec;
};

struct s_sound_globals_view
{
	byte unknown0000[0x2ab0];
	void *direct_sound;
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

extern "C" long __stdcall IDirectSound_CreateSoundStream(void *direct_sound,
	DSSTREAMDESC *description, void **stream, void *unknown);
extern "C" long __stdcall IDirectSoundStream_SetHeadroom(void *stream,
	long headroom);

static_assert(sizeof(DSSTREAMDESC) == 0x18, "DirectSound stream descriptor size");
static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, chunks) == 0x1c,
	"sound stream chunk-array offset");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");
static_assert(offsetof(s_sound_stream, codec) == 0x30,
	"sound stream codec offset");
static_assert(offsetof(s_sound_globals_view, direct_sound) == 0x2ab0,
	"DirectSound global offset");

// @retail 0x2ae660
bool sound_stream_create(s_sound_stream *stream, c_sound_stream_codec *codec,
	DSSTREAMDESC *description, short channel_count)
{
	stream->codec = codec;
	codec->attach(stream);
	description->dwFlags |= 0x20000000;
	void *direct_sound = ((s_sound_globals_view *)g_51ebe4)->direct_sound;
	bool success = false;
	if (IDirectSound_CreateSoundStream(direct_sound, description,
		&stream->stream, NULL) >= 0)
	{
		success = true;
	}
	IDirectSoundStream_SetHeadroom(stream->stream, 0);
	stream->channel_count = channel_count == 2 ? 1 : channel_count;
	stream->chunk_count = 0;
	stream->started_count = 0;
	stream->unknown28 = NONE;
	memset(stream->chunks, 0, sizeof(stream->chunks));

	return success;
}

#endif
