#include "async.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered cache-file selector shared by texture and sound reads.  It
 * preserves the live cache handles and delegates asynchronous ownership to
 * the genuine recovered task submission path. */

struct s_texture_cache_file
{
	HANDLE handle;
	byte unknown04[0x800];
};

static_assert(sizeof(s_texture_cache_file) == 0x804,
	"cache-file record size");
static_assert(offsetof(s_texture_cache_file, handle) == 0x00,
	"cache-file handle offset");

extern s_texture_cache_file g_557c90[6];
extern long g_55aca8;

// @retail 0x213760
long function_213760(dword location, long size, void *buffer,
	dword *bytes_read, bool *done, long type, long priority)
{
	long file_index = g_55aca8;
	long const selector = location >> 30;
	if (selector == 1)
		file_index = 2;
	else if (selector == 2)
		file_index = 0;
	else if (selector == 3)
		file_index = 1;

	s_file_handle file = { g_557c90[file_index].handle };
	return function_1a0f10(file, buffer, size, location & 0x3fffffff, type,
		priority, bytes_read, done);
}

#endif
