// Exact process-lifetime null allocator used by preallocated sound data arrays.
// Retail g_46875c points at the second four-byte allocator object at 0x479864;
// its two-slot table begins at 0x453300 and rejects allocation while making
// deallocation a no-op.  Direct initialization avoids a host constructor and
// does not claim ownership of the separate static sound arena.

#include "data_array.h"

#ifdef H2_NATIVE_PORT

using h2_sound_data_allocator_method = void (*)(void);

extern "C" void *h2_sound_data_allocator_allocate(void *self, long size)
{
	(void)self;
	(void)size;
	return 0;
}

extern "C" void h2_sound_data_allocator_deallocate(void *self, void *block)
{
	(void)self;
	(void)block;
}

extern "C" h2_sound_data_allocator_method const h2_sound_data_allocator_vtable[2] =
{
	reinterpret_cast<h2_sound_data_allocator_method>(h2_sound_data_allocator_allocate),
	reinterpret_cast<h2_sound_data_allocator_method>(h2_sound_data_allocator_deallocate),
};

struct s_sound_data_allocator_object
{
	h2_sound_data_allocator_method const *vtable;
};

static_assert(sizeof(s_sound_data_allocator_object) == 0x04,
	"sound data allocator object size");
static_assert(sizeof(h2_sound_data_allocator_vtable) == 0x08,
	"sound data allocator vtable size");

extern "C" s_sound_data_allocator_object h2_sound_data_allocator_object =
{
	h2_sound_data_allocator_vtable,
};

c_data_allocator *g_46875c =
	reinterpret_cast<c_data_allocator *>(&h2_sound_data_allocator_object);

#endif
