#include "unknown_218850.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered sound-cache request state machine.  Allocation/read
 * submission remains the genuine function_218a10 dependency, and blocking
 * waits remain cooperative rather than being reported complete early. */

static_assert(sizeof(s_sound_chunk) == 0x0c, "sound chunk size");
static_assert(offsetof(s_sound_chunk, cache_index) == 0x08,
	"sound chunk cache index offset");
static_assert(sizeof(s_sound_cache_entry) == 0x10,
	"sound cache entry size");
static_assert(offsetof(s_sound_cache_entry, loaded) == 0x02,
	"sound cache loaded offset");
static_assert(offsetof(s_sound_cache_entry, used) == 0x03,
	"sound cache used offset");
static_assert(offsetof(s_sound_cache_entry, lock_count) == 0x04,
	"sound cache lock offset");
static_assert(sizeof(s_sound_cache_page) == 0x18,
	"sound cache page size");
static_assert(offsetof(s_sound_cache_page, last_used) == 0x14,
	"sound cache page time offset");
static_assert(offsetof(s_sound_cache_allocator, time) == 0x38,
	"sound cache allocator time offset");
static_assert(offsetof(s_sound_cache_allocator, pages) == 0x64,
	"sound cache allocator pages offset");

void function_218a10(s_sound_chunk *chunk, long owner);
extern "C" int __stdcall SwitchToThread(void);

static void sound_cache_page_touch(s_sound_cache_allocator *allocator,
	long index)
{
	s_sound_cache_page *pages =
		reinterpret_cast<s_sound_cache_page *>(allocator->pages->data);
	pages[index & 0xffff].last_used = allocator->time;
}

// @retail 0x218850 standard
dword __stdcall function_218850(long owner, s_sound_chunk *sound, dword flags)
{
	dword result = 0;
	bool const block = (flags & 1) != 0;
	bool const load = ((flags >> 1) & 1) != 0;
	bool const lock = ((flags >> 2) & 1) != 0;

	if (sound->cache_index == NONE && owner != NONE && load)
		function_218a10(sound, owner);

	if (sound->cache_index != NONE)
	{
		sound_cache_page_touch(g_50210c, sound->cache_index);
		s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(sound->cache_index);
		if (block && !entry->loaded)
		{
			while (!entry->loaded)
				SwitchToThread();
		}
		if (entry->loaded)
		{
			if (!entry->used)
				entry->used = 1;
			result = 2;
		}
		if (lock)
		{
			entry->lock_count++;
			result |= 4;
		}
	}

	if (!(result & 2) && sound->cache_index != NONE)
		result |= 1;
	else
		result &= ~1u;
	return result;
}

#endif
