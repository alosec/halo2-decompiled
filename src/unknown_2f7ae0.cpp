// @flags /O2 /GL-
// Havok (third-party physics library): handing back each of 8 ids that is
// held, then deleting itself.

class class_2f7ae0_owner
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void release_id(unsigned short id) = 0;
};

class class_2f7ae0
{
public:
	virtual ~class_2f7ae0() {}

	long field_4;
	class_2f7ae0_owner *owner;
	char unknown_c[0x1c - 0xc];
	unsigned short ids[8];

	void function_2f7ae0();
};

// @retail 0x2f7ae0
void class_2f7ae0::function_2f7ae0()
{
	for (int i = 0; i < 8; i++)
	{
		unsigned short id = ids[i];
		if (id != 0xffff)
		{
			owner->release_id(id);
		}
	}
	delete this;
}
