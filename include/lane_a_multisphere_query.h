#ifndef LANE_A_MULTISPHERE_QUERY_H
#define LANE_A_MULTISPHERE_QUERY_H
#include "unknown_1efac0.h"
#include "child_query_2f9950.h"
#include "pair_agent_2ed170.h"
#include "physics_allocator.h"

// Same external constructor and prefix used by unknown_183910.cpp.
class c_2def90 : public c_a
{
public:
	c_2def90(real radius_);
	virtual long type();
	long user_value;
	real radius;
};

// The six-vector motion state already modelled in pair_agent_2ed170.h.
// A multisphere shape has its count at +0xc and sphere vectors at +0x10.
struct lane_a_multisphere : c_a
{
	long user_value;
	long sphere_count;
	__m128 spheres[8];
};

// The same child-agent interface used by unknown_2ea410.cpp. These view
// methods are only used through an existing object's virtual dispatch.
class lane_a_child_agent
{
public:
	virtual void slot_0() {}
	virtual void slot_1(void const *, void const *, void const *, void *) {}
	virtual void slot_2(void const *, void const *, void const *, void *) {}
	virtual void slot_3(void const *, void const *, void const *, void *, void *) {}
	virtual void slot_4(void const *, void const *, void const *, void *) {}
	virtual void slot_5() {}
	virtual void slot_6(bool *) {}
	virtual void slot_7(void const *, void const *, void const *) {}
};

struct lane_a_sphere_entry
{
	long sphere_index;
	lane_a_child_agent *agent;
};

struct lane_a_dispatch;
struct lane_a_entry_storage
{
	lane_a_sphere_entry *entries;
	long entry_count;
	long capacity_and_flags;
	lane_a_sphere_entry embedded[4];

	lane_a_entry_storage() : entries(embedded), entry_count(0), capacity_and_flags(0x80000004) {}
	~lane_a_entry_storage()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
			havok_allocator()->release(entries, (capacity_and_flags & 0x7fffffff) * sizeof(lane_a_sphere_entry), 0x12);
	}
};
// Agent construction preserves the allocation header; unlike the normal
// reference constructor it does not initialize the count (retail 2e8a40).
class lane_a_agent_reference
{
public:
	unsigned short memory_size;
	unsigned short reference_count;
	virtual ~lane_a_agent_reference() {}
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object, ((lane_a_agent_reference *)object)->memory_size, 0x10);
	}
};
class lane_a_agent_base : public lane_a_agent_reference
{
public:
	static void *operator new(unsigned int size)
	{
		lane_a_agent_base *object = (lane_a_agent_base *)g_memory_2d88a0->allocate_block(size, 0x1a);
		object->memory_size = (unsigned short)size;
		return object;
	}
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object, ((lane_a_agent_base *)object)->memory_size, 0x1a);
	}
	void *contact_manager;
	lane_a_agent_base(void *manager) : contact_manager(manager) {}
};
class lane_a_multisphere_agent : public lane_a_agent_base
{
public:
	lane_a_multisphere_agent(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager);
	virtual void function_2e8560(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void function_2e7f30(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void function_2e8240(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector);
	virtual void function_2e7dc0(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void cleanup();
	virtual void is_symmetric(bool *result) { *result = false; }
	virtual void update_filter(body_2ea940 const *, void const *, void const *) {}

	lane_a_entry_storage agents;
};

// A query side for a sphere belonging to its parent shape.
struct lane_a_sphere_body : body_2ea940
{
	lane_a_sphere_body(body_2ea940 const *parent_, void const *placement_)
	{
		parent = parent_;
		placement = placement_;
	}
};

// Two vector operations: rotate a direction, then out = a + b.
inline void lane_a_rotate(__m128 &out, s_extent_transform const &rotation, __m128 const &point)
{
	__m128 x = _mm_mul_ps(rotation.rows[0], _mm_shuffle_ps(point, point, 0));
	__m128 y = _mm_mul_ps(rotation.rows[1], _mm_shuffle_ps(point, point, 0x55));
	__m128 z = _mm_mul_ps(rotation.rows[2], _mm_shuffle_ps(point, point, 0xaa));
	out = _mm_add_ps(_mm_add_ps(x, y), z);
}

inline void lane_a_set_add(__m128 &out, __m128 const &a, __m128 const &b)
{
	out = _mm_add_ps(a, b);
}

struct lane_a_stop_collector
{
	void const *vtable;
	bool early_out;
	bool get_early_out() const { return early_out; }
};
#endif
