// @flags /O2 /GL-
// Havok (third-party physics library): an owner letting go of its listening
// object (taken out of the listener array when it was added, then deleted),
// and dropping every reference an object's list holds to one object.
#include "reference_counted_2d88a0.h"

bool __cdecl function_279740(long value);

class listener_30c420
{
public:
	virtual void slot_0() {}
};

class class_30c420
{
public:
	void function_30c420(listener_30c420 *listener);
};

class owned_30c500 : public referenced_2d88a0, public listener_30c420
{
};

class s_3111d0;
class s_30cb70;

class c_3101c0
{
public:
	char unknown0[0x23e];
	char field_23e;
	char unknown23f;
	bool field_240;
	char unknown241[0x13];
	owned_30c500 *field_254;

	void function_30c500();
};

// @retail 0x30c500
void c_3101c0::function_30c500()
{
	if (field_23e == 1)
	{
		field_23e = 0;
		if (field_240)
		{
			((class_30c420 *)this)->function_30c420(field_254);
			field_240 = false;
		}
		delete field_254;
		field_254 = 0;
	}
}

class deletable_30c560
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

struct array_30c560
{
	referenced_2d88a0 **data;
	int size;

	int index_of(referenced_2d88a0 *entry) const
	{
		for (int i = 0; i < size; i++)
		{
			if (data[i] == entry)
			{
				return i;
			}
		}
		return -1;
	}

	void fill_from_last(int index)
	{
		size--;
		data[index] = data[size];
	}
};

class class_30c560
{
public:
	char unknown0[0x108];
	array_30c560 entries;

	void function_30c560(referenced_2d88a0 *entry);
};

// @retail 0x30c560
void class_30c560::function_30c560(referenced_2d88a0 *entry)
{
	for (int i = 0; i < entries.size; i++)
	{
		if (entries.data[i] == entry)
		{
			entries.fill_from_last(entries.index_of(entry));
			entry->reference_count--;
			if (entry->reference_count == 0)
			{
				if (function_279740((long)entry))
				{
					entry->reference_count = 1;
				}
				else
				{
					((deletable_30c560 *)entry)->destroy(1);
				}
			}
		}
	}
}
