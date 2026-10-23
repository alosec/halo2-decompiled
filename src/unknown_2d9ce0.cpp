// @flags /O2 /GL-
// Havok (third-party physics library): writing a stream's buffer out to the
// stream beneath it.

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

// @retail 0x2d9ce0
int class_2d9ce0::flush()
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
