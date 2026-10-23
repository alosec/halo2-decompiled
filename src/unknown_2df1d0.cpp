// @flags /O2 /GL-
// Havok (third-party physics library): a constructor around the object
// whose only field starts cleared.

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct class_2df1d0 : class_2df1e0
{
	class_2df1d0();
};

// @retail 0x2df1d0
class_2df1d0::class_2df1d0()
{
}
