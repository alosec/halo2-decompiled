#ifndef LANE_B_CONSTRAINT_ENTITIES_H
#define LANE_B_CONSTRAINT_ENTITIES_H
#include "growable_array_2d91d0.h"
// Existing canonical entity prefixes retained verbatim; they are different
// pre-existing views of the same retail entity, not new complete entity types.
class hkWorld;
class entity_30bcc0
{
public:
	char unknown0[8];
	hkWorld *m_world;
	char unknown0c[0x88 - 0xc];
	void **listeners;
	long listener_count;
	long index_of(void *listener) const
	{
		for (long i = 0; i < listener_count; i++)
			if (listeners[i] == listener) return i;
		return -1;
	}
	void function_30bcc0(void *listener);
};
ELEMENTS_2D91D0(words_30bdc0, long);
class class_30bdc0
{
public:
	char unknown0[0x88];
	words_30bdc0 words;
	void function_30bdc0(long value);
};
#endif
