// @flags /O2 /GL-
// Havok (third-party physics library): fills a contact from a plane: the
// reversed normal, the distance, and a point and weight that depend on which
// of the object's two flags is set.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

struct plane_3032c0
{
	__m128 normal;
	float distance()
	{
		return ((float *)&normal)[3];
	}
};

struct contact_3032c0
{
	__m128 normal;
	__m128 point;
	float distance;
	float weight;
};

class source_3032c0
{
public:
	long first_flag;
	long second_flag;
	char pad_8[0x18];
	__m128 first_point;
	char pad_30[0x70];
	__m128 second_origin;
	char pad_b0[0x80];
	__m128 third_point;

	void function_3032c0(void *unused, plane_3032c0 *plane, contact_3032c0 *out) const;
};

// @retail 0x3032c0
void source_3032c0::function_3032c0(void *unused, plane_3032c0 *plane, contact_3032c0 *out) const
{
	out->distance = plane->distance();
	__m128 const normal = _mm_xor_ps(plane->normal, *(__m128 const *)g_4124a0);
	out->normal = normal;
	if (first_flag == 1)
	{
		out->point = first_point;
		out->weight = 1.0f;
	}
	else if (second_flag == 1)
	{
		__m128 const scalar = _mm_set_ss(plane->distance());
		out->point = _mm_add_ps(second_origin, _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), normal));
		out->weight = 0.0f;
	}
	else
	{
		out->point = third_point;
		out->weight = 0.5f;
	}
}
