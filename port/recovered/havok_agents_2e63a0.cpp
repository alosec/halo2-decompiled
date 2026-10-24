// @flags /O2 /GL-
// Havok (third-party physics library): the collision agents for two kinds of
// shapes that hold other shapes, and three small pieces of the dispatcher:
//   0x2e5fa0  the destructor of the dispatcher's array of registrations
//             (0x1c-byte elements)
//   0x2e6220  the name of a shape type ("HK_SHAPE_SPHERE", ...; -1 is
//             "HK_SHAPE_ALL", 0 and 0x14 are "unknown")
//   0x2e6380, 0x2e6390  the destructors of the two collector bases (the
//             point collectors' vtable 0x453b70, the penetration
//             collectors' 0x455580), run by their deleting destructors
//             0x2e7a80 and 0x2e7a50
//
// The wrapper agent (type 0x14, a shape whose one child sits at +0xc in the
// same place): 0x10 bytes {vtable, size, manager +8, the child's agent +0xc}.
// Its queries pass the child on in the wrapper's place.
//   vtable 0x412988 (made for (0x14, B)): 0x2f4380, 0x2e6450 penetrations,
//     0x2e6510 closest, 0x2e65d0 cast, 0x2e6690 process, 0x2ecc60 cleanup,
//     0x2fe820, 0x2e6740 update
//   vtable 0x4129a8 (made for (A, 0x14), the sides swapped): 0x2f4380,
//     0x2e6840, 0x2e68c0, 0x2e6b20, 0x2e6950, 0x2ecc60, 0x31bc80, 0x2e69e0
//   handlers (0x2e6cd0 registers them): (0x14, any) 0x2e66e0 create,
//     0x2e64a0, 0x2e6560, 0x2e6620; (any, 0x14) 0x2e6c60 create, 0x2e6780,
//     0x2e67e0, 0x2e6a20
//
// The bounding volume agent (type 0x11, HK_SHAPE_BV: a bounding shape at +0xc
// and the child it bounds at +0x10): 0x14 bytes {vtable, size, manager +8,
// the bounding shape's agent +0xc, the child's agent +0x10, made the first
// time the bounding shape overlaps and dropped again when it stops}. Only
// while the bounding shape overlaps the other side is the child queried.
//   vtable 0x4129dc (made for (0x11, B)): 0x2f4380, 0x2e7670 penetrations
//     (of the bounding shape only), 0x2e7370 closest, 0x2e7030 cast,
//     0x2e6e90 process, 0x2e6d40 cleanup, 0x2fe820, 0x2e77d0 update
//   vtable 0x412a30 (made for (A, 0x11)): 0x2f4380, 0x2e7910, 0x2e7970,
//     0x2e7bb0, 0x2e79d0, 0x2e6d40, 0x31bc80, 0x2e7a30
//   handlers (0x2e7d10 registers them): (0x11, any) 0x2e6e30 create,
//     0x2e7710, 0x2e7510, 0x2e71f0; (any, 0x11) 0x2e7ca0 create, 0x2e7850,
//     0x2e78b0, 0x2e7ab0
// Their queries are timed under "hkBvAgent" in the timing stream.
//
// The agents made for (A, B) with the special shape second run the (B, A)
// queries with the sides swapped, through collectors that flip what they
// pass on (havok_agent_parts_2e63a0.h); a cast also reverses its path, and
// processing flips the normals of the contact points it added.
#include "havok_agent_parts_2e63a0.h"

// ---------------------------------------------------------------------------
// the dispatcher's pieces

struct registrations_2e5fa0
{
	void *data;
	long size;
	long capacity_and_flags;

	void destroy();
};

// @retail 0x2e5fa0
void registrations_2e5fa0::destroy()
{
	if (capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * 0x1c, 0x12);
	}
}

