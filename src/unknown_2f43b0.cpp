// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the object kept for a pair
// of shapes whose first is a triangle.
#include "triangle_agent_2f42f0.h"

// @retail 0x2f43b0
swapped_triangle_agent_2f42f0 *__cdecl function_2f43b0(triangle_2f42f0 const **description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new swapped_triangle_agent_2f42f0(description, arg_3);
}
