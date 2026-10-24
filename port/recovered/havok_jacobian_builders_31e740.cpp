// @flags /O2 /GL-
// Havok (third-party physics library): the Jacobian builders the constraints'
// slot 9 (build_jacobians, called from 0x31c100) write their solver rows
// with, the contact constraint's own slots 9 and 10 with its contact and
// friction rows (0x31f620), the two contact modifiers built on it, and
// hkMath's atan2 approximation (0x3725e0) the hinges and the ragdoll use.
//
// Every builder takes the query (the two bodies' solver records, the
// solver's factors) and the writer (where the next Jacobian and the next
// schema go, as port/recovered/havok_solver_31c100.cpp builds it) and
// advances the writer past what it wrote:
//   0x31e770  the schema header every constraint starts with (0x18 bytes):
//             0x11801, its first Jacobian, its runtime records and their
//             size, the two bodies
//   0x31f5e0  the same with no runtime records
//   0x31e740  the right-hand side of a Jacobian already written
//   0x31e7b0  an angular row keeping two vectors perpendicular
//   0x31e900  angular motor rows, one per axis
//   0x31f000  an angular limit row
//   0x31ea90  a linear motor row along an axis through a point
//   0x31ec30  a linear row along a direction between two points
//   0x31ee10  the same, limited
//   0x31f160  three linear rows (ball and socket) from body-local pivots
//   0x31f3e0  the same from world pivots
//   0x320290  a callback schema the solver calls with its data block
//   0x31f620  the contact constraint's rows: one per contact point (pairs
//             of points coupled), two friction rows through the points'
//             centre, and with more than one point an angular friction row
//
// The solver's body record (0x80 bytes, written by the motions' slot 3, e.g.
// src/unknown_31ae70.cpp's 0x31b300): a kind, a fixed flag at 0xc, the
// velocities, the inverse inertia's diagonal (lane 3 the inverse mass), the
// centre of mass and the transposed axes.
//
// The angular Jacobian (0x20 bytes): the rows for A and B; A's lane 3 the
// inverse effective mass, B's lane 3 the right-hand side. The linear one
// (0x30 bytes): the direction (lane 3 the right-hand side), A's and B's
// angular parts (lane 3 the inverse effective mass and the effective mass).
//
// Retail does the scalar arithmetic on the x87 stack (extended precision,
// rounded where it stores); here it is single precision throughout.
#include <xmmintrin.h>
#include "reference_counted_2d88a0.h"

// ---------------------------------------------------------------------------
// the data the builders work on

struct solver_body_31e740
{
	char kind;
	char unknown1[0xb];
	// nonzero for a fixed (keyframed) body: its rows stay in world space
	char fixed;
	char unknownd[3];
	__m128 linear_velocity;
	__m128 angular_velocity;
	// lane 3 the inverse mass
	__m128 inverse_inertia;
	__m128 centre;
	__m128 axes[3];
};

typedef char assert_solver_body_31e740_size[sizeof(solver_body_31e740) == 0x80 ? 1 : -1];

struct query_31e740
{
	solver_body_31e740 *body_a;
	solver_body_31e740 *body_b;
	float factor_8;
	// the right-hand sides' factor
	float factor_c;
	float factor_10;
	float factor_14;
	float factor_18;
	float factor_1c;
};

struct writer_31e740
{
	char *jacobians;
	char *schemas;
	char *jacobians_limit;
	char *overflow;
};

struct angular_jacobian_31e740
{
	__m128 a;
	__m128 b;
};

struct linear_jacobian_31e740
{
	__m128 direction;
	__m128 angular_a;
	__m128 angular_b;
};

typedef char assert_angular_jacobian_31e740_size[sizeof(angular_jacobian_31e740) == 0x20 ? 1 : -1];
typedef char assert_linear_jacobian_31e740_size[sizeof(linear_jacobian_31e740) == 0x30 ? 1 : -1];

// ---------------------------------------------------------------------------
// vector pieces, in retail's order

// FLT_EPSILON (0x34000000 at 0x415594, 0x41559c)
static float const epsilon_31e740 = 1.1920929e-07f;

static __declspec(align(16)) const unsigned long sign_mask_31e740[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// the identity's three columns (retail data at 0x461ea0)
static __declspec(align(16)) const float identity_31e740[3][4] = {
	{1.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
};

static inline __m128 splat_31e740(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

static inline float lane_31e740(__m128 const &v, int index)
{
	return ((float const *)&v)[index];
}

static inline void set_lane_31e740(__m128 &v, int index, float value)
{
	((float *)&v)[index] = value;
}

// the sum of the first three lanes: (y + x) + z
static inline float sum3_31e740(__m128 const &v)
{
	return (lane_31e740(v, 1) + lane_31e740(v, 0)) + lane_31e740(v, 2);
}

static inline float dot3_31e740(__m128 const &a, __m128 const &b)
{
	return sum3_31e740(_mm_mul_ps(a, b));
}

static inline __m128 negate_31e740(__m128 const &v)
{
	return _mm_xor_ps(v, *(__m128 const *)sign_mask_31e740);
}

// a x b: a.yzx * b.zxy - a.zxy * b.yzx (lane 3 a.w * b.w less itself)
static inline __m128 cross_31e740(__m128 const &a, __m128 const &b)
{
	return _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9)));
}

