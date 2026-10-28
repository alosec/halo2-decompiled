// @flags /O2 /GL-
// Havok (third-party physics library): the shared object of kind 1, built
// with one reference and destroyed as its base is.
#include "kind_object_3192b0.h"

kind_object_3192b0 g_480bb8;

// @retail 0x3192b0 destructor kind_object_3192b0

// @retail 0x3192c0
kind_object_3192b0::kind_object_3192b0()
{
}

// score 1 (was 1): the call skips the intermediate destructor.
// A derived destructor, explicit member, early exit, object pointer,
// named late/early bit, byte flags and separate returns all score 1;
// a separate destructor declaration does not compile.
// Split destruction with long/short/char flags 33; native deletion 19;
// explicit destruction 20; allocator forms 1. Existing deallocation moved
// before/after construction: ordinary/inline/force-inline 1, noinline 22.
// Split destruction/native delete with unsigned dword/word/signed byte/signed dword flags
// 33/19/33/19/33/19/33/19; all restored; the intermediate call remains.
// @retail 0x3192d0 deleting kind_object_3192b0
