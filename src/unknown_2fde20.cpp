// @flags /O2 /GL-
// Havok (third-party physics library): setting an object from a description,
// as a free function.

struct entry_2fc0e0
{
	long key;
	long value;
};

struct s_2fc0e0
{
	long field_0;
	char field_4[0xc];
	long *values;
	int count;
};

class class_2fc0e0
{
public:
	char field_0[0xc];
	entry_2fc0e0 *entries;
	int count;

	void function_2fc0e0(s_2fc0e0 const *arg_0);
};

// @retail 0x2fde20
void __cdecl function_2fde20(class_2fc0e0 *object, s_2fc0e0 const *description)
{
	object->function_2fc0e0(description);
}
