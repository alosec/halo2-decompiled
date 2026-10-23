// @flags /O2 /GL-
// Havok (third-party physics library): sending events to the listeners of
// each of a world's four listener arrays.
#include "listener_world_315d20.h"

// @retail 0x318010
void __cdecl function_318010(listener_world_315d20 *world, void *event)
{
	LISTENER_WORLD_315D20_SEND(world->listeners_3c, slot2, event)
}

// @retail 0x318080
void __cdecl function_318080(listener_world_315d20 *world, void *event)
{
	LISTENER_WORLD_315D20_SEND(world->listeners_48, slot3, event)
}

// @retail 0x3180f0
void __cdecl function_3180f0(listener_world_315d20 *world, void *event)
{
	LISTENER_WORLD_315D20_SEND(world->listeners_48, slot2, event)
}

// @retail 0x318160
void __cdecl function_318160(listener_world_315d20 *world, void *event)
{
	LISTENER_WORLD_315D20_SEND(world->listeners_54, slot2, event)
}

// @retail 0x3181d0
void __cdecl function_3181d0(listener_world_315d20 *world, void *event)
{
	LISTENER_WORLD_315D20_SEND(world->listeners_54, slot1, event)
}

// @retail 0x318240
void __cdecl function_318240(listener_world_315d20 *world, void *event)
{
	LISTENER_WORLD_315D20_SEND(world->listeners_60, slot2, event)
}

// @retail 0x3182b0
void __cdecl function_3182b0(listener_world_315d20 *world, void *event)
{
	LISTENER_WORLD_315D20_SEND(world->listeners_60, slot3, event)
}
