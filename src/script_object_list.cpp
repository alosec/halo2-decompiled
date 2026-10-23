// @flags /O2 /Gr
/* SCRIPT_OBJECT_LIST.CPP: the first object of a script object list (the rest of
   the object lists are in unknown_1ded60.cpp and unknown_1dee80.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1dee50.h"

/* the first object of an object list; reference_index then walks the rest
   through function_1dee80 */
// @retail 0x1dee50
long script_list_head_object(long list_index, long *reference_index)
{
	long object_index = NONE;
	if (list_index != NONE)
	{
		*reference_index = ((s_object_list_1dee50 *)g_4f55d8->data)[list_index & 0xffff].first_reference_index;
		object_index = function_1dee80(reference_index);
	}
	return object_index;
}
