// @flags /O2 /GL-
// Havok (third-party physics library): an object that holds a reference to
// another reference-counted object.
#include "reference_counted_2d88a0.h"

class holder_2d9090
{
public:
	long field_4;
	referenced_2d88a0 *field_8;

	holder_2d9090(referenced_2d88a0 *object);
	virtual ~holder_2d9090() {}
};

// @retail 0x2d9090
holder_2d9090::holder_2d9090(referenced_2d88a0 *object)
{
	field_8 = object;
	object->reference_count++;
}
