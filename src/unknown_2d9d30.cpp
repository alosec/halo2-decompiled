// @flags /O2 /GL-
// Havok (third-party physics library): writing into a buffered stream,
// flushing the buffer to the stream beneath it each time it fills.

void __cdecl function_2d8db0(void *destination, void const *source, long count);

class stream_2d9d30
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual int write(const void *data, int size) = 0;
};

class class_2d9d30
{
public:
	void *vtable;
	long field_4;
	stream_2d9d30 *stream;
	char *buffer;
	int size;
	int capacity;

	int flush()
	{
		if (stream)
		{
			int count = size;
			int written = 0;
			while (written < count)
			{
				int result = stream->write(buffer + written, count - written);
				written += result;
				if (result == 0)
				{
					return written;
				}
			}
			size = 0;
			return written;
		}
		return 0;
	}

	int function_2d9d30(void const *data, int count);
};

// @retail 0x2d9d30
int class_2d9d30::function_2d9d30(void const *data, int count)
{
	char const *source = (char const *)data;
	int remaining = count;
	int space = capacity - size;
	while (remaining > space)
	{
		function_2d8db0(buffer + size, source, space);
		source += space;
		size += space;
		remaining -= space;
		int pending = size;
		if (flush() != pending)
		{
			return count - remaining;
		}
		space = capacity - size;
	}
	function_2d8db0(buffer + size, source, remaining);
	size += remaining;
	return count;
}
