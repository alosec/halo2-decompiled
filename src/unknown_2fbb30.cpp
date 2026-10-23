// @flags /O2 /GL-
// Havok (third-party physics library): a virtual method that returns the
// address of a static object in .data.
#include "unknown_244980.h"

struct object_4803c4
{
    long unknown0;
};

object_4803c4 g_4803c4;

// @retail 0x2fbb30
void *c_244980_base::v3()
{
    return &g_4803c4;
}
