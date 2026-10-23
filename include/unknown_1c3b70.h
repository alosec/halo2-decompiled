#ifndef UNKNOWN_1C3B70_H
#define UNKNOWN_1C3B70_H
#include "unknown_1efac0.h"
#include "havok_reference.h"
struct c_shape_counted_base : c_a
{
	c_shape_counted_base() { unknown06 = 1; }
};

struct s_shape_four_values
{
	volatile real x, y, z, w;
};

struct c_shape_global_owner : c_shape_counted_base
{
	long field_8;
	long field_c;
	s_shape_four_values center;
	s_shape_four_values extent;
	c_shape_global_owner();
	virtual ~c_shape_global_owner();
 virtual void v5() {}
 virtual void v6() {}
 virtual void v7() {}
 virtual void v8() {}
 virtual void v9() {}
 virtual void v10() {}
 virtual void v11() {}
 virtual dword next_key(dword key);
 virtual void *make_shape(dword key, void *storage);
	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_shape_global_owner *)block)->flags, 0x22);
	}
};

class c_1c5910 : public c_a
{
public:
	long field_8;
	c_havok_reference_counted *field_c;
	virtual ~c_1c5910();
	static void operator delete(void *arg_0)
	{
		g_480118->allocate((long)arg_0, ((c_1c5910 *)arg_0)->flags, 0x22);
	}
};

#endif
