// @flags /O1 /Gr
/* UNKNOWN_1A4742.CPP: the user interface heap ("ui memory pool", created by
   unknown_1a474c.cpp): its allocator object and the data arrays in it */

#include "unknown_11c920.h"
#include "data_array.h"
#include "screen_widgets.h"
#include "loop_allocator.h"

/* the allocator the user interface data arrays use (vtable 0x454a88) */
class c_user_interface_allocator : public c_data_allocator
{
public:
	virtual void *allocate(long size);
	virtual void deallocate(void *block);
};

c_user_interface_allocator g_47d92c;

#ifdef H2_NATIVE_PORT
extern "C" c_data_allocator *h2_native_user_interface_allocator()
{
	return &g_47d92c;
}
#endif

extern s_loop_allocator *g_51e998;
bool loop_allocate(s_loop_allocator *loop, void **pointer, long size, char const *file, long line);
void function_147ebe(void);
void *__stdcall function_1a47b1(long size, char const *file, long line);

// @retail 0x1a4742
void *c_user_interface_allocator::allocate(long size)
{
	return function_1a47fd(size);
}

// @retail 0x1a4747
void c_user_interface_allocator::deallocate(void *block)
{
	function_1a4826(block);
}

/* allocates from the heap; when it is full, frees what it can and tries once
   more */
// @retail 0x1a47b1
void *__stdcall function_1a47b1(long size, char const *file, long line)
{
	void *pointer;

	if (!loop_allocate(g_51e998, &pointer, size, file, line))
	{
		function_147ebe();
		if (!loop_allocate(g_51e998, &pointer, size, file, line))
		{
			pointer = 0;
		}
	}
	return pointer;
}

// @retail 0x1a47fd
void *__stdcall function_1a47fd(unsigned int size)
{
	return function_1a47b1(size, 0, 0);
}

// @retail 0x1a480d
s_record_pool *user_interface_data_new(const char *name, long maximum_count, long size)
{
	return data_new(name, maximum_count, size, 0, &g_47d92c);
}
