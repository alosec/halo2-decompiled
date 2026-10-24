// @flags /O2 /GL-
// Havok (third-party physics library): the collision agent for multi-sphere
// shapes (type 9, HK_SHAPE_MULTI_SPHERE: a count at +0xc, then the spheres
// from +0x10, each a centre with its radius in the last lane).
//
// Every query runs one sphere at a time: a sphere shape built on the stack
// (0x2def90, radius 0) is given the sphere's radius, and a copy of the side's
// motion is moved to the sphere's centre (the centre turned by the motion's
// axes, plus its position); the sphere stands in for the multi-sphere at the
// key of its index, with the multi-sphere's side as its parent. When done
// the stack sphere is destroyed as the reference base's destructor (vtable
// 0x452c1c) does it.
//
// The agent: 0x38 bytes {vtable, size, manager +8, an array of {sphere
// index, agent} pairs +0xc with room for four in the agent itself +0x18}. Its
// constructor 0x2e8a40 asks the dispatcher for an agent per sphere and keeps
// those it gets; the queries walk the pairs, the keys counting down from the
// last as the pairs go up (as retail has it). Its linear cast 0x2e8240 walks
// the spheres from the last instead, but always casts with the first pair's
// agent (as retail has it).
//   vtable 0x412a54 (made for (9, B)): 0x2e8e60 (destructor 0x2e8c80),
//     0x2e8560 penetrations, 0x2e7f30 closest, 0x2e8240 cast, 0x2e7dc0
//     process, 0x2ebc50 cleanup, 0x2fe820, 0x2dae30 update
//   vtable 0x412a74 (made for (A, 9)): 0x2e8f80 (destructor 0x2e8fb0),
//     0x2e8ce0, 0x2e8d40, 0x2e8e90, 0x2e8da0, 0x2ebc50, 0x31bc80, 0x2dae30
//   handlers (0x2e9030 registers them): (9, any) 0x2e8e00 create, 0x2e86e0,
//     0x2e80a0, 0x2e83c0; (any, 9) 0x2e8fc0 create, 0x2e8880, 0x2e88e0,
//     0x2e8940
// Penetration queries stop at the first sphere that overlaps (the
// collector's flag; the flipping collector of the (A, 9) agent never sets
// its own, so those run every sphere).
#include <new>
#include "havok_agent_parts_2e63a0.h"
#include "growable_array_2d91d0.h"
#include "shape_2de750.h"
#include "unknown_11c920.h"

// ---------------------------------------------------------------------------
// recovered code this file calls, declared as src declares it

// the sphere shape, built from a radius (src/unknown_2def90.cpp)
class c_2def90 : public radius_shape_2de750
{
public:
	c_2def90(real arg_0);
};

// the reference base's registration switch (src/unknown_2d7210.cpp)
class c_physics_runtime_base
{
	dword vtable;
	word flags;
public:
	void function_2d7240(bool enable);
};

// ---------------------------------------------------------------------------

struct __declspec(align(16)) multi_sphere_2e7dc0
{
	char unknown0[0xc];
	long count;
	__m128 spheres[1];
};

typedef char assert_multi_sphere_2e7dc0[(__builtin_offsetof(multi_sphere_2e7dc0, spheres) == 0x10) ? 1 : -1];

// the sphere standing in for one of the multi-sphere's
struct stack_sphere_2e7dc0
{
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	long user_value;
	float radius;
};

typedef char assert_stack_sphere_2e7dc0[(sizeof(stack_sphere_2e7dc0) == sizeof(c_2def90)) ? 1 : -1];

// one sphere's place and the body it is queried as
struct __declspec(align(16)) sphere_query_2e7dc0
{
	stack_sphere_2e7dc0 sphere;
	body_2e63a0 body;
	motion_2e63a0 motion;

