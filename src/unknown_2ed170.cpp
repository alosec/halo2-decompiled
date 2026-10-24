// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the object kept for a pair
// of shapes.
#include "pair_agent_2ed170.h"

// @retail 0x2ed170
pair_agent_2ed170 *__cdecl function_2ed170(description_2ed170 const *description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
#if H2_WHOLE_IMAGE
	/* retail's table (0x412ba4), stored inline: the class declares no
	   virtuals of its own for the image to bind it by */
	pair_agent_2ed170 *agent = new pair_agent_2ed170(description, other, dispatcher, arg_3);
	*(unsigned long *)agent = 0x412ba4;
	return agent;
#else
	return new pair_agent_2ed170(description, other, dispatcher, arg_3);
#endif
}
