// @flags /O2 /GL-
// Havok (third-party physics library): the constraint solver proper
// (0x31c940), which 0x31c100 (port/recovered/havok_solver_31c100.cpp) runs
// over the scratch area it lays out, with what it needs:
//   0x31c7d0  the licence check (the client keycode, or an evaluation key
//             with an expiry date); 0x31c940 solves nothing until it passed
//   0x31c540  the keycode hash of a name and the product string
//   0x3724f0  time(0) (the CRT's _time, 0x321aae)
//   0x31c520  the nth angular Jacobian (0x20 bytes) after this one
//   0x31c530  the nth full Jacobian (0x30 bytes) after this one
//
// The scratch area (see 0x31c100): the bodies from the base, 0x80 bytes
// each, a byte 2 starting each (the first a fixed body of no velocity) and a
// byte 3 after the last:
//   0x10  linear velocity           0x40  the linear velocities summed over
//   0x20  angular velocity                the iterations
//   0x30  inverse inertia (diagonal,0x50  the same for the angular ones
//         lane 3 the inverse mass)
// The Jacobians: a full one is three vectors (linear, angular for body A,
// angular for body B: 0x30 bytes), an angular one two (0x20); their last
// lanes hold the right-hand side and the inverse effective masses (which of
// them depends on the schema). The schemas: a byte type, a byte size, a byte
// type for the right-hand-side pass, then the type's own data:
//    0  end
//    1  a constraint's header (0x18): its Jacobians (4), where the results
//       go (8), body A (0xc), body B (0x10), the results' stride (0x14)
//    2  a non-penetration row (4)           3  two of them coupled (8)
//    4  an angular row, clamped (0x10)      5  a full row, clamped (0x10)
//    6  a full friction row (8)             7  2D friction (0x14)
//    8  2D friction and a turning one (0x18)
//    9  an angular friction row (8)         10 an angular limit (0x10)
//   11  a linear limit (0xc)                12 an angular row (4)
//   13  a full row (4)
//   14  a callback (its size in byte 1)    15 set tau and damping (0xc)
//   16  restore them (4)
// Each iteration solves every schema once, sums the bodies' velocities, and
// takes each row's relative velocity off its right-hand side; then the
// accumulated impulses and the right-hand sides go to the results.
//
// The retail code mixes x87 (the impulses, at extended precision) and SSE
// (the vectors); here everything is single precision, in retail's order of
// operations. Retail spins forever on a type it has no case for; so does
// this.
#include <xmmintrin.h>

// ---------------------------------------------------------------------------
// the licence check

// the error report (src/unknown_2d97d0.cpp)
void __cdecl function_2d97d0(char const *message);

// time(0): the CRT's _time (0x321aae, not recovered)
extern "C" long h2_retail_321aae(long *timer);

// the client keycode (0 for an evaluation key)
extern long g_445470;
// the key's owner, "Bungie" (or the evaluation key)
extern char const *g_46fbf0;
// the evaluation key's dates are kept xored with this
extern unsigned long g_415440;
// the platform an evaluation key must name ("PC") and the product
// ("rigidbody", running on into "softcloth": no terminator between them)
extern char const g_415444[];
extern char const g_415448[];
// the check passed
extern bool g_480c60;

// @retail 0x3724f0
long __cdecl function_3724f0()
{
	return h2_retail_321aae(0);
}

// a weight per character, 1 to 4, repeating every 16
static const unsigned long g_keycode_weights_31c540[0x10] = {
	2, 3, 1, 2, 3, 1, 4, 3, 2, 1, 3, 1, 2, 2, 3, 3,
};

