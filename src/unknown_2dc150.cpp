// @flags /O2 /GL-
// Havok (third-party physics library): creates an object holding an empty
// array that does not own its storage.
#include "reference_counted_2d88a0.h"

class array_owner_2dc150 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x13)

	struct array
	{
		void *data;
		long count;
		unsigned long capacity_and_flags;

		array() : data(0), count(0), capacity_and_flags(0x80000000) {}
	} items;
};

// @retail 0x2dc150
array_owner_2dc150 *__cdecl function_2dc150()
{
	return new array_owner_2dc150();
}
