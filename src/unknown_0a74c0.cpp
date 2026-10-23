// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A74C0.CPP: the check that two vectors form an orthonormal pair
   (lane M, for 0xdf380) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

inline bool function_a0190(vector3f const *vector);
__declspec(noinline) bool function_a0200(real a, real b);

// @retail 0xa74c0
bool function_a74c0(vector3f const *forward, vector3f const *up)
{
	return function_a0190(forward) && function_a0190(up) && function_a0200(dot3f(forward, up), 0.0f);
}
