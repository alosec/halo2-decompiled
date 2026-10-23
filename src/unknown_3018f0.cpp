// @flags /O2 /GL-
// Havok (third-party physics library): a hit passed on to the collector at +8
// seen from the other side: its point moved along its normal by its distance,
// the normal flipped and its two keys swapped; then the collector's early-out
// value is copied; and the same for a hit cast along a direction.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

static inline __m128 splat_3018f0(float value)
{
	__m128 v = _mm_set_ss(value);
	return _mm_shuffle_ps(v, v, 0);
}

struct hit_3018f0
{
	__m128 point;
	__m128 normal;
	long key_20;
	long key_24;
};

class collector_3018f0
{
public:
	virtual void slot_0() {}
	virtual void add(hit_3018f0 const *hit) {}

	float early_out;
};

class flipped_3018f0
{
public:
	long field_0;
	float early_out;
	collector_3018f0 *collector;

	void add(hit_3018f0 const *hit);
};

// @retail 0x3018f0
void flipped_3018f0::add(hit_3018f0 const *hit)
{
	hit_3018f0 flipped;
	flipped.key_20 = hit->key_24;
	flipped.key_24 = hit->key_20;
	flipped.point = _mm_add_ps(hit->point, _mm_mul_ps(splat_3018f0(hit->point.m128_f32[3]), hit->normal));
	flipped.point.m128_f32[3] = hit->point.m128_f32[3];
	flipped.normal = _mm_xor_ps(hit->normal, *(const __m128 *)g_4124a0);
	collector->add(&flipped);
	early_out = collector->early_out;
}

// the same for a hit cast along a direction: its point moved along the
// direction by its distance
class cast_flipped_301970
{
public:
	long field_0;
	float early_out;
	char unknown8[0x10 - 0x8];
	__m128 direction;
	collector_3018f0 *collector;

	void add(hit_3018f0 const *hit);
};

// @retail 0x301970
void cast_flipped_301970::add(hit_3018f0 const *hit)
{
	hit_3018f0 flipped;
	flipped.key_20 = hit->key_24;
	flipped.key_24 = hit->key_20;
	flipped.point = _mm_add_ps(hit->point, _mm_mul_ps(splat_3018f0(hit->point.m128_f32[3]), direction));
	flipped.point.m128_f32[3] = hit->point.m128_f32[3];
	flipped.normal = _mm_xor_ps(hit->normal, *(const __m128 *)g_4124a0);
	collector->add(&flipped);
	early_out = collector->early_out;
}
