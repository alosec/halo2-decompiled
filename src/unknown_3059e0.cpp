// @flags /O2 /GL-
// Havok (third-party physics library): a box's normal for a contact feature:
// an axis for a face, a scaled edge direction for an edge, or the feature's
// own normal otherwise.
#include <xmmintrin.h>

struct feature_3059e0
{
	char pad_0[0x20];
	__m128 normal;
	float distance;
	char pad_34[4];
	unsigned short type;
};

// a flag passed by value
struct flag_3059e0
{
	flag_3059e0(bool value_) : value(value_) {}
	flag_3059e0(flag_3059e0 const &other) : value(other.value) {}

	bool value;
};

class box_3059e0
{
public:
	char pad_0[0x40];
	__m128 edges[3];

	void function_3059e0(__m128 *out, unsigned short const *index, feature_3059e0 const *feature, flag_3059e0 unused) const;
};

// @retail 0x3059e0
void box_3059e0::function_3059e0(__m128 *out, unsigned short const *index, feature_3059e0 const *feature, flag_3059e0 unused) const
{
	int const type = feature->type;
	if (type <= 2)
	{
		int const axis = *index;
		*out = _mm_setzero_ps();
		((float *)out)[axis] = -feature->distance;
	}
	else if (type <= 5)
	{
		__m128 const scalar = _mm_set_ss(-feature->distance);
		*out = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), edges[*index]);
	}
	else
	{
		*out = feature->normal;
	}
}
