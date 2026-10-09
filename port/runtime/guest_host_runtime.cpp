#include "h2/host_abi.h"
#include <math.h>
#include <xtl.h>

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

struct short_rectangle2d;
long unicode_escape_character_lookup(unsigned short character, bool *found);

class c_1fa50
{
public:
    void function_1fa50(short_rectangle2d const *bounds, void const *clip,
                        void const *position, long line_gap, float scale,
                        long color, void const *shadow) const;
};

// @retail 0x1fa50, native renderer boundary
void c_1fa50::function_1fa50(short_rectangle2d const *bounds, void const *clip,
                             void const *position, long line_gap, float scale,
                             long color, void const *shadow) const
{
    unsigned int text[2048];
    const auto *source = reinterpret_cast<const unsigned short *>(this);
    long remaining = 2048;
    unsigned int *destination = text;
    do
    {
        if (remaining == 1)
        {
            *destination = 0;
            break;
        }
        const unsigned short *next = source + 1;
        const unsigned short first = *source;
        long value;
        if (first == 0x7c)
        {
            bool recognized = false;
            value = unicode_escape_character_lookup(*next, &recognized);
            if (recognized)
                ++next;
        }
        else
        {
            value = first;
        }
        source = next;
        *destination = static_cast<unsigned int>(value);
        if (!value)
            break;
        ++destination;
        --remaining;
    } while (remaining > 0);

    host_h2_text_request(text, bounds, clip, position, static_cast<int>(line_gap),
                         scale, static_cast<unsigned int>(color), shadow);
}

extern "C" DWORD __stdcall GetTickCount(void)
{
    return host_h2_get_milliseconds();
}

extern "C" DWORD __stdcall XGetDiskClusterSize(const char *)
{
    /* Halo 2 accounts saved-game storage in 16 KiB blocks.  The native port's
     * virtual saved-game drive uses one cluster per accounting block. */
    return 0x4000;
}

extern "C" void __stdcall Sleep(DWORD milliseconds)
{
    host_h2_sleep(milliseconds);
}

extern "C" double cos(double value)
{
    return host_h2_cos(value);
}
