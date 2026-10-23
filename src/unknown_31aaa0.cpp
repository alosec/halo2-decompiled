// @flags /O2 /GL-
// Havok (third-party physics library): setting a value through its
// reciprocal.

class class_31aaa0
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
	virtual void set_inverse(float inverse) = 0;

	void set(float value);
};

// @retail 0x31aaa0
void class_31aaa0::set(float value)
{
	set_inverse(1.0f / value);
}