// a vector in the body's frame: (axis0 x + axis1 y) + axis2 z
static inline __m128 to_body_31e740(solver_body_31e740 const *body, __m128 const &v)
{
	return _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(body->axes[0], _mm_shuffle_ps(v, v, 0x00)),
		_mm_mul_ps(body->axes[1], _mm_shuffle_ps(v, v, 0x55))),
		_mm_mul_ps(body->axes[2], _mm_shuffle_ps(v, v, 0xaa)));
}

// a fixed body's rows stay in world space
static inline __m128 body_row_31e740(solver_body_31e740 const *body, __m128 const &v)
{
	return body->fixed ? v : to_body_31e740(body, v);
}

// the angular part of the effective mass: sum(a^2 Ia + b^2 Ib)
static inline float angular_mass_31e740(solver_body_31e740 const *a, solver_body_31e740 const *b,
	__m128 const &row_a, __m128 const &row_b)
{
	__m128 const part_b = _mm_mul_ps(_mm_mul_ps(row_b, row_b), b->inverse_inertia);
	__m128 const part_a = _mm_mul_ps(_mm_mul_ps(row_a, row_a), a->inverse_inertia);
	return sum3_31e740(_mm_add_ps(part_a, part_b));
}

// an angular row about an axis; lane 3 of A's row the inverse effective
// mass, B's left for the right-hand side
static inline void angular_row_31e740(query_31e740 const *query, __m128 const &axis, angular_jacobian_31e740 *jacobian)
{
	solver_body_31e740 const *a = query->body_a;
	solver_body_31e740 const *b = query->body_b;
	jacobian->a = body_row_31e740(a, axis);
	jacobian->b = body_row_31e740(b, negate_31e740(axis));
	float const mass = angular_mass_31e740(a, b, jacobian->a, jacobian->b);
	set_lane_31e740(jacobian->a, 3, 1.0f / (mass + epsilon_31e740));
}

// a linear row along a direction between two points; the right-hand side
// (lane 3 of the direction) left to the caller
static inline void linear_row_31e740(query_31e740 const *query, __m128 const &point_a, __m128 const &point_b,
	__m128 const &direction, float epsilon, linear_jacobian_31e740 *jacobian)
{
	solver_body_31e740 const *a = query->body_a;
	solver_body_31e740 const *b = query->body_b;
	jacobian->direction = direction;
	__m128 const arm_a = _mm_sub_ps(point_a, a->centre);
	__m128 const arm_b = _mm_sub_ps(point_b, b->centre);
	jacobian->angular_a = body_row_31e740(a, cross_31e740(arm_a, direction));
	jacobian->angular_b = body_row_31e740(b, cross_31e740(direction, arm_b));
	float const linear = (lane_31e740(b->inverse_inertia, 3) + lane_31e740(a->inverse_inertia, 3)) + epsilon;
	float const mass = linear + angular_mass_31e740(a, b, jacobian->angular_a, jacobian->angular_b);
	set_lane_31e740(jacobian->angular_b, 3, mass);
	set_lane_31e740(jacobian->angular_a, 3, 1.0f / mass);
}

static inline void put_schema_31e740(writer_31e740 *writer, unsigned long value)
{
	*(unsigned long *)writer->schemas = value;
	writer->schemas += 4;
}

static inline void put_schema_float_31e740(writer_31e740 *writer, float value)
{
	*(float *)writer->schemas = value;
	writer->schemas += 4;
}

// ---------------------------------------------------------------------------
// the builders

struct schema_header_31e740
{
	unsigned long type;
	char *jacobians;
	void *records;
	solver_body_31e740 *body_a;
	solver_body_31e740 *body_b;
	long record_size;
};

typedef char assert_schema_header_31e740_size[sizeof(schema_header_31e740) == 0x18 ? 1 : -1];

// @retail 0x31e770
void __cdecl jacobian_header_31e770(query_31e740 const *query, writer_31e740 *writer, void *records, long record_size)
{
	schema_header_31e740 *header = (schema_header_31e740 *)writer->schemas;
	header->body_a = query->body_a;
	header->records = records;
	header->body_b = query->body_b;
	header->record_size = record_size;
	header->type = 0x11801;
	header->jacobians = writer->jacobians;
	writer->schemas = (char *)(header + 1);
}

// @retail 0x31f5e0
void __cdecl jacobian_header_31f5e0(query_31e740 const *query, writer_31e740 *writer)
{
	schema_header_31e740 *header = (schema_header_31e740 *)writer->schemas;
	header->body_a = query->body_a;
	header->body_b = query->body_b;
	header->jacobians = writer->jacobians;
	header->type = 0x11801;
	header->records = 0;
	header->record_size = 0;
	writer->schemas = (char *)(header + 1);
}

// the right-hand side of the linear Jacobian `back` Jacobians before the
// writer's
// @retail 0x31e740
void __cdecl jacobian_set_rhs_31e740(float value, query_31e740 const *query, writer_31e740 *writer, long back)
{
	linear_jacobian_31e740 *jacobian = (linear_jacobian_31e740 *)writer->jacobians - back;
	set_lane_31e740(jacobian->direction, 3, value * query->factor_c);
}

// two vectors kept perpendicular by turning about an axis: the error is
// their dot product
struct angular_info_31e7b0
{
	__m128 a;
	__m128 b;
	__m128 axis;
};

