// @flags /O2 /GL-
// Havok (third-party physics library): sending events, stamped with the
// world, to the listeners of one of a world's listener arrays.
#include "listener_world_315d20.h"

// @retail 0x315d20
void __cdecl function_315d20(listener_world_315d20 *world, event_315d20 *event)
{
	event->world = world;
	LISTENER_WORLD_315D20_SEND(world->listeners_60, slot0, event)
}

// @retail 0x315d90
void __cdecl function_315d90(listener_world_315d20 *world, event_315d20 *event)
{
	event->world = world;
	LISTENER_WORLD_315D20_SEND(world->listeners_60, slot1, event)
}

// @retail 0x315e00
void __cdecl function_315e00(listener_world_315d20 *world, event_315d20 *event)
{
	event->world = world;
	LISTENER_WORLD_315D20_SEND(world->listeners_60, slot2, event)
}
