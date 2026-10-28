#ifndef STDIO_READER_2D7880_H
#define STDIO_READER_2D7880_H
#include <stdio.h>
#include "stream_reader_2d9c80.h"
class marked_reader_2d7880 : public reader_2d9c80
{
public:
	long saved_position;
	marked_reader_2d7880() : saved_position(-1) {}
};
// Retail table 0x411d5c. The inherited skip slot uses existing retail 2d9c80.
class stdio_reader_2d7880 : public marked_reader_2d7880
{
public:
	static void *operator new(unsigned int size)
	{
		stdio_reader_2d7880 *object = (stdio_reader_2d7880 *)g_memory_2d88a0->allocate_block(size, 0x15);
		object->memory_size = (unsigned short)size;
		return object;
	}
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((stdio_reader_2d7880 *)object)->memory_size, 0x15);
	}
	FILE *file;
	bool_2d9e00 is_ok;
	stdio_reader_2d7880(const char *name);
	virtual ~stdio_reader_2d7880();
	virtual bool_2d9e00 status();
	virtual int read(void *data, int count);
	virtual reader_bool_2d7750 mark_supported() { return true; }
	virtual long set_mark(long limit);
	virtual long rewind_mark();
	virtual bool_2d9e00 seek_supported() { return true; }
	virtual long seek(long offset, long origin);
	virtual long tell();
};
#endif