// @retail 0x31e7b0
void __cdecl jacobian_angular_31e7b0(angular_info_31e7b0 const *info, query_31e740 const *query, writer_31e740 *writer)
{
	angular_jacobian_31e740 *jacobian = (angular_jacobian_31e740 *)writer->jacobians;
	angular_row_31e740(query, info->axis, jacobian);
	float const *a = (float const *)&info->a;
	float const *b = (float const *)&info->b;
	float const error = ((b[2] * a[2] + b[1] * a[1]) + b[0] * a[0]) * query->factor_c;
	writer->jacobians = (char *)(jacobian + 1);
	put_schema_31e740(writer, 0x4040c);
	set_lane_31e740(jacobian->b, 3, -error);
}

// angular motors about the axes of a frame (the ragdoll's and the limited
// hinge's strength): one row per axis, its error from the runtime record
struct motor_info_31e900
{
	__m128 const *axes;
	float const (*errors)[2];
	float strength;
	long count;
};

// @retail 0x31e900
void __cdecl jacobian_motors_31e900(motor_info_31e900 const *info, query_31e740 const *query, writer_31e740 *writer)
{
	angular_jacobian_31e740 *jacobian = (angular_jacobian_31e740 *)writer->jacobians;
	for (long i = 0; i < info->count; i++, jacobian++)
	{
		angular_row_31e740(query, info->axes[i], jacobian);
		put_schema_31e740(writer, 0x40809);
		put_schema_float_31e740(writer, query->factor_8 * info->strength);
		set_lane_31e740(jacobian->b, 3, info->errors[i][1] * query->factor_c);
	}
	writer->jacobians = (char *)jacobian;
}

// an angular limit: the angle (value) between the limits
struct limit_info_31f000
{
	__m128 axis;
	float minimum;
	float maximum;
	float value;
};

// @retail 0x31f000
void __cdecl jacobian_angular_limit_31f000(limit_info_31f000 const *info, query_31e740 const *query, writer_31e740 *writer)
{
	angular_jacobian_31e740 *jacobian = (angular_jacobian_31e740 *)writer->jacobians;
	angular_row_31e740(query, info->axis, jacobian);
	float const factor = -query->factor_c;
	put_schema_31e740(writer, 0x4100a);
	put_schema_float_31e740(writer, factor * info->minimum);
	put_schema_float_31e740(writer, factor * info->maximum);
	put_schema_float_31e740(writer, 0.5f);
	writer->jacobians = (char *)(jacobian + 1);
	set_lane_31e740(jacobian->b, 3, factor * info->value);
	set_lane_31e740(jacobian->a, 3, lane_31e740(jacobian->a, 3) * 0.5f);
}

// a linear motor along an axis through a point (the prismatic's friction):
// its strength, and its error from a runtime record
struct motor_info_31ea90
{
	__m128 point;
	__m128 axis;
	float strength;
	float const *record;
};

// @retail 0x31ea90
void __cdecl jacobian_linear_motor_31ea90(motor_info_31ea90 const *info, query_31e740 const *query, writer_31e740 *writer)
{
	linear_jacobian_31e740 *jacobian = (linear_jacobian_31e740 *)writer->jacobians;
	linear_row_31e740(query, info->point, info->point, info->axis, epsilon_31e740, jacobian);
	float const error = info->record[1] * query->factor_c;
	put_schema_31e740(writer, 0x30806);
	put_schema_float_31e740(writer, info->strength * query->factor_8);
	writer->jacobians = (char *)(jacobian + 1);
	set_lane_31e740(jacobian->direction, 3, error);
}

// two points kept apart along a direction by the error between them
struct linear_info_31ec30
{
	__m128 point_a;
	__m128 point_b;
	__m128 direction;
};

static inline float separation_31e740(__m128 const &point_a, __m128 const &point_b, __m128 const &direction)
{
	return dot3_31e740(_mm_sub_ps(point_b, point_a), direction);
}

// @retail 0x31ec30
void __cdecl jacobian_linear_31ec30(linear_info_31ec30 const *info, query_31e740 const *query, writer_31e740 *writer)
{
	linear_jacobian_31e740 *jacobian = (linear_jacobian_31e740 *)writer->jacobians;
	linear_row_31e740(query, info->point_a, info->point_b, info->direction, epsilon_31e740, jacobian);
	writer->jacobians = (char *)(jacobian + 1);
	set_lane_31e740(jacobian->direction, 3,
		separation_31e740(info->point_a, info->point_b, info->direction) * query->factor_c);
	put_schema_31e740(writer, 0x3040d);
}

struct limit_info_31ee10
{
	__m128 point_a;
	__m128 point_b;
	__m128 direction;
	float minimum;
	float maximum;
};

// @retail 0x31ee10
void __cdecl jacobian_linear_limit_31ee10(limit_info_31ee10 const *info, query_31e740 const *query, writer_31e740 *writer)
{
	linear_jacobian_31e740 *jacobian = (linear_jacobian_31e740 *)writer->jacobians;
	linear_row_31e740(query, info->point_a, info->point_b, info->direction, epsilon_31e740, jacobian);
	writer->jacobians = (char *)(jacobian + 1);
	set_lane_31e740(jacobian->direction, 3,
		separation_31e740(info->point_a, info->point_b, info->direction) * query->factor_c);
	float const factor = -query->factor_c;
	put_schema_31e740(writer, 0x30c0b);
	put_schema_float_31e740(writer, factor * info->minimum);
	put_schema_float_31e740(writer, factor * info->maximum);
}

