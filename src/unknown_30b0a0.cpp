// @flags /O2 /GL-
// Havok (third-party physics library): removing a value from a list by
// moving the last one into its place, after telling the listeners whether
// it was there.

class class_30af80_listener;

struct bool_30af80
{
	bool value;

	bool_30af80(bool value_)
	{
		value = value_;
	}
	bool_30af80(bool_30af80 const &other)
	{
		value = other.value;
	}
};


class class_30af80
{
public:
	char unknown0[0x3c];
	class_30af80_listener **listeners;
	int listener_count;
	char unknown44[0x3c];
	long *values;
	int count;

	void function_30af80(long value, bool_30af80 flag);
	void function_30b0a0(long value);

	int find_30b0a0(long value) const
	{
		int n = count;
		for (int i = 0; i < n; i++)
		{
			if (values[i] == value)
			{
				return i;
			}
		}
		return -1;
	}
};

// @retail 0x30b0a0
void class_30af80::function_30b0a0(long value)
{
	int index = find_30b0a0(value);
	function_30af80(value, bool_30af80(index >= 0));
	if (index >= 0)
	{
		count--;
		values[index] = values[count];
	}
}
