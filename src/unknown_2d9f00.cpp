// @flags /O2 /GL- /GX
// Havok (third-party physics library): a stream over a caller's memory
// block, optionally zeroed, keeping its last byte back when asked.
#include "reference_counted_2d88a0.h"

void __cdecl function_2d8de0(void *buffer, int value, unsigned long size);

class memory_stream_2d9f00 : public referenced_2d88a0
{
public:
	long field_8;
	char *buffer;
	long position;
	long limit;
	bool field_18;

	memory_stream_2d9f00(char *buffer_, long size, bool zero);
};

// @retail 0x2d9f00
memory_stream_2d9f00::memory_stream_2d9f00(char *buffer_, long size, bool zero)
	: field_8(0), buffer(buffer_), position(0), limit(zero ? size - 1 : size), field_18(false)
{
	if (zero)
	{
		function_2d8de0(buffer_, 0, size);
	}
}
