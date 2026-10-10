#include "data_array.h"
#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered looping-sound controller reference release. */

struct s_looping_sound_controller
{
	short salt;
	byte reference_count : 7;
	byte source_updated : 1;
	byte synch_count : 7;
	byte synch_playback : 1;
	long definition_index;
	dword random_state;
	real listener_values[4];
};

static_assert(sizeof(s_looping_sound_controller) == 0x1c,
	"looping controller size");
static_assert(offsetof(s_looping_sound_controller, definition_index) == 0x04,
	"looping controller definition offset");

extern void *g_51ebd8;

// @retail 0x219c80
void looping_sound_controller_release(long index)
{
	if (index != NONE)
	{
		s_record_pool *pool = reinterpret_cast<s_record_pool *>(g_51ebd8);
		s_looping_sound_controller *controller =
			reinterpret_cast<s_looping_sound_controller *>(pool->data) +
			(index & 0xffff);
		if (--controller->reference_count == 0)
			record_pool_release(pool, index);
	}
}

#endif