// a transform: three axes and a translation (a motion's at 0x80)
struct transform_31e740
{
	__m128 axes[3];
	__m128 translation;
};

// (axis0 x + axis1 y) + (axis2 z + translation)
static inline __m128 transform_point_31e740(transform_31e740 const *transform, __m128 const &p)
{
	__m128 const z = _mm_add_ps(_mm_mul_ps(transform->axes[2], _mm_shuffle_ps(p, p, 0xaa)), transform->translation);
	__m128 const xy = _mm_add_ps(
		_mm_mul_ps(transform->axes[0], _mm_shuffle_ps(p, p, 0x00)),
		_mm_mul_ps(transform->axes[1], _mm_shuffle_ps(p, p, 0x55)));
	return _mm_add_ps(xy, z);
}

static inline void ball_socket_rows_31e740(__m128 const &point_a, __m128 const &point_b, query_31e740 const *query, writer_31e740 *writer)
{
	linear_jacobian_31e740 *jacobian = (linear_jacobian_31e740 *)writer->jacobians;
	__m128 const separation = _mm_sub_ps(point_b, point_a);
	for (int i = 0; i < 3; i++, jacobian++)
	{
		__m128 const axis = *(__m128 const *)identity_31e740[i];
		linear_row_31e740(query, point_a, point_b, axis, epsilon_31e740, jacobian);
		set_lane_31e740(jacobian->direction, 3, dot3_31e740(separation, axis) * query->factor_c);
		put_schema_31e740(writer, 0x3040d);
	}
	writer->jacobians = (char *)jacobian;
}

// @retail 0x31f160
void __cdecl jacobian_ball_socket_31f160(transform_31e740 const *transform_a, transform_31e740 const *transform_b,
	__m128 const *pivot_a, __m128 const *pivot_b, query_31e740 const *query, writer_31e740 *writer)
{
	__m128 const point_a = transform_point_31e740(transform_a, *pivot_a);
	__m128 const point_b = transform_point_31e740(transform_b, *pivot_b);
	ball_socket_rows_31e740(point_a, point_b, query, writer);
}

// @retail 0x31f3e0
void __cdecl jacobian_ball_socket_31f3e0(__m128 const *point_a, __m128 const *point_b, query_31e740 const *query, writer_31e740 *writer)
{
	ball_socket_rows_31e740(*point_a, *point_b, query, writer);
}

// a schema the solver calls back: callback(solver, data) with the data block
// of `size` bytes this gives back
typedef void (__cdecl *schema_callback_320290)(void *solver, void *data);

// @retail 0x320290
void *__cdecl jacobian_callback_320290(schema_callback_320290 callback, long size, query_31e740 const *query, writer_31e740 *writer)
{
	unsigned long *schema = (unsigned long *)writer->schemas;
	schema[0] = ((unsigned long)(size + 8) << 8) | 0x2000e;
	*(schema_callback_320290 *)&schema[1] = callback;
	char *const data = (char *)(schema + 2);
	writer->schemas = data + size;
	return data;
}

// ---------------------------------------------------------------------------
// hkMath's atan2 approximation: a cubic in the smaller ratio of |y| and |x|,
// folded into the quadrant

static float const atan_epsilon_3725e0 = 1.1920929e-07f; // 0x43e50c
static float const atan_c1_3725e0 = 0.12102559f; // 0x3df7f844 at 0x43e518
static float const atan_c2_3725e0 = 0.09352280f; // 0x3dbf88e4 at 0x43e514
static float const pi_3725e0 = 3.14159274f; // 0x40490fdb at 0x43e510
static float const half_pi_3725e0 = 1.57079637f; // 0x3fc90fdb at 0x4123ec

static inline float atan_cubic_3725e0(float t)
{
	float const t2 = t * t;
	return (t - atan_c1_3725e0 * t2) - t2 * t * atan_c2_3725e0;
}

// @retail 0x3725e0
float __cdecl atan2_approximation_3725e0(float y, float x)
{
	float const ay = __builtin_fabsf(y);
	float const ax = __builtin_fabsf(x);
	float angle;
	// (a NaN takes the second way, as retail's unordered compare does)
	if (ay <= ax)
	{
		angle = atan_cubic_3725e0(ay / (ax + atan_epsilon_3725e0));
	}
	else
	{
		angle = half_pi_3725e0 - atan_cubic_3725e0(ax / (ay + atan_epsilon_3725e0));
	}
	if (x < 0.0f)
	{
		angle = pi_3725e0 - angle;
	}
	if (y < 0.0f)
	{
		angle = -angle;
	}
	return angle;
}

// ---------------------------------------------------------------------------
// the contact constraint's rows

// a contact point: its position (lane 3 the distance) and normal
struct contact_point_31f620
{
	__m128 position;
	__m128 normal;
};

// a contact point's solver properties (as havok_contact_mgr_318a80.cpp's
// contact_properties_318c10); the solver's record for the point's row
struct contact_properties_31f620
{
	float impulse_limit;
	float allowed_penetration;
	float friction;
	float restitution;
	long field_10;
	// the penetration recovery carried from step to step
	float recovery;
	// nonzero: coupled with the point before it
	unsigned short coupled;
	unsigned short unknown1a;
};

