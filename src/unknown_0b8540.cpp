#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /Gr

struct s_object_header_b8540
{
    short identifier;
    byte flags;
    byte type;
    short cluster_index;
    byte unknown06[2];
    byte *object;
};

bool object_or_parent_hidden(long object_index);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long remove, long add,
    long siblings, long own_flags);
void function_b8b70(long object_index);
void function_bf090(long object_index);
void __stdcall function_10a250(long object_index);
void function_15b220(long object_index, long index);
void __stdcall function_109400(long object_index);
void function_bb950(long object_index, bool add, long delta);
void function_a7a60(long object_index);
void function_10ace0(long object_index);

// @retail 0xb8540
void __stdcall function_b8540(long object_index)
{
    s_object_header_b8540 *header =
        &((s_object_header_b8540 *)g_4e0300->data)[object_index & 0xffff];
    byte *object = header->object;

    if (!object_or_parent_hidden(object_index) && function_b9d20(object_index))
        function_bef30(object_index, true, false, false, false);

    header->flags |= 0x10;
    function_b8b70(object_index);
    if ((1 << header->type) & 0x40)
        function_10a250(object_index);
    function_bf090(object_index);

    *(long *)(object + 0xa4) = NONE;
    *(short *)(object + 0xa8) = NONE;
    *(byte *)(object + 0xab) = NONE;
    char index = *(char *)(object + 0xaf);
    if (index != NONE)
        function_15b220(object_index, index);

    function_109400(object_index);
    function_bb950(object_index, false, NONE);
    function_a7a60(object_index);
    function_10ace0(object_index);
}
