// @flags /O2 /GL-
// Havok (third-party physics library): a description that adds a byte and a
// word; and an entity's being destroyed or removed, sent to everything that
// listens to it.
#include "description_312430.h"
#include "entity_listeners_315c00.h"

// @retail 0x315be0
description_30bd90::description_30bd90()
{
	field_18 = 1;
	field_1a = -1;
}

// @retail 0x315c00
void __cdecl function_315c00(entity_315c00 *entity)
{
	for (int i = entity->listeners.size - 1; i >= 0; i--)
	{
		entity_listener_315c00 *listener = entity->listeners.data[i];
		if (listener)
		{
			listener->slot_1(entity);
		}
	}
	for (int j = 0; j < entity->objects_88.size; j++)
	{
		listening_315c00 *object = entity->objects_88.data[j];
		if (object)
		{
			object->slot_1(entity);
		}
	}
	for (int k = 0; k < entity->objects_94.size; k++)
	{
		listening_315c00 *object = entity->objects_94.data[k];
		if (object)
		{
			object->slot_1(entity);
		}
	}
}

// @retail 0x315c90
void __cdecl function_315c90(entity_315c00 *entity)
{
	for (int i = entity->listeners.size - 1; i >= 0; i--)
	{
		entity_listener_315c00 *listener = entity->listeners.data[i];
		if (listener)
		{
			listener->slot_2(entity);
		}
	}
	for (int j = 0; j < entity->objects_88.size; j++)
	{
		listening_315c00 *object = entity->objects_88.data[j];
		if (object)
		{
			object->slot_2(entity);
		}
	}
	for (int k = 0; k < entity->objects_94.size; k++)
	{
		listening_315c00 *object = entity->objects_94.data[k];
		if (object)
		{
			object->slot_2(entity);
		}
	}
}
