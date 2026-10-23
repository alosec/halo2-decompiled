// @flags /O2 /GL-
// Havok (third-party physics library): copies a body's motion vectors (from
// +0x30 to +0xb0) into another object at the same offsets, with the two
// velocities left at zero.
#include <xmmintrin.h>

class body_31a7f0
{
public:
	char unknown0[0x30];
	__m128 vectors[9];

	void snapshot_into(body_31a7f0 *out) const;
};

// @retail 0x31a7f0
void body_31a7f0::snapshot_into(body_31a7f0 *out) const
{
	out->vectors[0] = vectors[0];
	out->vectors[1] = _mm_setzero_ps();
	out->vectors[2] = _mm_setzero_ps();
	out->vectors[3] = vectors[3];
	out->vectors[4] = vectors[4];
	out->vectors[5] = vectors[5];
	out->vectors[6] = vectors[6];
	out->vectors[7] = vectors[7];
	out->vectors[8] = vectors[8];
}
