// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0E6460.CPP: an object whose definition names a character (+0x30c)
   and sets bit 8 of its flags (+0x1f0) passes its position, that character
   and the definition's mode (+0x310) to 0x1e0fe0 (damage.cpp's 0xd6bc0 calls
   it) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"

struct s_e6460_object_header
{
    byte unknown00[8];
    byte *object;
};

point3f *function_b9dd0(long object_index, point3f *result);
long function_1e0fe0(point3f const *point, long unit_index, long character_index, short mode);

// @retail 0xe6460
void __stdcall function_e6460(long object_index)
{
    byte *object = ((s_e6460_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    long character_index = *(long *)(definition + 0x30c);

    if (character_index != NONE && TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 8) & 1))
    {
        point3f position;

        function_b9dd0(object_index, &position);
        function_1e0fe0(&position, object_index, character_index, *(word *)(definition + 0x310));
    }
}
