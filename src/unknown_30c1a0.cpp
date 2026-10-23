// @flags /O2 /GL-
// Havok (third-party physics library): replacing the object held at +0x250,
// taking a reference to the new one before dropping the old one's.

bool __cdecl function_279740(long value);

class counted_30c1a0
{
public:
	// the deleting destructor, first in the vtable
	virtual void destroy(unsigned int flags) = 0;

	unsigned short memory_size;
	unsigned short reference_count;
};

class c_3101c0
{
public:
	char field_0[0x250];
	counted_30c1a0 *held;

	void function_30c1a0(unsigned char *arg_0);
};

// @retail 0x30c1a0
void c_3101c0::function_30c1a0(unsigned char *arg_0)
{
	counted_30c1a0 *object = (counted_30c1a0 *)arg_0;
	object->reference_count++;
	counted_30c1a0 *old = held;
	old->reference_count--;
	if (old->reference_count == 0)
	{
		if (function_279740((long)old))
		{
			old->reference_count = 1;
		}
		else
		{
			old->destroy(1);
		}
	}
	held = object;
}
