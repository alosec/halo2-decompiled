#ifndef LISTENED_OBJECT_315950_H
#define LISTENED_OBJECT_315950_H
// Havok (third-party physics library): an object over 0x30b3a0's with an
// array of words and an array of listeners; destroying it tells each listener
// still registered, from the last to the first (destructor 0x315950,
// deleting destructor 0x30b450).
#include "reference_holder_30b3a0.h"

class class_315950;

class listener_315950
{
public:
	virtual void object_destroyed(class_315950 *object) = 0;
};

// arrays of 4-byte elements that give their blocks back when destroyed
#define WORDS_315950(name, type) \
	struct name \
	{ \
		type *data; \
		long size; \
		long capacity_and_flags; \
		~name() \
		{ \
			if ((capacity_and_flags & 0x80000000) == 0) \
			{ \
				type *block = data; \
				g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(type), 0x12); \
			} \
		} \
	}

WORDS_315950(words_315950, long);
WORDS_315950(listeners_315950, listener_315950 *);

class class_315950 : public class_30b3a0
{
public:
	words_315950 words;
	listeners_315950 listeners;

	virtual ~class_315950();

	REFERENCED_NEW_2D88A0(0x2c)
};

#endif
