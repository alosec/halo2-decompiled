/* UNKNOWN_1DEE50.H */

#ifndef UNKNOWN_1DEE50_H
#define UNKNOWN_1DEE50_H

#include "unknown_11c920.h"
#include "globals.h"
#include <xmmintrin.h>

/* the first object of an object list; reference_index then walks the rest */
long script_list_head_object(long list_index, long *reference_index);
long function_1dee80(long *reference_index);

/* the object references of the object lists (g_4f55d4, unknown_1dee80.cpp) */
struct s_object_reference_1dee50
{
	byte unknown00[4];
	long object_index;
	long next_reference_index;
};

extern s_record_pool *g_4f55d4;

/* the object lists (g_4f55d8), 12 bytes each */
struct s_object_list_1dee50
{
	byte unknown00[8];
	long first_reference_index;
};

extern s_record_pool *g_4f55d8;

/* the inline copy of script_list_head_object (0x1dee50) that retail places in
   callers in other files */
inline long object_list_get_first_inlined(long list_index, long *reference_index)
{
	long object_index = NONE;
	if (list_index != NONE)
	{
		*reference_index = ((s_object_list_1dee50 *)g_4f55d8->data)[list_index & 0xffff].first_reference_index;
		object_index = function_1dee80(reference_index);
	}
	return object_index;
}

/* the inline copy of function_1dee80 that LTCG places in callers' loops */
inline long function_x457076(long *reference_index)
{
	long object_index;
	if (*reference_index != NONE)
	{
		long size = g_4f55d4->size;
		byte *data = g_4f55d4->data;
		s_object_reference_1dee50 *reference = (s_object_reference_1dee50 *)(data + (*reference_index & 0xffff) * size);
		long next = reference->next_reference_index;

		if (next != NONE)
			_mm_prefetch((const char *)(data + (next & 0xffff) * size), _MM_HINT_T0);

		*reference_index = next;
		object_index = reference->object_index;
	}
	else
	{
		object_index = NONE;
	}
	return object_index;
}

#endif