// @flags /O2 /GL-
// Havok (third-party physics library): copying six fields out.

struct s_2d9570
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	long field_1c;
};

class class_2d9570
{
public:
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	long field_1c;
	long field_20;
	long field_24;

	void get(s_2d9570 *out) const;
};

// @retail 0x2d9570
void class_2d9570::get(s_2d9570 *out) const
{
	out->field_0 = field_8;
	out->field_8 = field_c;
	out->field_4 = field_10;
	out->field_c = field_14;
	out->field_18 = field_20;
	out->field_1c = field_24;
}
