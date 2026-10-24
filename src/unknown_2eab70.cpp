// @flags /Od /Ob1 /GL- /GX
// Havok (third-party physics library): the unoptimised build (inline
// functions expanded, nothing else optimised) of two queries of one shape
// against a shape holding a tree of parts. The first is the same query as
// src/unknown_2ea410.cpp: the agents are brought up to date, then each runs
// its query against its part. The second gathers the keys of the parts that
// overlap the first shape's bounds and runs the dispatcher's query for each
// part the input's filter allows. Both stop when the collector says to, and
// each phase is marked in the timing stream.
//
// Their small helpers were not expanded, and the linker folded them onto game
// functions with the same bytes, so they are called by their game names.
#include "pair_agents_2eab70.h"
#include "timer_mark_2ebc80.h"
#include "side_bounds_2e9140.h"

typedef unsigned char byte;

// a one-byte truth value (src/unknown_183c60.cpp)
struct s_flag_byte
{
	byte value;
	bool is_set() const;
	s_flag_byte *set(byte new_value);
};

// the game's linked-node getter, whose bytes are those of the tree shape's
// container getter (src/unknown_183c60.cpp)
struct s_node
{
	byte unknown00[0xc];
	s_node *next;
	s_node *function_965e0() const;
};

// the game's flag copy, whose bytes are those of the collector's stop flag
// getter (src/unknown_1ce2e0.cpp)
struct c_class_1ce2e0
{
	byte unknown00[4];
	byte value;

	byte *copy_value(byte *output) const;
};

class c_allocator;
c_allocator *function_46220(void);
void *__fastcall function_31bc70(void *arg_0);

// whether the timing stream has room for another mark, as a flag built in a
// temporary (a temporary's byte sits right under the frame's saved registers,
// where retail keeps it; a named flag would sit 3 bytes lower)
struct room_2eab70 : s_flag_byte
{
	room_2eab70()
	{
		set((char *)g_480154 < g_480158);
	}
};

// adds a mark to the timing stream when there is room
#define TIMER_ADD_2EAB70(text) \
	{ \
		if (room_2eab70().is_set()) \
		{ \
			timer_mark_2ebc80 *mark = (timer_mark_2ebc80 *)g_480154; \
			mark->label = text; \
			unsigned long time; \
			__asm { rdtsc } \
			__asm { mov time, eax } \
			mark->time = time; \
			g_480154 = (char *)(mark + 1); \
		} \
	}

class shape_2eab70
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	// the shape's kind
	virtual long kind() const { return 0; }
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	// the keys of the parts that overlap the bounds
	virtual void overlapping_keys(bounds_2e9140 const *bounds, void *keys) const {}
	// a container's part, built in the buffer given when it must be
	virtual shape_2eab70 const *part(long key, char *buffer) const { return 0; }
};

// one side of a query: a shape, the key of the part concerned, where it sits,
// and the side it is a part of
struct body_2eab70
{
	shape_2eab70 const *shape;
	long key;
	void const *placement;
	body_2eab70 const *parent;

	void set_part(long part_key, shape_2eab70 const *part_shape)
	{
		shape = part_shape;
		key = part_key;
	}
};

// the shape a side holds
inline shape_2eab70 const *shape_of_2eab70(void const *side)
{
	return ((body_2eab70 const *)side)->shape;
}

// runs the queries of one part of a pair
class pair_agent_2eab70
{
public:
	virtual void slot_0() {}
	virtual void query(void const *a, void const *b, void const *input, void *collector) {}
};

struct entry_2eab70
{
	long key;
	long field_4;
	pair_agent_2eab70 *agent;
};

struct entries_2eab70
{
	entry_2eab70 *data;
	long size;

	entry_2eab70 *end() const
	{
		return data + size;
	}
};

struct agent_list_2eab70
{
	char unknown0[0xc];
	entries_2eab70 entries;
};

