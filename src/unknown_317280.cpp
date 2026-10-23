// @flags /O2 /GL- /GX
// Havok (third-party physics library): the destructor of an object built on
// two interfaces that owns seven arrays (of 4-, 16- and 8-byte elements),
// each giving its storage back to the allocator unless it does not own it.
#include "reference_counted_2d88a0.h"

// storage given back to the allocator
inline void release_317280(void *data, long size)
{
	g_memory_2d88a0->release_block(data, size, 0x12);
}

// an array whose capacity's top bit says it does not own its storage
#define OWNED_ARRAY_317280(name, T) \
	struct name \
	{ \
		T *data; \
		long size; \
		unsigned long capacity_and_flags; \
		~name() \
		{ \
			if ((capacity_and_flags & 0x80000000) == 0) \
			{ \
				release_317280(data, (capacity_and_flags & 0x7fffffff) * sizeof(T)); \
			} \
		} \
	};

struct pair_317280
{
	long first;
	long second;
};

struct vector_317280
{
	float terms[4];
};

OWNED_ARRAY_317280(longs_317280, long)
OWNED_ARRAY_317280(vectors_317280, vector_317280)
OWNED_ARRAY_317280(pairs_317280, pair_317280)

class first_base_317280 : public referenced_2d88a0
{
public:
	virtual ~first_base_317280() {}

	char unknown8[0x14];
};

class second_base_317280
{
public:
	virtual ~second_base_317280() {}

	char unknown20[0x1c];
};

class owner_317280 : public first_base_317280, public second_base_317280
{
public:
	REFERENCED_NEW_2D88A0(0x2d)

	virtual ~owner_317280();

	longs_317280 longs_3c;
	long field_48;
	longs_317280 longs_4c;
	longs_317280 longs_58;
	long field_64;
	vectors_317280 vectors_68;
	pairs_317280 pairs_74;
	pairs_317280 pairs_80;
	pairs_317280 pairs_8c;
};

// @retail 0x317280
owner_317280::~owner_317280()
{
}

// @retail 0x317fe0 deleting owner_317280
// the deleting destructor reached through the second interface, at 0x1c
// @retail 0x317270 adjustor owner_317280 0x1c
