// @flags /O2 /GL-
// Havok (third-party physics library): flagging an event whose value falls
// below the threshold at +8.

struct s_30c4d0_source
{
	long field_0;
	long field_4;
	long field_8;
	float value;
};

struct s_30c4d0_event
{
	char unknown0[0x14];
	s_30c4d0_source *source;
	long field_18;
	long field_1c;
	long flagged;
};

class class_30c4d0
{
public:
	void *vtable;
	long field_4;
	float threshold;

	void function_30c4d0(s_30c4d0_event *event);
};

// @retail 0x30c4d0
void class_30c4d0::function_30c4d0(s_30c4d0_event *event)
{
	if (event->source->value < threshold)
	{
		field_4 = 0;
		event->flagged = 1;
	}
}
