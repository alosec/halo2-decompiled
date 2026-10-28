// @flags /O2 /GL- /GX
#include "sized_reference_a9ef0.h"
#include "stream_holder_2d8e70.h"
// Retail table 0x411d20: deleting destructor and the two stream accessors.
class c_stream_pair_2d7650 : public c_sized_reference_a9ef0
{
public:
	virtual stream_holder_2d8e70 *first_stream();
	virtual stream_holder_2d8e70 *second_stream();
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((c_stream_pair_2d7650 *)object)->memory_size, 0x13);
	}
	stream_holder_2d8e70 first;
	stream_holder_2d8e70 second;
};
// @retail 0x2d7650
stream_holder_2d8e70 *c_stream_pair_2d7650::first_stream()
{
	return &first;
}
// @retail 0x2d7660
stream_holder_2d8e70 *c_stream_pair_2d7650::second_stream()
{
	return &second;
}
// @retail 0x2d7670 deleting c_stream_pair_2d7650
// @retail 0x2d76a0 destructor c_stream_pair_2d7650
