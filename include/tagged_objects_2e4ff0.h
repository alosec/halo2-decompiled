#ifndef TAGGED_OBJECTS_2E4FF0_H
#define TAGGED_OBJECTS_2E4FF0_H
// Havok (third-party physics library): objects whose destructors are not
// written yet, each giving its block back to the allocator with its own tag
// when deleted (the size of the block is the word at +4).
#include "reference_counted_2d88a0.h"
#include "unknown_0a9f20.h"

// arrays of 2-, 4- and 16-byte elements that give their blocks back when
// destroyed (the 4-byte one's destructor is out of line, 0x2d7540)
#define ARRAY_2E4FF0(name, type) \
	struct name \
	{ \
		type *data; \
		long size; \
		long capacity_and_flags; \
		name() : data(0), size(0), capacity_and_flags(0x80000000) {} \
		~name() \
		{ \
			if ((capacity_and_flags & 0x80000000) == 0) \
			{ \
				long count = capacity_and_flags & 0x7fffffff; \
				type *block = data; \
				memory_2d88a0 *allocator = g_memory_2d88a0; \
				allocator->release_block(block, count * sizeof(type), 0x12); \
			} \
		} \
	};

ARRAY_2E4FF0(shorts_2e4ff0, short)
ARRAY_2E4FF0(words_2e4ff0, long)

// an entry of the object's block: two 2-byte values and an array of them
struct entry_2e4ff0
{
	long field_0;
	shorts_2e4ff0 values;
};

ARRAY_2E4FF0(entries_2e4ff0, entry_2e4ff0)

// an object of 0x80 bytes: two arrays of its own, three more of words, and
// a block of entries it frees itself
class class_2e4ff0 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x1c)
	class_2e4ff0(long arg_0, long arg_1, int bits);
	virtual ~class_2e4ff0();

	char unknown8[0x40 - 8];
	entries_2e4ff0 field_40;
	words_2e4ff0 field_4c[3];
	long entry_count;
	long field_74;
	entry_2e4ff0 *entries;
	long field_7c;
};

struct s_311340;

class class_30bf50
{
public:
	REFERENCED_NEW_2D88A0(0x28)
	// built from a body's description (0x30beb0)
	class_30bf50(s_311340 const *description);
	virtual ~class_30bf50();
};

class class_310a70
{
public:
	REFERENCED_NEW_2D88A0(0x2a)
	virtual ~class_310a70();
};

class typed_311410;

// an object of the same tag holding a typed object it owns (destructor 0x311410)
class class_311410 : public class_30bf50
{
public:
	REFERENCED_NEW_2D88A0(0x28)
	virtual ~class_311410();

	char unknown4[0x3c - 4];
	typed_311410 *field_3c;
};

// an object of 0xe0 bytes made from a source (0x2fdcf0 builds it)
class class_2fdcf0
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	char storage[0xe0];

	class_2fdcf0(void const *source);
};

#endif
