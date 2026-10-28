// @flags /O2 /GL-
#include "lane_a_overlap_listener.h"
// @retail 0x315910
void c_2784c5::function_315910(void *listener)
{
	lane_a_listener_array &array = field_3c;
	if (array.size == (array.capacity_and_flags & 0x7fffffff))
		function_2d91d0(&array, 4);
	array.data[array.size] = listener;
	array.size++;
}