// @retail 0x2e6220
char const *__cdecl function_2e6220(long type)
{
	switch (type)
	{
	case -1: return "HK_SHAPE_ALL";
	case 1: return "HK_SHAPE_CONVEX";
	case 2: return "HK_SHAPE_COLLECTION";
	case 3: return "HK_SHAPE_BV_TREE";
	case 4: return "HK_SHAPE_SPHERE";
	case 5: return "HK_SHAPE_TRIANGLE";
	case 6: return "HK_SHAPE_BOX";
	case 7: return "HK_SHAPE_CAPSULE";
	case 8: return "HK_SHAPE_CONVEX_VERTICES";
	case 9: return "HK_SHAPE_MULTI_SPHERE";
	case 10: return "HK_SHAPE_LIST";
	case 11: return "HK_SHAPE_TRIANGLE_COLLECTION";
	case 12: return "HK_SHAPE_MULTI_RAY";
	case 13: return "HK_SHAPE_HEIGHT_FIELD";
	case 14: return "HK_SHAPE_SAMPLED_HEIGHT_FIELD";
	case 15: return "HK_SHAPE_TRI_PATCH";
	case 16: return "HK_SHAPE_SPHERE_REP";
	case 17: return "HK_SHAPE_BV";
	case 18: return "HK_SHAPE_PLANE";
	case 19: return "HK_SHAPE_MOPP";
	case 21: return "HK_SHAPE_TRANSFORM";
	case 22: return "HK_SHAPE_PHANTOM_CALLBACK";
	case 23: return "HK_SHAPE_UTILITY_CONVEX_SWEEP";
	case 24: return "HK_SHAPE_USER0";
	case 25: return "HK_SHAPE_USER1";
	case 26: return "HK_SHAPE_USER2";
	case 27: return "HK_SHAPE_USER3";
	case 28: return "HK_SHAPE_USER4";
	case 29: return "HK_SHAPE_USER5";
	case 30: return "HK_SHAPE_USER6";
	case 31: return "HK_SHAPE_USER7";
	default: return "unknown";
	}
}

struct points_base_2e6380
{
	void const *vtable;

	void destroy();
};

// @retail 0x2e6380
void points_base_2e6380::destroy()
{
	vtable = (void const *)0x453b70;
}

struct penetrations_base_2e6390
{
	void const *vtable;

	void destroy();
};

// @retail 0x2e6390
void penetrations_base_2e6390::destroy()
{
	vtable = (void const *)0x455580;
}

// ---------------------------------------------------------------------------
// the wrapper agent

struct wrapper_shape_2e63a0
{
	char unknown0[0xc];
	void const *child;
};

class wrapper_agent_2e63a0
{
public:
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void *manager;
	agent_2e63a0 *child;

	wrapper_agent_2e63a0 *construct(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		void *manager_);

	void penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		penetrations_2e63a0 *collector);
	void closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input, points_2e63a0 *collector);
	void cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
		points_2e63a0 *cast_collector, points_2e63a0 *start_collector);
	void process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input, result_2e63a0 *result);
	void update(body_2e63a0 const *a, body_2e63a0 const *b, void const *input);

	// made for (A, 0x14)
	void flipped_penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		penetrations_2e63a0 *collector);
	void flipped_closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		points_2e63a0 *collector);
	void flipped_cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
		points_2e63a0 *cast_collector, points_2e63a0 *start_collector);
	void flipped_process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		result_2e63a0 *result);
	void flipped_update(body_2e63a0 const *a, body_2e63a0 const *b, void const *input);
};

typedef char assert_wrapper_agent_2e63a0[(sizeof(wrapper_agent_2e63a0) == 0x10) ? 1 : -1];

static __forceinline void wrapped_body_2e63a0(body_2e63a0 *out, body_2e63a0 const *side)
{
	child_body_2e63a0(out, side, ((wrapper_shape_2e63a0 const *)side->shape)->child);
}

// @retail 0x2e63a0
wrapper_agent_2e63a0 *wrapper_agent_2e63a0::construct(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, void *manager_)
{
	manager = manager_;
	vtable = (void const *)0x412988;
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, a);
	child = create_agent_2e63a0(&body, b, input, manager_);
	return this;
}

// @retail 0x2e6450
void wrapper_agent_2e63a0::penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, a);
	child->penetrations(&body, b, input, collector);
}

// @retail 0x2e6510
void wrapper_agent_2e63a0::closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, a);
	child->closest(&body, b, input, collector);
}

