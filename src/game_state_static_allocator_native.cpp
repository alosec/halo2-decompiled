// Source-local ARM-safe split of the recovered game-state bump allocation
// used by registered process lifetimes. The broad translation unit retains
// Xbox-only inline assembly and render declarations unrelated to this helper.

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "crc.h"

// @retail 0x123d40
void *function_123d40(
	char const *name,
	char const *type,
	long size)
{
	long aligned_size = (size + 3) & ~3;
	void *result = game_state_globals.base_address +
		game_state_globals.cpu_allocation_size;

	game_state_globals.cpu_allocation_size += aligned_size;
	function_163ba0(
		&game_state_globals.allocation_size_checksum,
		&aligned_size,
		sizeof(aligned_size));

	return result;
}
