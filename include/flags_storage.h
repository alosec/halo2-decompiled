#ifndef FLAGS_STORAGE_H
#define FLAGS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"


struct s_flag_entry
{
	long key;
	long value;
	real x;
};

struct s_flags_storage
{
    s_flag_entry field_0[16];
};

extern s_flags_storage g_flags_storage;
#define g_4ba074 (g_flags_storage.field_0)
typedef char assert_flags_offset_0[(offsetof(s_flags_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_flags_size[(sizeof(s_flags_storage) == 0xc0) ? 1 : -1];

#endif
