// @flags /O2 /GL-
// Havok (third-party physics library): an object with an array of pairs of
// reference-counted objects, filled from two lists and copied out into two.
#include "growable_array_2d91d0.h"
#include "word_holder_2df1f0.h"

struct pair_2fc200
{
	referenced_2d88a0 *field_0;
	referenced_2d88a0 *field_4;
};

ELEMENTS_2D91D0(pairs_2fc200, pair_2fc200);
ELEMENTS_2D91D0(references_2fc200, referenced_2d88a0 *);

struct lists_2fc200
{
	long field_0;
	references_2fc200 field_4;
	references_2fc200 field_10;
};

class class_2fc200 : public class_2df1f0
{
public:
	pairs_2fc200 pairs;

	void function_2fc200(lists_2fc200 *lists) const;
	void function_2fc2a0(referenced_2d88a0 **first, long count, referenced_2d88a0 **second);
};

// @retail 0x2fc200
void class_2fc200::function_2fc200(lists_2fc200 *lists) const
{
	function_2df210(&lists->field_0);
	lists->field_4.size = 0;
	long count = pairs.size;
	lists->field_4.resize(count);
	lists->field_10.resize(count);
	for (long i = 0; i < count; i++)
	{
		lists->field_4.data[i] = pairs.data[i].field_0;
		lists->field_10.data[i] = pairs.data[i].field_4;
	}
}

// @retail 0x2fc2a0
void class_2fc200::function_2fc2a0(referenced_2d88a0 **first, long count, referenced_2d88a0 **second)
{
	pairs.resize(count);
	for (long i = 0; i < count; i++)
	{
		if (first[i] != 0)
		{
			pairs.data[i].field_0 = first[i];
			pairs.data[i].field_4 = second != 0 ? second[i] : 0;
			first[i]->reference_count++;
		}
	}
}