typedef char assert_contact_properties_31f620_size[sizeof(contact_properties_31f620) == 0x1c ? 1 : -1];

// the friction the contact constraint keeps from step to step (at 0x50):
// the solver's friction record at 4, the two friction directions and the
// radius of the angular friction
struct friction_31f620
{
	unsigned long flags;
	float record_4;
	float rhs_0;
	float unknownc;
	float rhs_1;
	float unknown14;
	float angular_rhs;
	float radius;
	__m128 direction_0;
	__m128 direction_1;
};

typedef char assert_friction_31f620_size[sizeof(friction_31f620) == 0x40 ? 1 : -1];

// -0.05 (0xbd4cccd0 at 0x4155a4): the share of the penetration recovered per step
static float const recovery_rate_31f620 = -0.050000012f;

// the inverse of the rsqrtss estimate refined by one Newton step
static inline float inverse_sqrt_31f620(float value)
{
	float const estimate = _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(value)));
	return (0.5f * estimate) * (3.0f - (value * estimate) * estimate);
}

// 2x2 inverse of a coupled pair's effective masses: the coupling's factor
// for the second's schema word and the pair's effective masses scaled
static inline float couple_31f620(float coupling, float *mass_first, float *mass_second)
{
	float const first = *mass_first;
	float const second = *mass_second;
	float const inverse = 1.0f / (first * second - coupling * coupling);
	*mass_first = inverse * second;
	*mass_second = inverse * first;
	return -(inverse * coupling);
}

// the coupling of two linear rows through both bodies
static inline float coupling_31f620(query_31e740 const *query, linear_jacobian_31e740 const *first, linear_jacobian_31e740 const *second)
{
	solver_body_31e740 const *a = query->body_a;
	solver_body_31e740 const *b = query->body_b;
	__m128 const directions = _mm_mul_ps(second->direction, first->direction);
	__m128 const linear = _mm_add_ps(
		_mm_mul_ps(splat_31e740(lane_31e740(a->inverse_inertia, 3)), directions),
		_mm_mul_ps(splat_31e740(lane_31e740(b->inverse_inertia, 3)), directions));
	__m128 const angular_b = _mm_mul_ps(_mm_mul_ps(second->angular_b, first->angular_b), b->inverse_inertia);
	__m128 const angular_a = _mm_mul_ps(_mm_mul_ps(second->angular_a, first->angular_a), a->inverse_inertia);
	return sum3_31e740(_mm_add_ps(_mm_add_ps(angular_a, angular_b), linear));
}

