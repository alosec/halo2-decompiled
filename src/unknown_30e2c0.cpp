// @flags /O2 /GL- /GX
// Havok (third-party physics library): a world taking an entity out of its
// island, dropping the island when it is left empty, and flushing the pending
// list; and a world taking in an object that joins entities together, which
// merges the islands of the entities it joins.
#include "growable_array_2d91d0.h"

bool __cdecl function_279740(long value);

class entity_316000;
struct member_316d60;
struct listener_world_315d20;
class c_havok_body_group;

void __cdecl function_318010(listener_world_315d20 *world, void *event);

class deletable_30e2c0
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

class island_316000
{
public:
	char unknown0[0x24];
	long index;
	char unknown28;
	bool active;
	char unknown2a[0x40 - 0x2a];
	long field_40;

	void function_316000(entity_316000 *entity);
};

class class_316c50
{
public:
	void function_316d60(member_316d60 *member);
};

class hkWorld
{
public:
	void function_30ccd0(c_havok_body_group *island);
};

class class_30d160
{
public:
	void function_30d160(long first, long second);
};

class class_30d320
{
public:
	void function_30d400();
};

// an entity: the island it belongs to at +0x44
struct entity_30e2c0
{
	char unknown0[8];
	long field_8;
	char unknownc[0x44 - 0xc];
	island_316000 *island;
};

class world_30e2c0;

// an object joining entities: it lists them through its seventh slot
class joint_30e3b0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void entities(void *list) {}

	unsigned short memory_size;
	unsigned short reference_count;
	long field_8;
	world_30e2c0 *world;
};

ELEMENTS_2D91D0(islands_30e2c0, island_316000 *);
ELEMENTS_2D91D0(pointers_30e2c0, void *);

// a list of entities that starts out in the room for four held inside it
struct entities_30e3b0
{
	entity_30e2c0 **data;
	int size;
	int capacity_and_flags;
	entity_30e2c0 *embedded[4];

	entities_30e3b0()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x80000004;
	}
	~entities_30e3b0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(entity_30e2c0 *), 0x12);
		}
	}
};

class world_30e2c0
{
public:
	char unknown0[8];
	islands_30e2c0 active_islands;
	islands_30e2c0 inactive_islands;
	char unknown20[0x114 - 0x20];
	pointers_30e2c0 pending_114;

	void function_30e2c0(entity_316000 *entity);
	joint_30e3b0 *function_30e3b0(joint_30e3b0 *joint);
};

// takes the entity out of its island; an island left empty is taken out of
// its list and deleted. Then each pending object is run and let go
// @retail 0x30e2c0
void world_30e2c0::function_30e2c0(entity_316000 *entity)
{
	island_316000 *island = ((entity_30e2c0 *)entity)->island;
	((entity_30e2c0 *)entity)->field_8 = 0;
	island->function_316000(entity);
	if (island->index != -1 && island->field_40 == 0)
	{
		if (!island->active)
		{
			inactive_islands.data[island->index] = inactive_islands.data[inactive_islands.size - 1];
			inactive_islands.data[island->index]->index = island->index;
			inactive_islands.size--;
		}
		else
		{
			active_islands.data[island->index] = active_islands.data[active_islands.size - 1];
			active_islands.data[island->index]->index = island->index;
			active_islands.size--;
		}
		((deletable_30e2c0 *)island)->destroy(1);
	}
	for (int i = 0; i < pending_114.size; i++)
	{
		((class_30d320 *)pending_114.data[i])->function_30d400();
		referenced_2d88a0 *object = (referenced_2d88a0 *)pending_114.data[i];
		object->reference_count--;
		if (object->reference_count == 0)
		{
			if (function_279740((long)object))
			{
				object->reference_count = 1;
			}
			else
			{
				((deletable_30e2c0 *)object)->destroy(1);
			}
		}
	}
	pending_114.size = 0;
}

// the first linked entity's island takes the joint, and is woken when it
// sleeps; each other entity in another island has that island merged in
// @retail 0x30e3b0
joint_30e3b0 *world_30e2c0::function_30e3b0(joint_30e3b0 *joint)
{
	joint->reference_count++;
	entities_30e3b0 entities;
	joint->entities(&entities);
	joint->world = this;
	entity_30e2c0 *first = 0;
	for (int i = 0; i < entities.size; i++)
	{
		entity_30e2c0 *entity = entities.data[i];
		island_316000 *island = entity->island;
		bool unlinked = island->index == -1;
		if (!unlinked)
		{
			if (first == 0)
			{
				first = entity;
				((class_316c50 *)island)->function_316d60((member_316d60 *)joint);
				if (!island->active)
				{
					((hkWorld *)this)->function_30ccd0((c_havok_body_group *)island);
				}
			}
			else if (first->island != island)
			{
				((class_30d160 *)this)->function_30d160((long)first, (long)entity);
			}
		}
	}
	function_318010((listener_world_315d20 *)this, joint);
	return joint;
}
