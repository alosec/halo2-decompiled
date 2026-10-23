// @flags /O2 /GL-
// Havok (third-party physics library): a call passed on to the listener at
// +8 with its arguments swapped.

class class_3018d0_listener
{
public:
	virtual void slot0() = 0;
	virtual void slot1(long a, long b) = 0;
};

class class_3018d0
{
public:
	long field_0;
	long field_4;
	class_3018d0_listener *listener;

	void function_3018d0(long a, long b);
};

// @retail 0x3018d0
void class_3018d0::function_3018d0(long a, long b)
{
	listener->slot1(b, a);
}
