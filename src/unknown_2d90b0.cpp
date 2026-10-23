// @flags /O2 /GL- /GX
// Havok (third-party physics library): a stream holder's deleting destructor,
// and its constructor over a new memory stream on a caller's block.
#include "stream_holder_2d8e70.h"

class memory_stream_2d9f00 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x15)

	long field_8;
	char *buffer;
	long position;
	long limit;
	bool field_18;

	memory_stream_2d9f00(char *buffer_, long size, bool zero);
};

// @retail 0x2d90b0 deleting stream_holder_2d8e70

// @retail 0x2d90e0
stream_holder_2d8e70::stream_holder_2d8e70(char *buffer, long size, flag_2d8e70 zero)
{
	stream = new memory_stream_2d9f00(buffer, size, zero.value);
}