// @retail 0x31f620
void __cdecl jacobian_contact_31f620(contact_point_31f620 const *points, long count, contact_properties_31f620 *properties,
	friction_31f620 *friction, query_31e740 const *query, writer_31e740 *writer)
{
	solver_body_31e740 const *a = query->body_a;
	solver_body_31e740 const *b = query->body_b;
	float const inverse_count = 1.0f / (float)count;

	schema_header_31e740 *header = (schema_header_31e740 *)writer->schemas;
	header->records = properties;
	header->type = 0x11801;
	header->body_a = query->body_a;
	header->body_b = query->body_b;
	header->record_size = sizeof(contact_properties_31f620);
	header->jacobians = writer->jacobians;
	char *jacobians = writer->jacobians;
	char *schemas = (char *)(header + 1);

	// the normals' and the positions' sums, the friction and the impulse
	// limits summed
	__m128 position_sum = _mm_setzero_ps();
	__m128 normal_sum = _mm_setzero_ps();
	float friction_sum = 0.0f;
	float limit_sum = 0.0f;

	linear_jacobian_31e740 *jacobian = (linear_jacobian_31e740 *)jacobians;
	for (long i = 0; i < count; i++, jacobian++)
	{
		contact_point_31f620 const *point = &points[i];
		contact_properties_31f620 *property = &properties[i];
		linear_row_31e740(query, point->position, point->position, point->normal, epsilon_31e740, jacobian);
		friction_sum += property->friction;
		limit_sum += property->impulse_limit;

		// the penetration recovered this step
		float const recovery = property->recovery;
		float const error = lane_31e740(point->position, 3) - recovery;
		float const allowed = -property->allowed_penetration - error;
		float const proposed = recovery * recovery_rate_31f620;
		float const step = proposed < query->factor_10 ? proposed : query->factor_10;
		float kept = step + recovery;
		float rhs = error - step;
		if (step + step + 0.001f < allowed)
		{
			kept = kept - allowed;
			rhs = rhs + allowed;
		}
		if (__builtin_fabsf(rhs) < 1e-10f)
		{
			rhs = 0.0f;
		}
		property->recovery = kept < 0.0f ? kept : 0.0f;

		position_sum = _mm_add_ps(position_sum, point->position);
		normal_sum = _mm_add_ps(normal_sum, point->normal);
		set_lane_31e740(jacobian->direction, 3, -(rhs * query->factor_c));

		if (property->coupled == 0)
		{
			*(unsigned long *)schemas = 0x30402;
		}
		else
		{
			// coupled with the point before: the 2x2 block inverted, the
			// first's schema word marking the pair
			linear_jacobian_31e740 *previous = jacobian - 1;
			float const coupling = coupling_31f620(query, previous, jacobian) * query->factor_18;
			*(unsigned long *)(schemas - 4) = 0x50803;
			float mass_previous = lane_31e740(previous->angular_b, 3);
			float mass = lane_31e740(jacobian->angular_b, 3);
			float const factor = couple_31f620(coupling, &mass_previous, &mass);
			set_lane_31e740(previous->angular_b, 3, mass_previous);
			set_lane_31e740(jacobian->angular_b, 3, mass);
			*(float *)schemas = factor;
		}
		schemas += 4;
	}
	jacobians = (char *)jacobian;

	float limit = limit_sum * (friction_sum * inverse_count);
	if (!(limit > 0.0f))
	{
		writer->schemas = schemas;
		writer->jacobians = jacobians;
		return;
	}

	// the friction plane's normal: the first point's when the normals agree,
	// else their mean, else the first's when it is long enough, else up
	__m128 normal;
	float const normal_length_squared = dot3_31e740(normal_sum, normal_sum);
	if (inverse_count * inverse_count * normal_length_squared > 0.9999f)
	{
		normal = points[0].normal;
	}
	else if (normal_length_squared > 0.1f)
	{
		normal = _mm_mul_ps(splat_31e740(inverse_sqrt_31f620(normal_length_squared)), normal_sum);
	}
	else
	{
		normal = points[0].normal;
		if (dot3_31e740(normal, normal) < 0.9f)
		{
			normal = _mm_setzero_ps();
			set_lane_31e740(normal, 1, 1.0f);
			limit = 0.0f;
		}
	}

	// the friction directions: the last ones turned into the plane, a new
	// pair when the last first direction is (nearly) along the normal
	for (;;)
	{
		friction->direction_1 = cross_31e740(friction->direction_0, normal);
		if (dot3_31e740(friction->direction_1, friction->direction_1) > 0.001f)
		{
			break;
		}
		float const ax = __builtin_fabsf(lane_31e740(normal, 0));
		float const ay = __builtin_fabsf(lane_31e740(normal, 1));
		float const az = __builtin_fabsf(lane_31e740(normal, 2));
		int smallest = 0;
		int j = 1;
		int k = 2;
		float current = ax;
		if (ay < ax)
		{
			j = 0;
			current = ay;
			smallest = 1;
		}
		if (az < current)
		{
			k = smallest;
			smallest = 2;
		}
		float *direction = (float *)&friction->direction_0;
		direction[smallest] = 0.0f;
		direction[j] = lane_31e740(normal, k);
		direction[k] = -lane_31e740(normal, j);
		limit = 0.0f;
	}
	friction->direction_1 = _mm_mul_ps(
		splat_31e740(inverse_sqrt_31f620(dot3_31e740(friction->direction_1, friction->direction_1))),
		friction->direction_1);
	friction->direction_0 = cross_31e740(normal, friction->direction_1);

	// the two friction rows through the points' centre
	__m128 const centre = _mm_mul_ps(splat_31e740(inverse_count), position_sum);
	linear_jacobian_31e740 *first = (linear_jacobian_31e740 *)jacobians;
	linear_row_31e740(query, centre, centre, friction->direction_0, epsilon_31e740, first);
	set_lane_31e740(first->direction, 3, friction->rhs_0 * query->factor_c);
	linear_jacobian_31e740 *second = first + 1;
	linear_row_31e740(query, centre, centre, friction->direction_1, epsilon_31e740, second);
	set_lane_31e740(second->direction, 3, friction->rhs_1 * query->factor_c);
	jacobians = (char *)(second + 1);

	float const coupling = coupling_31f620(query, first, second);
	float const scaled_limit = limit * query->factor_1c;
	unsigned long *schema = (unsigned long *)schemas;
	schema[0] = 0x51407;
	float mass_first = lane_31e740(first->angular_b, 3);
	float mass_second = lane_31e740(second->angular_b, 3);
	float const inverse = 1.0f / (mass_second * mass_first - coupling * coupling);
	set_lane_31e740(first->angular_b, 3, inverse * mass_second);
	set_lane_31e740(second->angular_b, 3, inverse * mass_first);
	*(float **)&schema[1] = &friction->record_4;
	schema[4] = 8;
	((float *)schema)[2] = -(inverse * coupling);
	((float *)schema)[3] = scaled_limit;
	char *const after_friction = (char *)(schema + 5);
	if (count <= 1)
	{
		writer->schemas = after_friction;
		writer->jacobians = jacobians;
		return;
	}

	// the angular friction about the normal, its radius the points' mean
	// distance from their centre (measured again when flag 4 asks)
	if (friction->flags & 4)
	{
		friction->radius = 0.0f;
		for (long i = 0; i < count; i++)
		{
			__m128 const offset = _mm_sub_ps(points[i].position, centre);
			friction->radius = __builtin_sqrtf(dot3_31e740(offset, offset)) + friction->radius;
		}
		friction->radius = inverse_count * friction->radius;
		if (friction->radius < 9.99999997e-07f) // 0x358637bd at 0x4155a8
		{
			writer->schemas = after_friction;
			writer->jacobians = jacobians;
			return;
		}
		friction->flags &= ~4ul;
	}
	angular_jacobian_31e740 *angular = (angular_jacobian_31e740 *)jacobians;
	angular->a = body_row_31e740(a, normal);
	angular->b = body_row_31e740(b, negate_31e740(normal));
	set_lane_31e740(angular->a, 3, 1.0f / (angular_mass_31e740(a, b, angular->a, angular->b) + epsilon_31e740));
	float const inverse_radius = 1.0f / friction->radius;
	((float *)schema)[5] = friction->radius;
	schema[0] = 0x61808;
	set_lane_31e740(angular->a, 3, inverse_radius * lane_31e740(angular->a, 3));
	set_lane_31e740(angular->b, 3, friction->angular_rhs * query->factor_c);
	writer->schemas = (char *)(schema + 6);
	writer->jacobians = (char *)(angular + 1);
}

