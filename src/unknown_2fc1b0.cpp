// @flags /O2 /GL-
// Havok (third-party physics library): stepping through a list (-1 after
// the last) and reading an entry's second word.

struct s_2fc1b0_entry
{
	long field_0;
	long field_4;
};

class class_2fc1b0
{
public:
	long field_0;
	long field_4;
	long field_8;
	s_2fc1b0_entry *entries;
	int count;

	int next(int index) const;
	long get(int index) const;
};

// @retail 0x2fc1b0
int class_2fc1b0::next(int index) const
{
	index++;
	if (index >= count)
	{
		index = -1;
	}
	return index;
}

// @retail 0x2fc1e0
long class_2fc1b0::get(int index) const
{
	return entries[index].field_4;
}
