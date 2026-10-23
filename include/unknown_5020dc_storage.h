#ifndef UNKNOWN_5020DC_STORAGE_H
#define UNKNOWN_5020DC_STORAGE_H

#include "job_queue.h"
#include <stddef.h>

/* the game state's cache-file copies (0x5020dc, unknown_214f10.cpp): one
   global, so stores into it reload the lock's address */
struct s_game_state_cache
{
	dword memory;
	long size;
	long lock;
	bool volatile write_done;
	byte unknown_d[0x3];
	dword bytes_written;
	short slot;
	bool slot_valid[2];
	bool files_open;
	byte unknown_19[0x3];
	s_file_handle files[2];
};

extern s_game_state_cache g_game_state_cache;
#define g_5020dc (g_game_state_cache.memory)
#define g_5020e0 (g_game_state_cache.size)
#define g_5020e4 (g_game_state_cache.lock)
#define g_5020e8 (g_game_state_cache.write_done)
#define g_5020ec (g_game_state_cache.bytes_written)
#define g_5020f0 (g_game_state_cache.slot)
#define g_5020f2 (g_game_state_cache.slot_valid)
#define g_5020f4 (g_game_state_cache.files_open)
#define g_5020f8 (g_game_state_cache.files)
typedef char assert_game_state_cache_offset_1c[(offsetof(s_game_state_cache, files) == 0x1c) ? 1 : -1];
typedef char assert_game_state_cache_size[(sizeof(s_game_state_cache) == 0x24) ? 1 : -1];

#endif
