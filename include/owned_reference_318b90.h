#ifndef OWNED_REFERENCE_318B90_H
#define OWNED_REFERENCE_318B90_H
// Havok (third-party physics library): two reference-counted objects (vtables
// 0x415060 and 0x41506c) that keep the object they were made for; their
// blocks come from the allocator with tag 0x1d, and their deleting destructor
// is the one 0x30c3b0 shares.
#include "reference_base_318970.h"

class class_318b90 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x1d)

	class_318b90(void *owner);
	virtual void slot_1() {}

	void *owner;
};

class class_319080 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x1d)

	class_319080(void *owner);
	virtual void slot_1() {}
	virtual void slot_2() {}

	void *owner;
};

#endif
