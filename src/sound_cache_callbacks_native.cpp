#include "data_array.h"
#include "unknown_218850.h"

#ifdef H2_NATIVE_PORT

/* Exact recovered sound physical-cache busy and deletion callbacks. */

// @retail 0x2189a0
bool __stdcall sound_cache_entry_busy(long entry_index)
{
	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(entry_index);
	if (entry->lock_count || entry->reference_count)
		return true;
	return !entry->loaded;
}

// @retail 0x2189e0
void __stdcall sound_cache_entry_delete(long entry_index)
{
	SOUND_CACHE_ENTRY(entry_index)->chunk->cache_index = NONE;
	record_pool_release(g_502104, entry_index);
}

#endif