// @retail 0x2eab70
void class_2eab70::function_2eab70(void const *a, void const *b, void const *input, void *collector)
{
	TIMER_ADD_2EAB70("LtBvTree");
	((class_2e96a0 *)this)->function_2e96a0(a, b, input, function_2e90f0);
	TIMER_ADD_2EAB70("StNarrowPhase");
	entry_2eab70 *entry = ((agent_list_2eab70 *)this)->entries.data;
	entry_2eab70 *const end = ((agent_list_2eab70 *)this)->entries.end();
	body_2eab70 part;
	part.parent = (body_2eab70 const *)b;
	part.placement = ((body_2eab70 const *)b)->placement;
	s_node const *tree = (s_node const *)((body_2eab70 const *)b)->shape;
	shape_2eab70 const *container = (shape_2eab70 const *)tree->function_965e0();
	__declspec(align(16)) char buffer[0x100];
	while (entry != end)
	{
		shape_2eab70 const *child = container->part(entry->key, buffer);
		part.set_part(entry->key, child);
		entry->agent->query(a, &part, input, collector);
		byte stop;
		if (((s_flag_byte *)((c_class_1ce2e0 const *)collector)->copy_value(&stop))->is_set())
		{
			break;
		}
		entry++;
	}
	TIMER_ADD_2EAB70("lt");
}

typedef void (__cdecl *query_2ead90)(void const *a, void const *b, void const *input, void *collector);

// the queries for each pair of shape kinds
struct dispatcher_2ead90
{
	char unknown0[0x118c];
	query_2ead90 queries[32][32];
};

class filter_2ead90
{
public:
	// whether the part of the second side may meet the first side
#if H2_WHOLE_IMAGE
	/* an hkBool returned by value: through the hidden result pointer (x8 on
	   the port's ARM guest), as the filters' implementations return it */
	struct answer { char value; answer() {} };
	virtual answer allowed(void const *input, void const *a, void const *b, shape_2eab70 const *container, long key) const { return answer(); }
#else
	virtual byte *allowed(byte *out, void const *input, void const *a, void const *b, shape_2eab70 const *container, long key) const { return out; }
#endif
};

struct input_2ead90
{
	dispatcher_2ead90 *dispatcher;
	filter_2ead90 *filter;
};

class allocator_2ead90
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void release(void *block, long size, long kind) {}
};

inline void release_keys_2ead90(long *data, long count)
{
	allocator_2ead90 *allocator = (allocator_2ead90 *)function_46220();
	allocator->release(data, count * sizeof(long), 0x12);
}

// how many keys are kept in place
inline long keys_in_place_2ead90()
{
	return 0x80;
}

// up to 128 keys kept in place, more in a block from the allocator
struct keys_2ead90
{
	long *data;
	long size;
	unsigned long capacity_and_flags;
	long storage[0x80];

	keys_2ead90() : data(storage), size(0), capacity_and_flags(keys_in_place_2ead90() | 0x80000000) {}
	__forceinline ~keys_2ead90()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			release_keys_2ead90(data, capacity_and_flags & 0x7fffffff);
		}
	}
};

// @retail 0x2ead90
void __cdecl function_2ead90(void const *a, void const *b, void const *input, void *collector)
{
	TIMER_ADD_2EAB70("LtBvTree");
	bounds_2e9140 bounds;
	function_31bc70(&bounds);
	function_2e91c0(a, b, input, &bounds);
	shape_2eab70 const *tree = ((body_2eab70 const *)b)->shape;
	keys_2ead90 keys;
	tree->overlapping_keys(&bounds, &keys);
	TIMER_ADD_2EAB70("StNarrowPhase");
	long const kind_a = shape_of_2eab70(a)->kind();
	long *key = keys.data;
	long *const end = keys.data + keys.size;
	body_2eab70 part;
	part.parent = (body_2eab70 const *)b;
	part.placement = ((body_2eab70 const *)b)->placement;
	shape_2eab70 const *container = (shape_2eab70 const *)((s_node const *)tree)->function_965e0();
	__declspec(align(16)) char buffer[0x100];
	while (key != end)
	{
#if H2_WHOLE_IMAGE
		if (((input_2ead90 const *)input)->filter->allowed(input, a, b, container, *key).value)
#else
		byte allowed;
		if (((s_flag_byte *)((input_2ead90 const *)input)->filter->allowed(&allowed, input, a, b, container, *key))->is_set())
#endif
		{
			shape_2eab70 const *child = container->part(*key, buffer);
			part.set_part(*key, child);
			long const kind_b = child->kind();
			query_2ead90 query = ((input_2ead90 const *)input)->dispatcher->queries[kind_a][kind_b];
			query(a, &part, input, collector);
			byte stop;
			if (((s_flag_byte *)((c_class_1ce2e0 const *)collector)->copy_value(&stop))->is_set())
			{
				break;
			}
		}
		key++;
	}
	TIMER_ADD_2EAB70("lt");
}
