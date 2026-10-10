// Exact cached-chunk release isolated from the broader Xbox DirectSound
// stream implementation.  This owns queue/cache bookkeeping only; it does
// not fabricate stream progress, cache records, or a decoder backend.

#include "unknown_218850.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

class c_sound_stream_codec;

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
	byte unknown04[4];
	long unknown08;
	long unknown0c;
	byte unknown10[0xc];
	s_sound_chunk *chunks[2];
	void *stream;
	long unknown28;
	long offset;
	c_sound_stream_codec *codec;
};

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, chunks) == 0x1c,
	"sound stream chunk-array offset");

// @retail 0x2ae590
void sound_stream_release_chunk(s_sound_stream *stream, s_sound_chunk *chunk)
{
	stream->started_count--;
	stream->chunk_count--;
	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(chunk->cache_index);
	entry->lock_count--;
	memmove(&stream->chunks[0], &stream->chunks[1],
		stream->chunk_count * sizeof(s_sound_chunk *));
	memset(&stream->chunks[stream->chunk_count], 0,
		(2 - stream->chunk_count) * sizeof(s_sound_chunk *));

	if (stream->chunk_count == 0)
		stream->state = 1;
	else if (stream->chunk_count == 1)
		stream->state = 2;
}

#endif
