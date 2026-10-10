#include "data_array.h"
#include "physical_memory.h"
#include "unknown_218850.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered sound-cache page and record allocation.  The physical
 * allocator and asynchronous cache-file read route remain genuine providers;
 * this does not fabricate loaded completion. */

static_assert(sizeof(s_sound_chunk) == 0x0c, "sound chunk size");
static_assert(sizeof(s_sound_cache_entry) == 0x10,
	"sound cache entry size");
static_assert(offsetof(s_sound_cache_entry, loaded) == 0x02,
	"sound cache loaded offset");
static_assert(offsetof(s_sound_cache_entry, lock_count) == 0x04,
	"sound cache lock offset");
static_assert(offsetof(s_sound_cache_entry, owner) == 0x08,
	"sound cache owner offset");
static_assert(offsetof(s_sound_cache_entry, chunk) == 0x0c,
	"sound cache chunk offset");
static_assert(sizeof(s_sound_cache_allocator) == 0x68,
	"sound cache allocator prefix size");
static_assert(offsetof(s_sound_cache_allocator, page_shift) == 0x34,
	"sound cache page-shift offset");
static_assert(offsetof(s_sound_cache_allocator, pages) == 0x64,
	"sound cache page-pool offset");

long function_213760(dword location, long size, void *buffer,
	dword *bytes_read, bool *done, long type, long priority);

// @retail 0x218a10
void function_218a10(s_sound_chunk *chunk, long owner)
{
	long const index = function_13d370(
		reinterpret_cast<s_physical_object *>(g_50210c),
		SOUND_CHUNK_SIZE(chunk), 0);
	if (index == NONE)
		return;

	s_sound_cache_page *pages = reinterpret_cast<s_sound_cache_page *>(
		g_50210c->pages->data);
	byte *buffer = reinterpret_cast<byte *>(
		(pages[index & 0xffff].offset << g_50210c->page_shift) + g_502108);
	datum_new_at_index_with_salt(g_502104, index);
	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(index);
	chunk->cache_index = index;
	entry->owner = owner;
	entry->chunk = chunk;
	entry->lock_count = 0;
	entry->reference_count = 0;
	entry->used = 0;

	dword size = SOUND_CHUNK_SIZE(chunk);
	if (size & 0x1ff)
		size = (size | 0x1ff) + 1;
	function_213760(chunk->file_offset, size, buffer, NULL,
		(bool *)&entry->loaded, 5, 4);
}

#endif
