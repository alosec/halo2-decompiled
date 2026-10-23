// @flags /O2 /GL-
// Havok (third-party physics library): copying a table of 32 masks and a
// word out and back in.

struct s_2df840
{
	long field_0;
	unsigned long masks[32];
};

class class_2df840
{
public:
	char unknown0[0x18];
	long field_18;
	unsigned long masks[32];

	void get(s_2df840 *out) const;
	void set(const s_2df840 *in);
};

// @retail 0x2df840
void class_2df840::get(s_2df840 *out) const
{
	for (int i = 0; i < 32; i++)
	{
		out->masks[i] = masks[i];
	}
	out->field_0 = field_18;
}

// @retail 0x2df870
void class_2df840::set(const s_2df840 *in)
{
	for (int i = 0; i < 32; i++)
	{
		masks[i] = in->masks[i];
	}
	field_18 = in->field_0;
}
