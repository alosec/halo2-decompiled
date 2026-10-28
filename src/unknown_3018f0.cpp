// @flags /O2 /GL-
// Havok (third-party physics library): a hit passed on to the collector at +8
// seen from the other side: its point moved along its normal by its distance,
// the normal flipped and its two bodies swapped; then the collector's early-out
// value is copied; and the same for a hit cast along a direction.
#include "lane_b_flip_collectors.h"

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

static inline __m128 splat_3018f0(float value)
{
	__m128 v = _mm_set_ss(value);
	return _mm_shuffle_ps(v, v, 0);
}

// @retail 0x3018f0
void lane_b_flip_point_collector::add(lane_b_cd_point const *hit)
{
	lane_b_cd_point flipped;
	flipped.body_a = hit->body_b;
	flipped.body_b = hit->body_a;
	flipped.position = _mm_add_ps(hit->position, _mm_mul_ps(splat_3018f0(hit->position.m128_f32[3]), hit->normal));
	flipped.position.m128_f32[3] = hit->position.m128_f32[3];
	flipped.normal = _mm_xor_ps(hit->normal, *(const __m128 *)g_4124a0);
	collector->add(&flipped);
	early_out = collector->early_out;
}

// the same for a hit cast along a direction: its point moved along the
// direction by its distance
// @retail 0x301970
void lane_b_flip_cast_collector::add(lane_b_cd_point const *hit)
{
	lane_b_cd_point flipped;
	flipped.body_a = hit->body_b;
	flipped.body_b = hit->body_a;
	flipped.position = _mm_add_ps(hit->position, _mm_mul_ps(splat_3018f0(hit->position.m128_f32[3]), direction));
	flipped.position.m128_f32[3] = hit->position.m128_f32[3];
	flipped.normal = _mm_xor_ps(hit->normal, *(const __m128 *)g_4124a0);
	collector->add(&flipped);
	early_out = collector->early_out;
}
