// @flags /O2 /GL-
// Havok (third-party physics library): the constructor of a
// reference-counted object that implements four listener interfaces.
#include "reference_counted_2d88a0.h"

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

class listeners_30d1b0 : public referenced_2d88a0, public listener_a_30d1b0, public listener_b_30d1b0,
	public listener_c_30d1b0, public listener_d_30d1b0
{
public:
	listeners_30d1b0();
};

// @retail 0x30d1b0
listeners_30d1b0::listeners_30d1b0()
{
}
