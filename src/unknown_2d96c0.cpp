// @flags /O2 /GL-
// Havok (third-party physics library): builds the eight-value object from a
// structure, with its second and third values swapped.
#include "eight_values_2d96c0.h"

// @retail 0x2d96c0
eight_values_2d96c0::eight_values_2d96c0(values_2d96c0 const &values)
{
	field_8 = values.value_0;
	field_c = values.value_8;
	field_10 = values.value_4;
	field_14 = values.value_c;
	field_18 = values.value_10;
	field_1c = values.value_14;
	field_20 = values.value_18;
	field_24 = values.value_1c;
}
