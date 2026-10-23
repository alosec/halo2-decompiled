// @flags /O2 /GL-
// Havok (third-party physics library): clearing an entry of a list of
// pointers (the entry before the list when the pointer is not in it).

class c_world_callback_registration
{
public:
	char unknown0[0x78];
	void **entries;
	int entry_count;

	int find(void *entry) const
	{
		for (int i = 0; i < entry_count; i++)
		{
			if (entries[i] == entry)
			{
				return i;
			}
		}
		return -1;
	}

	void remove(void *callback);
};

// @retail 0x30c3e0
void c_world_callback_registration::remove(void *callback)
{
	entries[find(callback)] = 0;
}
