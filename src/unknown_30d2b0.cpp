// @flags /O2 /GL-
// Havok (third-party physics library): an owner taking in an object that
// links two members: the link joins the group of its first member, or of its
// second when the first is fixed, after the two members' groups are merged;
// a group left inactive is handed on, and listeners hear of the link.

class member_316cf0;
class c_havok_body_group;
struct listener_world_315d20;

void __cdecl function_318240(listener_world_315d20 *world, void *event);

class class_316c50
{
public:
	char unknown0[0x29];
	bool active;

	void function_316cf0(member_316cf0 *member);
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

// a member: the group it belongs to at +0x44
struct member_30d2b0
{
	char unknown0[0x44];
	class_316c50 *group;
};

// the object linking two members
struct link_30d2b0
{
	char unknown0[0xc];
	member_30d2b0 *first;
	member_30d2b0 *second;
};

// the owner, as the game's callers declare it, and its layout
struct s_1d1541
{
	void *function_30d2b0(void *arg_0);
};

struct owner_30d2b0
{
	char unknown0[0x2c];
	class_316c50 *fixed_group;
};

// it returns the link
// @retail 0x30d2b0
void *s_1d1541::function_30d2b0(void *arg_0)
{
	link_30d2b0 *link = (link_30d2b0 *)arg_0;
	class_316c50 *first_group = link->first->group;
	class_316c50 *second_group = link->second->group;
	if (first_group == ((owner_30d2b0 *)this)->fixed_group)
	{
		second_group->function_316cf0((member_316cf0 *)link);
		if (!second_group->active)
		{
			((hkWorld *)this)->function_30ccd0((c_havok_body_group *)second_group);
		}
	}
	else
	{
		if (second_group != ((owner_30d2b0 *)this)->fixed_group && first_group != second_group)
		{
			((class_30d160 *)this)->function_30d160((long)link->first, (long)link->second);
		}
		first_group->function_316cf0((member_316cf0 *)link);
		if (!first_group->active)
		{
			((hkWorld *)this)->function_30ccd0((c_havok_body_group *)first_group);
		}
	}
	function_318240((listener_world_315d20 *)this, link);
	return link;
}