// @retail 0x2e65d0
void wrapper_agent_2e63a0::cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
	points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
{
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, a);
	child->cast(&body, b, input, cast_collector, start_collector);
}

// @retail 0x2e6690
void wrapper_agent_2e63a0::process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	result_2e63a0 *result)
{
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, a);
	child->process(&body, b, input, result);
}

// @retail 0x2e6740
void wrapper_agent_2e63a0::update(body_2e63a0 const *a, body_2e63a0 const *b, void const *input)
{
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, a);
	child->update(&body, b, input);
}

// the static queries for (0x14, any): the other side's type is asked first
// @retail 0x2e64a0
void __cdecl function_2e64a0(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, a);
	long second_type = b->shape->type();
	long first_type = body.shape->type();
	input->dispatcher->penetrations[first_type][second_type](&body, b, input, collector);
}

// @retail 0x2e6560
void __cdecl function_2e6560(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, a);
	long second_type = b->shape->type();
	long first_type = body.shape->type();
	input->dispatcher->closest[first_type][second_type](&body, b, input, collector);
}

// @retail 0x2e6620
void __cdecl function_2e6620(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
	points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
{
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, a);
	long second_type = b->shape->type();
	long first_type = body.shape->type();
	input->dispatcher->cast[first_type][second_type](&body, b, input, cast_collector, start_collector);
}

// @retail 0x2e66e0
agent_2e63a0 *__cdecl function_2e66e0(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	void *manager)
{
	wrapper_agent_2e63a0 *agent = (wrapper_agent_2e63a0 *)g_memory_2d88a0->allocate_block(0x10, 0x1a);
	agent->memory_size = 0x10;
	return (agent_2e63a0 *)agent->construct(a, b, input, manager);
}

// the static queries for (any, 0x14): the (0x14, any) ones with the sides
// swapped
// @retail 0x2e6780
void __cdecl function_2e6780(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	flipped_penetrations_2e63a0 flipped(collector);
	function_2e64a0(b, a, input, &flipped);
}

// @retail 0x2e67e0
void __cdecl function_2e67e0(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	flipped_points_2e63a0 flipped(collector);
	function_2e6560(b, a, input, &flipped);
}

// @retail 0x2e6a20
void __cdecl function_2e6a20(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
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
		function_2e6620(b, a, &reversed, &flipped_cast, &flipped_start);
	}
	else
	{
		function_2e6620(b, a, &reversed, &flipped_cast, 0);
	}
}

// @retail 0x2e6c60
agent_2e63a0 *__cdecl function_2e6c60(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	void *manager)
{
	wrapper_agent_2e63a0 *agent = (wrapper_agent_2e63a0 *)g_memory_2d88a0->allocate_block(0x10, 0x1a);
	agent->memory_size = 0x10;
	agent->construct(b, a, input, manager);
	agent->vtable = (void const *)0x4129a8;
	return (agent_2e63a0 *)agent;
}

// the agent made for (A, 0x14): its child was made for (the wrapper's child,
// A)
// @retail 0x2e6840
void wrapper_agent_2e63a0::flipped_penetrations(body_2e63a0 const *a, body_2e63a0 const *b,
	input_2e63a0 const *input, penetrations_2e63a0 *collector)
{
	flipped_penetrations_2e63a0 flipped(collector);
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, b);
	child->penetrations(&body, a, input, &flipped);
}

// @retail 0x2e68c0
void wrapper_agent_2e63a0::flipped_closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	flipped_points_2e63a0 flipped(collector);
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, b);
	child->closest(&body, a, input, &flipped);
}

// @retail 0x2e6b20
void wrapper_agent_2e63a0::flipped_cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
	points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
{
	cast_input_2e63a0 reversed;
	reversed_cast_input_2e63a0(&reversed, input);
	flipped_cast_2e63a0 flipped_cast_collector;
	flipped_cast_init_2e63a0(&flipped_cast_collector, input->path, cast_collector);
	body_2e63a0 body;
	if (start_collector)
	{
		flipped_cast_2e63a0 flipped_start;
		flipped_cast_init_2e63a0(&flipped_start, input->path, start_collector);
		wrapped_body_2e63a0(&body, b);
		child->cast(&body, a, &reversed, &flipped_cast_collector, &flipped_start);
	}
	else
	{
		wrapped_body_2e63a0(&body, b);
		child->cast(&body, a, &reversed, &flipped_cast_collector, 0);
	}
}

