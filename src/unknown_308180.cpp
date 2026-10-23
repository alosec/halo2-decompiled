// @flags /O2 /GL-
// Havok (third-party physics library): a list of up to eight id pairs:
// removing one by moving the last into its place, and the constructor.

struct s_308180_entry
{
	unsigned short first;
	unsigned short second;
	unsigned short field_4;
};

class class_308180
{
public:
	char unknown0[0x20];
	s_308180_entry entries[8];
	long field_50;
	int count;
	bool changed;

	void remove(int index, long a, long b);
	class_308180();
};

// @retail 0x308180
void class_308180::remove(int index, long a, long b)
{
	s_308180_entry *last = &entries[count - 1];
	changed = false;
	entries[index] = *last;
	count--;
}

// @retail 0x3081b0
// the eight entries are cleared one after another, walking a pointer: retail
// has no loop here
class_308180::class_308180()
{
	s_308180_entry *e = entries;
	e->field_4 = 0;
	e++;
	e->field_4 = 0;
	e++;
	e->field_4 = 0;
	e++;
	e->field_4 = 0;
	e++;
	e->field_4 = 0;
	e++;
	e->field_4 = 0;
	e++;
	e->field_4 = 0;
	e++;
	e->field_4 = 0;
	e++;
	count = 0;
	field_50 = 0;
	changed = false;
}
