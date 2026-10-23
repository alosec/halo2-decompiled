#ifndef ENTITY_LISTENERS_315C00_H
#define ENTITY_LISTENERS_315C00_H
// Havok (third-party physics library): telling everything that listens to an
// entity that it is being destroyed (0x315c00) or removed (0x315c90): the
// entity's own listeners, last first, then the objects of two of its arrays,
// which listen through their second base.
#include "reference_counted_2d88a0.h"

class entity_315c00;

class entity_listener_315c00
{
public:
	virtual void slot_0(entity_315c00 *entity) = 0;
	// told that the entity is being destroyed (slot 1) or removed (slot 2)
	virtual void slot_1(entity_315c00 *entity) = 0;
	virtual void slot_2(entity_315c00 *entity) = 0;
};

// a reference-counted object that listens to the entities it is attached to
class listening_315c00 : public referenced_2d88a0, public entity_listener_315c00
{
};

struct entity_listeners_315c00
{
	entity_listener_315c00 **data;
	long size;
	long capacity_and_flags;
};

struct listening_objects_315c00
{
	listening_315c00 **data;
	long size;
	long capacity_and_flags;
};

class entity_315c00
{
public:
	char unknown0[0x7c];
	entity_listeners_315c00 listeners;
	listening_objects_315c00 objects_88;
	listening_objects_315c00 objects_94;
};

void __cdecl function_315c00(entity_315c00 *entity);
void __cdecl function_315c90(entity_315c00 *entity);

#endif
