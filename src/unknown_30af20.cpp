// @flags /O2 /GL-
// Havok (third-party physics library): asking every listener, from the last,
// about an event; each may change the answer, which starts as the flag.

#include "listeners_30af20.h"

struct event_30af20
{
	class_30af20 *source;
	long value;
	char flag;
	int answer;
};

class listener_30af20
{
public:
	virtual void ask(event_30af20 *event) = 0;
};

// @retail 0x30af20
int class_30af20::function_30af20(long value, flag_30af20 flag)
{
	event_30af20 event;
	event.value = value;
	event.source = this;
	event.flag = flag.value;
	event.answer = flag.value != 0;
	for (int i = listener_count - 1; i >= 0; i--)
	{
		listener_30af20 *listener = listeners[i];
		if (listener)
		{
			listener->ask(&event);
		}
	}
	return event.answer;
}