	// a stack sphere of radius 0, the side's motion copied, and the body
	// naming them both with the side as its parent
	__forceinline void begin(body_2e63a0 const *side)
	{
		motion = *(motion_2e63a0 const *)side->motion;
		// retail leaves the size word as it finds it on the stack (only its
		// top bit is ever looked at, below); here it starts clear
		sphere.memory_size = 0;
		new (&sphere) c_2def90(0.0f);
		body.motion = &motion;
		body.parent = side;
	}

	// the stack sphere made the multi-sphere's sphere `index` of the side, at
	// key `key`
	__forceinline void place(body_2e63a0 const *side, long index, long key)
	{
		multi_sphere_2e7dc0 const *shape = (multi_sphere_2e7dc0 const *)side->shape;
		__m128 centre = shape->spheres[index];
		__m128 x = _mm_shuffle_ps(centre, centre, 0x00);
		__m128 y = _mm_shuffle_ps(centre, centre, 0x55);
		__m128 z = _mm_shuffle_ps(centre, centre, 0xaa);
		__m128 turned = _mm_add_ps(_mm_add_ps(_mm_mul_ps(motion.axes[0], x), _mm_mul_ps(motion.axes[1], y)),
			_mm_mul_ps(motion.axes[2], z));
		motion.position = _mm_add_ps(((motion_2e63a0 const *)side->motion)->position, turned);
		sphere.radius = centre.m128_f32[3];
		body.shape = (shape_2e63a0 const *)&sphere;
		body.key = key;
	}

	// the reference base's destructor
	__forceinline void end()
	{
		sphere.vtable = (void const *)0x452c1c;
		if (sphere.memory_size & 0x8000)
		{
			sphere.memory_size &= 0x7fff;
			((c_physics_runtime_base *)&sphere)->function_2d7240(false);
		}
	}
};

// the sphere shape's type: retail calls the sphere's type method (0x9cd90,
// code it shares with others) directly
static long const k_sphere_type_2e7dc0 = 4;

struct pair_2e7dc0
{
	long index;
	agent_2e63a0 *agent;
};

ELEMENTS_2D91D0(pairs_2e7dc0, pair_2e7dc0);

class multi_sphere_agent_2e7dc0
{
public:
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void *manager;
	pairs_2e7dc0 pairs;
	pair_2e7dc0 storage[4];

	multi_sphere_agent_2e7dc0 *construct(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		void *manager_);
	void destroy();
	void destroy_flipped();

	void penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		penetrations_2e63a0 *collector);
	void closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input, points_2e63a0 *collector);
	void cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
		points_2e63a0 *cast_collector, points_2e63a0 *start_collector);
	void process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input, result_2e63a0 *result);

	// made for (A, 9)
	void flipped_penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		penetrations_2e63a0 *collector);
	void flipped_closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		points_2e63a0 *collector);
	void flipped_cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
		points_2e63a0 *cast_collector, points_2e63a0 *start_collector);
	void flipped_process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		result_2e63a0 *result);
};

typedef char assert_multi_sphere_agent_2e7dc0_pairs[(__builtin_offsetof(multi_sphere_agent_2e7dc0, pairs) == 0xc) ? 1 : -1];
typedef char assert_multi_sphere_agent_2e7dc0[(sizeof(multi_sphere_agent_2e7dc0) == 0x38) ? 1 : -1];

// @retail 0x2e8a40
multi_sphere_agent_2e7dc0 *multi_sphere_agent_2e7dc0::construct(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, void *manager_)
{
	manager = manager_;
	vtable = (void const *)0x412a54;
	pairs.data = storage;
	pairs.size = 0;
	pairs.capacity_and_flags = 0x80000004;
	long count = ((multi_sphere_2e7dc0 const *)a->shape)->count;
	pairs.ensure_capacity(count);

	sphere_query_2e7dc0 query;
	query.begin(a);
	for (long i = 0; i < count; i++)
	{
		query.place(a, i, i);
		dispatcher_2e63a0 *dispatcher = input->dispatcher;
		long first_type = query.body.shape->type();
		long second_type = b->shape->type();
		agent_2e63a0 *agent = dispatcher->create[first_type][second_type](&query.body, b, input, manager_);
		if (agent)
		{
			pair_2e7dc0 &pair = pairs.next_slot();
			pair.agent = agent;
			pair.index = i;
		}
	}
	query.end();
	return this;
}

