// @flags /O2 /GL-
// Havok (third-party physics library): an accessor that returns a small
// descriptor by value (hidden return pointer).

struct descriptor_2fcb50
{
	long kind;
	bool flag;
	long stride;

	descriptor_2fcb50(long kind_, long stride_, bool flag_)
	{
		kind = kind_;
		stride = stride_;
		flag = flag_;
	}
};

class class_2fcb50
{
public:
	descriptor_2fcb50 get_descriptor() const;
};

// @retail 0x2fcb50
descriptor_2fcb50 class_2fcb50::get_descriptor() const
{
	return descriptor_2fcb50(3, 0x10, true);
}
