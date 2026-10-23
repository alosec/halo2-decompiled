// @flags /O2 /GL-
// Havok (third-party physics library): removing an entity from the world
// together with the constraints that hold it, which are passed back.
#include "growable_array_2d91d0.h"

class entity_316000;

// whether the entity was removed now, given back by value
struct bool_30f8d0
{
	bool value;

	bool_30f8d0(bool value_)
	{
		value = value_;
	}
};

struct info_30fb40
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

// a constraint between two entities
class constraint_30fb40
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void get_info(info_30fb40 *info) {}

	char unknown4[8];
	entity_316000 *first;
	entity_316000 *second;
};

ELEMENTS_2D91D0(constraints_30fb40, constraint_30fb40 *);

// what keeps the world's constraints
class constraint_owner_30fb40
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void remove_constraint(constraint_30fb40 *constraint, info_30fb40 const *info) {}

	char unknown4[0x58 - 4];
	constraints_30fb40 constraints;
};

class class_30e140
{
public:
	char unknown0[0x2c];
	constraint_owner_30fb40 *constraint_owner;

	bool_30f8d0 function_30f8d0(entity_316000 *entity);
	void function_30fb40(entity_316000 *entity, constraints_30fb40 *removed);
};

// @retail 0x30fb40
void class_30e140::function_30fb40(entity_316000 *entity, constraints_30fb40 *removed)
{
	constraints_30fb40 *constraints = &constraint_owner->constraints;
	removed->size = 0;
	for (int i = 0; i < constraints->size; i++)
	{
		constraint_30fb40 *constraint = constraints->data[i];
		if (constraint->first == entity || constraint->second == entity)
		{
			removed->append_copy(constraints->data[i]);
			info_30fb40 info;
			constraints->data[i]->get_info(&info);
			constraint_owner->remove_constraint(constraints->data[i], &info);
			constraints_30fb40 *owned = &constraint_owner->constraints;
			owned->size--;
			for (int j = i; j < owned->size; j++)
			{
				owned->data[j] = owned->data[j + 1];
			}
			i--;
		}
	}
	function_30f8d0(entity);
}
