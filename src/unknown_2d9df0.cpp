// @flags /O2 /GL-
// Havok (third-party physics library): a buffered stream's flush, passed on.
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
	void function_2d9df0();
};

// @retail 0x2d9df0
void class_2d9ce0::function_2d9df0()
{
	flush();
}
