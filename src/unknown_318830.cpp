// @flags /O2 /GL-
// Sends a two-argument event to every listener of one of a world's listener
// arrays, last first, then squeezes out the listeners that removed
// themselves. Two arrays, at +0x9c and +0xa8, each with its own sender.

class listener_318830
{
public:
	virtual void slot0(void *first, void *second) = 0;
	virtual void slot1(void *first, void *second) = 0;
};

struct listener_array_318830
{
	listener_318830 **data;
	int size;
	int capacity;
};

struct listener_world_318830
{
	char unknown0[0x9c];
	listener_array_318830 listeners_9c;
	listener_array_318830 listeners_a8;
};

// @retail 0x318830
void __cdecl function_318830(listener_world_318830 *world, void *first, void *second)
{
	listener_array_318830 &array = world->listeners_9c;
	for (int i = array.size - 1; i >= 0; i--)
	{
		listener_318830 *listener = array.data[i];
		if (listener)
		{
			listener->slot1(first, second);
		}
	}
	for (int k = array.size - 1; k >= 0; k--)
	{
		if (array.data[k] == 0)
		{
			array.size--;
			for (int j = k; j < array.size; j++)
			{
				array.data[j] = array.data[j + 1];
			}
		}
	}
}

// @retail 0x3188c0
void __cdecl function_3188c0(listener_world_318830 *world, void *first, void *second)
{
	listener_array_318830 &array = world->listeners_a8;
	for (int i = array.size - 1; i >= 0; i--)
	{
		listener_318830 *listener = array.data[i];
		if (listener)
		{
			listener->slot1(first, second);
		}
	}
	for (int k = array.size - 1; k >= 0; k--)
	{
		if (array.data[k] == 0)
		{
			array.size--;
			for (int j = k; j < array.size; j++)
			{
				array.data[j] = array.data[j + 1];
			}
		}
	}
}
