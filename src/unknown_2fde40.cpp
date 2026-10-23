// @flags /O2 /GL- /GX
// Havok (third-party physics library): makes a new object over two lists of
// shapes (0x2fc390).
#include "reference_counted_2d88a0.h"

struct lists_2fc200;

class class_2fc200 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	char field_8[0x38 - 8];

	class_2fc200(lists_2fc200 const *lists);
};

// @retail 0x2fde40
referenced_2d88a0 *__cdecl function_2fde40(lists_2fc200 const *lists)
{
	return new class_2fc200(lists);
}
