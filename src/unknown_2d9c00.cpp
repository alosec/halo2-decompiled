// @flags /O2 /GL- /GX
// Havok (third-party physics library): a stream over another stream with an
// aligned buffer, and a stream's skip, which reads into a 512-byte scratch
// array until the count is used up or a read returns nothing.
#include "buffered_reader_2d9c00.h"

bool __cdecl function_279740(long value);

class deletable_2d8e70
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

// @retail 0x2d9b40
buffered_2d9c00::~buffered_2d9c00()
{
	c_sized_reference_a9ef0 *object = source;
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
				// the deleting destructor, first in the vtable, without the
				// null test `delete` adds
				((deletable_2d8e70 *)object)->destroy(1);
			}
		}
	}
	g_memory_2d88a0->release_aligned(buffer);
}

// @retail 0x2d9bd0 deleting buffered_2d9c00

// @retail 0x2d9c00
buffered_2d9c00::buffered_2d9c00(reader_2d9c80 *source_, long size_)
	: source(source_), position(0), end(0), capacity(size_), mark(-1), mark_limit(-1)
{
	source_->reference_count++;
	buffer = (char *)g_memory_2d88a0->allocate_aligned(0x40, size_, 0x15);
}

#include "stream_reader_2d9c80.h"

// @retail 0x2d9c80
long reader_2d9c80::skip(long count)
{
	char scratch[0x200];
	long left = count;
	while (left != 0)
	{
		long chunk = left > 0x200 ? 0x200 : left;
		long done = read(scratch, chunk);
		if (done == 0)
		{
			break;
		}
		left -= done;
	}
	return count - left;
}
