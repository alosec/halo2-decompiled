#ifndef RENDER_SOURCE_STORAGE_H
#define RENDER_SOURCE_STORAGE_H

#include "unknown_11c920.h"

struct s_record_source;
struct s_record_sources
{
    dword unknown00;
    long count;
    dword flags;
    long first;
    dword unknown10;
    s_record_source *sources;
};

struct s_render_source_storage
{
    s_record_sources groups[3];
};

extern s_render_source_storage g_render_source_storage;
#define g_4c1a48 (g_render_source_storage.groups)

#define g_4c1a80 (g_render_source_storage.groups[2].flags)
#endif
