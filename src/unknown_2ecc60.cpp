// @flags /O2 /GL-
// Havok (third-party physics library): telling the object at +0xc, then
// destroying itself through the first virtual slot.

class class_2ecc60_object
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void slot4() = 0;
	virtual void slot5() = 0;
};

class class_2ecc60
{
public:
	virtual void slot0(int flags) = 0;

	long field_4;
	long field_8;
	class_2ecc60_object *object;

	void function_2ecc60();
};

// @retail 0x2ecc60
void class_2ecc60::function_2ecc60()
{
	object->slot5();
	slot0(1);
}
