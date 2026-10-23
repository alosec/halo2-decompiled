// @flags /O2 /GL-
// Havok (third-party physics library): setting an object from a description,
// as a free function.

struct s_2dc030
{
	long field_0;
	char field_4[4];
	long field_8;
	float field_c;
	float field_10;
};

class c_2dbfc0
{
public:
	char field_0[0x20];
	float field_20;
	float field_24;
	float field_28;
	float field_2c;

	void function_2dbcf0(s_2dc030 const *arg_0);
};

// @retail 0x2dc0d0
void __cdecl function_2dc0d0(c_2dbfc0 *object, s_2dc030 const *description)
{
	object->function_2dbcf0(description);
}
