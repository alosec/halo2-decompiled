// @flags /O2 /GL-
// The shared serializable descriptor, initialized by retail 0x378aa0 using
// 0x2d9750 (eight machine-word arguments; first is the descriptor name).
#include "eight_values_2d96c0.h"

char const *g_461de0 = "hkSerializable";
eight_values_2d96c0 g_480120((long)g_461de0, 0, 0, 0, 0, 0, 0, 0);

// @retail 0x2d70e0
eight_values_2d96c0 *__cdecl function_2d70e0()
{
	return &g_480120;
}
