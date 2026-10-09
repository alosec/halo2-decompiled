#if H2_NATIVE_PORT

// @flags /O2 /Gr

#include "unknown_11c920.h"

// The native port has no audio graph yet. BSP switching must not traverse
// Xbox listener/environment state or fabricate cue playback.
// @retail 0x18bb80
void __stdcall function_18bb80(real elapsed)
{
	(void)elapsed;
}

#endif