// ---------------------------------------------------------------------------
// the contact constraint's slots 9 and 10 (vtable 0x4153c8; the class as
// port/recovered/havok_contact_mgr_318a80.cpp lays it out) and the two
// modifiers that share its layout: the mass changer (vtable 0x414d0c, the
// object at 0x480770) and the surface velocity (vtable 0x414ce0, at
// 0x480880). 0x315340 and 0x3153c0 (port/recovered/
// havok_constraint_joints_3128d0.cpp) turn a contact constraint into one.

// how much solver room a constraint takes
struct solver_info_31bd90
{
	long max_jacobian_size;
	long jacobians_size;
	long schemas_size;
	long jacobian_count;
};

struct contact_array_31bd90
{
	void *data;
	long size;
	long capacity_and_flags;
};

class contact_constraint_31bd90
{
public:
	void build_jacobians(query_31e740 const *query, writer_31e740 *writer);
	void solver_info(solver_info_31bd90 *info);

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void const *atom_vtable;
	void *entity_a;
	void *entity_b;
	void *owner;
	long field_18;
	char unknown1c[0x34 - 0x1c];
	contact_array_31bd90 points;
	char unknown40[0x50 - 0x40];
	friction_31f620 friction;
	contact_array_31bd90 properties;
	char unknown9c[0x110 - 0x9c];
};

typedef char assert_contact_constraint_31bd90_size[sizeof(contact_constraint_31bd90) == 0x110 ? 1 : -1];
typedef char assert_contact_constraint_31bd90_friction[__builtin_offsetof(contact_constraint_31bd90, friction) == 0x50 ? 1 : -1];
typedef char assert_contact_constraint_31bd90_properties[__builtin_offsetof(contact_constraint_31bd90, properties) == 0x90 ? 1 : -1];

// @retail 0x31bdf0
void contact_constraint_31bd90::build_jacobians(query_31e740 const *query, writer_31e740 *writer)
{
	jacobian_contact_31f620((contact_point_31f620 const *)points.data, points.size,
		(contact_properties_31f620 *)properties.data, &friction, query, writer);
}

// a row per point, two friction rows and, with two points or more, the
// angular friction row
// @retail 0x31bd90
void contact_constraint_31bd90::solver_info(solver_info_31bd90 *info)
{
	info->max_jacobian_size = 0;
	info->jacobians_size = 0;
	info->jacobian_count = 0;
	info->schemas_size = 0x18;
	long const count = properties.size;
	info->max_jacobian_size = count * 0x30 + 0x80;
	info->jacobians_size = (count + 2) * 0x30;
	info->schemas_size = count * 4 + 0x2c;
	info->jacobian_count = count + 2;
	if (count >= 2)
	{
		info->jacobians_size += 0x20;
		info->schemas_size += 4;
		info->jacobian_count++;
	}
}

// the modifiers' own fields, after the contact constraint's
class contact_modifier_315620 : public contact_constraint_31bd90
{
public:
	void build_surface_velocity(query_31e740 const *query, writer_31e740 *writer);
	void surface_velocity_info(solver_info_31bd90 *info);
	void build_mass_changer(query_31e740 const *query, writer_31e740 *writer);
	void mass_changer_info(solver_info_31bd90 *info);
};

// the modifiers' factors share the constraint's bytes at 0x40
static inline float &modifier_factor_315620(contact_constraint_31bd90 *constraint, int index)
{
	return ((float *)((char *)constraint + 0x40))[index];
}

// the surface velocity's data: the velocity and the first friction row
struct surface_velocity_315440
{
	float velocity[4];
	linear_jacobian_31e740 *friction;
};

// adds the surface velocity along the two friction rows to their
// right-hand sides
// @retail 0x315440
void __cdecl surface_velocity_callback_315440(void *solver, surface_velocity_315440 *data)
{
	__m128 velocity;
	((float *)&velocity)[0] = data->velocity[0];
	((float *)&velocity)[1] = data->velocity[1];
	((float *)&velocity)[2] = data->velocity[2];
	((float *)&velocity)[3] = data->velocity[3];
	linear_jacobian_31e740 *rows = data->friction;
	__m128 const first = rows[0].direction;
	__m128 const second = rows[1].direction;
	set_lane_31e740(rows[0].direction, 3, dot3_31e740(first, velocity) + lane_31e740(rows[0].direction, 3));
	set_lane_31e740(rows[1].direction, 3, dot3_31e740(second, velocity) + lane_31e740(rows[1].direction, 3));
}

// the entity's surface velocity (src/unknown_1cec30.cpp), null for none
extern "C" float const *h2_retail_1cf380(void *entity);

