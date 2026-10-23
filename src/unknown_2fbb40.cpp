// @flags /O2 /GL-
// Havok (third-party physics library): counting the items of an iteration
// that ends at -1.

class class_2fbb40
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void slot4() = 0;
	virtual void slot5() = 0;
	virtual void slot6() = 0;
	virtual void slot7() = 0;
	virtual void slot8() = 0;
	virtual void slot9() = 0;
	virtual void slot10() = 0;
	virtual int first() = 0;
	virtual int next(int item) = 0;

	int count();
};

// @retail 0x2fbb40
int class_2fbb40::count()
{
	int count = 0;
	for (int item = first(); item != -1; item = next(item))
	{
		count++;
	}
	return count;
}
