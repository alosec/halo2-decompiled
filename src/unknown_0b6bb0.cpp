#include "unknown_11c920.h"
#include "globals.h"
#include "object_iterator.h"
#include "object_queries.h"

// @flags /O2 /Gr

struct s_object_list;
extern s_object_list *g_4de2f4;

bool function_bf510(long object_index, s_location *location);
void __stdcall function_b8600(long object_index, long location);
void function_108960(long object_index);
void function_bfcc0(void);
void function_c00a0(void);

// @retail 0xb6bb0
void function_b6bb0(void)
{
    *(byte *)g_4de2f4 = true;
    s_type_f1af8e iterator;
    function_bae80(&iterator, NONE, 0);
    for (byte *object = (byte *)function_baeb0(&iterator); object;
        object = (byte *)function_baeb0(&iterator))
    {
        if (*(long *)(object + 0x14) == NONE &&
            !((*(dword *)(object + 4) >> 7) & 1))
        {
            s_location location;
            if (function_bf510(iterator.object_index, &location))
                function_b8600(iterator.object_index, (long)&location);
        }
        function_108960(iterator.object_index);
    }
    function_bfcc0();
    function_c00a0();
}
