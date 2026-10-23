// @flags /O2 /GL-
// Havok (third-party physics library): running a penetration query between
// two shapes, timed.
#include <xmmintrin.h>
#include "timer_mark_2ebc80.h"

struct transform_30a6b0
{
	__m128 rows[4];
};

class class_309b50
{
public:
	transform_30a6b0 transform;
	long field_40;
	long field_44;
	long field_48;
	long field_4c;
	long field_50;

	long function_309b50(long arg_0);
};

// @retail 0x30a6b0
long __cdecl function_30a6b0(long arg_0, long arg_1, transform_30a6b0 const *transform, long arg_3, long arg_4, long arg_5)
{
	timer_add_2ebc80("TtPenetration");
	class_309b50 query;
	query.transform.rows[0] = transform->rows[0];
	query.transform.rows[1] = transform->rows[1];
	query.transform.rows[2] = transform->rows[2];
	query.transform.rows[3] = transform->rows[3];
	query.field_40 = arg_0;
	query.field_44 = arg_1;
	query.field_48 = arg_3;
	query.field_4c = 0;
	query.field_50 = arg_4;
	long const result = query.function_309b50(arg_5);
	timer_add_2ebc80("Et");
	return result;
}
