// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object that holds a reference to
// another destroys it when it holds the last reference.
#include "unknown_0a9f20.h"
#include "reference_counted_2d88a0.h"

class deletable_2de7b0
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

class c_2de7b0 : public c_reference_a9f20
{
public:
	long field_8;
	referenced_2d88a0 *field_c;

	virtual ~c_2de7b0();
};

// @retail 0x2de7b0
c_2de7b0::~c_2de7b0()
{
	referenced_2d88a0 *object = field_c;
	object->reference_count--;
	if (object->reference_count == 0)
	{
		// the deleting destructor, first in the vtable, without the null test
		// `delete` adds
		((deletable_2de7b0 *)object)->destroy(1);
	}
}
