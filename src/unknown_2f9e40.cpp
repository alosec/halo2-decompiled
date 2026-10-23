// @flags /O2 /GL-
// Havok (third-party physics library): bringing the objects kept for a
// doubly wrapped collection's children up to date (0x2e9210), through a
// description of the inner wrapper.
#include "bounded_2e9530.h"
#include "child_refresh_2e9210.h"

// @retail 0x2f9e40
void bounded_2e9530::function_2f9e40(description_2e9210 const *description, typed_2e9210 **other, context_2e9210 const *context)
{
	description_2e9210 wrapped;
	wrapped.field_8 = description->field_8;
	wrapped.parent = description;
	wrapped.shape = (wrapper_2e9210 const *)description->shape->inner;
	wrapped.field_4 = description->field_4;
	function_2e9210(other, &wrapped, context);
}
