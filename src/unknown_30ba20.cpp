// @flags /O2 /GL-
// Havok (third-party physics library): taking an entry out of an object's
// list (its last entry fills the gap), telling every listener whether it was
// there; entries whose first word is zero are ignored.

class class_30af80;

struct bool_30af80
{
	bool value;

	bool_30af80(bool value_)
	{
		value = value_;
	}
	bool_30af80(bool_30af80 const &other)
	{
		value = other.value;
	}
};

struct s_30af80_event
{
	class_30af80 *source;
	long value;
	bool flag;
};

class class_30af80_listener
{
public:
	virtual void slot0() = 0;
	virtual void notify(s_30af80_event *event) = 0;
};

struct entry_30ba20
{
	long field_0;
};

class class_30af80
{
public:
	char unknown0[0x3c];
	class_30af80_listener **listeners;
	int listener_count;
	char unknown44[0x7c];
	entry_30ba20 **entries;
	int entry_count;

	void function_30af80(long value, bool_30af80 flag);

	void tell_listeners(long value, bool flag)
	{
		s_30af80_event event;
		event.value = value;
		event.source = this;
		event.flag = flag;
		for (int i = listener_count - 1; i >= 0; i--)
		{
			class_30af80_listener *listener = listeners[i];
			if (listener)
			{
				listener->notify(&event);
			}
		}
	}

	void function_30ba20(entry_30ba20 *entry);
};

// @retail 0x30ba20
void class_30af80::function_30ba20(entry_30ba20 *entry)
{
	if (entry->field_0 == 0)
	{
		return;
	}
	for (int i = entry_count - 1; i >= 0; i--)
	{
		if (entries[i] == entry)
		{
			function_30af80((long)entry, bool_30af80(true));
			entry_count--;
			entries[i] = entries[entry_count];
			return;
		}
	}
	tell_listeners((long)entry, false);
}
