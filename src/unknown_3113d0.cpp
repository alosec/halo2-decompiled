// @flags /O2 /GL- /GX
// Havok (third-party physics library): constructs a description in place, in
// the memory it is given.
#include <new>
#include "tagged_objects_2e4ff0.h"

struct s_311340
{
	s_311340();
};

// @retail 0x3113d0
void __cdecl function_3113d0(void *memory)
{
	new (memory) s_311340;
}

// the object held at 0x3c: types 6 and 7 own a further object, which the
// accessor 0x30c180 returns
class typed_311410
{
public:
	virtual ~typed_311410() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual long type() { return 0; }
};

class owned_311410
{
public:
	virtual ~owned_311410() {}
};

struct class_30c180
{
	char unknown0[0xc0];
	long field_c0;

	long function_30c180() const;
};

// deletes the held object, and first, for types 6 and 7, the object it owns
// @retail 0x311410
class_311410::~class_311410()
{
	typed_311410 *held = field_3c;
	if (field_3c->type() == 6)
	{
		owned_311410 *owned = (owned_311410 *)((class_30c180 *)held)->function_30c180();
		delete owned;
	}
	if (field_3c->type() == 7)
	{
		owned_311410 *owned = (owned_311410 *)((class_30c180 *)held)->function_30c180();
		delete owned;
	}
	delete held;
}
