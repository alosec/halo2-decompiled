// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0E5790.CPP: a biped's physics changes to mode 3 (0xe4a20 calls it
   when the biped lands or is knocked down); a biped that 0xe4050 accepts also
   clears the two flags after the physics state (+0x3f8, +0x3f9) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"

struct s_e5790_object_header
{
    byte unknown00[8];
    byte *biped;
};

void function_1e54d0(void *state, long mode);
bool function_e4050(long object_index);

// @retail 0xe5790
void function_e5790(long object_index)
{
    byte *biped = ((s_e5790_object_header *)g_4e0300->data)[object_index & 0xffff].biped;

    function_1e54d0(biped + 0x3dc, 3);
    if (function_e4050(object_index))
    {
        biped[0x3f9] = 0;
        biped[0x3f8] = 0;
    }
}
