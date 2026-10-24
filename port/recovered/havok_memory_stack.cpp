// @flags /O2 /GL- /GX
// Havok (third-party physics library): the stack area of the memory object
// Havok allocates from (g_4798b0, the game's, unknown_1470f0.cpp). Havok's
// local arrays take their storage from the top of this area and give it back
// in reverse order (include/reference_counted_2d88a0.h, allocate_stack). The
// area is a 0x4000-byte buffer at 0x4798d0, which retail installs from a
// static initialiser (0x3772d0) right after the object itself; a request that
// does not fit takes a chunk of its own from the memory (vtable slots 9 and
// 10 of the memory object, 0x2d7020 and 0x2d7090, after the destructor at
// slot 8; the table is 0x45378c), saving the area it replaces in the chunk's
// first 0x10 bytes.
//
// The initialiser must run after g_4798b0's own constructor, which clears the
// stack fields: the whole image runs the src/ units' static constructors
// (.guest_init) before the retail table.

#include "unknown_11c920.h"
#include "reference_counted_2d88a0.h"

// the memory object's stack fields, from +8
struct memory_stack_2d6fc0
{
	char *cursor;       // +8, the next free byte
	long free;          // +0xc, bytes left
	void *chunk;        // +0x10, the chunk the area lives in, 0 for the buffer
	char *base;         // +0x14, the area's first byte
};

// what the memory object is as seen from these functions: its vtable and
// reference count, then the stack fields
class memory_view_2d6fc0
{
public:
	void *vtable;
	long references;
	memory_stack_2d6fc0 stack;

	void set_stack_area(char *buffer, long size);
	void *grow_stack(long size);
	void shrink_stack(void *block, long size);
};

// the object itself (g_4798b0) and the buffer of its stack area
extern memory_view_2d6fc0 g_4798b0_memory_view;
extern char g_4798d0[0x4000];

// @retail 0x2d6fc0
void memory_view_2d6fc0::set_stack_area(char *buffer, long size)
{
	stack.cursor = buffer;
	stack.free = size;
}

/* takes a chunk of at least 0x1000 bytes for a block that does not fit what
   is left of the area, and makes the chunk the area */
// @retail 0x2d7020
void *memory_view_2d6fc0::grow_stack(long size)
{
	long chunk_size = size > 0x1000 ? size : 0x1000;
	memory_stack_2d6fc0 *saved = (memory_stack_2d6fc0 *)g_memory_2d88a0->allocate_block(chunk_size + 0x10, 0x12);
	*saved = stack;
	char *area = (char *)(saved + 1);
	stack.free = chunk_size - size;
	stack.chunk = saved;
	stack.base = area;
	stack.cursor = area + size;
	return area;
}

/* gives back the chunk 0x2d7020 took, and the area it replaced */
// @retail 0x2d7090
void memory_view_2d6fc0::shrink_stack(void *block, long size)
{
	(void)block;
	memory_stack_2d6fc0 *saved = (memory_stack_2d6fc0 *)stack.base - 1;
	long chunk_size = stack.free + size + 0x10;
	stack = *saved;
	g_memory_2d88a0->release_block(saved, chunk_size, 0x12);
}

// run from the CRT's table in retail, and by h2_run_retail_initializers
// (retail_initializers_generated.cpp) in the port
// @retail 0x3772d0
void __cdecl function_3772d0()
{
	g_4798b0_memory_view.set_stack_area(g_4798d0, sizeof(g_4798d0));
}
