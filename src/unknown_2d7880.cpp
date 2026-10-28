// @flags /O2 /Ob1 /GL- /GX
// A binary file reader with a saved seek position and one-byte status.
#include "stdio_reader_2d7880.h"
// @retail 0x2d7880
stdio_reader_2d7880::stdio_reader_2d7880(const char *name)
	: is_ok(true)
{
	file = fopen(name, "rb");
	is_ok.value = file != 0;
}
// @retail 0x2d78c0
long stdio_reader_2d7880::set_mark(long limit)
{
	saved_position = tell();
	return saved_position == -1;
}
// @retail 0x2d78e0
long stdio_reader_2d7880::rewind_mark()
{
	return seek(saved_position, 0);
}
// @retail 0x2d78f0
int stdio_reader_2d7880::read(void *data, int count)
{
	int done = (int)fread(data, 1, count, file);
	if (done <= 0)
	{
		is_ok.value = false;
	}
	return done;
}
// @retail 0x2d7920
bool_2d9e00 stdio_reader_2d7880::status()
{
	return is_ok;
}
// @retail 0x2d7930
long stdio_reader_2d7880::seek(long offset, long origin)
{
	return fseek(file, offset, origin) != 0;
}
// @retail 0x2d7950
long stdio_reader_2d7880::tell()
{
	return ftell(file);
}
// @retail 0x2d7990
stdio_reader_2d7880::~stdio_reader_2d7880()
{
	if (file)
	{
		fclose(file);
	}
}
// @retail 0x2d7960 deleting stdio_reader_2d7880
