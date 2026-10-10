#include "data_array.h"
#include "unknown_124f90.h"
#include "unknown_218ac0.h"
#include "unknown_218850.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered queued-chunk transition for one voice.  Cache requests,
 * reference release, stream state, and driver playback-rate reporting remain
 * genuine dependencies; this source does not invent stream progress. */

struct s_4e6380
{
	byte unknown00[0x1e0];
	real elapsed_time;
};

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

struct s_sound_reference_holder
{
	byte unknown00[8];
	long reference_index;
};

struct s_sound_globals_chunks_view
{
	byte unknown00[0x24];
	struct
	{
		byte unknown00[8];
		short first_permutation;
		short count;
	} *pitch_ranges;
	byte unknown28[4];
	s_sound_permutation *permutations;
	byte unknown30[0x14];
	s_sound_chunk *chunks;
};

struct s_sound_stream_view
{
	byte unknown00;
	char state : 3;
	char channel_count : 3;
	char unknown01_6 : 2;
	byte unknown02[0x36];
};

struct s_sound_driver_streams_view
{
	byte unknown00[0x0c];
	s_sound_stream_view streams[1];
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
struct s_permutation_group;

static_assert(sizeof(s_sound_voice) == 0x24, "sound voice size");
static_assert(offsetof(s_sound_voice, stream_reset) == 0x09,
	"sound voice reset offset");
static_assert(offsetof(s_sound_voice, channel_index) == 0x0c,
	"sound voice channel offset");
static_assert(offsetof(s_sound_voice, unknown10) == 0x10,
	"sound voice elapsed offset");
static_assert(offsetof(s_sound_voice, permutation) == 0x1c,
	"sound voice permutation offset");
static_assert(sizeof(s_sound_permutation) == 0x10,
	"sound permutation size");
static_assert(offsetof(s_sound_permutation, first_chunk) == 0x0c,
	"sound permutation chunk offset");
static_assert(sizeof(s_sound_stream_view) == 0x38, "sound stream size");
static_assert(offsetof(s_4e6380, elapsed_time) == 0x1e0,
	"sound elapsed time offset");
static_assert(offsetof(s_sound_globals_chunks_view, chunks) == 0x44,
	"sound globals chunks offset");

extern s_4e6380 *g_4e6380;
extern s_sound_voice *g_4e6378;
extern s_record_pool *g_4e637c;

void sound_reference_release(s_sound_reference_holder *holder);
long function_219290(short index, s_permutation_group *group);
real function_21f650(long channel_index, long mode);

static s_sound_globals_chunks_view *sound_chunk_tables(void)
{
	return reinterpret_cast<s_sound_globals_chunks_view *>(g_51ebd4);
}

static s_sound_driver_streams_view *sound_driver_streams(void)
{
	return reinterpret_cast<s_sound_driver_streams_view *>(g_51ebe4);
}

// @retail 0x12a450
void sound_voice_update_chunks(short voice_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	if (voice->channel_index == NONE || voice->stream_reset || !voice->permutation)
		return;

	short queued = sound_driver_streams()->streams[voice->channel_index].state;
	if (voice->next_permutation && queued < 3)
	{
		s_sound_globals_chunks_view *tables = sound_chunk_tables();
		sound_reference_release(reinterpret_cast<s_sound_reference_holder *>(
			&tables->chunks[voice->permutation->first_chunk + voice->chunk_index]));
		if (static_cast<short>(function_219290(voice->chunk_index,
			reinterpret_cast<s_permutation_group *>(
				const_cast<s_sound_permutation *>(voice->permutation)))) == NONE)
		{
			voice->unknown10 = 0.0f;
		}
		voice->permutation = voice->next_permutation;
		voice->chunk_index = voice->next_chunk_index;
		voice->next_permutation = NULL;
		voice->next_chunk_index = NONE;
		dword const request = function_218850(NONE,
			&tables->chunks[voice->permutation->first_chunk + voice->chunk_index], 0);
		if (!(request & 2))
			queued = 0;
	}
	if (voice->permutation && queued < 2)
	{
		sound_reference_release(reinterpret_cast<s_sound_reference_holder *>(
			&sound_chunk_tables()->chunks[
				voice->permutation->first_chunk + voice->chunk_index]));
		voice->permutation = NULL;
		voice->chunk_index = NONE;
	}
	if (sound_driver_streams()->streams[voice->channel_index].state > 0)
	{
		s_sound_playback *sound =
			reinterpret_cast<s_sound_playback *>(g_4e637c->data) +
			(voice->sound_index & 0xffff);
		s_sound_definition *definition = sound_definition_get(sound->definition_index);
		voice->unknown10 += function_21f650(voice->channel_index,
			static_cast<char>(definition->unknown03)) * g_4e6380->elapsed_time;
	}
}

#endif
