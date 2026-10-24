#ifndef HOLDER_30DC80_H
#define HOLDER_30DC80_H
// Havok (third-party physics library): a handler the dispatcher tells of the
// pairs it runs, which notes the groups those pairs reach in an array with
// room for four on the object itself.
#include "reference_counted_2d88a0.h"

// the handler's interface: a table only (its destructor is 0x30c0e0's)
class pair_handler_30dc80
{
public:
	virtual ~pair_handler_30dc80() {}

	// gives back the size of the interface alone, whatever the object's
	static void operator delete(void *block)
	{
		g_memory_2d88a0->release_block(block, sizeof(pair_handler_30dc80), 0x1c);
	}
	virtual void slot_1(void const *pair) = 0;
	virtual void slot_2(void const *pair) = 0;
};

struct group_30dc80;

struct values_30dc80
{
	group_30dc80 **data;
	long size;
	unsigned long capacity_and_flags;

	~values_30dc80()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(group_30dc80 *), 0x12);
		}
	}
};

class holder_30dc80 : public pair_handler_30dc80
{
public:
	void *body;
	values_30dc80 values;
	group_30dc80 *storage[4];

	holder_30dc80(void *body_)
	{
		values.data = storage;
		values.size = 0;
		values.capacity_and_flags = 0x80000004;
		body = body_;
#if H2_WHOLE_IMAGE
		/* retail's table (0x4138f0: 0x30f2a0, 0x30dad0, 0x30dba0), stored
		   inline by its users (0x30df80, 0x30e140); the image binds the class
		   to its base's table 0x41387c (its destructor's), whose pair slots
		   are empty, so the pairs a fixed body's insertion begins or its
		   removal ends never reached the other bodies' islands */
		*(void const **)this = (void const *)0x4138f0;
#endif
	}
	// the pairs a dispatcher adds and removes (0x30dad0, 0x30dba0)
	virtual void slot_1(void const *pair) {}
	virtual void slot_2(void const *pair) {}
};

#endif
