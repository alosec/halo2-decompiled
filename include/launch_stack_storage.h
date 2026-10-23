#ifndef LAUNCH_STACK_STORAGE_H
#define LAUNCH_STACK_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"

struct s_physical_memory_globals
{
	long field_0;
	long base_address;
	long field_8_3;
	long field_c_6[5];
	long field_20[5];
};

extern s_physical_memory_globals g_global_f9ae07;
typedef s_physical_memory_globals s_launch_stack_storage;
#define g_launch_stack_storage (g_global_f9ae07)
#define g_4e6420 (g_global_f9ae07.field_0)
#define g_4e642c (g_global_f9ae07.field_c_6)
#define g_4e6440 (g_global_f9ae07.field_20)

typedef char assert_launch_stack_offset_0[(offsetof(s_launch_stack_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_launch_stack_offset_c[(offsetof(s_launch_stack_storage, field_c_6) == 0xc) ? 1 : -1];
typedef char assert_launch_stack_offset_20[(offsetof(s_launch_stack_storage, field_20) == 0x20) ? 1 : -1];
typedef char assert_launch_stack_size[(sizeof(s_launch_stack_storage) == 0x34) ? 1 : -1];

#endif
