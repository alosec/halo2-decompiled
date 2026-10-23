#ifndef UPLOAD_STORAGE_H
#define UPLOAD_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_upload_storage
{
    dword field_0;
    long field_4;
    long field_8;
    byte upload_pending;    /* set while a result waits to be uploaded */
    byte unknown_d[0x3];
    long attempt_count;     /* the failed upload attempts; picks the server to try */
    dword field_14;
};

extern s_upload_storage g_upload_storage;
#define g_547610 (g_upload_storage.field_0)
#define g_547614 (g_upload_storage.field_4)
#define g_547618 (g_upload_storage.field_8)
#define g_54761c (g_upload_storage.upload_pending)
#define g_547620 (g_upload_storage.attempt_count)
#define g_547624 (g_upload_storage.field_14)
typedef char assert_upload_offset_0[(offsetof(s_upload_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_upload_offset_4[(offsetof(s_upload_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_upload_offset_8[(offsetof(s_upload_storage, field_8) == 0x8) ? 1 : -1];
typedef char assert_upload_offset_c[(offsetof(s_upload_storage, upload_pending) == 0xc) ? 1 : -1];
typedef char assert_upload_offset_10[(offsetof(s_upload_storage, attempt_count) == 0x10) ? 1 : -1];
typedef char assert_upload_offset_14[(offsetof(s_upload_storage, field_14) == 0x14) ? 1 : -1];

typedef char assert_upload_size[(sizeof(s_upload_storage) == 0x18) ? 1 : -1];

#endif
