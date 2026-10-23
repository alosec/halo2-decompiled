// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's settings into a
// description and back.
#include "description_312430.h"

// @retail 0x312430
void class_312430::function_312430(description_312430 *description) const
{
	description->field_4 = field_c;
	description->field_8 = field_24;
	description->pairs.ensure_capacity(pairs.size);
	for (int i = 0; i < pairs.size; i++)
	{
		description->pairs.append_unchecked(pairs[i]);
	}
	description->field_0 = field_28;
}

// @retail 0x3124c0
void class_312430::function_3124c0(description_312430 const *description)
{
	if (field_28 != description->field_0)
	{
		field_28 = description->field_0;
	}
	pairs.resize(description->pairs.size);
	for (int i = 0; i < description->pairs.size; i++)
	{
		pairs[i] = description->pairs[i];
	}
}
