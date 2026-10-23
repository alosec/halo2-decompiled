// @flags /O2 /GL-
// Havok (third-party physics library): handing back each of 3 ids that is
// held, then deleting itself.

class class_2f2870_owner
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void release_id(unsigned short id) = 0;
};

class class_2f2870
{
public:
	virtual ~class_2f2870() {}

	long field_4;
	class_2f2870_owner *owner;
	unsigned short ids[3];

	void function_2f2870();
};

// @retail 0x2f2870
void class_2f2870::function_2f2870()
{
	for (int i = 0; i < 3; i++)
	{
		unsigned short id = ids[i];
		if (id != 0xffff)
		{
			owner->release_id(id);
		}
	}
	delete this;
}
