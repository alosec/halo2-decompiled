#ifndef HAVOK_AGENT_PARTS_2E63A0_H
#define HAVOK_AGENT_PARTS_2E63A0_H
// Havok (third-party physics library): what the collision agents of
// havok_agents_2e63a0.cpp and havok_multi_sphere_agent_2e7dc0.cpp share:
// the sides of a query, the query inputs, the collectors, the dispatcher's
// tables of handlers for pairs of shape types (its layout as
// havok_dispatcher_2e5a90.cpp's s_1c3a40 has it) and the agent interface.
//
// Every agent's vtable has eight slots:
//   0  deleting destructor          4  process the pair's contacts
//   1  penetrations (overlap)       5  cleanup (tell children, then delete)
//   2  closest points               6  an hkBool answer (false for the agents
//   3  linear cast                     made for (A, B), true for (B, A))
//                                   7  a three-argument update passed on to
//                                      every child (the default does nothing)
// and every pair of shape types has four handlers in the dispatcher: create
// an agent, and the static penetrations, closest points and linear cast.
#include <xmmintrin.h>
#include "reference_counted_2d88a0.h"
#include "timer_mark_2ebc80.h"

// a shape: its sixth virtual method gives its type (the HK_SHAPE_* number
// 0x2e6220 names)
class shape_2e63a0
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual long type() const = 0;
};

// one side of a query: a shape, the key of the part concerned, where it sits
// (a motion_2e63a0) and the side it is a part of
struct body_2e63a0
{
	shape_2e63a0 const *shape;
	long key;
	void const *motion;
	body_2e63a0 const *parent;
};

typedef char assert_body_2e63a0[(sizeof(body_2e63a0) == 0x10) ? 1 : -1];

// a side's child shape, at the side's key and placement, with the side as its
// parent
__forceinline void child_body_2e63a0(body_2e63a0 *out, body_2e63a0 const *parent, void const *shape)
{
	out->shape = (shape_2e63a0 const *)shape;
	out->key = parent->key;
	out->motion = parent->motion;
	out->parent = parent;
}

// where a side sits: two vectors, then its transform (three axes and the
// position)
struct motion_2e63a0
{
	__m128 field_0;
	__m128 field_10;
	__m128 axes[3];
	__m128 position;
};

typedef char assert_motion_2e63a0[(sizeof(motion_2e63a0) == 0x60) ? 1 : -1];

struct dispatcher_2e63a0;

// a query's input: it starts with the dispatcher
struct input_2e63a0
{
	dispatcher_2e63a0 *dispatcher;
	long field_4;
	long field_8;
	long field_c;
};

// a linear cast's input: the same, then the path cast along
struct __declspec(align(16)) cast_input_2e63a0
{
	dispatcher_2e63a0 *dispatcher;
	long field_4;
	long field_8;
	long field_c;
	__m128 path;
	long field_20;
	long field_24;
};

typedef char assert_cast_input_2e63a0[(sizeof(cast_input_2e63a0) == 0x30) ? 1 : -1];

// a collector of penetrations: set once the pair overlaps (the queries stop
// early then)
struct penetrations_2e63a0
{
	void const *vtable;
	bool found;
};

// a collector of points: queries skip what lies beyond its early-out distance
struct points_2e63a0
{
	void const *vtable;
	float early_out;
};

// one contact point the processing adds: a position, a normal and the rest
struct contact_point_2e63a0
{
	__m128 position;
	__m128 normal;
	__m128 field_20;
};

struct contact_points_2e63a0
{
	contact_point_2e63a0 *data;
	long size;
	long capacity_and_flags;
};

// what the processing adds to
struct result_2e63a0
{
	contact_points_2e63a0 *points;
};

class agent_2e63a0;

typedef agent_2e63a0 *(__cdecl *create_2e63a0)(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, void *manager);
typedef void (__cdecl *penetrations_query_2e63a0)(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, penetrations_2e63a0 *collector);
typedef void (__cdecl *closest_query_2e63a0)(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, points_2e63a0 *collector);
typedef void (__cdecl *cast_query_2e63a0)(body_2e63a0 const *a, body_2e63a0 const *b,
	cast_input_2e63a0 const *input, points_2e63a0 *cast_collector, points_2e63a0 *start_collector);

// the dispatcher's four 32 x 32 tables, by the two sides' shape types
struct dispatcher_2e63a0
{
	char unknown0[0x18c];
	create_2e63a0 create[32][32];
	penetrations_query_2e63a0 penetrations[32][32];
	closest_query_2e63a0 closest[32][32];
	cast_query_2e63a0 cast[32][32];
};

