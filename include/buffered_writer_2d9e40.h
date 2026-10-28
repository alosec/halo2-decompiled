#ifndef BUFFERED_WRITER_2D9E40_H
#define BUFFERED_WRITER_2D9E40_H
#include "sized_reference_a9ef0.h"
struct bool_2d9e00
{
	bool value;
	bool_2d9e00(bool value_) { value = value_; }
};
class bool_2d9e40
{
	char value;
public:
	bool_2d9e40(bool value_) : value(value_) {}
	operator bool() const { return value != 0; }
};
class stream_writer_2d9e40 : public c_sized_reference_a9ef0
{
public:
	virtual bool_2d9e00 status() = 0;
	virtual int write(const void *data, int size) = 0;
	virtual void flush_stream() = 0;
	virtual void query(bool *result);
	virtual long seek(long arg_0, long arg_1);
	virtual long tell() { return -1; }
};
class buffered_writer_2d9e40 : public stream_writer_2d9e40
{
public:
	static void *operator new(unsigned int size)
	{
		buffered_writer_2d9e40 *object = (buffered_writer_2d9e40 *)g_memory_2d88a0->allocate_block(size, 0x15);
		object->memory_size = (unsigned short)size;
		return object;
	}
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((buffered_writer_2d9e40 *)object)->memory_size, 0x15);
	}
	stream_writer_2d9e40 *stream;
	char *buffer;
	int size;
	int capacity;
	bool_2d9e40 owns_buffer;
	buffered_writer_2d9e40(stream_writer_2d9e40 *stream_, long capacity_);
	virtual ~buffered_writer_2d9e40();
	virtual bool_2d9e00 status();
	virtual int write(const void *data, int count);
	virtual void flush_stream();
	int flush();
};
#include "../src/unknown_2d9ce0.cpp"
#endif
