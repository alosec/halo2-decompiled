#ifndef LISTENERS_30AF20_H
#define LISTENERS_30AF20_H
// Havok (third-party physics library): an object that asks its listeners about
// an event (src/unknown_30af20.cpp).

class listener_30af20;

struct flag_30af20
{
	char value;
};

class class_30af20
{
public:
	char unknown0[0x3c];
	listener_30af20 **listeners;
	int listener_count;

	int function_30af20(long value, flag_30af20 flag);
};

#endif
