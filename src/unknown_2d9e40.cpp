// @flags /O2 /GL- /GX
// Havok (third-party physics library): a buffered writing stream's
// constructor, and its destructor, which flushes its buffer, lets go of the stream beneath it and
// frees the buffer when it owns it.
#include "buffered_writer_2d9e40.h"

bool __cdecl function_279740(long value);

// @retail 0x2d9e40
buffered_writer_2d9e40::~buffered_writer_2d9e40()
{
	flush();
	stream_writer_2d9e40 *object = stream;
	if (object)
	{
		object->reference_count--;
		if (object->reference_count == 0)
		{
			if (function_279740((long)object))
			{
				object->reference_count = 1;
			}
			else
			{
				delete object;
			}
		}
	}
	if (owns_buffer)
	{
		g_memory_2d88a0->release_aligned(buffer);
	}
}

// @retail 0x2d9ed0 deleting buffered_writer_2d9e40

// a writer over a stream, which it holds a reference to, with a buffer of
// its own
// An inline logical-value constructor preserves the initialization boundary.
// @retail 0x2d9f80
buffered_writer_2d9e40::buffered_writer_2d9e40(stream_writer_2d9e40 *stream_, long capacity_)
	: stream(stream_), owns_buffer(true)
{
	if (stream)
	{
		stream->reference_count++;
	}
	buffer = (char *)g_memory_2d88a0->allocate_aligned(0x40, capacity_, 0x15);
	capacity = capacity_;
	size = 0;
}
