// @flags /O2 /GL-
// Havok (third-party physics library): a query of a shape holding a tree of
// parts against the child of a wrapping shape, into the caller's collector.
#include "child_query_2f9950.h"
#include "bounded_2e9530.h"
#include "child_refresh_2e9210.h"

// @retail 0x2fabf0
void __cdecl function_2fabf0(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector)
{
	body_2ea940 child(b, b->placement, (wrapper_2f9950 const *)b->shape);
	function_2ea940(a, &child, input, collector);
}

// the same as 0x2f9e40 with the two sides the other way round
// @retail 0x2fac30
void bounded_2e9530::function_2fac30(typed_2e9210 **other, description_2e9210 const *description, context_2e9210 const *context)
{
	description_2e9210 wrapped;
	wrapped.field_8 = description->field_8;
	wrapped.parent = description;
	wrapped.shape = (wrapper_2e9210 const *)description->shape->inner;
	wrapped.field_4 = description->field_4;
	function_2e9210(other, &wrapped, context);
}
