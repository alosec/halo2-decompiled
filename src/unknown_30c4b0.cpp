// @flags /O2 /GL-
// Havok (third-party physics library): an object with two interfaces, the
// second of which derives from another.

class first_30c4b0
{
public:
	virtual ~first_30c4b0() {}
	long field_4;
};

class second_base_30c4b0
{
public:
	virtual ~second_base_30c4b0() {}
};

class second_30c4b0 : public second_base_30c4b0
{
public:
	virtual ~second_30c4b0() {}
};

class both_30c4b0 : public first_30c4b0, public second_30c4b0
{
public:
	both_30c4b0();
};

// @retail 0x30c4b0
both_30c4b0::both_30c4b0()
{
}
