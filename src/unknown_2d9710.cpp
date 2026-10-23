// @flags /O2 /GL-
// Havok (third-party physics library): whether an object appears in the
// chain that starts at another object's first link.

struct bool_2d9710
{
	bool value;

	bool_2d9710(bool value_)
	{
		value = value_;
	}
};

class class_2d9710
{
public:
	long field_0;
	long field_4;
	long field_8;
	class_2d9710 *next;
};

class class_2d9710_owner
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual class_2d9710 *get_first() = 0;
};

class class_2d9710_link : public class_2d9710
{
public:
	bool_2d9710 is_in(class_2d9710_owner *owner) const;
};

// @retail 0x2d9710
bool_2d9710 class_2d9710_link::is_in(class_2d9710_owner *owner) const
{
	for (class_2d9710 *link = owner->get_first(); link; link = link->next)
	{
		if (link == this)
		{
			return bool_2d9710(true);
		}
	}
	return bool_2d9710(false);
}
