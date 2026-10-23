// @flags /O2 /GL-
// Havok (third-party physics library): adds a value to an object's list,
// unless a listener objects or (when asked to check) it is there already.
#include "growable_array_2d91d0.h"
#include "listeners_30af20.h"

struct value_30b2a0;

ELEMENTS_2D91D0(values_30b2a0, value_30b2a0 *);

class class_30b2a0 : public class_30af20
{
public:
	char unknown44[0x80 - 0x44];
	values_30b2a0 values;

	void function_30b2a0(value_30b2a0 *value, bool check);
};

// @retail 0x30b2a0
void class_30b2a0::function_30b2a0(value_30b2a0 *value, bool check)
{
	flag_30af20 found;
	found.value = false;
	if (check)
	{
		for (long i = 0; i < values.size; i++)
		{
			if (values.data[i] == value)
			{
				found.value = true;
				break;
			}
		}
	}
	if (function_30af20((long)value, found) == 0 && !found.value)
	{
		values.append(value);
	}
}