// the hash of up to 0x40 characters of the name against as many of the
// product string: each pair's xor (repeated in all four bytes) multiplied by
// and divided by its weight in turn, plus a running mix of the pairs before
// it
// @retail 0x31c540
unsigned long __cdecl function_31c540(char const *name)
{
	unsigned long weights[0x40];
	for (int i = 0; i < 0x40; i++)
	{
		weights[i] = g_keycode_weights_31c540[i & 0xf];
	}
	signed char product[0x40];
	signed char key[0x40];
	for (int i = 0; i < 0x40; i++)
	{
		product[i] = 0;
		key[i] = 0;
	}
	for (int i = 0; i < 0x40 && name[i] != 0; i++)
	{
		key[i] = name[i];
	}
	for (int i = 0; i < 0x40 && g_415448[i] != 0; i++)
	{
		product[i] = g_415448[i];
	}

	unsigned long sum = 0;
	unsigned long mix = 0xc18f77a2;
	unsigned long multiply = 1;
	for (int i = 0; i < 0x40; i++)
	{
		unsigned long const value = (unsigned long)(long)(product[i] ^ key[i]) * 0x1010101;
		sum += multiply ? weights[i] * value : value / weights[i];
		multiply = 1 - multiply;
		for (int j = 0; j < i; j++)
		{
			unsigned long const pair = (unsigned long)(long)(product[j] + key[j]) * 0x1010101;
			mix = (mix + mix) ^ pair;
			sum += mix;
		}
	}
	return sum;
}

// a client keycode must be the hash of the owner's name; an evaluation key
// ("...-PC-" then eight letters) holds its expiry date, which must be in the
// next 180 days
// @retail 0x31c7d0
void __cdecl function_31c7d0()
{
	if (g_445470 != 0)
	{
		if (g_445470 == (long)function_31c540(g_46fbf0))
		{
			g_480c60 = true;
		}
		else
		{
			function_2d97d0("Havok client keycode is invalid.\nPlease check that you have entered it correctly in keycode.cpp\n"
				"If this problem persists please contact Havok.com for assistance.\nNo simulation possible.");
		}
		return;
	}

	char const *const expired = "Havok evaluation key has expired or is invalid.\nPlease contact Havok.com for an extension.\n"
		"No simulation possible.";
	long const now = function_3724f0();
	char const *dash = g_46fbf0;
	while (*dash != '-')
	{
		if (*dash == 0)
		{
			function_2d97d0(expired);
			return;
		}
		dash++;
	}
	char const *platform = dash + 1;
	dash = platform;
	while (*dash != '-')
	{
		if (*dash == 0)
		{
			function_2d97d0(expired);
			return;
		}
		dash++;
	}
	for (long i = 0; platform + i != dash; i++)
	{
		if (platform[i] == 0 || g_415444[i] == 0 || platform[i] != g_415444[i])
		{
			function_2d97d0(expired);
			return;
		}
	}

	signed char const *letter = (signed char const *)dash;
	unsigned long const packed =
		((unsigned long)(letter[4] - 0x41) << 12) |
		((unsigned long)(letter[3] - 0x41) << 8) |
		((unsigned long)(letter[7] - 0x41) << 24) |
		((unsigned long)(letter[2] - 0x41) << 4) |
		((unsigned long)(letter[6] - 0x41) << 20) |
		((unsigned long)(letter[5] - 0x41) << 16) |
		(((unsigned long)(long)letter[8] << 28) - 0x10000000) |
		(unsigned long)(letter[1] - 0x41);
	long expiry = (long)(packed ^ g_415440);
	// the start of its day
	expiry -= expiry % 86400;
	if (expiry > now && expiry - now < 0xed4e00)
	{
		g_480c60 = true;
		return;
	}
	function_2d97d0(expired);
}

// ---------------------------------------------------------------------------
// the solver

struct body_31c940
{
	char kind;
	char unknown1[0xf];
	__m128 linear;
	__m128 angular;
	// the diagonal; lane 3 the inverse mass
	__m128 inverse_inertia;
	__m128 linear_sum;
	__m128 angular_sum;
	char unknown60[0x20];
};

typedef char assert_body_31c940_size[sizeof(body_31c940) == 0x80 ? 1 : -1];

struct jacobian_31c940
{
	__m128 linear;
	__m128 angular_a;
	__m128 angular_b;
};

struct angular_jacobian_31c940
{
	__m128 angular_a;
	__m128 angular_b;
};

// the Jacobians as 0x31c940 steps through them
class jacobians_31c520
{
public:
	char *angular_after(long count);
	char *full_after(long count);
};

// @retail 0x31c520
char *jacobians_31c520::angular_after(long count)
{
	return (char *)this + (count << 5);
}

// @retail 0x31c530
char *jacobians_31c520::full_after(long count)
{
	return (char *)this + count * 3 * 0x10;
}

