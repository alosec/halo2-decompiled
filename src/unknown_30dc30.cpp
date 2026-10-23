// @flags /O2 /GL-
// Havok (third-party physics library): appends a pair of words to an object's
// list.
#include "growable_array_2d91d0.h"

struct pair_30dc30
{
	long first;
	long second;
};

ELEMENTS_2D91D0(pairs_30dc30, pair_30dc30);

class class_30dc30
{
public:
	char unknown0[0x8c];
	pairs_30dc30 pairs;

	void function_30dc30(pair_30dc30 const *pair);
};

// @retail 0x30dc30
void class_30dc30::function_30dc30(pair_30dc30 const *pair)
{
	pairs.append_copy(*pair);
}
