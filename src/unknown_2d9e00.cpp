// @flags /O2 /GL-
// Havok (third-party physics library): a buffered stream's state, from the
// stream beneath it when there is one.

struct bool_2d9e00
{
	bool value;

	bool_2d9e00(bool value_)
	{
		value = value_;
	}
};

class class_2d9e00_stream
{
public:
	virtual void slot0() = 0;
	virtual bool_2d9e00 slot1() = 0;
};

class class_2d9e00
{
public:
	void *vtable;
	long field_4;
	class_2d9e00_stream *stream;
	long field_c;
	int position;
	int end;

	bool_2d9e00 function_2d9e00();
};

// @retail 0x2d9e00
bool_2d9e00 class_2d9e00::function_2d9e00()
{
	return stream ? stream->slot1() : bool_2d9e00(position != end);
}
