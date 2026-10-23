#pragma once
#include "unknown_1efac0.h"

struct c_244980_base : c_a
{
    virtual long v1(long first, long second);
    virtual void *v3();
};

struct c_marker_base : c_244980_base
{
    dword d8;
    union
    {
        dword dc;
        dword value0c;
    };
    c_marker_base(dword other)
    {
        dc = other;
        unknown06 = 1;
        d8 = 0;
    }
};

#ifndef H2_DEBUG_BREAK
#if H2_NATIVE_PORT
#define H2_DEBUG_BREAK() __builtin_trap()
#else
#define H2_DEBUG_BREAK() do { __asm { int 3 } __assume(0); } while (0)
#endif
#endif

struct c_marker_object : c_marker_base
{
    dword d10;
    c_marker_object(dword value, dword other) : c_marker_base(other)
    {
        d10 = value;
    }
    void operator delete(void *block)
    {
        g_480118->allocate((long)block, ((c_a *)block)->flags, 0x22);
    }
    virtual long v5() { return 2; }
    virtual void v6() { H2_DEBUG_BREAK(); }
    virtual void v7() { H2_DEBUG_BREAK(); }
    virtual void v8() { H2_DEBUG_BREAK(); }
    virtual void v9() { H2_DEBUG_BREAK(); }
    virtual void v10() { H2_DEBUG_BREAK(); }
    virtual void v11() { H2_DEBUG_BREAK(); }
    virtual void v12() { H2_DEBUG_BREAK(); }
    virtual c_marker_object *create(dword value, void *memory);
    virtual long v14(long unused) { return 0; }
};
