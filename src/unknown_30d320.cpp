// @flags /O2 /GL-
// Havok (third-party physics library): an object drops the cleared entries
// from three of its lists, keeping the others in order.
#include "growable_array_2d91d0.h"

ELEMENTS_2D91D0(pointers_30d320, void *);

// drops the null entries, keeping the others in order
static __forceinline void compact_30d320(pointers_30d320 *list)
{
	long count = list->size;
	long i;
	for (i = 0; i < count; i++)
	{
		if (list->data[i] == 0)
		{
			break;
		}
	}
	long kept = i;
	for (i = kept + 1; i < count; i++)
	{
		if (list->data[i] != 0)
		{
			list->data[kept] = list->data[i];
			kept++;
		}
	}
	list->resize(kept);
}

class class_30d320
{
public:
	char unknown0[0x7c];
	pointers_30d320 third;
	pointers_30d320 first;
	pointers_30d320 second;

	void function_30d320();
	void function_30d390();
	void function_30d400();
};

// @retail 0x30d320
void class_30d320::function_30d320()
{
	compact_30d320(&first);
}

// @retail 0x30d390
void class_30d320::function_30d390()
{
	compact_30d320(&second);
}

// @retail 0x30d400
void class_30d320::function_30d400()
{
	compact_30d320(&third);
	function_30d320();
	function_30d390();
}
