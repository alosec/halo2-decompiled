// @flags /O2 /GL-
// Havok (third-party physics library): flushing a stream.

class class_2d9070_stream
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void flush() = 0;
};

struct class_2d9070
{
	void *vtable;
	long field_4;
	class_2d9070_stream *stream;
};

// @retail 0x2d9070
class_2d9070 *__cdecl function_2d9070(class_2d9070 *writer)
{
	writer->stream->flush();
	return writer;
}
