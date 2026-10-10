// Native source split of recovered bounded tag-function evaluation at retail
// 0x13bb90. The matching Xbox definition remains in unknown_13b390.cpp,
// whose unrelated colour path uses x86 MMX/SSE and inline assembly.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <stddef.h>

struct s_tag_data
{
    long size;
    byte *address;
};

struct s_function_header
{
    byte type;
    byte flags;
    byte function_type[2];
    real_bounds bounds;
};

static_assert(sizeof(s_tag_data) == 8, "tag data layout");
static_assert(offsetof(s_tag_data, address) == 4, "tag data address offset");
static_assert(offsetof(s_function_header, flags) == 1, "function flags offset");
static_assert(offsetof(s_function_header, bounds) == 4, "function bounds offset");

real function_13b390(void const *function, real input, real range);

// @retail 0x13bb90
real function_13bb90(s_tag_data const *function, real input, real range)
{
    if (function->address && function->size > 0)
    {
        real value = function_13b390(function, input, range);
        s_function_header const *header = (s_function_header const *)function->address;
        if (!(header->flags & 0xf0))
        {
            real lo = header->bounds.lo;
            real hi = header->bounds.hi;
            value = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
            value = (hi - lo) * value + lo;
        }
        return value;
    }
    return 0.0f;
}
