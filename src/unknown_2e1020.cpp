// @flags /O2 /GL-
// Havok (third-party physics library): a three-axis sweep of boxes
// (axis_sweep_2e1090.h): the boxes of all of them into an array and the box
// of one picked by an index the caller points to.
#include "sweep_search_2e1090.h"

// the boxes of all the elements, in order
// @retail 0x2e1020
void class_2e1090::function_2e1020(bounds_array_2e1020 *out)
{
	if ((out->capacity_and_flags & 0x7fffffff) < elements.size)
	{
		function_2d9160(out, elements.size, sizeof(bounds_2e0020));
	}
	out->size = elements.size;
	for (int i = 0; i < elements.size; i++)
	{
		function_2e0020(&elements.data[i], &out->data[i]);
	}
}

// @retail 0x2e1090
void class_2e1090::function_2e1090(long const *index, bounds_2e0020 *bounds)
{
	function_2e0020(&elements.data[*index], bounds);
}
