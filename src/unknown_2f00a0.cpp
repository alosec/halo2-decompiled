// @flags /O2 /GL-
// Havok (third-party physics library): creation functions for small objects
// holding a value (their fourth argument) and a 16-bit index that starts at
// -1.
#include "reference_counted_2d88a0.h"

class valued_2f00a0
{
public:
	valued_2f00a0(long value_) : value(value_) {}
	virtual ~valued_2f00a0() {}
	unsigned short memory_size;
	unsigned short reference_count;
	long value;
};

class indexed_2f00a0 : public valued_2f00a0
{
public:
	REFERENCED_NEW_2D88A0(0x1a)

	short index;

	indexed_2f00a0(long value_) : valued_2f00a0(value_), index(-1) {}
};

// @retail 0x2f00a0
indexed_2f00a0 *__cdecl function_2f00a0(long arg_0, long arg_1, long arg_2, long value)
{
	return new indexed_2f00a0(value);
}

class indexed_2f00d0 : public indexed_2f00a0
{
public:
	REFERENCED_NEW_2D88A0(0x1a)

	indexed_2f00d0(long value_) : indexed_2f00a0(value_) {}
};

// @retail 0x2f00d0
indexed_2f00d0 *__cdecl function_2f00d0(long arg_0, long arg_1, long arg_2, long value)
{
	return new indexed_2f00d0(value);
}
