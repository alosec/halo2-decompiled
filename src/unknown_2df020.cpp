// @flags /O2 /GL-
// Havok (third-party physics library): appends an entry of two vectors and the
// outermost owners of two linked objects to an array.
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"

struct link_2df020
{
	long unknown0;
	long field_4;
	long unknown8;
	link_2df020 *parent;
};

struct source_2df020
{
	__m128 field_0;
	__m128 field_10;
	link_2df020 *field_20;
	link_2df020 *field_24;
};

struct entry_2df020
{
	__m128 field_0;
	__m128 field_10;
	link_2df020 *field_20;
	long field_24;
	link_2df020 *field_28;
	long field_2c;
};

static inline link_2df020 *outermost_2df020(link_2df020 *link)
{
	while (link->parent != 0)
	{
		link = link->parent;
	}
	return link;
}

ELEMENTS_2D91D0_ALIGNED(entries_2df020, entry_2df020);

class class_2df020
{
public:
	char unknown0[0x10];
	entries_2df020 entries;

	void function_2df020(source_2df020 const *source);
};

// @retail 0x2df020
void class_2df020::function_2df020(source_2df020 const *source)
{
	entry_2df020 &entry = entries.next_slot();
	entry.field_0 = source->field_0;
	entry.field_10 = source->field_10;
	entry.field_20 = outermost_2df020(source->field_20);
	entry.field_24 = source->field_20->field_4;
	entry.field_28 = outermost_2df020(source->field_24);
	entry.field_2c = source->field_24->field_4;
}
