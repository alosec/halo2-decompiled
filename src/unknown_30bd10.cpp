// @flags /O2 /GL-
// Havok (third-party physics library): clearing an entry of a list of
// pointers (the entry before the list when the pointer is not in it).

class c_1d1261
{
public:
	char unknown0[0x70];
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

	void function_30bd10(void *arg_0);
};

// @retail 0x30bd10
void c_1d1261::function_30bd10(void *arg_0)
{
	entries[find(arg_0)] = 0;
}
