// @flags /O2 /GL-
// Havok (third-party physics library): replacing the world's listener object
// (a new default one when none is given), and refreshing what points at it.
#include "reference_counted_2d88a0.h"

bool __cdecl function_279740(long value);

class c_2dfbf0;

struct s_30cb70
{
	byte field_0;
};

class listener_a_30d1b0
{
public:
	virtual void slot_0() {}
};

class listener_b_30d1b0
{
public:
	virtual void slot_0() {}
};

class listener_c_30d1b0
{
public:
	virtual void slot_0() {}
};

class listener_d_30d1b0
{
public:
	virtual void slot_0() {}
};

// the listener object (constructor 0x30d1b0)
class listeners_30d1b0 : public referenced_2d88a0, public listener_a_30d1b0, public listener_b_30d1b0,
	public listener_c_30d1b0, public listener_d_30d1b0
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	listeners_30d1b0();
};

class deletable_310900
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

struct holder_310900
{
	long field_0;
	listener_b_30d1b0 *listener;
};

class class_30f710
{
public:
	void function_30f710();
};

class c_3101c0
{
public:
	char unknown0[0xcc];
	holder_310900 *holder;
	listeners_30d1b0 *listeners;
	char unknownd4[0x144 - 0xd4];
	listener_b_30d1b0 *listener;

	void function_310900(c_2dfbf0 *arg_0, s_30cb70 arg_1);
};

// @retail 0x310900
void c_3101c0::function_310900(c_2dfbf0 *arg_0, s_30cb70 arg_1)
{
	listeners_30d1b0 *replacement = (listeners_30d1b0 *)arg_0;
	if (replacement == 0)
	{
		replacement = new listeners_30d1b0;
	}
	else
	{
		replacement->reference_count++;
	}
	listeners_30d1b0 *old = listeners;
	old->reference_count--;
	if (old->reference_count == 0)
	{
		if (function_279740((long)old))
		{
			old->reference_count = 1;
		}
		else
		{
			// the deleting destructor, first in the vtable, without the
			// null test `delete` adds
			((deletable_310900 *)old)->destroy(1);
		}
	}
	listeners = replacement;
	holder->listener = replacement;
	listener = listeners;
	if (arg_1.field_0)
	{
		((class_30f710 *)this)->function_30f710();
	}
}
