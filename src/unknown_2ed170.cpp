// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the object kept for a pair
// of shapes.
#include "pair_agent_2ed170.h"

// @retail 0x2ed170
pair_agent_2ed170 *__cdecl function_2ed170(description_2ed170 const *description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new pair_agent_2ed170(description, other, dispatcher, arg_3);
}
