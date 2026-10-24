// @flags /O2 /GL-
// Library code: the base of the page heaps and page allocators (vtable
// 0x41661c; slots 0x329d30, 0x329d60, 0x329d80 and the destructor
// 0x329e20). Its constructor (0x329d20, the base of the network page heap,
// src/unknown_081780.cpp) and its destructor (0x329e10, the base destructor
// of c_page_heap, include/unknown_1a1940.h) only store that table.
//
// The table is the retail one, stored by address.
#include "unknown_11c920.h"
#include "unknown_1a1940.h"

// as src/unknown_081780.cpp declares it
class c_library_329d20
{
public:
	c_library_329d20();
	virtual void v0() {}
};

// @retail 0x329d20
c_library_329d20::c_library_329d20()
{
	*(unsigned long *)this = 0x41661c;
}

// @retail 0x329e10
c_page_allocator::~c_page_allocator()
{
	*(unsigned long *)this = 0x41661c;
}
