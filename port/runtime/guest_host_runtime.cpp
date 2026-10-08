#include "h2/host_abi.h"

/* Keep Microsoft-mangled recovered entry points inside the ILP32 guest image.
 * The C host ABI receives rebased native pointers through the guest compiler
 * adapter, so the LP64 host never needs to decode a Microsoft C++ call. */

// @retail 0x18f1c0
void __stdcall function_18f1c0(long command)
{
    host_h2_video_control((int)command);
}

// @retail 0x52040
void function_52040(void const *request, void const *vertices)
{
    host_h2_render_request(request, vertices);
}
