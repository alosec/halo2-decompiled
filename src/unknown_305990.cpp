// @flags /O2 /GL-
// Havok (third-party physics library): replacing the components of a
// vector where another exceeds a value.
#include <xmmintrin.h>

// @retail 0x305990
void __cdecl function_305990(__m128 *vector, const __m128 *replacement, const __m128 *compare, const __m128 *limit)
{
	__m128 bound = *limit;
	__m128 original = *vector;
	__m128 mask = _mm_cmpgt_ps(*compare, _mm_shuffle_ps(bound, bound, 0));
	__m128 value = *replacement;
	*vector = _mm_or_ps(_mm_and_ps(mask, _mm_shuffle_ps(value, value, 0)), _mm_andnot_ps(mask, original));
}
