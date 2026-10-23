// @flags /O2 /GL-
// Havok (third-party physics library): a contact from a feature of one
// object: its point placed by the object's transform, its normal turned by
// it, and the feature's distance word.
#include <xmmintrin.h>

struct transform_305eb0
{
	__m128 axes[3];
	__m128 position;
};

struct key_305eb0
{
	unsigned short index;
	unsigned short field_2;
	unsigned short id;
};

struct feature_305eb0
{
	__m128 point;
	char unknown10[0x10];
	__m128 normal;
	char unknown30[4];
	long distance;
};

struct contact_305eb0
{
	__m128 point;
	__m128 normal;
	unsigned short id;
};

class pair_305eb0
{
public:
	char unknown0[0x14];
	transform_305eb0 *transform;

	void function_305eb0(contact_305eb0 *out, key_305eb0 const *key, feature_305eb0 const *feature) const;
};

// @retail 0x305eb0
void pair_305eb0::function_305eb0(contact_305eb0 *out, key_305eb0 const *key, feature_305eb0 const *feature) const
{
	out->id = key->id;
	transform_305eb0 const *t = transform;
	__m128 const p = feature->point;
	out->point = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(t->axes[0], _mm_shuffle_ps(p, p, 0x00)),
		_mm_mul_ps(t->axes[1], _mm_shuffle_ps(p, p, 0x55))),
		_mm_add_ps(_mm_mul_ps(t->axes[2], _mm_shuffle_ps(p, p, 0xaa)), t->position));
	t = transform;
	__m128 const n = feature->normal;
	out->normal = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(t->axes[0], _mm_shuffle_ps(n, n, 0x00)),
		_mm_mul_ps(t->axes[1], _mm_shuffle_ps(n, n, 0x55))),
		_mm_mul_ps(t->axes[2], _mm_shuffle_ps(n, n, 0xaa)));
	((long *)&out->point)[3] = feature->distance;
}
