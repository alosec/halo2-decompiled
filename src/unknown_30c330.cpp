// @flags /O2 /GL-
// Havok (third-party physics library): apply the fields of a flat state record.
#include <xmmintrin.h>
#include <stddef.h>

// Local views through the accessed extent; unknown ranges omit other fields.
// The 0x90 record and 0x1b0 object assertions describe modelled prefixes,
// not proven allocation sizes.
struct record_30c330
{
	__m128 vector_0;
	char unknown10[0x34];
	float field_44;
	long field_48;
	long count_4c;
	char unknown50[0x38];
	long field_88;
	long field_8c;
};

class class_30c330
{
public:
	char unknown0[0x30];
	long field_30;
	long field_34;
	char unknown38[0xa8];
	__m128 vector_e0;
	char unknownf0[0x74];
	float field_164;
	long field_168;
	float half_16c;
	char unknown170[8];
	float field_178;
	long count_17c;
	float reciprocal_180;
	char unknown184[0x24];
	float field_1a8;
	float reciprocal_1ac;

	void function_30c330(void const *record);
};

typedef char assert_record_vector_0[(offsetof(record_30c330, vector_0) == 0) ? 1 : -1];
typedef char assert_record_field_44[(offsetof(record_30c330, field_44) == 0x44) ? 1 : -1];
typedef char assert_record_field_48[(offsetof(record_30c330, field_48) == 0x48) ? 1 : -1];
typedef char assert_record_count_4c[(offsetof(record_30c330, count_4c) == 0x4c) ? 1 : -1];
typedef char assert_record_field_88[(offsetof(record_30c330, field_88) == 0x88) ? 1 : -1];
typedef char assert_record_field_8c[(offsetof(record_30c330, field_8c) == 0x8c) ? 1 : -1];
typedef char assert_record_size[(sizeof(record_30c330) == 0x90) ? 1 : -1];
typedef char assert_object_field_30[(offsetof(class_30c330, field_30) == 0x30) ? 1 : -1];
typedef char assert_object_field_34[(offsetof(class_30c330, field_34) == 0x34) ? 1 : -1];
typedef char assert_object_vector_e0[(offsetof(class_30c330, vector_e0) == 0xe0) ? 1 : -1];
typedef char assert_object_field_164[(offsetof(class_30c330, field_164) == 0x164) ? 1 : -1];
typedef char assert_object_field_168[(offsetof(class_30c330, field_168) == 0x168) ? 1 : -1];
typedef char assert_object_half_16c[(offsetof(class_30c330, half_16c) == 0x16c) ? 1 : -1];
typedef char assert_object_field_178[(offsetof(class_30c330, field_178) == 0x178) ? 1 : -1];
typedef char assert_object_count_17c[(offsetof(class_30c330, count_17c) == 0x17c) ? 1 : -1];
typedef char assert_object_reciprocal_180[(offsetof(class_30c330, reciprocal_180) == 0x180) ? 1 : -1];
typedef char assert_object_field_1a8[(offsetof(class_30c330, field_1a8) == 0x1a8) ? 1 : -1];
typedef char assert_object_reciprocal_1ac[(offsetof(class_30c330, reciprocal_1ac) == 0x1ac) ? 1 : -1];
typedef char assert_object_size[(sizeof(class_30c330) == 0x1b0) ? 1 : -1];

// Copy the record, halve its float at +0x44, and cache the reciprocal count.
// @retail 0x30c330
void class_30c330::function_30c330(void const *record)
{
	record_30c330 const *state = static_cast<record_30c330 const *>(record);
	vector_e0 = state->vector_0;
	field_164 = state->field_44;
	field_168 = state->field_48;
	half_16c = state->field_44 * 0.5f;
	count_17c = state->count_4c;
	// Retail finishes both reciprocal stores before reading the current value.
	*static_cast<float volatile *>(&reciprocal_1ac) =
		reciprocal_180 = 1.0f / (float)state->count_4c;
	// Pair this ordered read with the preceding cache store.
	field_1a8 = *static_cast<float const volatile *>(&field_178);
	field_30 = state->field_88;
	field_34 = state->field_8c;
}
