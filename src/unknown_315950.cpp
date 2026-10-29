// @flags /O2 /GL- /GX
// Havok (third-party physics library): destroying an object that tells its
// listeners it is going.
#include "listened_object_315950.h"

// score 9 (was 9): The initial index and loop branch differ; index-first for/while
// 53/53, direct reads 51/51; count with derived index 14/57; body decrement 9/53.
// Long count with null branch and decrement variants 9/9/9/9; 64-bit count variants 92/92/92/92.
// For body decrement with int/long and if/continue 9/9/9/9; while bottom
// decrement with local/direct reads 14/57; element pointer int/long 50/50.
// Outer/condition/constant receiver with count 9/9/9, bounded index 61/61/61;
// array-element reference with count/bounded index 50/65.
// @retail 0x315950
class_315950::~class_315950()
{
	int i = listeners.size;
	while (i > 0)
	{
		i--;
		listener_315950 *listener = listeners.data[i];
		if (listener)
		{
			listener->object_destroyed(this);
		}
	}
}
