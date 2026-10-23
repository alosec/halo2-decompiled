// @flags /O2 /GL- /GX
// Havok (third-party physics library): makes a new list of child objects for
// a shape holding children (0x2edd00).
#include "child_list_2edd00.h"

// @retail 0x2ee010
sized_2edd00 *__cdecl function_2ee010(description_2edd00 const *description, typed_2edd00 **other, context_2edd00 const *context, long arg_3)
{
	return new list_2eda30(description, other, context, arg_3);
}
