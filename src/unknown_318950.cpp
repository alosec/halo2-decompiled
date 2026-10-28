// @flags /O2 /GL-
#include "reference_base_318970.h"
// Release a contact-manager block using its recorded size and tag 0x1d.
// @retail 0x318950
void __cdecl function_318950(reference_base_318970 *object)
{
	g_memory_2d88a0->release_block(object,object->memory_size,0x1d);
}
