// @flags /O2 /GL-
// Havok (third-party physics library): copying out a pair of vectors.
#include <xmmintrin.h>

class class_30b110
{
public:
	char unknown0[0x60];
	__m128 bounds[2];

	void get_bounds(__m128 *out) const;
};

// @retail 0x30b110
void class_30b110::get_bounds(__m128 *out) const
{
	out[0] = bounds[0];
	out[1] = bounds[1];
}
