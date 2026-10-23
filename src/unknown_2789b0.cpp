// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_2789B0.CPP: the deleting destructor of an object made of the
   sized reference base (vtable 0x452c30, freed to the allocator at
   0x480118 with the size in its first word) and a second interface base
   at +8 (vtable 0x45a77c) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1efac0.h"

class c_sized_reference_2789b0
{
public:
    virtual ~c_sized_reference_2789b0() {}
    word flags;
    word unknown06;

    static void operator delete(void *block)
    {
        g_480118->allocate((long)block, ((c_sized_reference_2789b0 *)block)->flags, 0x10);
    }
};

class c_interface_2789b0
{
public:
    virtual ~c_interface_2789b0() {}
    virtual void function_1() {}
};

class c_object_2789b0 : public c_sized_reference_2789b0, public c_interface_2789b0
{
public:
    c_object_2789b0();
};

c_object_2789b0::c_object_2789b0()
{
}

// @retail 0x2789b0 deleting c_object_2789b0
// @retail 0x278b30 adjustor c_object_2789b0 8
