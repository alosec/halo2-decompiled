// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's first value and
// its value at +0xc out, and a constructor clearing its second field.

struct class_2df1e0
{
	long field_0;

	class_2df1e0();
};

struct class_2df1f0
{
	char unknown0[8];
	long field_8;

	void function_2df210(long *value) const;
};

struct s_2fc4e0
{
	long field_0;
	long field_4;
};

class class_2fc4e0
{
public:
	char field_0[0xc];
	long field_c;

	void function_2fc4e0(s_2fc4e0 *out) const;
};

// @retail 0x2fc4e0
void class_2fc4e0::function_2fc4e0(s_2fc4e0 *out) const
{
	((class_2df1f0 const *)this)->function_2df210(&out->field_0);
	out->field_4 = field_c;
}

struct cleared_2fc500 : class_2df1e0
{
	long field_4;

	cleared_2fc500();
};

// @retail 0x2fc500
cleared_2fc500::cleared_2fc500()
{
	field_4 = 0;
}