typedef char assert_dispatcher_2e63a0_create[(__builtin_offsetof(dispatcher_2e63a0, create) == 0x18c) ? 1 : -1];
typedef char assert_dispatcher_2e63a0_penetrations[(__builtin_offsetof(dispatcher_2e63a0, penetrations) == 0x118c) ? 1 : -1];
typedef char assert_dispatcher_2e63a0_closest[(__builtin_offsetof(dispatcher_2e63a0, closest) == 0x218c) ? 1 : -1];
typedef char assert_dispatcher_2e63a0_cast[(__builtin_offsetof(dispatcher_2e63a0, cast) == 0x318c) ? 1 : -1];

// the agent interface (slot 6 answers through a hidden pointer and is never
// called here)
class agent_2e63a0
{
public:
	virtual void destroy(unsigned long flags) = 0;
	virtual void penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		penetrations_2e63a0 *collector) = 0;
	virtual void closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		points_2e63a0 *collector) = 0;
	virtual void cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
		points_2e63a0 *cast_collector, points_2e63a0 *start_collector) = 0;
	virtual void process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		result_2e63a0 *result) = 0;
	virtual void cleanup() = 0;
	virtual void slot_6() = 0;
	virtual void update(body_2e63a0 const *a, body_2e63a0 const *b, void const *input) = 0;
};

// the agent the dispatcher makes for two sides
__forceinline agent_2e63a0 *create_agent_2e63a0(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, void *manager)
{
	dispatcher_2e63a0 *dispatcher = input->dispatcher;
	long first_type = a->shape->type();
	long second_type = b->shape->type();
	return dispatcher->create[first_type][second_type](a, b, input, manager);
}

// ---------------------------------------------------------------------------
// the collectors that pass results on seen from the other side, for the
// agents made for (B, A) that run the (A, B) queries with the sides swapped

// passes each penetration on with its two sides swapped (vtable 0x412980:
// 0x2e7a50, 0x3018d0)
struct flipped_penetrations_2e63a0 : penetrations_2e63a0
{
	penetrations_2e63a0 *collector;

	flipped_penetrations_2e63a0(penetrations_2e63a0 *collector_)
	{
		vtable = (void const *)0x412980;
		found = false;
		collector = collector_;
	}
};

// passes each point on moved along its normal by its distance, its normal
// flipped (vtable 0x412970: 0x2e7a80, 0x3018f0)
struct flipped_points_2e63a0 : points_2e63a0
{
	points_2e63a0 *collector;

	flipped_points_2e63a0(points_2e63a0 *collector_)
	{
		vtable = (void const *)0x412970;
		early_out = 3.40282347e+38f;
		collector = collector_;
	}
};

// passes each cast hit on moved along the path by its distance, its normal
// flipped (vtable 0x412978: 0x2e7a80, 0x301970)
struct __declspec(align(16)) flipped_cast_2e63a0 : points_2e63a0
{
	__m128 path;
	points_2e63a0 *collector;
};

typedef char assert_flipped_cast_2e63a0[(__builtin_offsetof(flipped_cast_2e63a0, collector) == 0x20) ? 1 : -1];

__forceinline void flipped_cast_init_2e63a0(flipped_cast_2e63a0 *flipped, __m128 path, points_2e63a0 *collector)
{
	flipped->early_out = 3.40282347e+38f;
	flipped->vtable = (void const *)0x412978;
	flipped->path = path;
	flipped->collector = collector;
}

static __declspec(align(16)) const unsigned long g_sign_mask_2e63a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

__forceinline __m128 negate_2e63a0(__m128 value)
{
	return _mm_xor_ps(value, *(__m128 const *)g_sign_mask_2e63a0);
}

// a cast's input with the path reversed (only the words retail copies)
__forceinline void reversed_cast_input_2e63a0(cast_input_2e63a0 *out, cast_input_2e63a0 const *input)
{
	out->dispatcher = input->dispatcher;
	out->field_4 = input->field_4;
	out->field_8 = input->field_8;
	out->field_c = input->field_c;
	out->field_20 = input->field_20;
	out->field_24 = input->field_24;
	out->path = negate_2e63a0(input->path);
}

// the normals of the contact points added from `first` on flipped
__forceinline void flip_normals_2e63a0(result_2e63a0 *result, long first)
{
	for (long i = first; i < result->points->size; i++)
	{
		contact_point_2e63a0 *point = &result->points->data[i];
		point->normal = negate_2e63a0(point->normal);
	}
}

#endif
