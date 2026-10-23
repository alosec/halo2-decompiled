// @flags /O2 /GL-
// Havok (third-party physics library): telling a world's listeners, then the
// listeners of every entity in a group, about the group (two events, through
// different slots), dropping the listener entries that were cleared.

// a listener of world or entity events, told through slots 1 and 2
class listener_318540
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1(void *subject) = 0;
	virtual void slot_2(void *subject) = 0;
};

// an array of listener pointers; removing an entry moves the later ones
// down, keeping their order
struct listeners_318540
{
	listener_318540 **data;
	long size;
	unsigned long capacity_and_flags;

	void remove_at_and_copy(long index)
	{
		size--;
		for (long i = index; i < size; i++)
		{
			data[i] = data[i + 1];
		}
	}
};

struct entity_318540
{
	char unknown0[0x70];
	listeners_318540 listeners;
};

struct group_318540
{
	char unknown0[0x3c];
	entity_318540 **entities;
	long count;
};

struct world_318540
{
	char unknown0[0x78];
	listeners_318540 listeners;
};

// @retail 0x318540
void __cdecl function_318540(world_318540 *world, group_318540 *group)
{
	for (long i = world->listeners.size - 1; i >= 0; i--)
	{
		listener_318540 *listener = world->listeners.data[i];
		if (listener)
		{
			listener->slot_1(group);
		}
	}
	for (long j = world->listeners.size - 1; j >= 0; j--)
	{
		if (world->listeners.data[j] == 0)
		{
			world->listeners.remove_at_and_copy(j);
		}
	}
	for (long k = 0; k < group->count; k++)
	{
		listeners_318540 &listeners = group->entities[k]->listeners;
		for (long i = listeners.size - 1; i >= 0; i--)
		{
			listener_318540 *listener = listeners.data[i];
			if (listener)
			{
				listener->slot_2(group->entities[k]);
			}
		}
		for (long j = listeners.size - 1; j >= 0; j--)
		{
			if (listeners.data[j] == 0)
			{
				listeners.remove_at_and_copy(j);
			}
		}
	}
}

// as 0x318540, through the other slots
// @retail 0x318620
void __cdecl function_318620(world_318540 *world, group_318540 *group)
{
	for (long i = world->listeners.size - 1; i >= 0; i--)
	{
		listener_318540 *listener = world->listeners.data[i];
		if (listener)
		{
			listener->slot_2(group);
		}
	}
	for (long j = world->listeners.size - 1; j >= 0; j--)
	{
		if (world->listeners.data[j] == 0)
		{
			world->listeners.remove_at_and_copy(j);
		}
	}
	for (long k = 0; k < group->count; k++)
	{
		listeners_318540 &listeners = group->entities[k]->listeners;
		for (long i = listeners.size - 1; i >= 0; i--)
		{
			listener_318540 *listener = listeners.data[i];
			if (listener)
			{
				listener->slot_1(group->entities[k]);
			}
		}
		for (long j = listeners.size - 1; j >= 0; j--)
		{
			if (listeners.data[j] == 0)
			{
				listeners.remove_at_and_copy(j);
			}
		}
	}
}
