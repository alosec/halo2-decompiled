// @flags /O2 /GL-
// Havok (third-party physics library): a table of 32 masks, one per group,
// saying which groups each group pairs with.

#include "pair_filter_class.h"

// pairs every group in one set of groups with every group in the other
// @retail 0x2df950
void c_2dfbf0::function_2df950(long arg_0, long arg_1)
{
	for (int i = 0; i < 32; i++)
	{
		unsigned long bit = 1 << i;
		if (arg_0 & bit)
		{
			enabled[i] |= arg_1;
		}
		if (arg_1 & bit)
		{
			enabled[i] |= arg_0;
		}
	}
}

// unpairs two groups
// @retail 0x2df990
void c_2dfbf0::function_2df990(long arg_0, long arg_1)
{
	enabled[arg_0] &= ~(1 << arg_1);
	enabled[arg_1] &= ~(1 << arg_0);
}
