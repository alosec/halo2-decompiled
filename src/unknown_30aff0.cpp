// @flags /O2 /GL-
// Havok (third-party physics library): an object with bounds of its own set
// from a description, when they moved by more than a small tolerance, or set
// directly; either way its overlaps are brought up to date.
#include "placed_shape_312800.h"

// every bit but the sign bit, in four lanes
__declspec(align(16)) static const unsigned long absolute_mask_455940[4] = {
	0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff
};

// whether the first three terms of two vectors are the same within a
// tolerance
inline bool equal_within_30aff0(__m128 const &a, __m128 const &b, float tolerance)
{
	return (_mm_movemask_ps(_mm_cmplt_ps(_mm_set1_ps(tolerance),
		_mm_and_ps(_mm_sub_ps(a, b), *(__m128 const *)absolute_mask_455940))) & 7) == 0;
}

// the object as callers in the game and in Havok see it
class class_30aff0 : public bounded_30aff0
{
public:
	void function_30aff0(void const *description);
};

class c_30b080 : public bounded_30aff0
{
public:
	void function_30b080(__m128 const *replacement_bounds);
};

// @retail 0x30aff0
void class_30aff0::function_30aff0(void const *info)
{
	c_contact_query_bounds_info const *description = (c_contact_query_bounds_info const *)info;
	((class_312430 *)this)->function_3124c0(description);
	if (!equal_within_30aff0(bounds.minimum, description->bounds.minimum, 1e-4f) ||
		!equal_within_30aff0(bounds.maximum, description->bounds.maximum, 1e-4f))
	{
		bounds.minimum = description->bounds.minimum;
		bounds.maximum = description->bounds.maximum;
		function_315a10(&bounds);
	}
}

// @retail 0x30b080
void c_30b080::function_30b080(__m128 const *replacement_bounds)
{
	bounds_312800 *kept = &bounds;
	kept->minimum = replacement_bounds[0];
	kept->maximum = replacement_bounds[1];
	function_315a10(kept);
}
