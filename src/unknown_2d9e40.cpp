// @flags /O2 /GL- /GX
// Havok (third-party physics library): a buffered writing stream's
// constructor, and its destructor, which flushes its buffer, lets go of the stream beneath it and
// frees the buffer when it owns it.
#include "reference_counted_2d88a0.h"

bool __cdecl function_279740(long value);

class class_2d9ce0_stream
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual int write(const void *data, int size) = 0;
};

class class_2d9ce0
{
public:
	void *vtable;
	long field_4;
	class_2d9ce0_stream *stream;
	char *buffer;
	int size;

	int flush();
};

class deletable_2d8e70
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

class buffered_writer_2d9e40 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x15)

	referenced_2d88a0 *stream;
	void *buffer;
	long size;
	long capacity;
	bool owns_buffer;

	buffered_writer_2d9e40(referenced_2d88a0 *stream_, long capacity_);
	~buffered_writer_2d9e40();
};

// @retail 0x2d9e40
buffered_writer_2d9e40::~buffered_writer_2d9e40()
{
	((class_2d9ce0 *)this)->flush();
	referenced_2d88a0 *object = stream;
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
	if (owns_buffer)
	{
		g_memory_2d88a0->release_aligned(buffer);
	}
}

// @retail 0x2d9ed0 deleting buffered_writer_2d9e40

// a writer over a stream, which it holds a reference to, with a buffer of
// its own
// score 30 (was 30): retail stores the stream before the vtable and reloads it
// for the test (as if the stream were set by a base between); an intermediate
// base gets the store order but scores 34.
// @retail 0x2d9f80
buffered_writer_2d9e40::buffered_writer_2d9e40(referenced_2d88a0 *stream_, long capacity_)
	: stream(stream_), owns_buffer(true)
{
	if (stream)
	{
		stream->reference_count++;
	}
	buffer = g_memory_2d88a0->allocate_aligned(0x40, capacity_, 0x15);
	capacity = capacity_;
	size = 0;
}
