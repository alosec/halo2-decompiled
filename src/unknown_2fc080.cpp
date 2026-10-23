// @flags /O2 /GL-
// Havok (third-party physics library): two queries passed on to the object
// held at +0xc.

struct s_2fc080_result
{
	float values[4];
};

struct s_2fc080_range
{
	long field_0;
	long field_4;
};

struct s_2fc080_input
{
	long field_0;
	s_2fc080_range range;
};

class class_2fc080_target;

// the body a query is built on: the shape it hit, its key, where it sits and
// the body it was made from
class c_2fc080_query
{
public:
	class_2fc080_target *shape;
	long key;
	long placement;
	const s_2fc080_input *parent;

	__forceinline c_2fc080_query(const s_2fc080_input *body)
	{
		parent = body;
		placement = body->range.field_4;
	}

	__forceinline void set_shape(class_2fc080_target *value)
	{
		key = parent->range.field_0;
		shape = value;
	}
};

class class_2fc080_target
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void slot4() = 0;
	virtual void slot5() = 0;
	virtual void slot6() = 0;
	virtual void slot7() = 0;
	virtual s_2fc080_result slot8(long a, long b) = 0;
	virtual void slot9(long a, const c_2fc080_query *query, long c) = 0;
};

class class_2fc080
{
public:
	char unknown0[0xc];
	class_2fc080_target *target;

	s_2fc080_result function_2fc080(long a, long b);
	void function_2fc0a0(long a, const s_2fc080_input *input, long c);
};

// @retail 0x2fc080
s_2fc080_result class_2fc080::function_2fc080(long a, long b)
{
	return target->slot8(a, b);
}

// @retail 0x2fc0a0
void class_2fc080::function_2fc0a0(long a, const s_2fc080_input *input, long c)
{
	c_2fc080_query query(input);
	query.set_shape(target);
	target->slot9(a, &query, c);
}
