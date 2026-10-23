// @flags /O2 /GL- /GX
// Havok (third-party physics library): a member's ended pairs as it leaves:
// the broad phase gives the pairs its removal ends; a member in a group hands
// them to its group, and a fixed member has the dispatcher run them with a
// handler that notes the groups they reach, each of which then takes its own
// pending ended pairs.
#include "placed_shape_312800.h"
#include "holder_30dc80.h"

// a group's ended pairs waiting to be run, each a pair of words
struct pair_30e140
{
	long first;
	long second;
};

ELEMENTS_2D91D0(pairs_30e140, pair_30e140);

// a group: the pairs it gathers come before its own pending pairs
class collector_3173f0
{
public:
	void function_3173f0(pair_30d870 *added, long added_count, pair_30d870 *removed, long removed_count);
};

struct group_30e140
{
	char unknown0[0x8c];
	pairs_30e140 pending;
};

// the member's shape, and the broad phase's handle for it
struct collidable_30e140
{
	void *shape;
	char unknown4[0x10 - 0x4];
	char handle[4];
};

struct member_30e140
{
	char unknown0[0xc];
	collidable_30e140 collidable;
	char unknown20[0x40 - 0x20];
	bool fixed;
	char unknown41[0x44 - 0x41];
	collector_3173f0 *group;
};

class entity_316000;

class class_30e140 : public world_312590
{
public:
	void function_30e140(entity_316000 *entity);
};

// @retail 0x30e140
void class_30e140::function_30e140(entity_316000 *entity)
{
	member_30e140 *member = (member_30e140 *)entity;
	pairs_30d870 removed(pair_capacity_234);
	collidable_30e140 *collidable = &member->collidable;
	if (collidable->shape == 0)
	{
		return;
	}
	pair_finder->slot_3(collidable->handle, &removed);
	if (removed.size > 0)
	{
		if (member->fixed)
		{
			holder_30dc80 recorder(collidable);
			dispatcher_3728c0 *const runner = dispatcher;
			runner->handler = (pair_handler_3173f0 *)&recorder;
			dispatcher->function_3728c0(0, 0, removed.data, removed.size, pair_listener);
			dispatcher_3728c0 *const restored = dispatcher;
			pair_handler_3173f0 *const own = &restored->own_handler;
			restored->handler = own;
			for (int i = 0; i < recorder.values.size; i++)
			{
				group_30e140 *group = (group_30e140 *)recorder.values.data[i];
				long const waiting = group->pending.size;
				((collector_3173f0 *)group)->function_3173f0(0, 0, (pair_30d870 *)group->pending.data, waiting);
				group->pending.resize(0);
			}
		}
		else
		{
			member->group->function_3173f0(0, 0, removed.data, removed.size);
		}
	}
}
