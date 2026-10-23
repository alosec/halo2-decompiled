// @flags /O2 /GL-
// Havok (third-party physics library): an accessor that returns a small
// descriptor by value (hidden return pointer).

struct descriptor_2fc650
{
	long kind;
	bool flag;
	long stride;

	descriptor_2fc650(long kind_, long stride_, bool flag_)
	{
		kind = kind_;
		stride = stride_;
		flag = flag_;
	}
};

class class_2fc650
{
public:
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	descriptor_2fc650 get_descriptor() const;
};

// @retail 0x2fc650
descriptor_2fc650 class_2fc650::get_descriptor() const
{
	return descriptor_2fc650(field_c, 0x10, false);
}