struct solver_info_31c940
{
	long field_0;
	float tau;
	float damping;
	float friction_tau;
	// the right-hand sides are handed back multiplied by this
	float result_scale;
	long field_14;
	long field_18;
	long iterations;
	// the bodies' summed velocities are read back multiplied by this
	// (0x31c100)
	float velocity_scale;
};

typedef void(__cdecl *schema_callback_31c940)(void *jacobians, void *data);

// the lane accessors
#define LANE_W(v) ((v).m128_f32[3])

static __forceinline __m128 splat_31c940(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// z + (y + x)
static __forceinline float sum3_31c940(__m128 v)
{
	__m128 sum = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	sum = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), sum);
	float result;
	_mm_store_ss(&result, sum);
	return result;
}

// the bodies' velocity along a full row
static __forceinline float velocity_31c940(jacobian_31c940 const *j, body_31c940 const *a, body_31c940 const *b)
{
	__m128 const angular = _mm_add_ps(_mm_mul_ps(a->angular, j->angular_a), _mm_mul_ps(b->angular, j->angular_b));
	__m128 const linear = _mm_mul_ps(_mm_sub_ps(a->linear, b->linear), j->linear);
	return sum3_31c940(_mm_add_ps(angular, linear));
}

// and along an angular one
static __forceinline float angular_velocity_31c940(__m128 const &angular_a, __m128 const &angular_b,
	body_31c940 const *a, body_31c940 const *b)
{
	return sum3_31c940(_mm_add_ps(_mm_mul_ps(a->angular, angular_a), _mm_mul_ps(b->angular, angular_b)));
}

// an impulse along a full row
static __forceinline void apply_31c940(float impulse, jacobian_31c940 const *j, body_31c940 *a, body_31c940 *b)
{
	__m128 const x = splat_31c940(impulse);
	__m128 const xa = _mm_mul_ps(x, a->inverse_inertia);
	__m128 const xb = _mm_mul_ps(x, b->inverse_inertia);
	a->linear = _mm_add_ps(a->linear, _mm_mul_ps(_mm_shuffle_ps(xa, xa, 0xff), j->linear));
	b->linear = _mm_sub_ps(b->linear, _mm_mul_ps(_mm_shuffle_ps(xb, xb, 0xff), j->linear));
	a->angular = _mm_add_ps(a->angular, _mm_mul_ps(xa, j->angular_a));
	b->angular = _mm_add_ps(b->angular, _mm_mul_ps(xb, j->angular_b));
}

// and along an angular one
static __forceinline void apply_angular_31c940(float impulse, __m128 const &angular_a, __m128 const &angular_b,
	body_31c940 *a, body_31c940 *b)
{
	__m128 const x = splat_31c940(impulse);
	a->angular = _mm_add_ps(a->angular, _mm_mul_ps(_mm_mul_ps(x, a->inverse_inertia), angular_a));
	b->angular = _mm_add_ps(b->angular, _mm_mul_ps(_mm_mul_ps(x, b->inverse_inertia), angular_b));
}

#define SCHEMA_FLOAT(s, offset) (*(float *)((s) + (offset)))
#define SCHEMA_LONG(s, offset) (*(long *)((s) + (offset)))
#define SCHEMA_POINTER(s, offset) (*(char **)((s) + (offset)))
#define SCHEMA_SIZE(s) (*(unsigned char *)((s) + 1))

// the final pass's case for each schema type
static const unsigned char g_31e724[0x11] = {0, 1, 2, 3, 4, 2, 2, 5, 6, 4, 4, 2, 4, 2, 7, 7, 7};

