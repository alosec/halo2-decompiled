// @flags /O2 /GL-
// Havok (third-party physics library): setting an object from a description,
// as a free function.
#include <xmmintrin.h>

struct s_2ddd00
{
	long field_0;
	long field_4;
	char field_8[8];
	__m128 field_10;
	__m128 field_20;
};

class class_2ddd00
{
public:
	char field_0[0x10];
	__m128 field_10;
	__m128 field_20;

	void function_2ddd00(s_2ddd00 const *arg_0);
};

// @retail 0x2df300
void __cdecl function_2df300(class_2ddd00 *object, s_2ddd00 const *description)
{
	object->function_2ddd00(description);
}
