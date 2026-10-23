// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the object kept for a pair
// of shapes whose second is a triangle.
#include "triangle_agent_2f42f0.h"

// @retail 0x2f42f0
triangle_agent_2f42f0 *__cdecl function_2f42f0(description_2ed170 const *description, triangle_2f42f0 const **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new triangle_agent_2f42f0(other, arg_3);
}
