// @flags /O2 /GL-
// Havok (third-party physics library): keeping the closest hit (its
// distance in the first vector's w).
#include <xmmintrin.h>

struct s_301a00_hit
{
	__m128 position;
	__m128 normal;
};

class class_301a00
{
public:
	void *vtable;
	float early_out;
	bool has_hit;
	s_301a00_hit hit;

	void add_hit(const s_301a00_hit *candidate);
};

// @retail 0x301a00
void class_301a00::add_hit(const s_301a00_hit *candidate)
{
	if (!has_hit || candidate->position.m128_f32[3] < hit.position.m128_f32[3])
	{
		has_hit = true;
		hit.position = candidate->position;
		hit.normal = candidate->normal;
		early_out = candidate->position.m128_f32[3];
	}
}
