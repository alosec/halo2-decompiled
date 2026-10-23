// @flags /O2 /GL-
// Havok (third-party physics library): an accessor that returns a small
// descriptor by value (hidden return pointer).

struct descriptor_2fe330
{
	long kind;
	bool flag;
	long stride;

	descriptor_2fe330(long kind_, long stride_, bool flag_)
	{
		kind = kind_;
		stride = stride_;
		flag = flag_;
	}
};

class class_2fe330
{
public:
	descriptor_2fe330 get_descriptor() const;
};

// @retail 0x2fe330
descriptor_2fe330 class_2fe330::get_descriptor() const
{
	return descriptor_2fe330(8, 0x10, true);
}
