// @flags /O2 /Ob1 /GL- /GX
// A binary file stream: writing, closing on failure, flushing and seeking.
#include "stdio_writer_2d7aa0.h"
// @retail 0x2d7aa0
stdio_writer_2d7aa0::stdio_writer_2d7aa0(const char *name)
	: owns_file(true)
{
	file = fopen(name, "wb");
}
// @retail 0x2d7ad0
int stdio_writer_2d7aa0::write(const void *data, int count)
{
	if (file)
	{
		int written = (int)fwrite(data, 1, count, file);
		if (written <= 0)
		{
			close();
		}
		return written;
	}
	return 0;
}
// @retail 0x2d7b10
void stdio_writer_2d7aa0::close()
{
	if (file > 0 && owns_file)
	{
		fclose(file);
	}
	file = 0;
}
// @retail 0x2d7b40
void stdio_writer_2d7aa0::flush_stream()
{
	if (file)
	{
		fflush(file);
	}
}
// @retail 0x2d7b50
bool_2d9e00 stdio_writer_2d7aa0::status()
{
	return file != 0;
}
// @retail 0x2d7b70
long stdio_writer_2d7aa0::seek(long offset, long origin)
{
	return fseek(file, offset, origin) != 0;
}
// @retail 0x2d7b90
long stdio_writer_2d7aa0::tell()
{
	return ftell(file);
}
// @retail 0x2d7bd0
stdio_writer_2d7aa0::~stdio_writer_2d7aa0()
{
	if (file > 0 && owns_file)
	{
		fclose(file);
	}
	file = 0;
}
// @retail 0x2d7ba0 deleting stdio_writer_2d7aa0
