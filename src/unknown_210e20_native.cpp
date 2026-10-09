// Native source split of recovered structure-node bitmap refresh at retail
// 0x210e20. The matching Xbox definition remains in unknown_20fe20.cpp,
// whose unrelated audio/geometry helpers use x86 inline assembly.

#include "unknown_11c920.h"
#include <stddef.h>

struct s_match_node_210e20_native
{
    byte unknown00[0x18];
    byte flags;
    byte unknown19[0x24 - 0x19];
};

struct s_match_nodes_210e20_native
{
    byte unknown00[0x5c];
    long count;
    s_match_node_210e20_native *nodes;
};

static_assert(sizeof(s_match_node_210e20_native) == 0x24, "match node size");
static_assert(offsetof(s_match_nodes_210e20_native, count) == 0x5c, "match node count offset");
static_assert(offsetof(s_match_nodes_210e20_native, nodes) == 0x60, "match node pointer offset");

extern void *g_4e0348;
extern void *g_4f93a4;

void function_210e20(void)
{
    s_match_nodes_210e20_native *globals = (s_match_nodes_210e20_native *)g_4e0348;
    for (long index = 0; index < globals->count; index++)
    {
        if (globals->nodes[index].flags & 8)
            ((dword *)g_4f93a4)[index >> 5] &= ~(1u << (index & 0x1f));
    }
}
