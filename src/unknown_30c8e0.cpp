// @flags /O2 /GL- /GX
// Havok (third-party physics library): adding a member to one of an owner's
// groups: a fixed member to the owner's own group, any other to a new group
// when the owner makes one per member, else to the first group, made when
// there is none.
#include "growable_array_2d91d0.h"

class object_3171a0
{
public:
	REFERENCED_NEW_2D88A0(0x2d)

	char unknown0[0x24];
	long field_24;
	char unknown28[0xb0 - 0x28];

	object_3171a0(long value);
};

struct member_316c90;

class class_316c50
{
public:
	void function_316c90(member_316c90 *member);
};

ELEMENTS_2D91D0(groups_30c8e0, object_3171a0 *);

class owner_30c8e0;

struct member_30c8e0
{
	char unknown0[8];
	owner_30c8e0 *owner;
	char unknown0c[0x40 - 0xc];
	bool fixed;
};

class owner_30c8e0
{
public:
	char unknown0[8];
	groups_30c8e0 groups;
	char unknown14[0x2c - 0x14];
	class_316c50 *fixed_group;
	char unknown30[0x23c - 0x30];
	bool group_per_member;

	void function_30c8e0(member_30c8e0 *member);
};

// @retail 0x30c8e0
void owner_30c8e0::function_30c8e0(member_30c8e0 *member)
{
	member->owner = this;
	if (member->fixed)
	{
		fixed_group->function_316c90((member_316c90 *)member);
		return;
	}
	if (group_per_member)
	{
		object_3171a0 *group = new object_3171a0((long)this);
		group->field_24 = groups.size;
		((class_316c50 *)group)->function_316c90((member_316c90 *)member);
		groups.append(group);
		return;
	}
	if (groups.size == 0)
	{
		object_3171a0 *group = new object_3171a0((long)this);
		group->field_24 = groups.size;
		groups.append(group);
	}
	((class_316c50 *)groups.data[0])->function_316c90((member_316c90 *)member);
}
