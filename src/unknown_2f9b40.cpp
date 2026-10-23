// @flags /O2 /GL-
// Havok (third-party physics library): copying a block of vectors, words and
// two flags, member by member.
#include "block_2f9b40.h"

// @retail 0x2f9b40
block_2f9b40::block_2f9b40(block_2f9b40 const &other)
	: field_0(other.field_0), field_10(other.field_10), field_20(other.field_20), field_24(other.field_24),
	  field_30(other.field_30), field_40(other.field_40), field_50(other.field_50), field_60(other.field_60),
	  field_70(other.field_70), field_74(other.field_74), field_78(other.field_78),
	  field_7c(other.field_7c), field_7d(other.field_7d)
{
}
