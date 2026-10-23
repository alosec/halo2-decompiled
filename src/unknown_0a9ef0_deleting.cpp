// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A9EF0_DELETING.CPP: the deleting destructor of the sized
   reference base (vtable 0x452c30) that frees itself to the allocator at
   0x480118 with the size it keeps in its first word; the same layout as
   the base in src/unknown_0a9f20.cpp. (src/unknown_0a9ef0.cpp holds other
   functions.) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1efac0.h"

class c_sized_reference_a9ef0
{
public:
    virtual ~c_sized_reference_a9ef0();
    word flags;
    word unknown06;

    static void operator delete(void *block)
    {
        g_480118->allocate((long)block, ((c_sized_reference_a9ef0 *)block)->flags, 0x10);
    }
};

// @retail 0xa9ef0 deleting
c_sized_reference_a9ef0::~c_sized_reference_a9ef0()
{
}
