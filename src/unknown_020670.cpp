// @flags /O2 /Gr
#include "render_state_storage.h"
/* UNKNOWN_020670.CPP: the compiler's vector constructor iterator, and a
   bitmap slot release */

#include "unknown_11c920.h"
#include "visibility_slot.h"
#include <string.h>

typedef void *(__fastcall *constructor_proc)(void *);


// @retail 0x20670
void __stdcall vector_constructor_iterator(void *array, unsigned size, int count, constructor_proc constructor)
{
	while (--count >= 0)
	{
		constructor(array);
		array = (char *)array + size;
	}
}


// @retail 0x20e50
void function_20e50(long index)
{
	s_slot *slot = &g_51f40c.slots[index];
	long i = 0;

	do
	{
		g_51f40c.bitsets[i][index >> 5] &= ~(1 << (index & 0x1f));
		i++;
	}
	while (i < 3);

	memset(slot, 0, sizeof(s_slot));
	((dword *)slot)[1] |= 0xff80;
}

// @retail 0x2e1e0
void function_2e1e0(long object_index)
{
    if (object_index != NONE)
    {
        long index = object_index & 0xffff;
        for (long i = 0; i < g_51f40c.count; ++i)
        {
            s_slot *slot = &g_51f40c.slots[i];
            if (((dword *)slot)[0] & 0x10)
            {
                if (index == NONE || index == slot->d)
                    function_20e50(i);
            }
        }
    }
}

s_render_state_storage g_render_state_storage;
