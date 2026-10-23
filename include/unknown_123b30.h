/* UNKNOWN_123B30.H: the game state allocator */

#ifndef UNKNOWN_123B30_H
#define UNKNOWN_123B30_H

/* the header at the start of the game state (0x1288 bytes) */
struct s_arena_header
{
	long checksum;
	long base_address;
	char map_name[0x100];
	char version[0x20];
	dword unknown128;
	dword unknown12c;
	long field_130_2[0x446];
	short unknown1248;
	byte unknown124a[0x1288 - 0x124a];
};

/* game_state_globals (0x4e3b60, 0x2540 bytes): two saved copies of the
   header, then the allocator */
struct s_game_state_globals
{
	bool initialized;
	bool flag1;
	bool flag2;
	bool flag3;
	short slot;
	short unknown6;
	s_arena_header saved_headers[2];
	dword time;
	short counter;
	byte *base_address;
	long cpu_allocation_size;
	long unknown8;
	dword allocation_size_checksum;
	long game_time;
	s_arena_header *arena;
	bool arena_flag;
	long unknown253c;
};



/* the part of a saved buffer after its header, or no pointer without a buffer */
static inline void const *buffer_after_header(void const *buffer, long header_size)
{
	return buffer ? (char const *)buffer + header_size : 0;
}

void *function_123d40(char const *name, char const *type, long size);
void *game_state_malloc_aligned(char const *name, char const *type, long size, long alignment_bits);

#include "allocator_storage.h"

#endif