// @retail 0x2e6950
void wrapper_agent_2e63a0::flipped_process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	result_2e63a0 *result)
{
	long first = result->points->size;
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, b);
	child->process(&body, a, input, result);
	flip_normals_2e63a0(result, first);
}

// @retail 0x2e69e0
void wrapper_agent_2e63a0::flipped_update(body_2e63a0 const *a, body_2e63a0 const *b, void const *input)
{
	body_2e63a0 body;
	wrapped_body_2e63a0(&body, b);
	child->update(&body, a, input);
}

// ---------------------------------------------------------------------------
// the bounding volume agent

struct bv_shape_2e6d70
{
	char unknown0[0xc];
	void const *bounding;
	void const *child;
};

// the closest hit of a cast (vtable 0x4129d4: 0x2e7a80, 0x301a00 keeps the
// nearest), here only asked whether there was one
struct __declspec(align(16)) closest_hit_2e7030 : points_2e63a0
{
	bool has_hit;
	__m128 position;
	__m128 normal;

	closest_hit_2e7030()
	{
		vtable = (void const *)0x4129d4;
		has_hit = false;
		position.m128_f32[3] = 3.40282347e+38f;
		early_out = 3.40282347e+38f;
	}
};

typedef char assert_closest_hit_2e7030[(__builtin_offsetof(closest_hit_2e7030, position) == 0x10) ? 1 : -1];

// whether there is any penetration (vtable 0x4129cc: 0x2e7a50, 0x3019f0
// sets the flag)
struct overlap_2e6e90 : penetrations_2e63a0
{
	overlap_2e6e90()
	{
		vtable = (void const *)0x4129cc;
		found = false;
	}
};

class bv_agent_2e6d70
{
public:
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void *manager;
	agent_2e63a0 *bounding_agent;
	agent_2e63a0 *child_agent;

	bv_agent_2e6d70 *construct(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		void *manager_);
	void cleanup();

	void penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		penetrations_2e63a0 *collector);
	void closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input, points_2e63a0 *collector);
	void cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
		points_2e63a0 *cast_collector, points_2e63a0 *start_collector);
	void process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input, result_2e63a0 *result);
	void update(body_2e63a0 const *a, body_2e63a0 const *b, void const *input);

	// made for (A, 0x11)
	void flipped_penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		penetrations_2e63a0 *collector);
	void flipped_closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		points_2e63a0 *collector);
	void flipped_cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
		points_2e63a0 *cast_collector, points_2e63a0 *start_collector);
	void flipped_process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
		result_2e63a0 *result);
	void flipped_update(body_2e63a0 const *a, body_2e63a0 const *b, void const *input);
};

typedef char assert_bv_agent_2e6d70[(sizeof(bv_agent_2e6d70) == 0x14) ? 1 : -1];

static __forceinline bv_shape_2e6d70 const *bv_shape_of_2e6d70(body_2e63a0 const *side)
{
	return (bv_shape_2e6d70 const *)side->shape;
}

// the side's bounding shape in its place
static __forceinline void bounding_body_2e6d70(body_2e63a0 *out, body_2e63a0 const *side)
{
	child_body_2e63a0(out, side, bv_shape_of_2e6d70(side)->bounding);
}

// the same body moved on to the bounded child
static __forceinline void bounded_body_2e6d70(body_2e63a0 *body, body_2e63a0 const *side)
{
	body->key = side->key;
	body->shape = (shape_2e63a0 const *)bv_shape_of_2e6d70(side)->child;
}

// @retail 0x2e6d70
bv_agent_2e6d70 *bv_agent_2e6d70::construct(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	void *manager_)
{
	manager = manager_;
	vtable = (void const *)0x4129dc;
	body_2e63a0 body;
	bounding_body_2e6d70(&body, a);
	bounding_agent = create_agent_2e63a0(&body, b, input, manager_);
	child_agent = 0;
	return this;
}

