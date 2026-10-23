// @flags /O2 /GL-
// Havok (third-party physics library): adding an entry to a table of at most
// eight unless it is there already, giving its index or -1.

struct bool_3058b0
{
	bool value;

	bool_3058b0(bool value_)
	{
		value = value_;
	}
};

struct s_3058b0_pair
{
	unsigned short first;
	unsigned short second;
};

struct s_3058b0_entry
{
	unsigned short first;
	unsigned short second;
	unsigned short field_4;
};

class class_3058b0
{
public:
	char unknown0[0x20];
	s_3058b0_entry entries[8];
	long field_50;
	int count;

	bool_3058b0 contains(const s_3058b0_pair *pair) const;
	int function_308200(long arg_0, long arg_1, const s_3058b0_entry *entry);
};

// @retail 0x308200
int class_3058b0::function_308200(long arg_0, long arg_1, const s_3058b0_entry *entry)
{
	int index = count;
	if (index > 8)
	{
		return -1;
	}
	if (!contains((const s_3058b0_pair *)entry).value && index < 8)
	{
		entries[index] = *entry;
		count++;
		return index;
	}
	return -1;
}
