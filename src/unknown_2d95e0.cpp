// @flags /O2 /GL-
// Havok (third-party physics library): two lists that continue a parent's
// lists: indices below the parent's count go to the parent.

struct s_2d95e0_entry
{
	float values[4];
};

class class_2d95e0
{
public:
	virtual void slot0() {}
	virtual void slot1() {}
	virtual void slot2() {}
	virtual void slot3() {}
	virtual void slot4() {}
	virtual void slot5() {}
	virtual long item_at(int index) { return 0; }
	virtual int get_item_count() { return 0; }
	virtual s_2d95e0_entry *get_entry(int index) { return 0; }
	virtual int get_entry_count() { return 0; }
};

class class_2d95e0_derived : public class_2d95e0
{
public:
	long field_4;
	long field_8;
	class_2d95e0 *parent;
	long field_10;
	long field_14;
	long *items;
	int item_count;
	s_2d95e0_entry *entries;
	int entry_count;

	long item_at(int index);
	int get_item_count();
	s_2d95e0_entry *get_entry(int index);
	int get_entry_count();
};

// @retail 0x2d95e0
long class_2d95e0_derived::item_at(int index)
{
	int parent_count = parent ? parent->get_item_count() : 0;
	if (index < parent_count)
	{
		return parent->item_at(index);
	}
	return items[index - parent_count];
}

// @retail 0x2d9620
int class_2d95e0_derived::get_item_count()
{
	return item_count + (parent ? parent->get_item_count() : 0);
}

// @retail 0x2d9650
s_2d95e0_entry *class_2d95e0_derived::get_entry(int index)
{
	int parent_count = parent ? parent->get_entry_count() : 0;
	if (index < parent_count)
	{
		return parent->get_entry(index);
	}
	return &entries[index - parent_count];
}

// @retail 0x2d9690
int class_2d95e0_derived::get_entry_count()
{
	return entry_count + (parent ? parent->get_entry_count() : 0);
}
