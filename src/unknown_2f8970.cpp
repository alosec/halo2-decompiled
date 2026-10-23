// @flags /O2 /GL-
// Havok (third-party physics library): handing back the id of every active
// entry, then destroying itself through the first virtual slot.

class class_2f8970_owner
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void release_id(unsigned long id) = 0;
};

struct s_2f8970_entry
{
	bool active;
	char field_1;
	unsigned short id;
};

class class_2f8970
{
public:
	virtual void slot0(int flags) = 0;

	long field_4;
	class_2f8970_owner *owner;
	s_2f8970_entry *entries;
	int count;

	void function_2f8970();
};

// @retail 0x2f8970
void class_2f8970::function_2f8970()
{
	int entry_count = count;
	for (int i = 0; i < entry_count; i++)
	{
		s_2f8970_entry *entry = &entries[i];
		if (entry->active)
		{
			owner->release_id(entry->id);
		}
	}
	slot0(1);
}
