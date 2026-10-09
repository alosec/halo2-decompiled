#include "h2/host_abi.h"
#include "h2/render_protocol.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_122870.h"
#include "unknown_234c64.h"
#include <math.h>
#include <xtl.h>

/* Keep Microsoft-mangled recovered entry points inside the ILP32 guest image.
 * The C host ABI receives rebased native pointers through the guest compiler
 * adapter, so the LP64 host never needs to decode a Microsoft C++ call. */

extern "C" DWORD __stdcall XGetLanguage()
{
    return 1; /* English */
}

bool function_122d60(s_cache_file_location location, long size, void *buffer)
{
    return host_h2_cache_read(location.offset, size, buffer) != 0;
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

void function_18f5e3();
void function_215790();

extern "C" int h2_main_menu_guest_entry()
{
    /* Phase one owns only deterministic offline profile/save bootstrap. */
    function_18f5e3();
    function_215790();
    return 0;
}

extern "C" int h2_native_profile_list(unsigned int *identifiers, unsigned int capacity)
{
    return host_h2_profile_list(identifiers, capacity);
}

extern "C" int h2_native_profile_create(const void *record, unsigned int size)
{
    return host_h2_profile_create(record, size);
}

extern "C" int h2_native_profile_read(unsigned int identifier, void *record,
                                        unsigned int size)
{
    return host_h2_profile_read(identifier, record, size);
}

extern "C" int h2_native_profile_write(unsigned int identifier, const void *record,
                                         unsigned int size)
{
    return host_h2_profile_write(identifier, record, size);
}

extern "C" int h2_native_profile_delete(unsigned int identifier)
{
    return host_h2_profile_delete(identifier);
}

long __stdcall function_1a03a0(long controller, word *name);
bool __stdcall function_215f40(long type, unsigned char *buffer);
bool function_216120(word *name, long type);

extern "C" int h2_native_profile_round_trip()
{
    unsigned int identifiers[2] = { 0xffffffffu, 0xffffffffu };
    unsigned char record[0x1e0] = {};
    unsigned char loaded[0x1e0] = {};
    word name[] = { 'H', 'a', 'l', 'o', '0', '0', '0', '1', 0 };
    if (h2_native_profile_list(identifiers, 2) != 0)
        return 1;
    int identifier = function_1a03a0(0, name);
    if (identifier < 0)
        return 2;
    int result = 0;
    if (h2_native_profile_list(identifiers, 2) != 1 ||
        identifiers[0] != (unsigned int)identifier)
        result = 3;
    else if (!h2_native_profile_read((unsigned int)identifier, loaded, sizeof(loaded)))
        result = 4;
    else if (loaded[8] != 'H' || loaded[9] != 0 ||
             loaded[10] != 'a' || loaded[11] != 0 ||
             loaded[12] != 'l' || loaded[13] != 0 ||
             loaded[14] != 'o' || loaded[15] != 0 ||
             loaded[16] != '0' || loaded[18] != '0' ||
             loaded[20] != '0' || loaded[22] != '1' || loaded[24] != 0 ||
             !(loaded[4] & 1) || loaded[0x118] != 10 || loaded[0x11b] != 10)
        result = 5;
    else
    {
        word next_name[128] = {};
        if (!function_215f40(0, reinterpret_cast<unsigned char *>(next_name)) ||
            next_name[0] != 'H' || next_name[1] != 'a' || next_name[2] != 'l' ||
            next_name[3] != 'o' || next_name[4] != '0' || next_name[5] != '0' ||
            next_name[6] != '0' || next_name[7] != '2' || next_name[8] != 0 ||
            function_216120(name, 0) || !function_216120(next_name, 0))
            result = 5;
        for (unsigned int index = 0; index < sizeof(record); ++index)
            record[index] = loaded[index];
    }
    if (!result)
    {
        record[0x1df] = 0x5a;
        if (!h2_native_profile_write((unsigned int)identifier, record, sizeof(record)))
            result = 6;
        else if (!h2_native_profile_read((unsigned int)identifier, loaded, sizeof(loaded)) ||
                 loaded[0x1df] != 0x5a)
            result = 7;
    }
    if (!h2_native_profile_delete((unsigned int)identifier))
        return result ? result : 8;
    if (!result && h2_native_profile_list(identifiers, 2) != 0)
        result = 9;
    return result;
}

struct s_loop_allocator;
class c_memory_source;
extern s_loop_allocator *g_51e998;
void function_18e250(s_loop_allocator *, long, char const *, c_memory_source *);
void function_1476c7();
bool function_147d13();
void __stdcall function_1483c3(long);

alignas(16) static unsigned char g_native_ui_heap[0x100050];

struct s_string_table;
bool string_table_load(s_string_table *table);

extern "C" int h2_main_menu_guest_load_strings()
{
    if (!g_4e034c)
        return 1;
    s_string_table *english = reinterpret_cast<s_string_table *>(
        reinterpret_cast<unsigned char *>(g_4e034c) + 0x188);
    return string_table_load(english) ? 0 : 2;
}

extern "C" void h2_native_controller_connect(long controller);

class c_press_start_screen
{
public:
    void handle_start(s_controller_reference **controller, long *item);
};

extern "C" int h2_main_menu_guest_press_start()
{
    c_window_channel *channels[] = {
        &g_54d598.window_0,
        &g_54d598.windows_1[4],
        &g_54d598.window_2,
        &g_54d598.windows_3[4],
        &g_54d598.window_4,
        &g_54d598.windows_5[4],
        &g_54d598.default_window,
    };
    for (long index = 0; index < 7; ++index)
    {
        if (channels[index]->current && channels[index]->current->screen_id == 9)
        {
            h2_native_controller_connect(0);
            s_controller_reference controller{};
            controller.controller_index = 0;
            s_controller_reference *controller_pointer = &controller;
            long item = 0;
            reinterpret_cast<c_press_start_screen *>(channels[index]->current)->
                handle_start(&controller_pointer, &item);
            for (long user = 0; user < 5; ++user)
            {
                if (g_54d598.windows_3[user].next)
                    return 0;
            }
            for (long user = 0; user < 5; ++user)
            {
                if (g_54d598.windows_5[user].next)
                    return 0;
            }
            return 3;
        }
    }
    return 2;
}

bool __stdcall function_252a32(long controller);
static long g_native_profile_name_reads;

extern "C" int h2_main_menu_guest_accept()
{
    c_window_channel *channels[] = {
        &g_54d598.window_0,
        &g_54d598.windows_1[4],
        &g_54d598.window_2,
        &g_54d598.windows_3[4],
        &g_54d598.window_4,
        &g_54d598.windows_5[4],
        &g_54d598.default_window,
    };
    for (long index = 0; index < 7; ++index)
    {
        if (channels[index]->current && channels[index]->current->screen_id == 0x1e)
        {
            s_widget_event event = { 5, 0, 0 };
            return channels[index]->current->v10(&event) ? 0 : 3;
        }
        if (channels[index]->current && channels[index]->current->screen_id == 0xa)
        {
            c_class_1473c9 *const screen = channels[index]->current;
            c_class_1474e8 *const list = reinterpret_cast<c_class_1474e8 *>(
                reinterpret_cast<c_screen_with_menu *>(screen)->list);
            c_class_1a2c81 *const item = list->get_focused_item();
            const long reads = g_native_profile_name_reads;
            s_widget_event event = { 5, 0, 0 };
            if (!item || !item->v10(&event))
                return 4;
            /* Create New must obtain one durable default name and enqueue the
             * genuine recovered type-1 keyboard. */
            if (g_native_profile_name_reads != reads + 1)
                return 5;
            return 0;
        }
    }
    return 2;
}

extern "C" int h2_main_menu_guest_cancel()
{
    c_window_channel *channels[] = {
        &g_54d598.window_0,
        &g_54d598.windows_1[4],
        &g_54d598.window_2,
        &g_54d598.windows_3[4],
        &g_54d598.window_4,
        &g_54d598.windows_5[4],
        &g_54d598.default_window,
    };
    for (long index = 0; index < 7; ++index)
    {
        if (channels[index]->current && channels[index]->current->screen_id == 0x1e)
        {
            /* The current native-offline fallback uses the retail mode-4 chooser,
             * whose B prompt is invite-specific. Return through its recovered
             * press-start callback instead of presenting that irrelevant prompt. */
            return function_252a32(0) ? 0 : 3;
        }
        if (channels[index]->current && channels[index]->current->screen_id != 9)
        {
            s_widget_event event = { 5, 0, 1 };
            return channels[index]->current->v10(&event) ? 0 : 3;
        }
    }
    return 2;
}

extern "C" int h2_main_menu_guest_open_press_start()
{
    if (h2_main_menu_guest_entry() != 0)
        return 1;
    if (!g_51e998) {
        g_51e998 = reinterpret_cast<s_loop_allocator *>(g_native_ui_heap);
        function_18e250(g_51e998, 0x100000, "ui memory pool", 0);
        /* Recovered function_1a474c enables ownerless looping allocation. */
        g_native_ui_heap[0x3c] = 1;
        g_native_ui_heap[0x3d] = 1;
    }
    function_1476c7();
    function_1483c3(0);
    return function_147d13() ? 0 : 2;
}

class c_render_window;
void __stdcall function_1479c3(float elapsed);
void function_147af1(long index, long fallback_index, c_render_window *window);

struct s_native_render_window
{
    short top;
    short left;
    short bottom;
    short right;
};

extern "C" int h2_main_menu_guest_frame()
{
    function_1479c3(1.0f / 60.0f);
    s_native_render_window window = { 0, 0, 480, 640 };
    function_147af1(4, -1, reinterpret_cast<c_render_window *>(&window));
    return 0;
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

    (void)color;
    auto channel = [](float value) -> unsigned int {
        if (value < 0.0f)
            value = 0.0f;
        else if (value > 1.0f)
            value = 1.0f;
        return static_cast<unsigned int>(value * 255.0f + 0.5f);
    };
    auto pack = [&](const color4f& value) -> unsigned int {
        return (channel(value.alpha) << 24) |
               (channel(value.red) << 16) |
               (channel(value.green) << 8) |
               channel(value.blue);
    };
    h2_text_request_metadata metadata{};
    metadata.version = H2_TEXT_PROTOCOL_VERSION;
    metadata.font = g_4e73a0.font;
    metadata.style = g_4e73a0.style;
    metadata.justification = g_4e73a0.justification;
    metadata.flags = g_4e73a0.flags;
    metadata.shadow_color = g_4e73a0.shadow ? pack(g_4e73a0.field_24) : 0;
    host_h2_text_request(text, bounds, clip, position, static_cast<int>(line_gap),
                         scale, pack(g_4e73a0.color), &metadata);
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

bool __stdcall function_215f40(long type, unsigned char *buffer)
{
    ++g_native_profile_name_reads;
    if (!buffer || type != 0)
        return false;
    word *name = reinterpret_cast<word *>(buffer);
    name[0] = 0;
    unsigned int identifiers[4096];
    int count = h2_native_profile_list(identifiers, 4096);
    if (count < 0 || count > 4096)
        return false;
    for (unsigned int suffix = 1; suffix <= 4096; ++suffix)
    {
        word candidate[9] = { 'H', 'a', 'l', 'o', 0, 0, 0, 0, 0 };
        unsigned int value = suffix;
        for (int digit = 7; digit >= 4; --digit)
        {
            candidate[digit] = (word)('0' + value % 10);
            value /= 10;
        }
        bool available = true;
        for (int index = 0; index < count && available; ++index)
        {
            unsigned char record[0x1e0];
            if (!h2_native_profile_read(identifiers[index], record, sizeof(record)))
                return false;
            word const *stored = reinterpret_cast<word const *>(record + 8);
            int character = 0;
            for (; character < 9 && stored[character] == candidate[character]; ++character)
                if (!candidate[character])
                {
                    available = false;
                    break;
                }
        }
        if (available)
        {
            for (int character = 0; character < 9; ++character)
                name[character] = candidate[character];
            return true;
        }
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
