// @flags /O2 /GL- /GX
// Havok (third-party physics library): creates an object that places a
// child, and builds an object with an identity transform in place.
#include <new>
#include "placed_child_2dec70.h"

// @retail 0x2df4c0
placed_child_2dec70 *__cdecl function_2df4c0(child_info_2dec70 const &info)
{
	return new placed_child_2dec70(info);
}

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct rotation_2df520
{
	__m128 columns[3];

	// the element in a row of a column
	float &element(int row, int column) { return ((float *)columns)[column * 4 + row]; }

	void load_unit_columns()
	{
		columns[0] = _mm_setzero_ps();
		columns[1] = _mm_setzero_ps();
		columns[2] = _mm_setzero_ps();
		element(0, 0) = 1.0f;
		element(1, 1) = 1.0f;
		element(2, 2) = 1.0f;
	}
};

struct identity_2df520 : class_2df1e0
{
	long field_4;
	rotation_2df520 rotation;
	__m128 translation;

	identity_2df520() : field_4(0)
	{
		rotation.load_unit_columns();
		translation = _mm_setzero_ps();
	}
};

// @retail 0x2df520
void __cdecl function_2df520(void *memory)
{
	new (memory) identity_2df520();
}
