// @flags /O2 /GL-
// Havok (third-party physics library): takes an entry off an object's list,
// clears the entry's owner field and marks the list changed.

struct entry_3169c0
{
	char pad_0[0x10];
	void *owner;
};

struct entries_3169c0
{
	entry_3169c0 **data;
	long size;
	long capacity;

	int index_of(entry_3169c0 const *entry) const
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

	void erase_slot(int index)
	{
		size--;
		for (int i = index; i < size; i++)
		{
			data[i] = data[i + 1];
		}
	}
};

class list_owner_3169c0
{
public:
	char pad_0[0x28];
	bool changed;
	char pad_29[0x23];
	entries_3169c0 entries;

	void function_3169c0(entry_3169c0 *entry);
};

// @retail 0x3169c0
void list_owner_3169c0::function_3169c0(entry_3169c0 *entry)
{
	entries.erase_slot(entries.index_of(entry));
	entry->owner = 0;
	changed = true;
}
