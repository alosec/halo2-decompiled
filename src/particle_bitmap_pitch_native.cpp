// Source-local ARM-portable reconstruction of the compact bitmap row-size helper.
// The Xbox implementation remains in unknown_136490.cpp.

#include "unknown_11c920.h"

struct s_type_7ba8e9
{
	dword signature;
	short width;
	short height;
	char depth;
	byte unknown09;
	short type;
	short format;
	word flags;
};
static_assert(sizeof(s_type_7ba8e9) == 0x10, "s_type_7ba8e9 prefix");

static __forceinline short bitmap_format_bits(short format)
{
	if (format == NONE || format == 4 || format == 5 || format == 7 ||
		format == 12 || format == 13)
	{
		return 0;
	}
	if (format <= 2 || (format >= 15 && format <= 18))
		return 8;
	if (format == 3 || format == 6 || format == 8 || format == 9 ||
		format >= 22)
	{
		return 16;
	}
	if (format <= 11)
		return 32;
	if (format == 14)
		return 4;
	if (format == 19)
		return 128;
	if (format == 20)
		return 96;
	return 48;
}

// @retail 0x1365a0
__attribute__((optnone)) long function_1365a0(
	s_type_7ba8e9 const *bitmap, long mipmap_index)
{
	short width = bitmap->width >> mipmap_index > 1
		? bitmap->width >> mipmap_index
		: 1;
	if (bitmap->flags & 2)
		width += -width & 3;
	short bits = bitmap_format_bits(bitmap->format);
	return bits * width / 8;
}
