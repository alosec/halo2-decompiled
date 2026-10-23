// @flags /O2 /GL-
// Havok (third-party physics library): the entry of a list with the
// smallest value.

struct entry_309120
{
	char unknown0[0x10];
	float value;
	char unknown14[0x3c];
};

class list_309120
{
public:
	char unknown0[0x1b0];
	entry_309120 *entries;
	long count;

	entry_309120 *function_309120() const;
};

// the first entry with the smallest value
// @retail 0x309120
entry_309120 *list_309120::function_309120() const
{
	entry_309120 *entry = entries;
	float smallest = entry->value;
	entry_309120 *best = entry;
	for (long i = count - 2; i >= 0; i--)
	{
		entry++;
		float const value = entry->value;
		if (value < smallest)
		{
			smallest = value;
			best = entry;
		}
	}
	return best;
}
