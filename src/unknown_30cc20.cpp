// @flags /O2 /GL-
// Havok (third-party physics library): a world's lists: moving an island
// between the active and inactive lists, keeping each island's index in its
// list, adding referenced entries to the pending lists, and flushing the
// pending lists, dropping their references.
#include "growable_array_2d91d0.h"
#include "reference_base_318970.h"

class hkBool
{
public:
	hkBool() {}

	char m_bool;
};

struct world_318540;
struct group_318540;
void __cdecl function_318540(world_318540 *world, group_318540 *group);
void __cdecl function_318620(world_318540 *world, group_318540 *group);
bool __cdecl function_279740(long value);

class deletable_30cc20
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

// drops a reference, deleting the object with the last one; objects that
// came with the cache file are never deleted, their count goes back to 1
static __forceinline void drop_reference_30cc20(void *pointer)
{
	referenced_2d88a0 *object = (referenced_2d88a0 *)pointer;
	object->reference_count--;
	if (object->reference_count == 0)
	{
		if (function_279740((long)object))
		{
			object->reference_count = 1;
		}
		else
		{
			// the deleting destructor, first in the vtable, without the null
			// test `delete` adds
			((deletable_30cc20 *)object)->destroy(1);
		}
	}
}

class c_havok_body_group
{
public:
	char unknown0[0x24];
	long index;
	char unknown28;
	bool active;
	char unknown2a[0x30 - 0x2a];
	bool field_30;
	char unknown31[0x34 - 0x31];
	long field_34;
	long field_38;
};

ELEMENTS_2D91D0(islands_30cc20, c_havok_body_group *);
ELEMENTS_2D91D0(pointers_30cc20, void *);

struct pending_30cc20
{
	unsigned short unknown0[3];
	unsigned short reference_count;
	char unknown8[0x44 - 8];
	c_havok_body_group *island;
};

class hkWorld
{
public:
	char unknown0[8];
	islands_30cc20 active_islands;
	islands_30cc20 inactive_islands;
	char unknown20[0x108 - 0x20];
	pointers_30cc20 pending_islands;
	pointers_30cc20 pending_114;

	void function_30cc20(void *pending);
	void world_island_discard(c_havok_body_group *island);
	void function_30ccd0(c_havok_body_group *island);
	void function_30cd50(void *pending);
	void function_30ce20();
};

// @retail 0x30cc20
void hkWorld::function_30cc20(void *pending)
{
	pending_islands.append(pending);
	((referenced_2d88a0 *)pending)->reference_count++;
}

// @retail 0x30cc60
void hkWorld::world_island_discard(c_havok_body_group *island)
{
	inactive_islands.append(island);
	active_islands.data[island->index] = active_islands.data[active_islands.size - 1];
	active_islands.data[island->index]->index = island->index;
	active_islands.size--;
	long &index = island->index;
	index = inactive_islands.size - 1;
	island->active = false;
	function_318620((world_318540 *)this, (group_318540 *)island);
}

// @retail 0x30ccd0
void hkWorld::function_30ccd0(c_havok_body_group *island)
{
	active_islands.append(island);
	inactive_islands.data[island->index] = inactive_islands.data[inactive_islands.size - 1];
	inactive_islands.data[island->index]->index = island->index;
	long &size = inactive_islands.size;
	--size;
	long &index = island->index;
	index = active_islands.size - 1;
	bool &active = island->active;
	active = true;
	island->field_34 = 0;
	island->field_38 = 0;
	function_318540((world_318540 *)this, (group_318540 *)island);
	island->field_30 = false;
}

// @retail 0x30cd50
void hkWorld::function_30cd50(void *pending)
{
	((referenced_2d88a0 *)pending)->reference_count++;
	pending_114.append(pending);
}

struct owner_30cda0
{
	char unknown0[0x44];
	void *handler;
};

struct contact_30cda0
{
	char unknown0[0xc];
	owner_30cda0 *owner;
};

class c_1d1261
{
public:
	char unknown0[0xfc];
	pointers_30cc20 collected;

	hkBool function_30ca30(void *arg_0);
	void function_30cda0();
};

// @retail 0x30cda0
void c_1d1261::function_30cda0()
{
	for (long i = 0; i < collected.size; i++)
	{
		contact_30cda0 *contact = (contact_30cda0 *)collected.data[i];
		if (contact->owner->handler != 0)
		{
			function_30ca30(contact);
		}
		drop_reference_30cc20(collected.data[i]);
	}
	collected.size = 0;
}

// @retail 0x30ce20
void hkWorld::function_30ce20()
{
	for (long i = 0; i < pending_islands.size; i++)
	{
		c_havok_body_group *island = ((pending_30cc20 *)pending_islands.data[i])->island;
		if (island != 0 && island->active)
		{
			world_island_discard(island);
		}
		drop_reference_30cc20(pending_islands.data[i]);
	}
	pending_islands.size = 0;
}

// an object, a reference-counted object whose destructor is the base's: its
// deleting destructor gives the block back with tag 4
class class_30cea0 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(4)
};

// @retail 0x30cea0 deleting class_30cea0
