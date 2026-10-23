// @flags /O2 /GL-
// Havok (third-party physics library): whether a pair of ids is listed.

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
};

// @retail 0x3058b0
bool_3058b0 class_3058b0::contains(const s_3058b0_pair *pair) const
{
	for (int i = count - 1; i >= 0; i--)
	{
		if (entries[i].first == pair->first && entries[i].second == pair->second)
		{
			return bool_3058b0(true);
		}
	}
	return bool_3058b0(false);
}
