// @flags /O2 /GL- /GX
// Havok (third-party physics library): a description's constructor (a
// distance of 20 and a second distance of 5), creating the child-carrying
// object from its own description, and copying an object's description out
// as a free function.
#include "reference_counted_2d88a0.h"

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct class_2df1d0 : class_2df1e0
{
	class_2df1d0();
};

struct class_2df1a0 : class_2df1d0
{
	float field_4;

	class_2df1a0();
};

struct s_2dc030 : class_2df1a0
{
	long field_8;
	float field_c;
	float field_10;

	s_2dc030();
};

// @retail 0x2dc030
s_2dc030::s_2dc030()
{
	field_8 = 0;
	field_c = 20.0f;
	field_10 = 5.0f;
}

struct info_2dbf10;

class c_2dbfc0 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	char unknown8[0x38];

	c_2dbfc0(info_2dbf10 const &info);
};

// @retail 0x2dc060
c_2dbfc0 *__cdecl function_2dc060(info_2dbf10 const &info)
{
	return new c_2dbfc0(info);
}

struct s_2dbcc0;

class class_2dbcc0
{
public:
	void function_2dbcc0(s_2dbcc0 *out) const;
};

// @retail 0x2dc0c0
void __cdecl function_2dc0c0(class_2dbcc0 const *object, s_2dbcc0 *out)
{
	object->function_2dbcc0(out);
}
