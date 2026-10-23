// @flags /O2 /GL-
// Havok (third-party physics library): a contact from a feature of the
// first object: the feature's point in world space, and the normal made from
// the other object's direction for the feature (numbered after the first
// three) scaled by the reversed distance.
#include <xmmintrin.h>

struct transform_305e10
{
	__m128 rotation[3];
	__m128 translation;
};

struct key_305e10
{
	unsigned short index;
	unsigned short field_2;
	unsigned short id;
};

struct feature_305e10
{
	__m128 point;
	char pad_10[0x20];
	float distance;
	long field_34;
};

struct contact_305e10
{
	__m128 point;
	__m128 normal;
	unsigned short id;
};

class pair_305e10
{
public:
	char pad_0[0x14];
	transform_305e10 *transform;
	__m128 *directions;

	void function_305e10(contact_305e10 *out, key_305e10 const *key, feature_305e10 const *feature) const;
};

// @retail 0x305e10
void pair_305e10::function_305e10(contact_305e10 *out, key_305e10 const *key, feature_305e10 const *feature) const
{
	int const index = key->index - 3;
	out->id = key->id;
	transform_305e10 const *t = transform;
	__m128 const p = feature->point;
	out->point = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(t->rotation[0], _mm_shuffle_ps(p, p, 0)),
		_mm_mul_ps(t->rotation[1], _mm_shuffle_ps(p, p, 0x55))),
		_mm_add_ps(_mm_mul_ps(t->rotation[2], _mm_shuffle_ps(p, p, 0xaa)), t->translation));
	__m128 const scalar = _mm_set_ss(-feature->distance);
	__m128 const &direction = directions[index];
	out->normal = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), direction);
	((long *)&out->point)[3] = feature->field_34;
}
