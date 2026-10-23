// @flags /O2 /GL-
// Havok (third-party physics library): adds an item to an object's list,
// unless the item is empty, a listener objects or (when asked to check) it is
// there already.
#include "growable_array_2d91d0.h"
#include "listeners_30af20.h"

struct item_30bab0
{
	long field_0;
};

ELEMENTS_2D91D0(items_30bab0, item_30bab0 *);

class class_30bab0 : public class_30af20
{
public:
	char unknown44[0xc0 - 0x44];
	items_30bab0 items;

	void function_30bab0(item_30bab0 *item, bool check);
};

// @retail 0x30bab0
void class_30bab0::function_30bab0(item_30bab0 *item, bool check)
{
	if (item->field_0 != 0)
	{
		flag_30af20 found;
		found.value = false;
		if (check)
		{
			for (long i = 0; i < items.size; i++)
			{
				if (items.data[i] == item)
				{
					found.value = true;
					break;
				}
			}
		}
		if (function_30af20((long)item, found) == 0 && !found.value)
		{
			items.next_slot() = item;
		}
	}
}
