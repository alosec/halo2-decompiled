// @flags /O2 /GL-
// Havok (third-party physics library): an object's state copied out into a
// flat record, with one ratio of two of its values.
#include <xmmintrin.h>

struct detail_30c200
{
	char flag_0;
	char unknown1[3];
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	char flag_14;
	char unknown15[3];
	long field_18;
	long field_1c;
};

struct link_30c200
{
	long field_0;
	long field_4;
	long field_8;
	detail_30c200 *detail;
};

struct record_30c200
{
	__m128 vector_0;
	char flag_10;
	char unknown11[3];
	long field_14;
	char unknown18[8];
	__m128 vector_20;
	__m128 vector_30;
	long field_40;
	long field_44;
	long field_48;
	long field_4c;
	char flag_50;
	char flag_51;
	char unknown52[2];
	float value_54;
	float ratio_58;
	long field_5c;
	long field_60;
	char flag_64;
	char unknown65[3];
	long field_68;
	long field_6c;
	long field_70;
	char unknown74[4];
	char flag_78;
	char unknown79[3];
	long field_7c;
	long field_80;
	char flag_84;
	char unknown85[3];
	long field_88;
	long field_8c;
};

class object_30c200
{
public:
	char unknown0[0x30];
	long field_30;
	long field_34;
	char unknown38[0x94];
	link_30c200 *link;
	char unknownd0[0x10];
	__m128 vector_e0;
	char unknownf0[0x74];
	long field_164;
	long field_168;
	char unknown16c[0x10];
	long field_17c;
	char unknown180[0x90];
	__m128 vector_210;
	__m128 vector_220;
	long field_230;
	long field_234;
	char unknown238[4];
	char flag_23c;
	char flag_23d;
	char flag_23e;
	char flag_23f;
	char unknown240[4];
	float value_244;
	float value_248;
	long field_24c;

	void function_30c200(record_30c200 *out) const;
};

// @retail 0x30c200
void object_30c200::function_30c200(record_30c200 *out) const
{
	out->vector_0 = vector_e0;
	out->flag_10 = flag_23c;
	out->vector_20 = vector_210;
	out->vector_30 = vector_220;
	out->field_14 = field_234;
	out->field_40 = field_230;
	out->field_44 = field_164;
	out->field_48 = field_168;
	out->field_4c = field_17c;
	out->field_60 = link->field_8;
	out->field_68 = link->detail->field_4;
	out->field_6c = link->detail->field_8;
	out->field_70 = link->detail->field_c;
	out->flag_64 = link->detail->flag_0;
	out->flag_78 = link->detail->flag_14;
	out->field_7c = link->detail->field_18;
	out->field_80 = link->detail->field_1c;
	out->flag_50 = flag_23e;
	out->flag_51 = flag_23f;
	out->value_54 = value_244;
	out->ratio_58 = value_248 / value_244;
	out->field_5c = field_24c;
	out->flag_84 = flag_23d;
	out->field_88 = field_30;
	out->field_8c = field_34;
}
