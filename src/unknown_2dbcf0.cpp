// @flags /O2 /GL-
// Havok (third-party physics library): setting an object from a
// description: its first value, a distance and its square, and a second
// distance with the square of how far it falls short of the first.

struct class_2df1f0
{
	char unknown0[8];
	long field_8;

	void function_2df200(long const *value);
};

struct s_2dc030
{
	long field_0;
	char field_4[4];
	long field_8;
	float field_c;
	float field_10;
};

class c_2dbfc0
{
public:
	char field_0[0x20];
	float field_20;
	float field_24;
	float field_28;
	float field_2c;

	void function_2dbcf0(s_2dc030 const *arg_0);
};

// @retail 0x2dbcf0
void c_2dbfc0::function_2dbcf0(s_2dc030 const *arg_0)
{
	((class_2df1f0 *)this)->function_2df200(&arg_0->field_0);
	field_24 = arg_0->field_c;
	field_20 = arg_0->field_c * arg_0->field_c;
	float second = arg_0->field_10;
	field_28 = second;
	float gap = field_24 - second;
	field_2c = gap * gap;
}
