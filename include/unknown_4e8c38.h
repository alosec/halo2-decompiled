#pragma once
#include "unknown_11c920.h"

struct s_entry_155760
{
	byte unknown00[4];
	real value;
	byte unknown08[4];
	void *proc;
	real value10;
	real value14;
	byte unknown18[0x58 - 0x18];
	word word58;
	byte unknown5a[0x108 - 0x5a];
	byte flag108;
	byte unknown109[3];
	real value10c;
	byte unknown110[0x140 - 0x110];
};

struct s_storage_4e8c38
{
    union
    {
        dword field_0[0x143];
        struct
        {
            dword field_0;
            long field_4;
            bool field_8;
            byte field_9[3];
            s_entry_155760 field_c[4];
        } field_0_view;
    };
};
extern s_storage_4e8c38 g_storage_4e8c38;
#define g_4e8c38 (g_storage_4e8c38.field_0)
#define g_4e8c3c (g_storage_4e8c38.field_0_view.field_4)
#define g_4e8c40 (g_storage_4e8c38.field_0_view.field_8)
#define g_4e8c44 (g_storage_4e8c38.field_0_view.field_c)
typedef char assert_entry_155760_size[(sizeof(s_entry_155760) == 0x140) ? 1 : -1];
typedef char assert_storage_4e8c38_size[(sizeof(s_storage_4e8c38) == 0x50c) ? 1 : -1];
