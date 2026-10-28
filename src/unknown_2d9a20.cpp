// @flags /O2 /GL-
#include "buffered_reader_2d9c00.h"
#include "string_helpers_2d8c90.h"
#include "growable_array_2d91d0.h"

// @retail 0x2d9a20
bool_2d9e00 buffered_2d9c00::status()
{
	return bool_2d9e00(position != end || source->status().value);
}
// @retail 0x2d9a60
reader_bool_2d7750 buffered_2d9c00::mark_supported()
{
	return reader_bool_2d7750(capacity != 0);
}
// @retail 0x2d9a80
long buffered_2d9c00::set_mark(long value)
{
	mark = position;
	mark_limit = value;
	return value > capacity;
}
// @retail 0x2d9aa0
long buffered_2d9c00::rewind_mark()
{
	if (mark >= 0) { position = mark; return 0; }
	return 1;
}
// @retail 0x2d9ac0
bool_2d9e00 buffered_2d9c00::seek_supported()
{
	return bool_2d9e00(source && source->seek_supported().value);
}
// @retail 0x2d9b00
long buffered_2d9c00::seek(long offset, long origin)
{
	mark = -1;
	mark_limit = -1;
	position = 0;
	end = 0;
	return source->seek(offset, origin);
}
// @retail 0x2d9b20
long buffered_2d9c00::tell()
{
	int buffered = end - position;
	int offset = source->tell();
	if (offset >= 0) return offset - buffered;
	return -1;
}
int buffered_2d9c00::refill()
{
	if (!source->status().value) return 1;
	if (mark < 0) { position = 0; end = 0; }
	else if (position - mark > mark_limit)
	{ position = 0; end = 0; mark = -1; mark_limit = -1; }
	else if (mark > 0)
	{
		int kept = position - mark;
		int partial = kept % 512;
		int start = partial ? 512 - partial : 0;
		function_2d8dd0(buffer + start, buffer + mark, kept);
		mark = start;
		int blocks = kept / 512 + (partial != 0);
		position = blocks * 512;
		end = position;
	}
	int wanted = capacity - end;
	int total = 0;
	while (total < wanted)
	{
		int count = source->read(buffer + position, wanted);
		total += count;
		end += count;
		if (count != wanted) return total == 0;
	}
	return 0;
}
int buffered_2d9c00::read(void *destination, int count)
{
	char *out = (char *)destination;
	int left = count;
	int available = end - position;
	while (left > available)
	{
		function_2d8db0(out, buffer + position, available);
		out += available;
		left -= available;
		position += available;
		if (refill()) return count - left;
		available = end - position;
	}
	function_2d8db0(out, buffer + position, left);
	position += left;
	return count;
}
long buffered_2d9c00::skip(long count)
{
	int left = count;
	int available = end - position;
	while (left > available)
	{
		left -= available;
		if (refill()) return count - left;
		available = end - position;
	}
	position += left;
	return count;
}
