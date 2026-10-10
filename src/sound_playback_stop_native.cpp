#include "data_array.h"
#include "unknown_124f90.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered playing-sound stop sequence. */

static_assert(sizeof(s_sound_playback) == 0xbc, "sound playback size");
static_assert(offsetof(s_sound_playback, object_index) == 0x10,
	"sound playback object offset");
static_assert(offsetof(s_sound_playback, source) == 0x14,
	"sound playback source offset");
static_assert(offsetof(s_sound_playback, value_ac) == 0xac,
	"sound playback voice offset");

extern s_record_pool *g_4e637c;

void sound_voice_free(short voice_index);
void function_126960(s_sound_playback *sound);
void sound_playback_delete(long sound_index);

static s_sound_playback *sound_playback_get(long sound_index)
{
	return reinterpret_cast<s_sound_playback *>(g_4e637c->data) +
		(sound_index & 0xffff);
}

// @retail 0x127320
void __stdcall function_127320(long sound_index, long reason)
{
	s_sound_playback *sound = sound_playback_get(sound_index);
	if (sound->value_ac != NONE)
	{
		sound_voice_free(sound->value_ac);
		sound->value_ac = NONE;
	}
	function_126960(sound);
	if (sound->source && sound->source->stop)
		sound->source->stop(sound->object_index, sound_index, reason);
	sound_playback_delete(sound_index);
}

#endif
