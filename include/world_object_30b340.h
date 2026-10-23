#ifndef WORLD_OBJECT_30B340_H
#define WORLD_OBJECT_30B340_H
// Havok (third-party physics library): a reference-counted object that keeps
// a reference to the object it is given and a list whose owner it is
// (constructor 0x30b340).
#include "reference_counted_2d88a0.h"

struct list_30b340
{
	long field_0;
	void *owner;
	void *data;
	int size;
	unsigned long capacity;

	list_30b340() : field_0(0), owner(0), data(0), size(0), capacity(0x80000000) {}
};

class class_30b340 : public referenced_2d88a0
{
public:
	long field_8;
	referenced_2d88a0 *object;
	long field_10;
	long field_14;
	long field_18;
	long field_1c;
	long field_20;
	long field_24;
	list_30b340 list;

	class_30b340(referenced_2d88a0 *object_, long value);
};

#endif