// @retail 0x2e8c80
void multi_sphere_agent_2e7dc0::destroy()
{
	if (pairs.capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(pairs.data, (pairs.capacity_and_flags & 0x7fffffff) * sizeof(pair_2e7dc0),
			0x12);
	}
	vtable = (void const *)0x452c30;
}

// the (A, 9) agent's destructor: the same code
// @retail 0x2e8fb0
void multi_sphere_agent_2e7dc0::destroy_flipped()
{
	destroy();
}

// @retail 0x2e8560
void multi_sphere_agent_2e7dc0::penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	sphere_query_2e7dc0 query;
	query.begin(a);
	pair_2e7dc0 *pair = pairs.data;
	for (long key = pairs.size - 1; key >= 0; key--, pair++)
	{
		query.place(a, pair->index, key);
		pair->agent->penetrations(&query.body, b, input, collector);
		if (collector->found)
		{
			break;
		}
	}
	query.end();
}

// @retail 0x2e7f30
void multi_sphere_agent_2e7dc0::closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	sphere_query_2e7dc0 query;
	query.begin(a);
	pair_2e7dc0 *pair = pairs.data;
	for (long key = pairs.size - 1; key >= 0; key--, pair++)
	{
		query.place(a, pair->index, key);
		pair->agent->closest(&query.body, b, input, collector);
	}
	query.end();
}

// @retail 0x2e8240
void multi_sphere_agent_2e7dc0::cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
	points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
{
	sphere_query_2e7dc0 query;
	query.begin(a);
	pair_2e7dc0 *first = pairs.data;
	for (long i = pairs.size - 1; i >= 0; i--)
	{
		query.place(a, i, i);
		first->agent->cast(&query.body, b, input, cast_collector, start_collector);
	}
	query.end();
}

// @retail 0x2e7dc0
void multi_sphere_agent_2e7dc0::process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	result_2e63a0 *result)
{
	sphere_query_2e7dc0 query;
	query.begin(a);
	pair_2e7dc0 *pair = pairs.data;
	for (long key = pairs.size - 1; key >= 0; key--, pair++)
	{
		query.place(a, pair->index, key);
		pair->agent->process(&query.body, b, input, result);
	}
	query.end();
}

// the static queries for (9, any): every sphere through the dispatcher's
// handler for (sphere, the other side's type)
// @retail 0x2e86e0
void __cdecl function_2e86e0(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	multi_sphere_2e7dc0 const *shape = (multi_sphere_2e7dc0 const *)a->shape;
	sphere_query_2e7dc0 query;
	query.begin(a);
	long second_type = b->shape->type();
	for (long i = 0; i < shape->count; i++)
	{
		query.place(a, i, i);
		input->dispatcher->penetrations[k_sphere_type_2e7dc0][second_type](&query.body, b, input, collector);
		if (collector->found)
		{
			break;
		}
	}
	query.end();
}

// @retail 0x2e80a0
void __cdecl function_2e80a0(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	multi_sphere_2e7dc0 const *shape = (multi_sphere_2e7dc0 const *)a->shape;
	sphere_query_2e7dc0 query;
	query.begin(a);
	long second_type = b->shape->type();
	for (long i = 0; i < shape->count; i++)
	{
		query.place(a, i, i);
		input->dispatcher->closest[k_sphere_type_2e7dc0][second_type](&query.body, b, input, collector);
	}
	query.end();
}