// @retail 0x2e6d40
void bv_agent_2e6d70::cleanup()
{
	bounding_agent->cleanup();
	if (child_agent)
	{
		child_agent->cleanup();
		child_agent = 0;
	}
	((agent_2e63a0 *)this)->destroy(1);
}

// @retail 0x2e6e90
void bv_agent_2e6d70::process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	result_2e63a0 *result)
{
	timer_add_2ebc80("LthkBvAgent");
	body_2e63a0 body;
	bounding_body_2e6d70(&body, a);
	overlap_2e6e90 overlap;
	bounding_agent->penetrations(&body, b, input, &overlap);
	if (overlap.found)
	{
		timer_add_2ebc80("Stchild");
		bounded_body_2e6d70(&body, a);
		if (!child_agent)
		{
			child_agent = create_agent_2e63a0(&body, b, input, manager);
		}
		child_agent->process(&body, b, input, result);
	}
	else if (child_agent)
	{
		child_agent->cleanup();
		child_agent = 0;
	}
	timer_add_2ebc80("lt");
}

// @retail 0x2e7030
void bv_agent_2e6d70::cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
	points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
{
	timer_add_2ebc80("LthkBvAgent");
	body_2e63a0 body;
	bounding_body_2e6d70(&body, a);
	closest_hit_2e7030 hit;
	bounding_agent->cast(&body, b, input, &hit, &hit);
	if (hit.has_hit)
	{
		timer_add_2ebc80("Stchild");
		bounded_body_2e6d70(&body, a);
		if (!child_agent)
		{
			child_agent = create_agent_2e63a0(&body, b, (input_2e63a0 const *)input, manager);
		}
		child_agent->cast(&body, b, input, cast_collector, start_collector);
	}
	else if (child_agent)
	{
		child_agent->cleanup();
		child_agent = 0;
	}
	timer_add_2ebc80("lt");
}

// @retail 0x2e7370
void bv_agent_2e6d70::closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	timer_add_2ebc80("LthkBvAgent");
	body_2e63a0 body;
	bounding_body_2e6d70(&body, a);
	overlap_2e6e90 overlap;
	bounding_agent->penetrations(&body, b, input, &overlap);
	if (overlap.found)
	{
		timer_add_2ebc80("Stchild");
		bounded_body_2e6d70(&body, a);
		if (!child_agent)
		{
			child_agent = create_agent_2e63a0(&body, b, input, manager);
		}
		child_agent->closest(&body, b, input, collector);
	}
	else if (child_agent)
	{
		child_agent->cleanup();
		child_agent = 0;
	}
	timer_add_2ebc80("lt");
}

// @retail 0x2e7670
void bv_agent_2e6d70::penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	timer_add_2ebc80("TthkBvAgent");
	body_2e63a0 body;
	bounding_body_2e6d70(&body, a);
	bounding_agent->penetrations(&body, b, input, collector);
	timer_add_2ebc80("Et");
}

// @retail 0x2e77d0
void bv_agent_2e6d70::update(body_2e63a0 const *a, body_2e63a0 const *b, void const *input)
{
	body_2e63a0 body;
	bounding_body_2e6d70(&body, a);
	bounding_agent->update(&body, b, input);
	if (child_agent)
	{
		bounded_body_2e6d70(&body, a);
		child_agent->update(&body, b, input);
	}
}

