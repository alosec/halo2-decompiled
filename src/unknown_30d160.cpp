// @flags /O2 /GL-
// Havok (third-party physics library): appends a pair of words to an object's
// list.
#include "growable_array_2d91d0.h"

struct pair_30d160
{
	long first;
	long second;
};

ELEMENTS_2D91D0(pairs_30d160, pair_30d160);

class class_30d160
{
public:
	char unknown0[0x20];
	pairs_30d160 pairs;

	void function_30d160(long first, long second);
};

// @retail 0x30d160
void class_30d160::function_30d160(long first, long second)
{
	pairs.resize(pairs.size + 1);
	pairs.data[pairs.size - 1].first = first;
	pairs.data[pairs.size - 1].second = second;
}