// @retail 0x2e83c0
void __cdecl function_2e83c0(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
	points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
{
	multi_sphere_2e7dc0 const *shape = (multi_sphere_2e7dc0 const *)a->shape;
	sphere_query_2e7dc0 query;
	query.begin(a);
	long second_type = b->shape->type();
	for (long i = 0; i < shape->count; i++)
	{
		query.place(a, i, i);
		input->dispatcher->cast[k_sphere_type_2e7dc0][second_type](&query.body, b, input, cast_collector,
			start_collector);
	}
	query.end();
}

// @retail 0x2e8e00
agent_2e63a0 *__cdecl function_2e8e00(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	void *manager)
{
	multi_sphere_agent_2e7dc0 *agent = (multi_sphere_agent_2e7dc0 *)g_memory_2d88a0->allocate_block(0x38, 0x1a);
	agent->memory_size = 0x38;
	return (agent_2e63a0 *)agent->construct(a, b, input, manager);
}

// the static queries for (any, 9)
// @retail 0x2e8880
void __cdecl function_2e8880(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	flipped_penetrations_2e63a0 flipped(collector);
	function_2e86e0(b, a, input, &flipped);
}

// @retail 0x2e88e0
void __cdecl function_2e88e0(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	flipped_points_2e63a0 flipped(collector);
	function_2e80a0(b, a, input, &flipped);
}

// @retail 0x2e8940
void __cdecl function_2e8940(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
	points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
{
	cast_input_2e63a0 reversed;
	reversed_cast_input_2e63a0(&reversed, input);
	flipped_cast_2e63a0 flipped_cast;
	flipped_cast_init_2e63a0(&flipped_cast, input->path, cast_collector);
	if (start_collector)
	{
		flipped_cast_2e63a0 flipped_start;
		flipped_cast_init_2e63a0(&flipped_start, input->path, start_collector);
		function_2e83c0(b, a, &reversed, &flipped_cast, &flipped_start);
	}
	else
	{
		function_2e83c0(b, a, &reversed, &flipped_cast, 0);
	}
}

// @retail 0x2e8fc0
agent_2e63a0 *__cdecl function_2e8fc0(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	void *manager)
{
	multi_sphere_agent_2e7dc0 *agent = (multi_sphere_agent_2e7dc0 *)g_memory_2d88a0->allocate_block(0x38, 0x1a);
	agent->memory_size = 0x38;
	agent->construct(b, a, input, manager);
	agent->vtable = (void const *)0x412a74;
	return (agent_2e63a0 *)agent;
}

// the agent made for (A, 9): its own (9, A) methods with the sides swapped
// @retail 0x2e8ce0
void multi_sphere_agent_2e7dc0::flipped_penetrations(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, penetrations_2e63a0 *collector)
{
	flipped_penetrations_2e63a0 flipped(collector);
	penetrations(b, a, input, &flipped);
}

// @retail 0x2e8d40
void multi_sphere_agent_2e7dc0::flipped_closest(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, points_2e63a0 *collector)
{
	flipped_points_2e63a0 flipped(collector);
	closest(b, a, input, &flipped);
}

// @retail 0x2e8e90
void multi_sphere_agent_2e7dc0::flipped_cast(body_2e63a0 const *a, body_2e63a0 const *b,
	cast_input_2e63a0 const *input, points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
{
	cast_input_2e63a0 reversed;
	reversed_cast_input_2e63a0(&reversed, input);
	flipped_cast_2e63a0 flipped_cast_collector;
	flipped_cast_init_2e63a0(&flipped_cast_collector, input->path, cast_collector);
	if (start_collector)
	{
		flipped_cast_2e63a0 flipped_start;
		flipped_cast_init_2e63a0(&flipped_start, input->path, start_collector);
		cast(b, a, &reversed, &flipped_cast_collector, &flipped_start);
	}
	else
	{
		cast(b, a, &reversed, &flipped_cast_collector, 0);
	}
}

// @retail 0x2e8da0
void multi_sphere_agent_2e7dc0::flipped_process(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, result_2e63a0 *result)
{
	long first = result->points->size;
	process(b, a, input, result);
	flip_normals_2e63a0(result, first);
}
