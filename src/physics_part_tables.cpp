// @flags /O2 /Gr
/* PHYSICS_PART_TABLES.CPP: the shape objects that biped ('bipd', at +0x264)
   and creature ('crea', at +0xd4) tags hold in place in three blocks
   (unknown_11c840.cpp constructs them after the tags load). The objects
   have virtual functions, so they must be constructed where they lie;
   unknown_0a9ef0.cpp asks the first one for its shape type (7) and its
   radius at +0xc. */

#include "unknown_11c920.h"
#include "unknown_26b230.h"
#include "globals.h"
#include <new>

/* a shape object: a virtual table, the value 0x80 at +6 and 0 at +8 */
class c_part
{
public:
	virtual ~c_part() {}
	c_part() : value08(0) {}
	byte unknown04[2];
	word value06;
	long value08;
};

/* the first shape of a pair */
class c_part_a : public c_part
{
public:
	virtual ~c_part_a();
	c_part_a() { value06 = 0x80; }
	byte unknown0c[4];
};

/* a shape that points at another */
class c_part_linked : public c_part
{
public:
	c_part_linked(c_part_a *a) : owner(a) {}
	c_part_a *owner;
};

/* the second shape of a pair, pointing at the first */
class c_part_b : public c_part_linked
{
public:
	virtual ~c_part_b();
	c_part_b(c_part_a *a) : c_part_linked(a) { value06 = 0x80; }
};

/* the shape of a single element */
class c_part_c : public c_part
{
public:
	virtual ~c_part_c();
	c_part_c() { value06 = 0x80; }
	byte unknown0c[0x24];
};

/* the destructors are never called: the objects live in tag data */
// @retail 0x1c2370
c_part_a::~c_part_a()
{
	__assume(0);
}

// @retail 0x1c2380
c_part_b::~c_part_b()
{
	__assume(0);
}

// @retail 0x1c2360
c_part_c::~c_part_c()
{
	__assume(0);
}

/* an element of the first and third blocks (0x80 bytes): a pair of shapes */
struct s_pair_element
{
	byte unknown00[0x20];
	c_part_a a;
	c_part_b b;
	byte unknown40[0x40];
};

/* an element of the second block (0x50 bytes): one shape */
struct s_single_element
{
	byte unknown00[0x20];
	c_part_c c;
};

typedef char assert_pair_element_size[(sizeof(s_pair_element) == 0x80) ? 1 : -1];
typedef char assert_single_element_size[(sizeof(s_single_element) == 0x50) ? 1 : -1];

/* the three blocks, at +0x20 of the tag's physics data */
struct s_part_table
{
	byte unknown00[0x20];
	long pair_a_count;
	s_pair_element *pairs_a;
	long single_count;
	s_single_element *singles;
	long pair_b_count;
	s_pair_element *pairs_b;

	void construct_shapes();
	c_part *get_shape(long index);
};

/* constructs every shape of the three blocks in place */
// @retail 0x1c2390
void s_part_table::construct_shapes()
{
	long i;

	for (i = 0; i < pair_a_count; i++)
	{
		s_pair_element *pair = &pairs_a[i];
		new (&pair->a) c_part_a;
		new (&pair->b) c_part_b(&pair->a);
	}
	for (i = 0; i < single_count; i++)
	{
		new (&singles[i].c) c_part_c;
	}
	for (i = 0; i < pair_b_count; i++)
	{
		s_pair_element *pair = &pairs_b[i];
		new (&pair->a) c_part_a;
		new (&pair->b) c_part_b(&pair->a);
	}
}

/* the shape of the given element: from the second block when it has any,
   otherwise the second shape of the third block's pair */
// @retail 0x1c2460
c_part *s_part_table::get_shape(long index)
{
	if (single_count)
	{
		return &singles[index].c;
	}
	return &pairs_b[index].b;
}
