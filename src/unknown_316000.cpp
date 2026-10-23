// @flags /O2 /GL-
// Havok (third-party physics library): taking an entity out of an island's
// unordered list (the last one takes its index), and out of an ordered list
// (dropping the list's reference), telling the island in each case.
#include "reference_counted_2d88a0.h"

bool __cdecl function_279740(long value);

class kind_316000
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long slot_5() { return 0; }
};

struct state_316060
{
	long field_0[4];
};

class entity_316000 : public referenced_2d88a0
{
public:
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10(state_316060 *state) {}

	char unknown8[0xc];
	long field_14;
	char unknown18[0x24];
	kind_316000 *field_3c;
	char unknown40[4];
	long field_44;
	char unknown48[0x10];
	long index;
};

class deletable_316060
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

struct array_316060
{
	entity_316000 **data;
	int size;

	int index_of(entity_316000 *entity) const
	{
		for (int i = 0; i < size; i++)
		{
			if (data[i] == entity)
			{
				return i;
			}
		}
		return -1;
	}

	void remove_at_ordered(int index)
	{
		size--;
		for (int i = index; i < size; i++)
		{
			data[i] = data[i + 1];
		}
	}
};

class island_316000
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3(entity_316000 *entity, state_316060 *state) {}
	virtual void slot_4(long change) {}

	char unknown4[0x24];
	bool dirty;
	char unknown29[0x13];
	entity_316000 **entities;
	int entity_count;
	char unknown44[0x14];
	array_316060 ordered;

	void function_316000(entity_316000 *entity);
	void function_316060(entity_316000 *entity);
};

// @retail 0x316000
void island_316000::function_316000(entity_316000 *entity)
{
	entities[entity->index] = entities[entity_count - 1];
	entities[entity->index]->index = entity->index;
	entity_count--;
	entity->field_44 = 0;
	entity->index = -1;
	slot_4(-entity->field_3c->slot_5());
	dirty = true;
}

// @retail 0x316060
void island_316000::function_316060(entity_316000 *entity)
{
	ordered.remove_at_ordered(ordered.index_of(entity));
	state_316060 state;
	entity->slot_10(&state);
	slot_3(entity, &state);
	dirty = true;
	entity->field_14 = 0;
	entity->reference_count--;
	if (entity->reference_count == 0)
	{
		if (function_279740((long)entity))
		{
			entity->reference_count = 1;
		}
		else
		{
			((deletable_316060 *)entity)->destroy(1);
		}
	}
}
