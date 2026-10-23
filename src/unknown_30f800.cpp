// @flags /O2 /GL-
// Havok (third-party physics library): removing an entity from the world.
// While the world is locked the entity waits in a list (with a reference)
// and the result is no; otherwise the pending operations run first, the
// entity leaves the world's lists, its listeners and the world's are told,
// and the world lets go of it.
#include "unknown_1cec30.h"
#include "growable_array_2d91d0.h"
#include "listener_world_315d20.h"
#include "entity_listeners_315c00.h"

bool __cdecl function_279740(long value);
void __cdecl function_3180f0(listener_world_315d20 *world, void *event);

class entity_316000;

class deletable_30f800
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

// the world as the functions this one calls see it
class class_30f030
{
public:
	void function_30f6b0();
};

class class_30e140
{
public:
	void function_30e140(entity_316000 *entity);
};

class world_30e2c0
{
public:
	void function_30e2c0(entity_316000 *entity);
};

ELEMENTS_2D91D0(entities_30f800, hkEntity *);

// the world's fields this function reads
struct world_30f800
{
	char unknown0[0x24];
	long pending_count;
	char unknown28[0xf0 - 0x28];
	entities_30f800 waiting;
	char unknownfc[0x12c - 0xfc];
	unsigned char locked;
};

// @retail 0x30f800
hkBool hkWorld::entity_world_remove(hkEntity *entity)
{
	world_30f800 *world = (world_30f800 *)this;
	referenced_2d88a0 *counted = (referenced_2d88a0 *)entity;
	hkBool result;
	if (world->locked == 1)
	{
		world->waiting.append(entity);
		counted->reference_count++;
		result.m_bool = false;
		return result;
	}
	if (world->pending_count > 0)
	{
		((class_30f030 *)this)->function_30f6b0();
	}
	((class_30e140 *)this)->function_30e140((entity_316000 *)entity);
	function_3180f0((listener_world_315d20 *)this, entity);
	function_315c90((entity_315c00 *)entity);
	((world_30e2c0 *)this)->function_30e2c0((entity_316000 *)entity);
	counted->reference_count--;
	if (counted->reference_count == 0)
	{
		if (function_279740((long)counted))
		{
			counted->reference_count = 1;
		}
		else
		{
			// the deleting destructor, first in the vtable, without the null
			// test `delete` adds
			((deletable_30f800 *)counted)->destroy(1);
		}
	}
	result.m_bool = true;
	return result;
}
