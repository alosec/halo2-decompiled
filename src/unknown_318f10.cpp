// @flags /O2 /GL-
// Contact-id indirection and collection. This is only the accessed storage
// prefix, not a reconstructed virtual interface or complete object layout.
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"
struct s_contact_point_318f10 { __m128 position, normal; };
struct s_contact_property_318f30 { unsigned char bytes[0x1c]; };
struct s_contact_ids_318f50
{
	unsigned char *data;
	long size, capacity_and_flags;
	void collect(array_2d91d0 *out) const;
};
struct s_contact_storage_318f10
{
	unsigned char unknown0[8];
	void *world;
	long remaining;
	unsigned char unknown10[0x20];
	s_contact_ids_318f50 ids;
	unsigned long unknown3c[2];
	s_contact_point_318f10 *points;
	unsigned char unknown48[0x58];
	s_contact_property_318f30 *properties;
	s_contact_point_318f10 *point(unsigned short id);
	s_contact_property_318f30 *property(unsigned short id);
	void collect(array_2d91d0 *out) const;
};
// @retail 0x318f10
s_contact_point_318f10 *s_contact_storage_318f10::point(unsigned short id)
{
	return points + ids.data[id];
}
// @retail 0x318f30
s_contact_property_318f30 *s_contact_storage_318f10::property(unsigned short id)
{
	return properties + ids.data[id];
}
// @retail 0x318f50
void s_contact_ids_318f50::collect(array_2d91d0 *out) const
{
	for (long i = 0; i < size; ++i)
	{
		if (data[i] != 0xff)
		{
			if (out->size == (out->capacity_and_flags & 0x7fffffff))
				function_2d91d0(out,2);
			((unsigned short *)out->data)[out->size] = (unsigned short)i;
			out->size++;
		}
	}
}
// @retail 0x318fa0
void s_contact_storage_318f10::collect(array_2d91d0 *out) const
{
	ids.collect(out);
}
