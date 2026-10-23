#ifndef ALLOCATOR_STORAGE_H
#define ALLOCATOR_STORAGE_H
#include "unknown_11c920.h"
#include <stddef.h>
#include "unknown_123b30.h"
struct s_allocator_storage { s_game_state_globals state; };
extern s_allocator_storage g_allocator_storage;
#define game_state_globals (g_allocator_storage.state)
#define g_4e3b60 (g_allocator_storage.state)
#define g_4e6080 (g_allocator_storage.state.base_address)
#define g_4e6084 (g_allocator_storage.state.cpu_allocation_size)
#define g_4e608c (g_allocator_storage.state.allocation_size_checksum)
typedef char assert_allocator_size[(sizeof(s_allocator_storage) == 0x2540) ? 1 : -1];
#define g_4e6098 (g_allocator_storage.state.arena_flag)
typedef char assert_allocator_arena_flag[(offsetof(s_allocator_storage, state.arena_flag) == 0x2538) ? 1 : -1];
#endif
