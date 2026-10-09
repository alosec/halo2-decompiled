// Native source split of the recovered transparent-plane cache rebuild at retail
// 0x1e1c0. The matching Xbox definition remains in unknown_01cf50.cpp, whose
// unrelated renderer code depends on x86 intrinsics and the Xbox D3D ABI.

#include "unknown_11c920.h"
#include <stddef.h>

struct hash_table;

struct s_cache_lookup_key_1e1c0_native
{
    short index;
    word flag : 1;
    word part : 15;
    byte unknown04[16];
};

struct s_cache_source_entries_1e1c0_native
{
    byte unknown00[0x158];
    long count;
    s_cache_lookup_key_1e1c0_native *entries;
};

static_assert(sizeof(s_cache_lookup_key_1e1c0_native) == 0x14,
    "cache lookup key size");
static_assert(offsetof(s_cache_source_entries_1e1c0_native, count) == 0x158,
    "cache source count offset");
static_assert(offsetof(s_cache_source_entries_1e1c0_native, entries) == 0x15c,
    "cache source entries offset");

extern void *g_4e0348;
extern hash_table *g_51f400;

bool function_13e270(hash_table *table, void *key, void const *data);

void function_1e1c0(void)
{
    s_cache_source_entries_1e1c0_native *source =
        (s_cache_source_entries_1e1c0_native *)g_4e0348;
    if (g_51f400)
    {
        for (long i = 0; i < source->count; ++i)
        {
            s_cache_lookup_key_1e1c0_native *key = &source->entries[i];
            function_13e270(g_51f400, key, key->unknown04);
        }
    }
}
