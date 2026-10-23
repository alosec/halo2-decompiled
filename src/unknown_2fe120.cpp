// @flags /O2 /GL- /GX
// Havok (third-party physics library): builds an object with two cleared
// fields in place, and copies its values out.
#include <new>

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct class_2df1f0
{
	char unknown0[8];
	long field_8;

	void function_2df210(long *value) const;
};

struct cleared_2fe120 : class_2df1e0
{
	long field_4;
	long field_8;

	cleared_2fe120() : field_4(0), field_8(0) {}
};

// @retail 0x2fe120
void __cdecl function_2fe120(void *memory)
{
	new (memory) cleared_2fe120();
}

struct s_2fe170
{
	long field_0;
	long field_4;
	long field_8;
};

class class_2fe170
{
public:
	char field_0[0xc];
	long field_c;
	long field_10;

	void function_2fe170(s_2fe170 *out) const;
};

// @retail 0x2fe170
void class_2fe170::function_2fe170(s_2fe170 *out) const
{
	((class_2df1f0 const *)this)->function_2df210(&out->field_0);
	out->field_4 = field_c;
	out->field_8 = field_10;
}
