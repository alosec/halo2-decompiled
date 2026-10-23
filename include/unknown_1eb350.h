#ifndef UNKNOWN_1EB350_H
#define UNKNOWN_1EB350_H

#include "unknown_11c920.h"
#include "globals.h"

class c_havok_reference_counted;
struct s_244980_ray;
struct s_244980_output;
class c_244c10;

struct c_shape_library_base_a
{
	c_shape_library_base_a() {}
	c_shape_library_base_a(c_havok_reference_counted *arg_0, long arg_1);
	virtual ~c_shape_library_base_a();
	word allocation_size;
	word references;
	byte field_8[0x14 - 8];
	void function_2df720(s_244980_ray const *ray, s_244980_output *output, c_244c10 *callback);

	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_shape_library_base_a *)block)->allocation_size, 0x22);
	}
};

struct c_shape_library_base_b
{
	virtual ~c_shape_library_base_b();
	word allocation_size;
	word references;

	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_shape_library_base_b *)block)->allocation_size, 0x22);
	}
};

#endif