// returns 1 once the results are written, 0 when the licence check failed
// @retail 0x31c940
int __cdecl function_31c940(void *info_in, void *schemas_in, void *base_in, void *accumulators_in)
{
	if (!g_480c60)
	{
		function_31c7d0();
		if (!g_480c60)
		{
			return 0;
		}
	}

	solver_info_31c940 *const info = (solver_info_31c940 *)info_in;
	char *const schemas = (char *)schemas_in;
	char *const base = (char *)base_in;
	float *const accumulators = (float *)accumulators_in;

	for (long iteration = 0; iteration < info->iterations; iteration++)
	{
		// what schema 16 restores (schema 15 saves over it)
		float saved_tau_15 = info->tau;
		float saved_damping_15 = info->damping;

		// solve every row once
		char *s = schemas;
		float *accumulator = accumulators;
		body_31c940 *a;
		body_31c940 *b;
		char *j;
#define READ_HEADER() \
		do { \
			a = (body_31c940 *)SCHEMA_POINTER(s, 0xc); \
			b = (body_31c940 *)SCHEMA_POINTER(s, 0x10); \
			j = SCHEMA_POINTER(s, 4); \
			s += 0x18; \
		} while (0)
		READ_HEADER();
		bool solving = true;
		while (solving)
		{
			switch (*s)
			{
			case 0:
				solving = false;
				break;
			case 1:
				READ_HEADER();
				break;
			case 2:
			{
				jacobian_31c940 *row = (jacobian_31c940 *)j;
				float const velocity = velocity_31c940(row, a, b);
				float const impulse = LANE_W(row->linear) * info->tau - velocity * info->damping;
				if (impulse > 0.0f)
				{
					float const x = impulse * LANE_W(row->angular_a);
					apply_31c940(x, row, a, b);
					accumulator[0] += x;
				}
				s += 4;
				j += 0x30;
				accumulator += 1;
				break;
			}
			case 3:
			{
				// two rows coupled through a 2x2 inverse mass: each impulse
				// is used only when positive
				jacobian_31c940 *row0 = (jacobian_31c940 *)j;
				jacobian_31c940 *row1 = row0 + 1;
				float const velocity0 = velocity_31c940(row0, a, b);
				float const velocity1 = velocity_31c940(row1, a, b);
				float const impulse0 = LANE_W(row0->linear) * info->tau - velocity0 * info->damping;
				float const impulse1 = LANE_W(row1->linear) * info->tau - velocity1 * info->damping;
				float const coupling = SCHEMA_FLOAT(s, 4);
				float const x0 = LANE_W(row0->angular_b) * impulse0 + impulse1 * coupling;
				float const x1 = LANE_W(row1->angular_b) * impulse1 + impulse0 * coupling;
				bool single0 = false;
				if (x0 > 0.0f)
				{
					if (x1 > 0.0f)
					{
						apply_31c940(x0, row0, a, b);
						accumulator[0] += x0;
						apply_31c940(x1, row1, a, b);
						accumulator[1] += x1;
					}
					else
					{
						single0 = true;
					}
				}
				else
				{
					float const x = impulse1 * LANE_W(row1->angular_a);
					if (!(x <= 0.0f))
					{
						apply_31c940(x, row1, a, b);
						accumulator[1] += x;
					}
					else
					{
						single0 = true;
					}
				}
				if (single0)
				{
					float const x = impulse0 * LANE_W(row0->angular_a);
					if (x > 0.0f)
					{
						apply_31c940(x, row0, a, b);
						accumulator[0] += x;
					}
				}
				s += 8;
				j += 0x60;
				accumulator += 2;
				break;
			}
			case 4:
			{
				angular_jacobian_31c940 *row = (angular_jacobian_31c940 *)j;
				float const velocity = angular_velocity_31c940(row->angular_a, row->angular_b, a, b);
				float x = (LANE_W(row->angular_b) - velocity) * LANE_W(row->angular_a) * info->tau;
				if (x > SCHEMA_FLOAT(s, 4))
				{
					x = x * (SCHEMA_FLOAT(s, 4) / x);
				}
				else if (x < SCHEMA_FLOAT(s, 8))
				{
					x = x * (SCHEMA_FLOAT(s, 8) / x);
				}
				apply_angular_31c940(x, row->angular_a, row->angular_b, a, b);
				accumulator[0] += x;
				s += 0x10;
				j += 0x20;
				accumulator += 1;
				break;
			}
			case 5:
			{
				jacobian_31c940 *row = (jacobian_31c940 *)j;
				float const velocity = velocity_31c940(row, a, b);
				float x = (LANE_W(row->linear) - velocity) * LANE_W(row->angular_a) * info->tau;
				if (x > SCHEMA_FLOAT(s, 4))
				{
					x = x * (SCHEMA_FLOAT(s, 4) / x);
				}
				else if (x < SCHEMA_FLOAT(s, 8))
				{
					x = x * (SCHEMA_FLOAT(s, 8) / x);
				}
				apply_31c940(x, row, a, b);
				accumulator[0] += x;
				j = ((jacobians_31c520 *)j)->full_after(1);
				s += 0x10;
				accumulator += 1;
				break;
			}
			case 6:
			{
				// friction: the impulse no larger than the schema's limit,
				// the right-hand side scaled with it
				jacobian_31c940 *row = (jacobian_31c940 *)j;
				float const limit = SCHEMA_FLOAT(s, 4);
				float const velocity = velocity_31c940(row, a, b);
				float x = (LANE_W(row->linear) * info->friction_tau - velocity * info->damping) * LANE_W(row->angular_a);
				float const size = __builtin_fabsf(x);
				if (size > limit)
				{
					float const factor = limit / size;
					x *= factor;
					LANE_W(row->linear) = factor * LANE_W(row->linear);
				}
				apply_31c940(x, row, a, b);
				accumulator[0] += x;
				s += 8;
				j += 0x30;
				accumulator += 1;
				break;
			}
			case 7:
			{
				// 2D friction: the two impulses within a circle
				jacobian_31c940 *row0 = (jacobian_31c940 *)j;
				jacobian_31c940 *row1 = row0 + 1;
				float const velocity0 = velocity_31c940(row0, a, b);
				float const velocity1 = velocity_31c940(row1, a, b);
				float const impulse0 = LANE_W(row0->linear) * info->friction_tau - velocity0 * info->damping;
				float const impulse1 = LANE_W(row1->linear) * info->friction_tau - velocity1 * info->damping;
				float const limit_squared = SCHEMA_FLOAT(s, 0xc) * SCHEMA_FLOAT(s, 0xc);
				float x0 = LANE_W(row0->angular_b) * impulse0 + impulse1 * SCHEMA_FLOAT(s, 8);
				float x1 = LANE_W(row1->angular_b) * impulse1 + impulse0 * SCHEMA_FLOAT(s, 8);
				float const size_squared = x0 * x0 + x1 * x1;
				if (size_squared > limit_squared)
				{
					float const factor = __builtin_sqrtf(limit_squared / size_squared);
					x0 *= factor;
					x1 *= factor;
					LANE_W(row0->linear) = factor * LANE_W(row0->linear);
					LANE_W(row1->linear) = factor * LANE_W(row1->linear);
				}
				apply_31c940(x0, row0, a, b);
				accumulator[0] += x0;
				apply_31c940(x1, row1, a, b);
				accumulator[1] += x1;
				s += 0x14;
				j += 0x60;
				accumulator += 2;
				break;
			}
			case 8:
			{
				// 2D friction and a turning one, all three within a sphere;
				// the turning impulse then scaled by the schema's factor
				jacobian_31c940 *row0 = (jacobian_31c940 *)j;
				jacobian_31c940 *row1 = row0 + 1;
				angular_jacobian_31c940 *row2 = (angular_jacobian_31c940 *)(row1 + 1);
				float const velocity0 = velocity_31c940(row0, a, b);
				float const velocity1 = velocity_31c940(row1, a, b);
				float const velocity2 = angular_velocity_31c940(row2->angular_a, row2->angular_b, a, b);
				float x2 = (LANE_W(row2->angular_b) * info->friction_tau - velocity2 * info->damping) * LANE_W(row2->angular_a);
				float const impulse0 = LANE_W(row0->linear) * info->friction_tau - velocity0 * info->damping;
				float const impulse1 = LANE_W(row1->linear) * info->friction_tau - velocity1 * info->damping;
				float const limit = SCHEMA_FLOAT(s, 0xc);
				float x0 = impulse0 * LANE_W(row0->angular_b) + impulse1 * SCHEMA_FLOAT(s, 8);
				float x1 = impulse1 * LANE_W(row1->angular_b) + impulse0 * SCHEMA_FLOAT(s, 8);
				float const size_squared = x2 * x2 + x1 * x1 + x0 * x0;
				if (size_squared > limit * limit)
				{
					float const factor = limit / __builtin_sqrtf(size_squared);
					x0 *= factor;
					x1 *= factor;
					x2 *= factor;
					LANE_W(row0->linear) = factor * LANE_W(row0->linear);
					LANE_W(row1->linear) = factor * LANE_W(row1->linear);
					LANE_W(row2->angular_b) = factor * LANE_W(row2->angular_b);
				}
				x2 *= SCHEMA_FLOAT(s, 0x14);
				apply_31c940(x0, row0, a, b);
				accumulator[0] += x0;
				apply_31c940(x1, row1, a, b);
				accumulator[1] += x1;
				apply_angular_31c940(x2, row2->angular_a, row2->angular_b, a, b);
				accumulator[2] += x2;
				s += 0x18;
				j += 0x80;
				accumulator += 3;
				break;
			}
			case 9:
			{
				// angular friction: as 6, the right-hand side in the angular
				// row's second lane
				angular_jacobian_31c940 *row = (angular_jacobian_31c940 *)j;
				float const limit = SCHEMA_FLOAT(s, 4);
				float const velocity = angular_velocity_31c940(row->angular_a, row->angular_b, a, b);
				float x = (LANE_W(row->angular_b) * info->friction_tau - velocity * info->damping) * LANE_W(row->angular_a);
				float const size = __builtin_fabsf(x);
				if (size > limit)
				{
					float const factor = limit / size;
					x *= factor;
					LANE_W(row->angular_b) = factor * LANE_W(row->angular_b);
				}
				apply_angular_31c940(x, row->angular_a, row->angular_b, a, b);
				accumulator[0] += x;
				s += 8;
				j += 0x20;
				accumulator += 1;
				break;
			}
			case 10:
			{
				// an angular limit: the position (the right-hand side) past
				// the lower one (4) or the upper one (8), scaled by 0xc
				angular_jacobian_31c940 *row = (angular_jacobian_31c940 *)j;
				float const velocity = angular_velocity_31c940(row->angular_a, row->angular_b, a, b) * info->damping;
				float const below = (LANE_W(row->angular_b) - SCHEMA_FLOAT(s, 4)) * SCHEMA_FLOAT(s, 0xc) - velocity;
				if (below > 0.0f)
				{
					float const x = below * LANE_W(row->angular_a);
					apply_angular_31c940(x, row->angular_a, row->angular_b, a, b);
					accumulator[0] += x;
				}
				else
				{
					float const above = (LANE_W(row->angular_b) - SCHEMA_FLOAT(s, 8)) * SCHEMA_FLOAT(s, 0xc) - velocity;
					if (above < 0.0f)
					{
						float const x = above * LANE_W(row->angular_a);
						apply_angular_31c940(x, row->angular_a, row->angular_b, a, b);
						accumulator[0] += x;
					}
				}
				s += 0x10;
				j += 0x20;
				accumulator += 1;
				break;
			}
			case 11:
			{
				// a linear limit: both sides are tried, the velocity taken
				// once before either
				jacobian_31c940 *row = (jacobian_31c940 *)j;
				float const velocity = velocity_31c940(row, a, b) * info->damping;
				float const below = (LANE_W(row->linear) - SCHEMA_FLOAT(s, 4)) * info->tau - velocity;
				if (below > 0.0f)
				{
					float const x = below * LANE_W(row->angular_a);
					apply_31c940(x, row, a, b);
					accumulator[0] += x;
				}
				float const above = (LANE_W(row->linear) - SCHEMA_FLOAT(s, 8)) * info->tau - velocity;
				if (above < 0.0f)
				{
					float const x = above * LANE_W(row->angular_a);
					apply_31c940(x, row, a, b);
					accumulator[0] += x;
				}
				s += 0xc;
				j += 0x30;
				accumulator += 1;
				break;
			}
			case 12:
			{
				angular_jacobian_31c940 *row = (angular_jacobian_31c940 *)j;
				float const velocity = angular_velocity_31c940(row->angular_a, row->angular_b, a, b);
				float const x = LANE_W(row->angular_a) * (LANE_W(row->angular_b) * info->tau - velocity * info->damping);
				apply_angular_31c940(x, row->angular_a, row->angular_b, a, b);
				accumulator[0] += x;
				s += 4;
				j += 0x20;
				accumulator += 1;
				break;
			}
			case 13:
			{
				jacobian_31c940 *row = (jacobian_31c940 *)j;
				float const velocity = velocity_31c940(row, a, b);
				float const x = LANE_W(row->angular_a) * (LANE_W(row->linear) * info->tau - velocity * info->damping);
				apply_31c940(x, row, a, b);
				accumulator[0] += x;
				s += 4;
				j += 0x30;
				accumulator += 1;
				break;
			}
			case 14:
				((schema_callback_31c940)SCHEMA_POINTER(s, 4))(j, s + 8);
				s += SCHEMA_SIZE(s);
				break;
			case 15:
				saved_damping_15 = info->damping;
				saved_tau_15 = info->tau;
				info->damping = SCHEMA_FLOAT(s, 8);
				info->tau = SCHEMA_FLOAT(s, 4);
				s += 0xc;
				break;
			case 16:
				info->tau = saved_tau_15;
				info->damping = saved_damping_15;
				s += 4;
				break;
			default:
				// retail reads the same type again, for ever
				break;
			}
		}
#undef READ_HEADER

		// the bodies' velocities summed over the iterations
		for (char *body = base;;)
		{
			body_31c940 *const record = (body_31c940 *)body;
			if (record->kind == 2)
			{
				if (iteration == 0)
				{
					record->angular_sum = record->angular;
					record->linear_sum = record->linear;
				}
				else
				{
					record->angular_sum = _mm_add_ps(record->angular_sum, record->angular);
					record->linear_sum = _mm_add_ps(record->linear_sum, record->linear);
				}
				body += 0x80;
			}
			else if (record->kind == 3)
			{
				break;
			}
			// (any other kind: retail reads it again, for ever)
		}

		// each row's relative velocity off its right-hand side (dispatched on
		// the schemas' third byte)
		s = schemas;
		body_31c940 *va;
		body_31c940 *vb;
#define READ_VELOCITY_HEADER() \
		do { \
			va = (body_31c940 *)SCHEMA_POINTER(s, 0xc); \
			vb = (body_31c940 *)SCHEMA_POINTER(s, 0x10); \
			j = SCHEMA_POINTER(s, 4); \
			s += 0x18; \
		} while (0)
		READ_VELOCITY_HEADER();
		bool correcting = true;
		while (correcting)
		{
			switch (s[2])
			{
			case 0:
				correcting = false;
				break;
			case 1:
				READ_VELOCITY_HEADER();
				break;
			case 2:
				s += SCHEMA_SIZE(s);
				break;
			case 3:
			{
				jacobian_31c940 *row = (jacobian_31c940 *)j;
				LANE_W(row->linear) = LANE_W(row->linear) - velocity_31c940(row, va, vb);
				j += 0x30;
				s += SCHEMA_SIZE(s);
				break;
			}
			case 4:
			{
				angular_jacobian_31c940 *row = (angular_jacobian_31c940 *)j;
				LANE_W(row->angular_b) = LANE_W(row->angular_b) - angular_velocity_31c940(row->angular_a, row->angular_b, va, vb);
				j += 0x20;
				s += SCHEMA_SIZE(s);
				break;
			}
			case 5:
			{
				jacobian_31c940 *row0 = (jacobian_31c940 *)j;
				jacobian_31c940 *row1 = row0 + 1;
				float const velocity1 = velocity_31c940(row1, va, vb);
				float const velocity0 = velocity_31c940(row0, va, vb);
				LANE_W(row0->linear) = LANE_W(row0->linear) - velocity0;
				LANE_W(row1->linear) = LANE_W(row1->linear) - velocity1;
				j += 0x60;
				s += SCHEMA_SIZE(s);
				break;
			}
			case 6:
			{
				jacobian_31c940 *row0 = (jacobian_31c940 *)j;
				jacobian_31c940 *row1 = row0 + 1;
				angular_jacobian_31c940 *row2 = (angular_jacobian_31c940 *)(row1 + 1);
				float const velocity1 = velocity_31c940(row1, va, vb);
				float const velocity0 = velocity_31c940(row0, va, vb);
				LANE_W(row0->linear) = LANE_W(row0->linear) - velocity0;
				LANE_W(row1->linear) = LANE_W(row1->linear) - velocity1;
				LANE_W(row2->angular_b) = LANE_W(row2->angular_b) - angular_velocity_31c940(row2->angular_a, row2->angular_b, va, vb);
				j += 0x80;
				s += SCHEMA_SIZE(s);
				// (retail tests the next schema's first byte, not its third,
				// for a header here)
				if (s[0] == 1)
				{
					READ_VELOCITY_HEADER();
				}
				break;
			}
			default:
				// retail reads the same type again, for ever
				break;
			}
		}
#undef READ_VELOCITY_HEADER
	}

	// the accumulated impulses and the right-hand sides (scaled) to where
	// each constraint wants its results: pairs of words, a stride apart
	char *s = schemas;
	unsigned long const *accumulator = (unsigned long const *)accumulators;
	long stride = SCHEMA_LONG(s, 0x14);
	char *j = SCHEMA_POINTER(s, 4);
	char *out = SCHEMA_POINTER(s, 8);
	s += 0x18;
	for (;;)
	{
		long const type = *s;
		if ((unsigned long)type > 0x10)
		{
			// retail reads the same type again, for ever
			continue;
		}
		switch (g_31e724[type])
		{
		case 0:
			return 1;
		case 1:
			stride = SCHEMA_LONG(s, 0x14);
			j = SCHEMA_POINTER(s, 4);
			out = SCHEMA_POINTER(s, 8);
			s += 0x18;
			break;
		case 2:
			((unsigned long *)out)[0] = accumulator[0];
			((float *)out)[1] = LANE_W(((jacobian_31c940 *)j)->linear) * info->result_scale;
			out += stride;
			j = ((jacobians_31c520 *)j)->full_after(1);
			s += SCHEMA_SIZE(s);
			accumulator += 1;
			break;
		case 3:
			((unsigned long *)out)[0] = accumulator[0];
			((float *)out)[1] = LANE_W(((jacobian_31c940 *)j)[0].linear) * info->result_scale;
			out += stride;
			((unsigned long *)out)[0] = accumulator[1];
			((float *)out)[1] = LANE_W(((jacobian_31c940 *)j)[1].linear) * info->result_scale;
			out += stride;
			j = ((jacobians_31c520 *)j)->full_after(2);
			accumulator += 2;
			s += 8;
			break;
		case 4:
			((unsigned long *)out)[0] = accumulator[0];
			((float *)out)[1] = LANE_W(((angular_jacobian_31c940 *)j)->angular_b) * info->result_scale;
			out += stride;
			j = ((jacobians_31c520 *)j)->angular_after(1);
			s += SCHEMA_SIZE(s);
			accumulator += 1;
			break;
		case 5:
		{
			// 2D friction keeps its own results' place and stride
			char *own = SCHEMA_POINTER(s, 4);
			((unsigned long *)own)[0] = accumulator[0];
			((float *)own)[1] = LANE_W(((jacobian_31c940 *)j)[0].linear) * info->result_scale;
			own += SCHEMA_LONG(s, 0x10);
			((unsigned long *)own)[0] = accumulator[1];
			((float *)own)[1] = LANE_W(((jacobian_31c940 *)j)[1].linear) * info->result_scale;
			j = ((jacobians_31c520 *)j)->full_after(2);
			accumulator += 2;
			s += 0x14;
			break;
		}
		case 6:
		{
			char *own = SCHEMA_POINTER(s, 4);
			((unsigned long *)own)[0] = accumulator[0];
			((float *)own)[1] = LANE_W(((jacobian_31c940 *)j)[0].linear) * info->result_scale;
			own += SCHEMA_LONG(s, 0x10);
			((unsigned long *)own)[0] = accumulator[1];
			((float *)own)[1] = LANE_W(((jacobian_31c940 *)j)[1].linear) * info->result_scale;
			own += SCHEMA_LONG(s, 0x10);
			((unsigned long *)own)[0] = accumulator[2];
			((float *)own)[1] = LANE_W(((angular_jacobian_31c940 *)(j + 0x60))->angular_b) * info->result_scale;
			// (retail leaves the header's results pointer cleared here)
			out = 0;
			j = ((jacobians_31c520 *)(j + 0x60))->angular_after(1);
			accumulator += 3;
			s += 0x18;
			break;
		}
		default:
			s += SCHEMA_SIZE(s);
			break;
		}
	}
}
