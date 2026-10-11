#include "data_array.h"
#include "unknown_124f90.h"
#include "unknown_218850.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

struct s_sound_stream;

struct s_sound_permutation
{
	byte unknown00[0xc];
	short first_chunk;
	byte unknown0e[2];
};

struct s_sound_voice
{
	long sound_index;
	long driver_voice_index;
	byte definition_type;
	bool stream_reset;
	byte unknown0a;
	byte unknown0b;
	short channel_index;
	short unknown0e;
	real unknown10;
	byte unknown14[4];
	short chunk_index;
	short next_chunk_index;
	s_sound_permutation const *permutation;
	s_sound_permutation const *next_permutation;
};

namespace
{
struct s_sound_globals_chunks_view
{
	byte unknown00[0x24];
	void *pitch_ranges;
	byte unknown28[4];
	s_sound_permutation *permutations;
	byte unknown30[0x14];
	s_sound_chunk *chunks;
};

struct s_sound_driver_streams_view
{
	byte unknown00[0xc];
	byte streams[1];
};

static_assert(sizeof(s_sound_permutation) == 0x10,
	"sound permutation size");
static_assert(offsetof(s_sound_permutation, first_chunk) == 0xc,
	"sound permutation first chunk offset");
static_assert(sizeof(s_sound_voice) == 0x24, "sound voice size");
static_assert(offsetof(s_sound_voice, channel_index) == 0xc,
	"sound voice channel offset");
static_assert(offsetof(s_sound_voice, unknown10) == 0x10,
	"sound voice progress offset");
static_assert(offsetof(s_sound_voice, chunk_index) == 0x18,
	"sound voice chunk offset");
static_assert(offsetof(s_sound_voice, permutation) == 0x1c,
	"sound voice permutation offset");
static_assert(sizeof(s_sound_playback) == 0xbc, "sound playback size");
static_assert(offsetof(s_sound_playback, definition_index) == 0xc,
	"sound playback definition offset");
static_assert(offsetof(s_sound_playback, pitch_range_index) == 0x9c,
	"sound playback pitch-range offset");
static_assert(offsetof(s_sound_playback, chunk_index) == 0x9e,
	"sound playback chunk offset");
static_assert(offsetof(s_sound_globals_chunks_view, permutations) == 0x2c,
	"sound permutation table offset");
static_assert(offsetof(s_sound_globals_chunks_view, chunks) == 0x44,
	"sound chunk table offset");
static_assert(offsetof(s_sound_driver_streams_view, streams) == 0xc,
	"driver streams offset");
}

extern s_sound_voice *g_4e6378;
struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

void function_126960(s_sound_playback *sound);
void sound_stream_add_chunk(s_sound_stream *stream, s_sound_chunk *chunk);

// @retail 0x1286b0
void sound_playback_set_chunk(long sound_index, long definition_index,
	char pitch_range_index, char permutation_index, short chunk_index)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	function_126960(sound);
	sound->definition_index = definition_index;
	sound->pitch_range_index = pitch_range_index;
	sound->permutation_index = permutation_index;
	sound->chunk_index = chunk_index;
}

// @retail 0x129f20
void sound_voice_queue_chunk(short voice_index,
	s_sound_permutation const *permutation, short chunk_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	s_sound_globals_chunks_view *tables =
		reinterpret_cast<s_sound_globals_chunks_view *>(g_51ebd4);
	s_sound_chunk *chunk =
		&tables->chunks[permutation->first_chunk + chunk_index];

	if (voice->next_permutation)
	{
		s_sound_chunk *next_chunk = &tables->chunks[
			voice->next_permutation->first_chunk + voice->next_chunk_index];
		s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(next_chunk->cache_index);
		entry->lock_count--;
		voice->next_chunk_index = NONE;
	}
	s_sound_driver_streams_view *driver =
		reinterpret_cast<s_sound_driver_streams_view *>(g_51ebe4);
	sound_stream_add_chunk(reinterpret_cast<s_sound_stream *>(
		&driver->streams[voice->channel_index * 0x34]), chunk);
	function_218850(NONE, chunk, 4);
	if (voice->permutation)
	{
		voice->next_chunk_index = chunk_index;
		voice->next_permutation = permutation;
	}
	else
	{
		voice->chunk_index = chunk_index;
		voice->permutation = permutation;
		voice->unknown10 = 0.0f;
	}
}

#endif
