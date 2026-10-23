// @flags /O2 /GL-
// Havok (third-party physics library): telling every listener, from the
// last, about an event.
#include "description_312430.h"
#include <xmmintrin.h>

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

class class_30af80
{
public:
	char unknown0[0x3c];
	class_30af80_listener **listeners;
	int listener_count;

	void function_30af80(long value, bool_30af80 flag);
};

// @retail 0x30af80
void class_30af80::function_30af80(long value, bool_30af80 flag)
{
	s_30af80_event event;
	event.value = value;
	event.source = this;
	event.flag = flag.value;
	for (int i = listener_count - 1; i >= 0; i--)
	{
		class_30af80_listener *listener = listeners[i];
		if (listener)
		{
			listener->notify(&event);
		}
	}
}

// an object that adds two vectors to the description
struct description_30afc0 : description_312430
{
	char unknown18[0x20 - 0x18];
	__m128 field_20;
	__m128 field_30;
};

class class_30afc0 : public class_312430
{
public:
	char unknown3c[0x60 - 0x3c];
	__m128 field_60;
	__m128 field_70;

	void function_30afc0(description_30afc0 *description) const;
};

// fills the description from the object, with the two vectors it adds
// @retail 0x30afc0
void class_30afc0::function_30afc0(description_30afc0 *description) const
{
	function_312430(description);
	description->field_20 = field_60;
	description->field_30 = field_70;
}
