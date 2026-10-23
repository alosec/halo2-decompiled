// @flags /O2 /GL-
// Havok (third-party physics library): a constructor that starts its second
// field at the global default of 0.05.

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct class_2df1d0 : class_2df1e0
{
	class_2df1d0();
};

// the default 0.05 the constructor below copies
float g_461eec = 0.05f;

struct class_2df1a0 : class_2df1d0
{
	float field_4;

	class_2df1a0();
};

// @retail 0x2df1a0
class_2df1a0::class_2df1a0()
{
	field_4 = g_461eec;
}
