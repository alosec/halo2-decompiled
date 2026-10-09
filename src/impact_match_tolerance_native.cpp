#ifdef H2_NATIVE_PORT

#include "unknown_11c920.h"

// Retail initializer 0x00378170 computes fcos(45.0) and stores the result at
// 0x0055c2d0. Own the exactly rounded binary32 value without x87 startup code.
real g_55c2d0 = 0.52532196044921875f;

#endif
