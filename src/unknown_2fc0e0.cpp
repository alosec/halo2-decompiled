// @flags /O2 /GL-
// Havok (third-party physics library): setting an object from a
// description: its first value, and its entries' values when the counts
// agree.

struct class_2df1f0
{
	char unknown0[8];
	long field_8;

	void function_2df200(long const *value);
};

struct entry_2fc0e0
{
	long key;
	long value;
};

struct s_2fc0e0
{
	long field_0;
	char field_4[0xc];
	long *values;
	int count;
};

class class_2fc0e0
{
public:
	char field_0[0xc];
	entry_2fc0e0 *entries;
	int count;

	void function_2fc0e0(s_2fc0e0 const *arg_0);
};

// @retail 0x2fc0e0
void class_2fc0e0::function_2fc0e0(s_2fc0e0 const *arg_0)
{
	((class_2df1f0 *)this)->function_2df200(&arg_0->field_0);
	int n = count;
	if (arg_0->count == n)
	{
		for (int i = 0; i < n; i++)
		{
			entries[i].value = arg_0->values[i];
		}
	}
}
