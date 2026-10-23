// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_120850.CPP */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

__declspec(noinline) short function_120850(vector3f const *v);

// @retail 0x120850
inline short function_120850(vector3f const *v)
{
	real x = (real)fabs(v->i);
	real y = (real)fabs(v->j);
	real z = (real)fabs(v->k);

	if (z >= y && z >= x)
		return 2;
	if (y >= x)
		return 1;
	return 0;
}

#if H2_NATIVE_PORT
/* native: emit the inline definition so the guest image can link it */
__attribute__((used)) static void *h2_emit_function_120850 = (void *)&function_120850;
#endif
