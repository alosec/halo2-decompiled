// @flags /O2 /GL-
// Havok (third-party physics library): tells each side of a new or removed
// pair about the other, when that side is of the kind that listens (2).

struct flag_315e70
{
	bool value;

	flag_315e70(bool value_)
	{
		value = value_;
	}

	flag_315e70(flag_315e70 const &other)
	{
		value = other.value;
	}
};

class entity_315e70;

class handler_315e70
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void pair_added(entity_315e70 *other, flag_315e70 flag) {}
	virtual void pair_removed(entity_315e70 *other) {}
};

struct head_315e70
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

struct collidable_315e70
{
	long field_0;
	long field_4;
	long type;
	long field_c;
};

class entity_315e70 : public head_315e70, public collidable_315e70
{
public:
	handler_315e70 *handler;
};

struct pair_315e70
{
	collidable_315e70 *a;
	collidable_315e70 *b;
};

class dispatcher_315e70
{
public:
	void pair_added(pair_315e70 const *pair);
	void pair_removed(pair_315e70 const *pair);
};

// @retail 0x315e70
void dispatcher_315e70::pair_added(pair_315e70 const *pair)
{
	if (pair->a->type == 2)
	{
		entity_315e70 *a = static_cast<entity_315e70 *>(pair->a);
		entity_315e70 *b = static_cast<entity_315e70 *>(pair->b);
		handler_315e70 *handler = a->handler;
		handler->pair_added(b, flag_315e70(false));
	}
	if (pair->b->type == 2)
	{
		entity_315e70 *a = static_cast<entity_315e70 *>(pair->a);
		entity_315e70 *b = static_cast<entity_315e70 *>(pair->b);
		handler_315e70 *handler = b->handler;
		handler->pair_added(a, flag_315e70(false));
	}
}

// @retail 0x315ef0
void dispatcher_315e70::pair_removed(pair_315e70 const *pair)
{
	if (pair->a->type == 2)
	{
		entity_315e70 *a = static_cast<entity_315e70 *>(pair->a);
		entity_315e70 *b = static_cast<entity_315e70 *>(pair->b);
		a->handler->pair_removed(b);
	}
	if (pair->b->type == 2)
	{
		entity_315e70 *a = static_cast<entity_315e70 *>(pair->a);
		entity_315e70 *b = static_cast<entity_315e70 *>(pair->b);
		b->handler->pair_removed(a);
	}
}
