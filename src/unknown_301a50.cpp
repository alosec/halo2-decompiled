// @flags /O2 /GL-
// Havok (third-party physics library): an object whose pointer at +8 starts
// empty.

class base_301a50
{
public:
	base_301a50() : field_8(0) {}
	virtual ~base_301a50() {}
	long field_4;
	void *field_8;
};

class holder_301a50 : public base_301a50
{
public:
	holder_301a50();
};

// @retail 0x301a50
holder_301a50::holder_301a50()
{
}
