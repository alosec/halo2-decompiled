// @flags /O2 /GL-
// Havok (third-party physics library): writing a stream's buffer out to the
// stream beneath it.

#ifndef BUFFERED_WRITER_2D9CE0_INLINE
#define BUFFERED_WRITER_2D9CE0_INLINE
#include "buffered_writer_2d9e40.h"

// @retail 0x2d9ce0
inline int buffered_writer_2d9e40::flush()
{
	if (stream)
	{
		int count = size;
		int written = 0;
		while (written < count)
		{
			int result = stream->write(buffer + written, count - written);
			written += result;
			if (result == 0)
			{
				return written;
			}
		}
		size = 0;
		return written;
	}
	return 0;
}
#endif
