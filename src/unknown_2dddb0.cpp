// @flags /O2 /GL-
// Havok (third-party physics library): gathering vertices by their byte
// offsets, each tagged in w with its offset.
#include <xmmintrin.h>

class class_2dddb0
{
public:
	char unknown0[0x10];

	void gather(const unsigned short *offsets, int count, __m128 *out) const;
};

// @retail 0x2dddb0
void class_2dddb0::gather(const unsigned short *offsets, int count, __m128 *out) const
{
	for (int i = count - 1; i >= 0; i--)
	{
		*out = *(const __m128 *)((const char *)this + 0x10 + *offsets);
		((unsigned long *)out)[3] = *offsets | 0x3f000000;
		out++;
		offsets++;
	}
}
