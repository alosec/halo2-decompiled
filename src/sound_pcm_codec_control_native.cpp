// Exact compact PCM codec submission policy isolated from the broader Xbox
// DirectSound stream implementation.  This reads genuine queue state only;
// it does not fabricate a packet or stream progress.

#include "unknown_11c920.h"
#include "unknown_218850.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_cache_usage
{
	dword flags[0x90];
	byte counts[1];
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
	byte unknown03[0x19];
	s_sound_chunk *chunks[2];
	byte unknown24[0x10];
};

class c_pcm_codec
{
public:
	virtual void mark_cache_usage(s_sound_stream *stream,
		s_sound_cache_usage *usage);
	virtual bool can_submit(s_sound_stream *stream, bool submitted);
};

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, chunks) == 0x1c,
	"sound stream chunk-array offset");
static_assert(sizeof(c_pcm_codec) == 0x04, "PCM codec size");

// @retail 0x2ae940
void c_pcm_codec::mark_cache_usage(s_sound_stream *stream,
	s_sound_cache_usage *usage)
{
	for (long index = 0; index < stream->chunk_count; index++)
	{
		if (index < stream->started_count)
		{
			s_sound_chunk *chunk = stream->chunks[index];
			long cache_index = chunk->cache_index & 0xffff;
			usage->counts[cache_index]++;
			usage->flags[cache_index >> 5] |= 1 << (cache_index & 0x1f);
		}
	}
}

// @retail 0x2aeb00
bool c_pcm_codec::can_submit(s_sound_stream *stream, bool submitted)
{
	(void)submitted;
	return stream->chunk_count > stream->started_count;
}

#endif
