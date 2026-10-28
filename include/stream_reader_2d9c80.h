#ifndef STREAM_READER_2D9C80_H
#define STREAM_READER_2D9C80_H
#include "buffered_writer_2d9e40.h"
// The ten-slot reader interface, including the existing default skip method.
class reader_bool_2d7750
{
	char value;
public:
	reader_bool_2d7750(bool value_) : value(value_) {}
	reader_bool_2d7750 operator!() const { return value == 0; }
	operator bool() const { return value != 0; }
};
class reader_2d9c80 : public c_sized_reference_a9ef0
{
public:
	virtual bool_2d9e00 status() = 0;
	virtual int read(void *data, int count) = 0;
	virtual long skip(long count);
	virtual reader_bool_2d7750 mark_supported() = 0;
	virtual long set_mark(long limit) = 0;
	virtual long rewind_mark() = 0;
	virtual bool_2d9e00 seek_supported() = 0;
	virtual long seek(long offset, long origin) = 0;
	virtual long tell() = 0;
};
#endif
