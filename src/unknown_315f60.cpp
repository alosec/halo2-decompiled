// @flags /O2 /GL-
// Havok (third-party physics library): adding and taking away usage
// figures.

struct s_315f60
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

inline long maximum_315f60(long a, long b)
{
	return a > b ? a : b;
}

class class_315f60
{
public:
	long field_0;
	long field_4;
	long peak;
	long field_c;
	long field_10;
	long field_14;
	long field_18;

	void add(long unused, const s_315f60 *amount);
	void remove(long unused, const s_315f60 *amount);
	void add_14(long amount);
	void remove_14(long amount);
};

// @retail 0x315f60
void class_315f60::add(long unused, const s_315f60 *amount)
{
	peak = maximum_315f60(peak, amount->field_0);
	peak = maximum_315f60(peak, amount->field_4);
	field_c += amount->field_4;
	field_10 += amount->field_8;
	field_18 += amount->field_c;
}

// @retail 0x315fb0
void class_315f60::remove(long unused, const s_315f60 *amount)
{
	field_c -= amount->field_4;
	field_10 -= amount->field_8;
	field_18 -= amount->field_c;
}

// @retail 0x315fe0
void class_315f60::add_14(long amount)
{
	field_14 += amount;
}

// @retail 0x315ff0
void class_315f60::remove_14(long amount)
{
	field_14 -= amount;
}
