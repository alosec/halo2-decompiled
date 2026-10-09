#include "unknown_11c920.h"
#include "globals.h"
// @flags /O2 /Gr

struct s_tag_iterator
{
    long unknown00;
    long unknown04;
    long datum_index;
    long next_index;
    long group_tag;
};

class c_part;
struct s_pair_element;
struct s_single_element;
struct s_part_table
{
    byte unknown00[0x20];
    long pair_a_count;
    s_pair_element *pairs_a;
    long single_count;
    s_single_element *singles;
    long pair_b_count;
    s_pair_element *pairs_b;
    void function_1c2390();
    c_part *function_1c2460(long index);
};

long function_122c70(s_tag_iterator *arg_0);
void function_2122b0(long arg_0);
void function_205550(void *arg_0);
void function_1ea9c0(long arg_0, long arg_1);

#ifdef H2_NATIVE_PORT
extern "C" long h2_native_cache_tag_count();
extern "C" void h2_native_physics_runtime_reset();

struct s_native_tag_instance_view
{
    long group_tag;
    long datum_index;
    void *address;
    long size;
};

static long g_native_initialized_tags;
static long g_native_deferred_tags;

extern "C" int h2_native_tag_initialization_status()
{
    return (g_native_initialized_tags << 16) |
        (g_native_deferred_tags & 0xffff);
}

static long h2_native_next_tag(long *next_index)
{
    s_native_tag_instance_view *instances =
        (s_native_tag_instance_view *)g_4e3b44;
    while (*next_index < h2_native_cache_tag_count())
    {
        s_native_tag_instance_view *instance = &instances[(*next_index)++];
        if (instance->group_tag != NONE && instance->datum_index != NONE)
            return instance->datum_index;
    }
    return NONE;
}
#endif

// @retail 0x11c840
void function_11c840()
{
#ifdef H2_NATIVE_PORT
    g_native_initialized_tags = 0;
    g_native_deferred_tags = 0;
    h2_native_physics_runtime_reset();
    long native_next_index = 0;
    long local_1 = h2_native_next_tag(&native_next_index);
#else
    s_tag_iterator local_0;
    local_0.next_index = 0;
    local_0.group_tag = NONE;
    long local_1 = function_122c70(&local_0);
#endif
    while (local_1 != NONE)
    {
        switch (*(dword *)&g_4e3b44[(short)local_1])
        {
        case 0x62697064:
            ((s_part_table *)(g_4e3b44[local_1 & 0xffff].bytes + 0x264))->function_1c2390();
#ifdef H2_NATIVE_PORT
            g_native_initialized_tags++;
#endif
            break;
        case 0x636f6c6c:
            function_2122b0(local_1);
#ifdef H2_NATIVE_PORT
            g_native_initialized_tags++;
#endif
            break;
        case 0x63726561:
            ((s_part_table *)(g_4e3b44[local_1 & 0xffff].bytes + 0xd4))->function_1c2390();
#ifdef H2_NATIVE_PORT
            g_native_initialized_tags++;
#endif
            break;
        case 0x70686d6f:
#ifndef H2_NATIVE_PORT
            function_1ea9c0(local_1, 0);
#else
            g_native_deferred_tags++;
#endif
            break;
        case 0x76656869:
            function_205550(g_4e3b44[local_1 & 0xffff].bytes + 0x2ac);
#ifdef H2_NATIVE_PORT
            g_native_initialized_tags++;
#endif
            break;
        }
#ifdef H2_NATIVE_PORT
        local_1 = h2_native_next_tag(&native_next_index);
#else
        local_1 = function_122c70(&local_0);
#endif
    }
}
