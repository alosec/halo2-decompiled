// @flags /O2 /GL- /GX
// Havok (third-party physics library): listing the pairs of ids of bodies
// that are joined: the bodies each joining object lists, paired with the
// first movable one; each constraint's two bodies; and each linked pair's two
// bodies; in every case only when both bodies are movable.
#include "growable_array_2d91d0.h"

// a body: whether it is fixed, and its id
struct body_317d90
{
	char unknown0[0x40];
	bool fixed;
	char unknown41[0x58 - 0x41];
	long id;
};

struct id_pair_317d90
{
	long first;
	long second;
};

ELEMENTS_2D91D0(id_pairs_317d90, id_pair_317d90);

// room for one more pair at the end
inline id_pair_317d90 *added_pair_317d90(id_pairs_317d90 *pairs)
{
	long size = pairs->size + 1;
	pairs->ensure_capacity(size);
	pairs->size = size;
	return &pairs->data[size - 1];
}

// a list of bodies that starts out in the room for ten held inside it
struct bodies_317d90
{
	body_317d90 **data;
	long size;
	long capacity_and_flags;
	body_317d90 *embedded[10];

	bodies_317d90()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x8000000a;
	}
	~bodies_317d90()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(body_317d90 *), 0x12);
		}
	}
};

// an object joining bodies, which lists them through its seventh slot
class joint_317d90
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void list_bodies(bodies_317d90 *bodies) {}
};

struct constraint_317d90
{
	char unknown0[0xc];
	body_317d90 *first;
	body_317d90 *second;
};

struct end_317d90
{
	char unknown0[0x10];
	body_317d90 *body;
};

struct link_317d90
{
	end_317d90 *first;
	end_317d90 *second;
	char unknown8[8];
};

class class_317d90
{
public:
	char unknown0[0x4c];
	joint_317d90 **joints;
	long joint_count;
	char unknown54[4];
	constraint_317d90 **constraints;
	long constraints_used;
	char unknown60[8];
	link_317d90 *links;
	long link_count;

	void function_317d90(id_pairs_317d90 *pairs) const;
};

// score 59 (was 105): retail loads the joint count into a register before
// comparing it, ours compares the member in place; retail compares the found
// index as a full word, ours as a sixteen bit value, so ours sign extends it
// when it indexes the bodies
// @retail 0x317d90
void class_317d90::function_317d90(id_pairs_317d90 *pairs) const
{
	pairs->size = 0;
	long i;
	{
		bodies_317d90 bodies;
		for (i = 0; i < joint_count; i++)
		{
			bodies.size = 0;
			joints[i]->list_bodies(&bodies);
			short first = -1;
			long j = 0;
			for (; first == -1 && j < bodies.size; j++)
			{
				if (!bodies.data[j]->fixed)
				{
					first = j;
				}
			}
			for (; j < bodies.size; j++)
			{
				if (!bodies.data[j]->fixed)
				{
					id_pair_317d90 *pair = added_pair_317d90(pairs);
					pair->first = bodies.data[first]->id;
					pair->second = bodies.data[j]->id;
				}
			}
		}
	}
	for (i = 0; i < constraints_used; i++)
	{
		if (!constraints[i]->first->fixed && !constraints[i]->second->fixed)
		{
			id_pair_317d90 *pair = added_pair_317d90(pairs);
			pair->first = constraints[i]->first->id;
			pair->second = constraints[i]->second->id;
		}
	}
	for (i = 0; i < link_count; i++)
	{
		body_317d90 *first = links[i].first->body;
		body_317d90 *second = links[i].second->body;
		if (!first->fixed && !second->fixed)
		{
			id_pair_317d90 *pair = added_pair_317d90(pairs);
			pair->first = first->id;
			pair->second = second->id;
		}
	}
}
