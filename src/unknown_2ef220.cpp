// @flags /O2 /GL-
// Havok (third-party physics library): handing back every listed id, then
// destroying itself through the first virtual slot.

class class_2ef220_owner
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void release_id(unsigned short id) = 0;
};

struct s_2ef220_entry
{
	unsigned short id;
	unsigned short field_2;
	unsigned short field_4;
};

class class_2ef220
{
public:
	virtual void slot0(int flags) = 0;

	long field_4;
	class_2ef220_owner *owner;
	char unknownc[0x28];
	s_2ef220_entry entries[8];
	int count;

	void function_2ef220();
};

// @retail 0x2ef220
void class_2ef220::function_2ef220()
{
	for (int i = 0; i < count; i++)
	{
		owner->release_id(entries[i].id);
	}
	slot0(1);
}
