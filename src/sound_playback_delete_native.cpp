#include "data_array.h"
#include "unknown_124f90.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered playing-sound record deletion. */

struct s_sound_controller_view
{
	byte unknown00[3];
	byte playing_count;
	long unknown04;
	byte unknown08[0x14];
};

static_assert(sizeof(s_sound_controller_view) == 0x1c,
	"sound controller size");
static_assert(offsetof(s_sound_controller_view, playing_count) == 0x03,
	"sound controller playing-count offset");
static_assert(offsetof(s_sound_controller_view, unknown04) == 0x04,
	"sound controller state offset");

extern s_record_pool *g_4e637c;
extern void *g_51ebd8;

void looping_sound_controller_release(long index);

static s_sound_playback *sound_playback_get(long sound_index)
{
	return reinterpret_cast<s_sound_playback *>(g_4e637c->data) +
		(sound_index & 0xffff);
}

// @retail 0x127390
void sound_playback_delete(long sound_index)
{
	s_sound_playback *sound = sound_playback_get(sound_index);
	if (sound->effect_index != NONE)
	{
		s_record_pool *controller_pool =
			reinterpret_cast<s_record_pool *>(g_51ebd8);
		s_sound_controller_view *controller =
			reinterpret_cast<s_sound_controller_view *>(controller_pool->data) +
			(sound->effect_index & 0xffff);
		if (sound->unknown03 == NONE && controller->unknown04 != NONE)
			controller->playing_count = (controller->playing_count - 1) & 0x7f;
		looping_sound_controller_release(sound->effect_index);
	}
	record_pool_release(g_4e637c, sound_index);
}

#endif
