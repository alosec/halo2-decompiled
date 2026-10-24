// @flags /O2 /GL-
// Havok (third-party physics library): refilling a buffered stream's buffer
// (keeping the bytes after a mark, when there is one, at the same offset
// within a 512-byte block), and reading and skipping bytes in it.
#include "growable_array_2d91d0.h"
#include "string_helpers_2d8c90.h"

// a yes-or-no result returned through a hidden pointer
struct flag_2d9850
{
	bool value;

	flag_2d9850(bool value_)
	{
		value = value_;
	}
};

// the stream beneath: whether it can still be read (slot 1), and reading
// into a buffer (slot 2), which gives the number of bytes read
class source_2d9850
{
public:
	virtual void slot_0() {}
	virtual flag_2d9850 slot_1() { return flag_2d9850(false); }
	virtual int read(void *buffer, int count) { return 0; }
};

class class_2d99d0
{
public:
	char unknown0[8];
	source_2d9850 *source;
	char *buffer;
	int position;
	int end;
	int capacity;
	// where the bytes to keep start, -1 for none, and how many may be kept
	int mark;
	int mark_limit;

	// refills the buffer; nonzero when nothing more could be read
	int function_2d9850();
	int function_2d9940(void *destination, int count);
	int function_2d99d0(int count);
};

// the bytes after the mark are moved to the start of the block they lie in,
// so that they end on a block boundary; the space that is left is then read
// from the stream
// @retail 0x2d9850
int class_2d99d0::function_2d9850()
{
	if (!source->slot_1().value)
	{
		return 1;
	}
	if (mark < 0)
	{
		position = 0;
		end = 0;
	}
	else if (position - mark > mark_limit)
	{
		position = 0;
		end = 0;
		mark = -1;
		mark_limit = -1;
	}
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
		if (count != wanted)
		{
			return total == 0;
		}
	}
	return 0;
}

// copies `count` bytes out; returns how many were copied, fewer when the
// stream ran out
// @retail 0x2d9940
int class_2d99d0::function_2d9940(void *destination, int count)
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
		if (function_2d9850())
		{
			return count - left;
		}
		available = end - position;
	}
	function_2d8db0(out, buffer + position, left);
	position += left;
	return count;
}

// skips `count` bytes; returns how many were skipped, fewer when the stream
// ran out
// @retail 0x2d99d0
int class_2d99d0::function_2d99d0(int count)
{
	int left = count;
	int available = end - position;
	while (left > available)
	{
		left -= available;
		if (function_2d9850())
		{
			return count - left;
		}
		available = end - position;
	}
	position += left;
	return count;
}
