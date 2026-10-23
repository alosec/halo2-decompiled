// @flags /O2 /GL-
// Havok (third-party physics library): an owner taking in a member: the member
// gets the next id (counting down for fixed members, up for the others), joins
// a group and is entered into the pair finder with the owner locked, gains a
// reference, and listeners hear of it.

class hkEntity;
struct listener_world_315d20;

void __cdecl function_318080(listener_world_315d20 *world, void *event);

struct member_30c8e0
{
	char unknown0[6];
	unsigned short reference_count;
	char unknown8[0x20 - 0x8];
	long id;
	char unknown24[0x40 - 0x24];
	bool fixed;
};

class owner_30c8e0
{
public:
	void function_30c8e0(member_30c8e0 *member);
	void function_30df80(member_30c8e0 *member);
};

// the owner's lock and its two id counters
struct owner_30f2d0
{
	char unknown0[0x12c];
	char locked;
	char unknown12d[0x130 - 0x12d];
	long next_id;
	long next_fixed_id;
};

// the owner, as the game's callers declare it
class hkWorld
{
public:
	hkEntity *entity_world_insert(hkEntity *entity);
};

// returns the entity it inserted
// @retail 0x30f2d0
hkEntity *hkWorld::entity_world_insert(hkEntity *entity)
{
	owner_30f2d0 *owner = (owner_30f2d0 *)this;
	member_30c8e0 *member = (member_30c8e0 *)entity;
	long was_locked = owner->locked;
	owner->locked = 1;
	long id;
	if (member->fixed)
	{
		id = owner->next_fixed_id--;
	}
	else
	{
		id = owner->next_id++;
	}
	member->id = id;
	((owner_30c8e0 *)this)->function_30c8e0(member);
	((owner_30c8e0 *)this)->function_30df80(member);
	owner->locked = (char)was_locked;
	member->reference_count++;
	function_318080((listener_world_315d20 *)this, member);
	return entity;
}
