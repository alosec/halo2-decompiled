// @flags /O2 /GL-
// Havok (third-party physics library): the default settings a world is
// built from: gravity (0, -9.8, 0), a world bounding box from -500 to 500 on
// each axis, and assorted tolerances, counts and switches.

// four floats, the last 0 here
struct vector_3111d0
{
	float x;
	float y;
	float z;
	float w;

	void set(float x_value, float y_value, float z_value)
	{
		x = x_value;
		float &y_component = y;
		y_component = y_value;
		z = z_value;
		float &w_component = w;
		w_component = 0.0f;
	}
};

// a one-byte boolean with a constructor
struct flag_3111d0
{
	char value;

	flag_3111d0() {}
	flag_3111d0(bool value_) : value((char)value_) {}
};

struct s_3111d0
{
	vector_3111d0 gravity;
	flag_3111d0 flag_10;
	char unknown11[3];
	long field_14;
	char unknown18[8];
	vector_3111d0 lower;
	vector_3111d0 upper;
	float field_40;
	float field_44;
	float field_48;
	long field_4c;
	flag_3111d0 flag_50;
	flag_3111d0 flag_51;
	char unknown52[2];
	float field_54;
	float field_58;
	float field_5c;
	float field_60;
	flag_3111d0 flag_64;
	char unknown65[3];
	float field_68;
	float field_6c;
	float field_70;
	float field_74;
	flag_3111d0 flag_78;
	char unknown79[3];
	float field_7c;
	long field_80;
	flag_3111d0 flag_84;
	char unknown85[3];
	float field_88;
	float field_8c;

	s_3111d0();
};

// @retail 0x3111d0
s_3111d0::s_3111d0()
{
	gravity.set(0.0f, -9.8f, 0.0f);
	field_14 = 0x400;
	flag_10 = true;
	lower.set(-500.0f, -500.0f, -500.0f);
	upper.set(500.0f, 500.0f, 500.0f);
	field_40 = 0.0f;
	field_44 = 0.6f;
	field_48 = 1.0f;
	field_4c = 4;
	flag_50 = false;
	flag_51 = false;
	field_54 = -0.03f;
	field_58 = 0.3f;
	field_5c = 0.001f;
	field_60 = 0.1f;
	flag_64 = true;
	field_68 = 0.05f;
	field_6c = 0.2f;
	field_70 = 0.01f;
	field_74 = 0.999f;
	flag_78 = true;
	field_7c = 0.01f;
	field_80 = 20;
	flag_84 = true;
	field_88 = 0.2f;
	field_8c = 10.0f;
}
