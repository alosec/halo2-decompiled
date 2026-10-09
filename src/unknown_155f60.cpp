// @flags /O2 /Ob1 /Gr
/* Native split of the D3D-independent movie-state query from
   unknown_01e930.cpp. The Xbox build keeps the original body there. */

#include "unknown_11c920.h"
#include "globals.h"

#ifdef H2_NATIVE_PORT
// @retail 0x155f60
bool function_155f60(void)
{
	return g_4e9188.movie && g_4e9188.initialized;
}
#endif
