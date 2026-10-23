// @flags /O2 /GL-
// Havok (third-party physics library): the deleting destructor of an object
// given back with tag 0x2a, and an object set from a description in two
// steps.
#include "tagged_objects_2e4ff0.h"

// @retail 0x3110d0 deleting class_310a70

struct s_311100;

class c_311100
{
public:
	void function_30fcb0(s_311100 const *description);
	void function_30fef0(s_311100 const *description);
	void function_311100(s_311100 const *arg_0);
};

// @retail 0x311100
void c_311100::function_311100(s_311100 const *arg_0)
{
	function_30fcb0(arg_0);
	function_30fef0(arg_0);
}
