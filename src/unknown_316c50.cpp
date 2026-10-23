// @flags /O2 /GL-
// Havok (third-party physics library): an object adding entries to its lists:
// pairs of words, members it tells about the member's shape, members it asks
// for a description first and holds a reference to, and members it owns.
#include "growable_array_2d91d0.h"

struct pair_316c50
{
	long first;
	long second;
};

class shape_316c90
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long slot_5() { return 0; }
};

class class_316c50;

struct member_316c90
{
	char unknown0[0x3c];
	shape_316c90 *shape;
	char unknown40[0x44 - 0x40];
	class_316c50 *owner;
	char unknown48[0x58 - 0x48];
	long index;
};

struct description_316cf0
{
	long words[4];
};

class member_316cf0
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
	virtual void slot_10(description_316cf0 *description) {}

	unsigned short unknown4;
	unsigned short reference_count;
	char unknown8[0x14 - 8];
	class_316c50 *owner;
};

struct member_316d60
{
	char unknown0[0x10];
	class_316c50 *owner;
};

ELEMENTS_2D91D0(members_316c90, member_316c90 *);
ELEMENTS_2D91D0(members_316d60, member_316d60 *);
ELEMENTS_2D91D0(members_316cf0, member_316cf0 *);
ELEMENTS_2D91D0(pairs_316c50, pair_316c50);

class class_316c50
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2(member_316cf0 *member, description_316cf0 *description) {}
	virtual void slot_3() {}
	virtual void slot_4(long shape_value) {}

	char unknown4[0x3c - 4];
	members_316c90 shaped_members;
	char unknown48[0x4c - 0x48];
	members_316d60 owned_members;
	members_316cf0 described_members;
	char unknown64[0x74 - 0x64];
	pairs_316c50 pairs;

	void function_316c50(pair_316c50 const *pair);
	void function_316c90(member_316c90 *member);
	void function_316cf0(member_316cf0 *member);
	void function_316d60(member_316d60 *member);
};

// @retail 0x316c50
void class_316c50::function_316c50(pair_316c50 const *pair)
{
	pairs.append_copy(*pair);
}

// @retail 0x316c90
void class_316c50::function_316c90(member_316c90 *member)
{
	member->owner = this;
	shaped_members.append(member);
	member->index = shaped_members.size - 1;
	slot_4(member->shape->slot_5());
}

// @retail 0x316cf0
void class_316c50::function_316cf0(member_316cf0 *member)
{
	description_316cf0 description;
	described_members.append_copy(member);
	member->slot_10(&description);
	slot_2(member, &description);
	member->reference_count++;
	member->owner = this;
}

// @retail 0x316d60
void class_316c50::function_316d60(member_316d60 *member)
{
	owned_members.append(member);
	member->owner = this;
}
