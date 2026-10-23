// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object built from a position and a
// rotation, its transform made from them and everything else cleared.
#include "derived_311920.h"

// @retail 0x3114d0
class_3114d0::class_3114d0(void const *a, void const *b)
{
	field_40 = _mm_setzero_ps();
	field_50 = _mm_setzero_ps();
	field_c = 1.0f;
	field_14 = 0;
	field_18 = 0;
	field_20 = _mm_setzero_ps();
	field_60 = *(__m128 const *)a;
	field_60.m128_f32[3] = 0.0f;
	field_70 = *(__m128 const *)a;
	transform_translation = *(__m128 const *)a;
	rotation = *(__m128 const *)b;
	transform_rotation.function_372500((__m128 const *)b);
	field_70.m128_f32[3] = 0.0f;
}
