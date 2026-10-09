#include "h2/host_abi.h"
#include <math.h>
#include <xtl.h>

/* Keep Microsoft-mangled recovered entry points inside the ILP32 guest image.
 * The C host ABI receives rebased native pointers through the guest compiler
 * adapter, so the LP64 host never needs to decode a Microsoft C++ call. */

extern "C" DWORD __stdcall XGetLanguage()
{
    return 1; /* English */
}

long function_11ca80(long value)
{
    switch (value)
    {
    case 2: return 1;
    case 3: return 2;
    case 4: return 3;
    case 5: return 4;
    case 6: return 5;
    case 7: return 6;
    case 8: return 7;
    default: return 0;
    }
}

/* The native host has no attract-movie decoder yet. */
bool function_223976()
{
    return false;
}

void function_223a21()
{
}

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

struct s_save_list_2164c0;

void function_2164c0(long, s_save_list_2164c0 *list)
{
    /* The native virtual saved-game drive starts with no user-created files.
     * Preserve the recovered list ABI while host persistence is unavailable. */
    if (list)
        *reinterpret_cast<long *>(list) = 0;
}

extern "C" BOOL __stdcall GetDiskFreeSpaceExA(const char *, ULARGE_INTEGER *available,
                                                ULARGE_INTEGER *total,
                                                ULARGE_INTEGER *free_bytes)
{
    /* Fixed capacity for the native virtual saved-game drive. */
    constexpr unsigned long long capacity = 8ull * 1024ull * 1024ull * 1024ull;
    if (available)
        available->QuadPart = capacity;
    if (total)
        total->QuadPart = capacity;
    if (free_bytes)
        free_bytes->QuadPart = capacity;
    return TRUE;
}

struct s_player_profile_settings;
struct s_screen_parameters;
class c_class_1473c9;

#define H2_NATIVE_UNAVAILABLE_SCREEN(name) \
    c_class_1473c9 *__stdcall name(s_screen_parameters *) { return 0; }

/* These secondary screen families are not part of the static native menu
 * bootstrap yet. Their recovered callers remain intact, while an attempted
 * transition fails closed instead of constructing a partial object/vtable. */
H2_NATIVE_UNAVAILABLE_SCREEN(function_230616)
H2_NATIVE_UNAVAILABLE_SCREEN(function_230691)
H2_NATIVE_UNAVAILABLE_SCREEN(function_2310b7)
H2_NATIVE_UNAVAILABLE_SCREEN(function_231995)
H2_NATIVE_UNAVAILABLE_SCREEN(function_231db5)
H2_NATIVE_UNAVAILABLE_SCREEN(function_2320c4)
H2_NATIVE_UNAVAILABLE_SCREEN(function_2323c3)
H2_NATIVE_UNAVAILABLE_SCREEN(function_23246a)
H2_NATIVE_UNAVAILABLE_SCREEN(function_23252e)
H2_NATIVE_UNAVAILABLE_SCREEN(function_2325fb)
H2_NATIVE_UNAVAILABLE_SCREEN(function_23334f)
H2_NATIVE_UNAVAILABLE_SCREEN(function_24b4a9)
#undef H2_NATIVE_UNAVAILABLE_SCREEN

void function_18fb34(long, s_player_profile_settings *, long)
{
    /* No persisted profile can be applied during empty native bootstrap. */
}

void function_215367(long, long, void *, long)
{
    /* Native saved-game task storage is not implemented yet. */
}

void function_2153dd(long, long, s_player_profile_settings *, long)
{
    /* Native profile persistence is not implemented yet. */
}

void function_2157c0(long)
{
    /* No asynchronous native save operation can complete yet. */
}

void __stdcall function_215e60(long)
{
    /* The native virtual save list is empty. */
}

bool __stdcall function_215f40(long, unsigned char *buffer)
{
    if (buffer)
    {
        buffer[0] = 0;
        buffer[1] = 0;
    }
    return false;
}

void function_2238f4(long, unsigned long, unsigned long, unsigned long)
{
    /* Native builds cannot launch the Xbox dashboard. */
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
