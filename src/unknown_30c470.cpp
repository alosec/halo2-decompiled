// @flags /O2 /GL-
// Havok (third-party physics library): clearing an entry of a list of
// pointers (the entry before the list when the pointer is not in it).

class c_library_30c470
{
public:
	char unknown0[0x6c];
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

	void detach(void *callback);
};

// @retail 0x30c470
void c_library_30c470::detach(void *callback)
{
	entries[find(callback)] = 0;
}
