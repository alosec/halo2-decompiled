// @flags /O2 /GL-
// Havok (third-party physics library): writing into a buffered stream,
// flushing the buffer to the stream beneath it each time it fills.

void __cdecl function_2d8db0(void *destination, void const *source, long count);

#include "buffered_writer_2d9e40.h"

// @retail 0x2d9d30
int buffered_writer_2d9e40::write(void const *data, int count)
{
	char const *source = (char const *)data;
	int remaining = count;
	int space = capacity - size;
	while (remaining > space)
	{
		function_2d8db0(buffer + size, source, space);
		source += space;
		size += space;
		remaining -= space;
		int pending = size;
		if (flush() != pending)
		{
			return count - remaining;
		}
		space = capacity - size;
	}
	function_2d8db0(buffer + size, source, remaining);
	size += remaining;
	return count;
}
