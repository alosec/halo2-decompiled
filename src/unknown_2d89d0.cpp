// @flags /O2 /GL-
// Havok (third-party physics library): keeping a copy of a string once: a
// string already kept is left alone; otherwise it is recorded, and copied,
// with its length rounded up to whole words, after a three-word header in a
// growable block of bytes.
#include "growable_array_2d91d0.h"
#include "string_helpers_2d8c90.h"

struct name_2d89d0
{
	char const *text;
	char const *key;
};

ELEMENTS_2D91D0(names_2d89d0, name_2d89d0);
ELEMENTS_2D91D0(bytes_2d89d0, char);

// the header before each copy
struct copy_2d89d0
{
	long field_0;
	char const *text;
	unsigned int size;
};

// room for `count` more bytes at the end of the block, and where they start
inline char *expand_2d89d0(bytes_2d89d0 &bytes, long count)
{
	long offset = bytes.size;
	bytes.ensure_capacity(offset + count);
	char *start = bytes.data + offset;
	bytes.size = offset + count;
	return start;
}

class strings_2d89d0
{
public:
	names_2d89d0 names;
	bytes_2d89d0 bytes;

	void function_2d89d0(char const *text, unsigned int length);
};

// @retail 0x2d89d0
void strings_2d89d0::function_2d89d0(char const *text, unsigned int length)
{
	for (int i = names.size - 1; i >= 0; i--)
	{
		if (text == names.data[i].key)
		{
			return;
		}
	}
	name_2d89d0 name;
	name.text = text;
	name.key = text;
	names.append(name);
	if (length == 0)
	{
		length = string_length_2d8d80(text) + 1;
	}
	unsigned int size = ((length - 1) & ~3) + 4;
	copy_2d89d0 *copy = (copy_2d89d0 *)expand_2d89d0(bytes, size + sizeof(copy_2d89d0));
	copy->field_0 = 0;
	copy->text = text;
	copy->size = size;
	function_2d8db0(copy + 1, text, size);
}
