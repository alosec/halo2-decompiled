// @flags /O2 /GL-
// Havok (third-party physics library): a triangle's three vertices, each
// carrying the radius in w.
#include <xmmintrin.h>

class class_2fcc40
{
public:
	long field_0;
	long field_4;
	long field_8;
	float radius;
	__m128 vertices[3];

	__m128 *get_vertices(__m128 *out) const;
};

// @retail 0x2fcc40
__m128 *class_2fcc40::get_vertices(__m128 *out) const
{
	out[0] = vertices[0];
	out[0].m128_f32[3] = radius;
	out[1] = vertices[1];
	__m128 *next = out + 1;
	next->m128_f32[3] = radius;
	next++;
	*next = vertices[2];
	next->m128_f32[3] = radius;
	return out;
}
