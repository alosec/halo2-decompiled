/* UNKNOWN_0A9F20.H: a reference object (vtable 0x452c1c) over a base (vtable
   0x452c30) that frees itself to the allocator at 0x480118 with the size it
   keeps in its first word; several Havok classes derive from it, the capsule
   0x2de0a0 builds (vtable 0x455458) among them */
#pragma once

#include "globals.h"
#include <xmmintrin.h>

class c_sized_reference
{
public:
    /* built with one reference */
    c_sized_reference() { reference_count = 1; }
    virtual ~c_sized_reference() {}
    word flags;
    word reference_count;

    static void operator delete(void *block)
    {
        g_480118->allocate((long)block, ((c_sized_reference *)block)->flags, 0x10);
    }
};

class c_reference_a9f20 : public c_sized_reference
{
public:
    virtual ~c_reference_a9f20();
};

/* the reference object with a value of its own, zero when built */
class c_valued_reference : public c_reference_a9f20
{
public:
    c_valued_reference() : field_8(0) {}
    long field_8;
};

/* a capsule between two points; destroying one runs only the reference
   object's destructor (0xdc370) */
class __declspec(align(16)) c_z_capsule_temp : public c_valued_reference
{
public:
    c_z_capsule_temp(void const *first, void const *second, real radius);
    real radius;
    /* the two points, the radius in the last lane of each */
    __m128 first_point;
    __m128 second_point;
};
