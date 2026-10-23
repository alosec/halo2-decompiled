// @flags /O2 /GL-
// Havok (third-party physics library): appends a block to an array, copied
// from a blank one whose two flags are clear, and returns it.
#include "block_2f9b40.h"
#include "growable_array_2d91d0.h"

// a block's layout with only its flags set; the rest is copied as it is
struct blank_2f9e80
{
	__m128 unknown0[7];
	long unknown70[3];
	bool field_7c;
	bool field_7d;
};

ELEMENTS_2D91D0_ALIGNED(blocks_2f9e80, block_2f9b40);

class class_2f9e80
{
public:
	blocks_2f9e80 blocks;

	block_2f9b40 *function_2f9e80();
};

// @retail 0x2f9e80
block_2f9b40 *class_2f9e80::function_2f9e80()
{
	blank_2f9e80 blank;
	blank.field_7d = false;
	blank.field_7c = false;
	// constructs the new slot in place (the compiler's explicit constructor call)
	blocks.next_slot().block_2f9b40::block_2f9b40((block_2f9b40 const &)blank);
	return &blocks.data[blocks.size - 1];
}
