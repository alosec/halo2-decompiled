#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /Gr

struct s_object_header_bef30
{
    short identifier;
    byte flags;
    byte type;
    short cluster_index;
    byte unknown06[2];
    byte *object;
};

struct s_object_attachment_bef30
{
    byte type;
    byte unknown01[3];
    long datum_index;
};

bool function_b9ce0(long object_index);
void function_c3220(long datum_index);
void __stdcall function_c2d00(long datum_index);
void __stdcall function_c1720(long object_index, bool remove, bool add);

// @retail 0xbef30
void __stdcall function_bef30(long object_index, long remove, long add,
    long siblings, long own_flags)
{
    while (true)
    {
        s_object_header_bef30 *header =
            &((s_object_header_bef30 *)g_4e0300->data)[object_index & 0xffff];
        byte *object = header->object;
        bool parent_active;
        if (own_flags)
        {
            if ((header->flags & 0x10) || (*(dword *)(object + 4) & 1))
                return;
            parent_active = false;
        }
        else
        {
            parent_active = function_b9ce0(object_index);
            if (parent_active)
                return;
        }

        if ((*(dword *)(object + 4) >> 4) & 1)
        {
            long count = *(short *)(object + 0x11c) / sizeof(s_object_attachment_bef30);
            s_object_attachment_bef30 *attachment =
                (s_object_attachment_bef30 *)(object + *(short *)(object + 0x11e));
            while (count-- > 0)
            {
                if (attachment->type == 0 && attachment->datum_index != NONE && !parent_active)
                {
                    if (remove)
                        function_c3220(attachment->datum_index);
                    if (add)
                        function_c2d00(attachment->datum_index);
                }
                ++attachment;
            }
        }

        if ((*(dword *)(object + 4) >> 6) & 1)
            function_c1720(object_index, (bool)remove, (bool)add);

        long child = *(long *)(object + 0x10);
        if (child != NONE)
            function_bef30(child, remove, add, true, !parent_active);

        if (!siblings)
            return;
        object_index = *(long *)(object + 0x0c);
        if (object_index == NONE)
            return;
    }
}
