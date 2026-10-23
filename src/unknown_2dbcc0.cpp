// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description out:
// the base's values, then three of its own fields.
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

struct s_2dbcc0 : s_2fc4e0
{
	long field_8;
	long field_c;
	long field_10;
};

class class_2dbcc0 : public class_2fc4e0
{
public:
	char field_10[0x14];
	long field_24;
	long field_28;
	char field_2c[4];
	long field_30;

	void function_2dbcc0(s_2dbcc0 *out) const;
};

// @retail 0x2dbcc0
void class_2dbcc0::function_2dbcc0(s_2dbcc0 *out) const
{
	function_2fc4e0(out);
	out->field_8 = field_30;
	out->field_c = field_24;
	out->field_10 = field_28;
}