// the contact rows, then (when friction rows were written) the callback that
// adds A's surface velocity, scaled by the factor at 0x44, to them
// @retail 0x315620
void contact_modifier_315620::build_surface_velocity(query_31e740 const *query, writer_31e740 *writer)
{
	char *const start = writer->jacobians;
	build_jacobians(query, writer);
	float const *velocity = h2_retail_1cf380(entity_a);
	if (!velocity)
	{
		return;
	}
	long const contact_size = points.size * (long)sizeof(linear_jacobian_31e740);
	if (writer->jacobians - start == contact_size)
	{
		return;
	}
	surface_velocity_315440 *data = (surface_velocity_315440 *)jacobian_callback_320290(
		(schema_callback_320290)surface_velocity_callback_315440, sizeof(surface_velocity_315440), query, writer);
	float const scale = modifier_factor_315620(this, 1);
	data->velocity[0] = scale * velocity[0];
	data->velocity[1] = scale * velocity[1];
	data->velocity[2] = scale * velocity[2];
	data->velocity[3] = scale * velocity[3];
	data->friction = (linear_jacobian_31e740 *)(start + contact_size);
}

// @retail 0x3155f0
void contact_modifier_315620::surface_velocity_info(solver_info_31bd90 *info)
{
	solver_info(info);
	info->schemas_size += 0x28;
}

// the mass changer's data: the bodies' inverse inertias kept (restore), or
// the factors to scale them by (scale)
struct mass_changer_315280
{
	float inertia_a[4];
	float inertia_b[4];
	float factor_a;
	float factor_b;
	solver_body_31e740 *body_a;
	solver_body_31e740 *body_b;
};

typedef char assert_mass_changer_315280_size[sizeof(mass_changer_315280) == 0x30 ? 1 : -1];

// @retail 0x315280
void __cdecl mass_changer_scale_315280(void *solver, mass_changer_315280 *data)
{
	float *inertia_a = (float *)&data->body_a->inverse_inertia;
	inertia_a[0] = data->factor_a * inertia_a[0];
	inertia_a[1] = data->factor_a * inertia_a[1];
	inertia_a[2] = inertia_a[2] * data->factor_a;
	inertia_a[3] = data->factor_a * inertia_a[3];
	float *inertia_b = (float *)&data->body_b->inverse_inertia;
	inertia_b[0] = data->factor_b * inertia_b[0];
	inertia_b[1] = data->factor_b * inertia_b[1];
	inertia_b[2] = data->factor_b * inertia_b[2];
	inertia_b[3] = data->factor_b * inertia_b[3];
}

// @retail 0x3152f0
void __cdecl mass_changer_restore_3152f0(void *solver, mass_changer_315280 *data)
{
	float *inertia_a = (float *)&data->body_a->inverse_inertia;
	float *inertia_b = (float *)&data->body_b->inverse_inertia;
	for (int i = 0; i < 4; i++)
	{
		inertia_a[i] = data->inertia_a[i];
	}
	for (int i = 0; i < 4; i++)
	{
		inertia_b[i] = data->inertia_b[i];
	}
}

// the contact rows built with the bodies' inverse inertias scaled by the
// factors at 0x40 and 0x44, the solver told to scale and restore them
// around them too
// @retail 0x315740
void contact_modifier_315620::build_mass_changer(query_31e740 const *query, writer_31e740 *writer)
{
	solver_body_31e740 *a = query->body_a;
	solver_body_31e740 *b = query->body_b;
	jacobian_header_31f5e0(query, writer);
	float const factor_a = modifier_factor_315620(this, 0);
	float const factor_b = modifier_factor_315620(this, 1);
	mass_changer_315280 *scale = (mass_changer_315280 *)jacobian_callback_320290(
		(schema_callback_320290)mass_changer_scale_315280, sizeof(mass_changer_315280), query, writer);
	scale->factor_a = factor_a;
	scale->factor_b = factor_b;
	scale->body_a = a;
	scale->body_b = b;

	float kept_a[4];
	float kept_b[4];
	float *inertia_a = (float *)&a->inverse_inertia;
	float *inertia_b = (float *)&b->inverse_inertia;
	for (int i = 0; i < 4; i++)
	{
		kept_a[i] = inertia_a[i];
		kept_b[i] = inertia_b[i];
	}
	for (int i = 0; i < 4; i++)
	{
		inertia_a[i] = factor_a * inertia_a[i];
	}
	for (int i = 0; i < 4; i++)
	{
		inertia_b[i] = factor_b * inertia_b[i];
	}
	build_jacobians(query, writer);

	mass_changer_315280 *restore = (mass_changer_315280 *)jacobian_callback_320290(
		(schema_callback_320290)mass_changer_restore_3152f0, sizeof(mass_changer_315280), query, writer);
	for (int i = 0; i < 4; i++)
	{
		restore->inertia_a[i] = kept_a[i];
		restore->inertia_b[i] = kept_b[i];
	}
	for (int i = 0; i < 4; i++)
	{
		inertia_a[i] = kept_a[i];
		inertia_b[i] = kept_b[i];
	}
	restore->body_b = b;
	restore->body_a = a;
}

// @retail 0x315720
void contact_modifier_315620::mass_changer_info(solver_info_31bd90 *info)
{
	solver_info(info);
	info->schemas_size += 0x88;
}
