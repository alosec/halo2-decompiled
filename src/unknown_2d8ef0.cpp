// @flags /O2 /GL-
// Havok (third-party physics library): writing one character to a stream.

class class_2d8ef0_stream
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual int write(const void *data, int size) = 0;
};

class class_2d8ef0
{
public:
	void *vtable;
	long field_4;
	class_2d8ef0_stream *stream;

	class_2d8ef0 *append_character(char value);
};

// @retail 0x2d8ef0
class_2d8ef0 *class_2d8ef0::append_character(char value)
{
	stream->write(&value, 1);
	return this;
}
