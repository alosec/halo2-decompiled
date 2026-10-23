// @flags /O2 /GL-
// Havok (third-party physics library): comparing a vector with a value per
// component.
#include <xmmintrin.h>

// @retail 0x305900
// which of x, y and z exceed the value, as bits
void __cdecl function_305900(const __m128 *vector, const float *value, int *mask)
{
	__m128 limit = _mm_set_ss(*value);
	limit = _mm_shuffle_ps(limit, limit, 0);
	*mask = _mm_movemask_ps(_mm_cmpgt_ps(*vector, limit)) & 7;
}
