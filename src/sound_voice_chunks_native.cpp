#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered top-level queued-chunk traversal.  Per-voice stream/cache
 * mutation remains a genuine deeper provider and mutable-lifetime gate. */

struct s_4e6380
{
	byte unknown00[0x208];
	short voice_count;
};

static_assert(offsetof(s_4e6380, voice_count) == 0x208,
	"sound voice count offset");

extern s_4e6380 *g_4e6380;
void sound_voice_update_chunks(short voice_index);

// @retail 0x1290d0
void sound_voices_update_chunks(void)
{
	for (short voice_index = 0; voice_index < g_4e6380->voice_count;
		voice_index++)
	{
		sound_voice_update_chunks(voice_index);
	}
}

#endif
