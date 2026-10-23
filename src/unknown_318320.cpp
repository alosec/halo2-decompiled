// @flags /O2 /GL-
// Havok (third-party physics library): sending events to two more of a
// world's listener arrays: three events that first clear the event's world,
// and one that passes the world itself.
#include "listener_world_315d20.h"

struct listener_world_318320
{
	char unknown0[0x6c];
	listener_array_315d20 listeners_6c;
	char unknown78[0x3c];
	listener_array_315d20 listeners_b4;
};

// @retail 0x318320
void __cdecl function_318320(listener_world_318320 *world, event_315d20 *event)
{
	event->world = 0;
	LISTENER_WORLD_315D20_SEND(world->listeners_b4, slot0, event)
}

// @retail 0x3183b0
void __cdecl function_3183b0(listener_world_318320 *world, event_315d20 *event)
{
	event->world = 0;
	LISTENER_WORLD_315D20_SEND(world->listeners_b4, slot1, event)
}

// @retail 0x318440
void __cdecl function_318440(listener_world_318320 *world, event_315d20 *event)
{
	event->world = 0;
	LISTENER_WORLD_315D20_SEND(world->listeners_b4, slot2, event)
}

// @retail 0x3184d0
void __cdecl function_3184d0(listener_world_318320 *world)
{
	LISTENER_WORLD_315D20_SEND(world->listeners_6c, slot1, world)
}
