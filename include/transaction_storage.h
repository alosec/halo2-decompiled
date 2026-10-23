#ifndef TRANSACTION_STORAGE_H
#define TRANSACTION_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"

struct s_upload_file_ab;
struct s_upload_ab
{
    long kind;
    char path[0x80];
    char header[0x400];
    char part[0x100];
    char ending[0x80];
    long header_length;
    long part_length;
    long ending_length;
    char content_type[0x20];
    char filename[0x104];
    char extra_headers[0x200];
    byte const *memory;
    s_upload_file_ab *file;
    long file_length;
    long position;
};

struct s_transaction_storage
{
    s_upload_ab state;
};

extern s_transaction_storage g_transaction_storage;
#define g_546abc (g_transaction_storage.state.kind)
#define g_5470cc (g_transaction_storage.state.content_type)
#define g_5473f0 (g_transaction_storage.state.memory)
#define g_5473f8 (g_transaction_storage.state.file_length)
typedef char assert_transaction_offset_0[(offsetof(s_transaction_storage, state.kind) == 0x0) ? 1 : -1];
typedef char assert_transaction_offset_610[(offsetof(s_transaction_storage, state.content_type) == 0x610) ? 1 : -1];
typedef char assert_transaction_offset_934[(offsetof(s_transaction_storage, state.memory) == 0x934) ? 1 : -1];
typedef char assert_transaction_offset_93c[(offsetof(s_transaction_storage, state.file_length) == 0x93c) ? 1 : -1];

typedef char assert_transaction_size[(sizeof(s_transaction_storage) == 0x944) ? 1 : -1];

#endif
