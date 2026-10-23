// @flags /O2 /GL-
// Havok (third-party physics library): queries between the two sides of a
// pair passed on to the function that runs them, with the sides changed.
#include "pair_agents_2eab70.h"

void __cdecl function_2f40e0(void const *a, void const *b, void const *input, void *collector);

// the query passed on to the free function that runs it
// @retail 0x2f4290
void class_2f4290::function_2f4290(void const *a, void const *b, void const *input, void *collector)
{
	function_2f40e0(a, b, input, collector);
}
