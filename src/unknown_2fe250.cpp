// @flags /O2 /GL-
// Havok (third-party physics library): setting an object's value at +8, as
// a free function.

struct class_2df1f0
{
	char unknown0[8];
	long field_8;

	void function_2df200(long const *value);
};

// @retail 0x2fe250
void __cdecl function_2fe250(class_2df1f0 *object, long const *value)
{
	object->function_2df200(value);
}
