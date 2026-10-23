// @flags /O2 /GL-
// Havok (third-party physics library): clearing a pair of vectors.
#include <xmmintrin.h>

class class_2fdfb0
{
public:
	void function_2fdfb0(long a, long b, __m128 *out) const;
};

// @retail 0x2fdfb0
void class_2fdfb0::function_2fdfb0(long a, long b, __m128 *out) const
{
	out[0] = _mm_setzero_ps();
	out[1] = _mm_setzero_ps();
}
