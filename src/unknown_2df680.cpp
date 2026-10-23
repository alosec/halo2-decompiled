// @flags /O2 /GL- /GX
// Havok (third-party physics library): the destructor of a shape reference
// that also holds a counted object, dropping one reference to it.

class class_3009c0
{
public:
	void drop_one_reference();
};

struct c_shape_reference
{
	virtual ~c_shape_reference();
	unsigned short flags;
	unsigned short unknown06;
	long field_8;
	void *object;
};

struct c_shape_library_base_a : c_shape_reference
{
	class_3009c0 *counted;

	virtual ~c_shape_library_base_a();
};

// @retail 0x2df680
c_shape_library_base_a::~c_shape_library_base_a()
{
	counted->drop_one_reference();
}
