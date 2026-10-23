// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object holding a reference to a
// child object and a counted handle, built from its parts or from a
// description.
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

	c_shape_library_base_a(c_havok_reference_counted *arg_0, long arg_1);
	c_shape_library_base_a(child_info_2df7d0 const &info);
	virtual ~c_shape_library_base_a();
};

// @retail 0x2df760
c_shape_library_base_a::c_shape_library_base_a(c_havok_reference_counted *arg_0, long arg_1)
	: child_holder_2df760(arg_0)
{
	handle = arg_1;
	((class_3009b0 *)arg_1)->function_3009b0();
}

// @retail 0x2df7d0
c_shape_library_base_a::c_shape_library_base_a(child_info_2df7d0 const &info)
	: child_holder_2df760(info)
{
	handle = info.handle;
	((class_3009b0 *)handle)->function_3009b0();
}
