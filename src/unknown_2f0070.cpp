// @flags /O2 /GL-
// Havok (third-party physics library): handing back an id when one is held,
// then destroying itself through the first virtual slot.

class class_2f0070_owner
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void release_id(unsigned short id) = 0;
};

class class_2f0070
{
public:
	virtual void slot0(int flags) = 0;

	long field_4;
	class_2f0070_owner *owner;
	unsigned short id;

	void function_2f0070();
};

// @retail 0x2f0070
void class_2f0070::function_2f0070()
{
	unsigned short held = id;
	if (held != 0xffff)
	{
		owner->release_id(held);
	}
	slot0(1);
}
