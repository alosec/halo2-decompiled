#ifndef LISTENER_WORLD_315D20_H
#define LISTENER_WORLD_315D20_H
// Havok (third-party physics library): an object holding four arrays of
// listeners. Each event goes to every listener of one array, last first;
// listeners may remove themselves while it does (leaving a null entry), and
// the null entries are squeezed out afterwards.

struct event_315d20
{
	char unknown0[0x10];
	void *world;
};

class listener_315d20
{
public:
	virtual void slot0(void *event) = 0;
	virtual void slot1(void *event) = 0;
	virtual void slot2(void *event) = 0;
	virtual void slot3(void *event) = 0;
};

struct listener_array_315d20
{
	listener_315d20 **data;
	int size;
	int capacity;
};

struct listener_world_315d20
{
	char unknown0[0x3c];
	listener_array_315d20 listeners_3c;
	listener_array_315d20 listeners_48;
	listener_array_315d20 listeners_54;
	listener_array_315d20 listeners_60;
};

// sends the event to one slot of every listener in the array, last first,
// then squeezes out the listeners that removed themselves
#define LISTENER_WORLD_315D20_SEND(array, slot, event) \
	{ \
		for (int i = (array).size - 1; i >= 0; i--) \
		{ \
			listener_315d20 *listener = (array).data[i]; \
			if (listener) \
			{ \
				listener->slot(event); \
			} \
		} \
		for (int k = (array).size - 1; k >= 0; k--) \
		{ \
			if ((array).data[k] == 0) \
			{ \
				(array).size--; \
				for (int j = k; j < (array).size; j++) \
				{ \
					(array).data[j] = (array).data[j + 1]; \
				} \
			} \
		} \
	}

#endif
