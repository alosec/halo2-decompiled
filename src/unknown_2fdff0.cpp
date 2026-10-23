// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating an object holding a child
// object and a counted handle from a description; constructing, in place, a
// cleared object; and copying an object's description out, as a free
// function.
#include <new>
#include "shape_2de750.h"
#include "havok_reference.h"

struct class_3009b0
{
	char unknown0[0x24];
	long field_24;

	void function_3009b0();
};

struct child_info_2df7d0 : shape_info_2de750
{
	c_havok_reference_counted *child;
	long handle;
};

class child_holder_2df760 : public shape_2de750
{
public:
	c_havok_reference_counted *child;

	child_holder_2df760(c_havok_reference_counted *child_) : child(child_)
	{
		child_->reference_count++;
	}
	child_holder_2df760(child_info_2df7d0 const &info) : shape_2de750(info), child(info.child)
	{
		child->reference_count++;
	}
};

struct c_shape_library_base_a : child_holder_2df760
{
	long handle;

	REFERENCED_NEW_2D88A0(0x22)

	c_shape_library_base_a(child_info_2df7d0 const &info);
	virtual ~c_shape_library_base_a();
};

// @retail 0x2fdff0
c_shape_library_base_a *__cdecl function_2fdff0(child_info_2df7d0 const &info)
{
	return new c_shape_library_base_a(info);
}

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct cleared_2fc500 : class_2df1e0
{
	long field_4;

	cleared_2fc500();
};

struct cleared_2fe050 : cleared_2fc500
{
	long field_8;

	cleared_2fe050() : field_8(0) {}
};

// @retail 0x2fe050
void __cdecl function_2fe050(void *memory)
{
	new (memory) cleared_2fe050();
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

// @retail 0x2fe0a0
void __cdecl function_2fe0a0(class_2fe170 const *object, s_2fe170 *out)
{
	object->function_2fe170(out);
}
