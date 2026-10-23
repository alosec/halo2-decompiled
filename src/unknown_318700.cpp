// @flags /O2 /GL-
// Havok (third-party physics library): telling an object's listeners, from
// the last, about an event, squeezing out the cleared entries, and then
// telling the listener kept at +0xc0.

struct event_318700;

class listener_318700
{
public:
	virtual void slot_0() {}
	virtual void notify(struct class_30c180 *source, event_318700 *event) {}
};

struct array_318700
{
	listener_318700 **data;
	int size;

	void remove_at_ordered(int index)
	{
		size--;
		for (int i = index; i < size; i++)
		{
			data[i] = data[i + 1];
		}
	}
};

struct class_30c180
{
	char unknown0[0x84];
	array_318700 listeners;

	long function_30c180() const;
};

// @retail 0x318700
void __cdecl function_318700(class_30c180 *source, event_318700 *event)
{
	int i;
	for (i = source->listeners.size - 1; i >= 0; i--)
	{
		listener_318700 *listener = source->listeners.data[i];
		if (listener)
		{
			listener->notify(source, event);
		}
	}
	for (i = source->listeners.size - 1; i >= 0; i--)
	{
		if (source->listeners.data[i] == 0)
		{
			source->listeners.remove_at_ordered(i);
		}
	}
	listener_318700 *last = (listener_318700 *)source->function_30c180();
	if (last)
	{
		last->notify(source, event);
	}
}
