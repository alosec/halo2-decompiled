// @flags /O2 /GL-
// Havok (third-party physics library): sends a two-argument event (the world
// and a value) to every listener of one of the world's listener arrays,
// last first, then squeezes out the listeners that removed themselves.

class listener_3187a0
{
public:
	virtual void slot0(void *world, void *value) = 0;
	virtual void slot1(void *world, void *value) = 0;
};

struct listener_array_3187a0
{
	listener_3187a0 **data;
	int size;
	int capacity;
};

struct listener_world_3187a0
{
	char unknown0[0x90];
	listener_array_3187a0 listeners_90;
};

// @retail 0x3187a0
void __cdecl function_3187a0(listener_world_3187a0 *world, void *value)
{
	listener_array_3187a0 &array = world->listeners_90;
	for (int i = array.size - 1; i >= 0; i--)
	{
		listener_3187a0 *listener = array.data[i];
		if (listener)
		{
			listener->slot1(world, value);
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
