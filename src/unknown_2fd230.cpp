// @flags /O2 /GL-
// Havok (third-party physics library): three words read from the object at
// +0x30, and a zero.

struct s_2fd230_source
{
	long field_0;
	long unknown4[3];
	long field_10;
	long unknown14[3];
	long field_20;
};

struct s_2fd230
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

class class_2fd230
{
public:
	char unknown0[0x30];
	s_2fd230_source *source;

	void get(s_2fd230 *out) const;
};

// @retail 0x2fd230
void class_2fd230::get(s_2fd230 *out) const
{
	out->field_0 = source->field_0;
	out->field_4 = source->field_10;
	out->field_8 = source->field_20;
	out->field_c = 0;
}
