// @flags /O2 /GL-
// Havok (third-party physics library): an accessor that returns a small
// descriptor by value (hidden return pointer).

struct descriptor_2ddca0
{
	long kind;
	bool flag;
	long stride;

	descriptor_2ddca0(long kind_, long stride_, bool flag_)
	{
		kind = kind_;
		stride = stride_;
		flag = flag_;
	}
};

class class_2ddca0
{
public:
	descriptor_2ddca0 get_descriptor() const;
};

// @retail 0x2ddca0
descriptor_2ddca0 class_2ddca0::get_descriptor() const
{
	return descriptor_2ddca0(2, 0x10, false);
}