// the static queries for (0x11, any)
// @retail 0x2e71f0
void __cdecl function_2e71f0(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
	points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
{
	timer_add_2ebc80("LthkBvAgent");
	body_2e63a0 body;
	bounding_body_2e6d70(&body, a);
	long first_type = body.shape->type();
	long second_type = b->shape->type();
	cast_query_2e63a0 query = input->dispatcher->cast[first_type][second_type];
	closest_hit_2e7030 hit;
	query(&body, b, input, &hit, &hit);
	if (hit.has_hit)
	{
		timer_add_2ebc80("Stchild");
		bounded_body_2e6d70(&body, a);
		long child_type = body.shape->type();
		input->dispatcher->cast[child_type][second_type](&body, b, input, cast_collector, start_collector);
	}
	timer_add_2ebc80("lt");
}

// @retail 0x2e7510
void __cdecl function_2e7510(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	timer_add_2ebc80("LthkBvAgent");
	body_2e63a0 body;
	bounding_body_2e6d70(&body, a);
	long first_type = body.shape->type();
	long second_type = b->shape->type();
	penetrations_query_2e63a0 query = input->dispatcher->penetrations[first_type][second_type];
	overlap_2e6e90 overlap;
	query(&body, b, input, &overlap);
	if (overlap.found)
	{
		timer_add_2ebc80("Stchild");
		bounded_body_2e6d70(&body, a);
		long child_type = body.shape->type();
		input->dispatcher->closest[child_type][second_type](&body, b, input, collector);
	}
	timer_add_2ebc80("lt");
}

// @retail 0x2e7710
void __cdecl function_2e7710(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	timer_add_2ebc80("TthkBvAgent");
	body_2e63a0 body;
	bounding_body_2e6d70(&body, a);
	long first_type = body.shape->type();
	long second_type = b->shape->type();
	input->dispatcher->penetrations[first_type][second_type](&body, b, input, collector);
	timer_add_2ebc80("Et");
}

// @retail 0x2e6e30
agent_2e63a0 *__cdecl function_2e6e30(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	void *manager)
{
	bv_agent_2e6d70 *agent = (bv_agent_2e6d70 *)g_memory_2d88a0->allocate_block(0x14, 0x1a);
	agent->memory_size = 0x14;
	return (agent_2e63a0 *)agent->construct(a, b, input, manager);
}

// the static queries for (any, 0x11)
// @retail 0x2e7850
void __cdecl function_2e7850(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	flipped_penetrations_2e63a0 flipped(collector);
	function_2e7710(b, a, input, &flipped);
}

// @retail 0x2e78b0
void __cdecl function_2e78b0(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	flipped_points_2e63a0 flipped(collector);
	function_2e7510(b, a, input, &flipped);
}

// @retail 0x2e7ab0
void __cdecl function_2e7ab0(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
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
		function_2e71f0(b, a, &reversed, &flipped_cast, &flipped_start);
	}
	else
	{
		function_2e71f0(b, a, &reversed, &flipped_cast, 0);
	}
}

// @retail 0x2e7ca0
agent_2e63a0 *__cdecl function_2e7ca0(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	void *manager)
{
	bv_agent_2e6d70 *agent = (bv_agent_2e6d70 *)g_memory_2d88a0->allocate_block(0x14, 0x1a);
	agent->memory_size = 0x14;
	agent->construct(b, a, input, manager);
	agent->vtable = (void const *)0x412a30;
	return (agent_2e63a0 *)agent;
}

// the agent made for (A, 0x11): its own (0x11, A) methods with the sides
// swapped
// @retail 0x2e7910
void bv_agent_2e6d70::flipped_penetrations(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	penetrations_2e63a0 *collector)
{
	flipped_penetrations_2e63a0 flipped(collector);
	penetrations(b, a, input, &flipped);
}

// @retail 0x2e7970
void bv_agent_2e6d70::flipped_closest(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	points_2e63a0 *collector)
{
	flipped_points_2e63a0 flipped(collector);
	closest(b, a, input, &flipped);
}

// @retail 0x2e7bb0
void bv_agent_2e6d70::flipped_cast(body_2e63a0 const *a, body_2e63a0 const *b, cast_input_2e63a0 const *input,
	points_2e63a0 *cast_collector, points_2e63a0 *start_collector)
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

// @retail 0x2e79d0
void bv_agent_2e6d70::flipped_process(body_2e63a0 const *a, body_2e63a0 const *b, input_2e63a0 const *input,
	result_2e63a0 *result)
{
	long first = result->points->size;
	process(b, a, input, result);
	flip_normals_2e63a0(result, first);
}

// @retail 0x2e7a30
void bv_agent_2e6d70::flipped_update(body_2e63a0 const *a, body_2e63a0 const *b, void const *input)
{
	update(b, a, input);
}
