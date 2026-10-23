// @flags /O2 /GL-
// Havok (third-party physics library): telling every listed object, then
// destroying itself through the first virtual slot.

class class_2eda00_object
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void slot4() = 0;
	virtual void slot5() = 0;
};

class class_2eda00
{
public:
	virtual void slot0(int flags) = 0;

	long field_4;
	long field_8;
	class_2eda00_object **objects;
	int count;

	void function_2eda00();
};

// @retail 0x2eda00
void class_2eda00::function_2eda00()
{
	for (int i = 0; i < count; i++)
	{
		objects[i]->slot5();
	}
	slot0(1);
}
