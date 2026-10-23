// @flags /O2 /Gr
#include "view_context_storage.h"
/* UNKNOWN_03EB20.CPP: whether a cluster is the one the current zone is in.
   Decompiled by lane R for the effects (0x178bc0, 0x1785c0, 0x179730). */

#include "unknown_11c920.h"
#include "globals.h"


struct s_3eb20_bsp
{
	byte unknown00[0xac];
	long cluster_count;
	short *clusters;
};

static __forceinline bool cluster_selection_matches(long cluster_index, long index)
{
    if (index != NONE && g_4686c4 != NONE)
    {
        s_3eb20_bsp *bsp = (s_3eb20_bsp *)g_4e0348;
        if (bsp->cluster_count > 0 && bsp->clusters[index] == cluster_index)
            return true;
    }
    return false;
}

__declspec(noinline) bool function_3eb20(long cluster_index);

// @retail 0x3eb20
bool function_3eb20(long cluster_index)
{
    bool result = false;
    if (cluster_index != NONE && g_4b9ee9)
    {
        if (cluster_selection_matches(cluster_index, g_4b9eec))
            result = true;
    }
    return result;
}
