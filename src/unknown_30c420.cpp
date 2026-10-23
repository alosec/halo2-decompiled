// @flags /O2 /GL-
// Havok (third-party physics library): removing a listener from an object's
// listener array at +0xb4; its entry is cleared (the array squeezes out
// cleared entries later), or the entry before the array when it is not
// there.

class listener_30c420;

struct listener_array_30c420
{
	listener_30c420 **data;
	int size;
	int capacity;

	int index_of(listener_30c420 *listener) const
	{
		for (int i = 0; i < size; i++)
		{
			if (data[i] == listener)
			{
				return i;
			}
		}
		return -1;
	}
};

class class_30c420
{
public:
	char unknown0[0xb4];
	listener_array_30c420 listeners;

	void function_30c420(listener_30c420 *listener);
};

// @retail 0x30c420
void class_30c420::function_30c420(listener_30c420 *listener)
{
	int index = listeners.index_of(listener);
	listeners.data[index] = 0;
}
