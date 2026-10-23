// @flags /O2 /GL- /GX
// Havok (third-party physics library): copying an object's values out, and
// building the eight-value object from a structure.
#include "eight_values_2d96c0.h"

struct s_2d9570
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	long field_1c;
};

class class_2d9570
{
public:
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	long field_1c;
	long field_20;
	long field_24;

	void get(s_2d9570 *out) const;
};

// @retail 0x2da000
void __cdecl function_2da000(class_2d9570 const *object, s_2d9570 *out)
{
	object->get(out);
}

// @retail 0x2da070
eight_values_2d96c0 *__cdecl function_2da070(values_2d96c0 const &values)
{
	return new eight_values_2d96c0(values);
}
