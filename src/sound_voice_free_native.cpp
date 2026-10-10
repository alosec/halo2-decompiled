#include "data_array.h"
#include "unknown_124f90.h"
#include "unknown_218850.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered voice teardown.  Stream flushing and driver-voice release
 * remain genuine backend boundaries. */

struct s_sound_permutation
{
	byte unknown00[0x0c];
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

struct s_sound_globals_chunks_view
{
	byte unknown00[0x44];
	s_sound_chunk *chunks;
};

struct s_sound_system_channels_view
{
	dword free_bits[2];
	byte unknown08[0x38];
	long first_channel[3];
	long last_channel[3];
	dword available_bits[2];
	dword streaming_bits[2];
	byte unknown68[8];
	dword used_bits[2];
};

struct s_sound_stream
{
	byte unknown00[0x38];
};

struct s_sound_driver_streams_view
{
	byte unknown00[0x0c];
	s_sound_stream streams[1];
};

struct s_4e6380;
struct s_sound_globals;
struct s_bink_sound_settings;
extern s_4e6380 *g_4e6380;
extern s_sound_voice *g_4e6378;
extern s_sound_globals *g_51ebd4;
extern s_bink_sound_settings *g_51ebe4;

void sound_stream_flush(s_sound_stream *stream);
void sound_voice_release(long voice_index);

static_assert(sizeof(s_sound_voice) == 0x24, "sound voice size");
static_assert(offsetof(s_sound_voice, stream_reset) == 0x09,
	"sound voice reset offset");
static_assert(offsetof(s_sound_voice, channel_index) == 0x0c,
	"sound voice channel offset");
static_assert(offsetof(s_sound_voice, chunk_index) == 0x18,
	"sound voice chunk offset");
static_assert(offsetof(s_sound_voice, permutation) == 0x1c,
	"sound voice permutation offset");
static_assert(sizeof(s_sound_stream) == 0x38, "sound stream size");
static_assert(offsetof(s_sound_globals_chunks_view, chunks) == 0x44,
	"sound globals chunks offset");
static_assert(offsetof(s_sound_system_channels_view, streaming_bits) == 0x60,
	"sound streaming bits offset");
static_assert(offsetof(s_sound_driver_streams_view, streams) == 0x0c,
	"sound driver streams offset");

// @retail 0x12a5d0
void sound_voice_free(short voice_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	s_sound_chunk *chunks =
		reinterpret_cast<s_sound_globals_chunks_view *>(g_51ebd4)->chunks;
	if (voice->next_permutation)
	{
		s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(
			chunks[voice->next_permutation->first_chunk +
				voice->next_chunk_index].cache_index);
		entry->lock_count--;
		voice->next_permutation = NULL;
	}
	if (voice->permutation)
	{
		s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(
			chunks[voice->permutation->first_chunk +
				voice->chunk_index].cache_index);
		entry->lock_count--;
		voice->permutation = NULL;
	}
	if (voice->channel_index != NONE)
	{
		short const channel_index = voice->channel_index;
		sound_stream_flush(
			&reinterpret_cast<s_sound_driver_streams_view *>(g_51ebe4)->
				streams[channel_index]);
		reinterpret_cast<s_sound_system_channels_view *>(g_4e6380)->
			streaming_bits[channel_index >> 5] &=
			~(1u << (channel_index & 31));
	}
	if (voice->driver_voice_index != NONE)
		sound_voice_release(voice->driver_voice_index);
	voice->unknown0a = 0;
	voice->unknown0b = 0;
	voice->stream_reset = false;
	voice->channel_index = NONE;
	voice->unknown0e = NONE;
	voice->driver_voice_index = NONE;
}

#endif
