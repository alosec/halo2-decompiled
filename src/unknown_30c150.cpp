// @flags /O2 /GL-
// Havok (third-party physics library): returns a block to the game's
// allocator (g_480118, slot 5) with its 16-bit size and the pool tag 0x28.
#include "globals.h"

struct block_30c150
{
    long unknown0;
    unsigned short size;
};

// @retail 0x30c150
void __cdecl function_30c150(block_30c150 *block)
{
    g_480118->allocate((long)block, block->size, 0x28);
}

// Three field accessors. The game code calls the first as a fastcall free
// function on the world object (hkWorld is the name it already uses).
class hkWorld;

struct world_30c170
{
    char unknown0[0x38];
    unsigned char *field_38;
};

// @retail 0x30c170
unsigned char *__fastcall function_30c170(hkWorld *world)
{
    return ((world_30c170 *)world)->field_38;
}

struct class_30c180
{
    char unknown0[0xc0];
    long field_c0;

    long function_30c180() const;
};

// @retail 0x30c180
long class_30c180::function_30c180() const
{
    return field_c0;
}

// The game code that calls this (src/unknown_2777d0.cpp) reads the two
// fields at 0xcc and 0xd0 of the same object.
class c_278b60_filter;
struct s_278b60_dispatch;
struct s_278b60_filter;

class c_library_30c190
{
public:
    c_278b60_filter *filter();
    char unknown00[0xcc];
    s_278b60_dispatch *dispatch;
    s_278b60_filter *filter_object;
    char unknown_d4[0x250 - 0xd4];
    c_278b60_filter *filter_250;
};

// @retail 0x30c190
c_278b60_filter *c_library_30c190::filter()
{
    return filter_250;
}
