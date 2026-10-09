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

struct s_string_table
{
    void *references;
    char *data;
    long count;
    long data_size;
    long references_offset;
    long data_offset;
    bool loaded;
    unsigned char unknown19[3];
};
static_assert(sizeof(s_string_table) == 0x1c, "string table layout");
bool string_table_load(s_string_table *table);
void level_handle_tables_initialize();
void function_19c190();
struct s_entry_b;
s_entry_b *function_19c1f0(long map_id);
extern "C" c_data_allocator *h2_native_user_interface_allocator();

extern "C" int h2_main_menu_guest_load_campaign_metadata()
{
    /* The retail startup normally assigns the engine allocator before this
     * module initializes. The native UI allocator is backed by the owned guest
     * loop heap and provides the same c_data_allocator contract. */
    g_468758 = h2_native_user_interface_allocator();
    level_handle_tables_initialize();
    if (!g_4ee4e8 || !g_4ee4e4)
        return 1;
    function_19c190();
    /* The matching main-menu tag table contains the retail Campaign's exact
     * fifteen built-in level handles. Fail closed if metadata ownership ever
     * regresses to a partial, empty, or fabricated list. */
    if (!g_4ee4e8->valid || g_4ee4e8->actual_count != 15 ||
        g_4ee4e8->maximum_count != 20)
        return 2;
    struct s_level_handle_datum
    {
        short salt;
        unsigned char downloaded;
        unsigned char reserved;
        long map_id;
    };
    long map_ids[15] = {};
    long count = 0;
    for (long index = 0; index < g_4ee4e8->high_water_index; ++index)
    {
        if (!(g_4ee4e8->bitmap[index >> 5] & (1u << (index & 31))))
            continue;
        const long map_id = reinterpret_cast<s_level_handle_datum *>(
            g_4ee4e8->data + index * g_4ee4e8->size)->map_id;
        if (!function_19c1f0(map_id))
            return 3;
        for (long previous = 0; previous < count; ++previous)
            if (map_ids[previous] == map_id)
                return 4;
        if (count >= 15)
            return 5;
        map_ids[count++] = map_id;
    }
    return count == 15 ? 0 : 6;
}

extern "C" int h2_main_menu_guest_load_strings()
{
    if (!g_4e034c)
        return 1;
    s_string_table *english = reinterpret_cast<s_string_table *>(
        reinterpret_cast<unsigned char *>(g_4e034c) + 0x188);
    if (!string_table_load(english))
        return 2;
    return english->loaded && english->references && english->data &&
        english->count > 0 && english->data_size > 0 ? 0 : 3;
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
extern "C" int h2_native_virtual_keyboard_accept();
static long g_native_profile_name_reads;

extern "C" int h2_main_menu_guest_accept()
{
    const int keyboard_result = h2_native_virtual_keyboard_accept();
    if (keyboard_result != 2)
        return keyboard_result;
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
        if (channels[index]->current &&
            (channels[index]->current->screen_id == 6 ||
             channels[index]->current->screen_id == 0xda ||
             channels[index]->current->screen_id == 0xb))
        {
            const long current_screen = channels[index]->current->screen_id;
            const long expected_screen = current_screen == 6
                ? 0xda
                : (current_screen == 0xda ? 0xb : 0xc);
            c_class_1474e8 *const list = reinterpret_cast<c_class_1474e8 *>(
                reinterpret_cast<c_screen_with_menu *>(channels[index]->current)->list);
            c_class_1a2c81 *const item = list->get_focused_item();
            s_widget_event event = { 5, 0, 0 };
            if (!item || !item->v10(&event))
                return 6;
            for (long destination = 0; destination < 7; ++destination)
            {
                if (channels[destination]->next &&
                    channels[destination]->next->screen_id == expected_screen)
                    return 0;
            }
            return 7;
        }
        if (channels[index]->current && channels[index]->current->screen_id == 0xc)
        {
            c_class_1474e8 *const list = reinterpret_cast<c_class_1474e8 *>(
                reinterpret_cast<c_screen_with_menu *>(channels[index]->current)->list);
            c_class_1a2c81 *const item = list->get_focused_item();
            s_widget_event event = { 5, 0, 0 };
            return item && item->v10(&event) ? 0 : 8;
        }
    }
    return h2_native_virtual_keyboard_accept();
}

static c_class_1474e8 *h2_native_current_menu_list()
{
    c_window_channel *channels[] = {
        &g_54d598.windows_3[4],
        &g_54d598.windows_5[4],
    };
    for (long index = 0; index < 2; ++index)
    {
        c_class_1473c9 *const screen = channels[index]->current;
        if (screen && (screen->screen_id == 6 || screen->screen_id == 0xda ||
                       screen->screen_id == 0xb || screen->screen_id == 0xc))
            return reinterpret_cast<c_class_1474e8 *>(
                reinterpret_cast<c_screen_with_menu *>(screen)->list);
    }
    return 0;
}

static int h2_native_main_menu_move(long event_type)
{
    c_class_1474e8 *const list = h2_native_current_menu_list();
    if (!list)
        return 2;
    const long before = list->get_focused_datum();
    s_widget_event event = { event_type, 0, 0 };
    if (!list->v10(&event))
        return 3;
    const long after = list->get_focused_datum();
    return before != NONE && after != NONE && before != after ? 0 : 4;
}

char *function_148d73();
long map_location_get(char const *map_name);
long map_location_progress_get(char const *map_name);
extern "C" long h2_native_cache_file_find(char const *map_name);
extern "C" int h2_native_load_campaign_map(char const *map_name);
extern "C" int h2_native_campaign_bsp_metadata();

/* unknown_213d20.cpp is recovered with Microsoft's C++ ABI, while the cache
 * loader runtime also serves Itanium-ABI recovered units. Keep one ownership
 * implementation behind a narrow C bridge rather than duplicating it. */
long cache_file_find(char const *map_name)
{
    return h2_native_cache_file_find(map_name);
}

struct s_game_options
{
    long state;
    unsigned char unknown04;
    unsigned char unknown05;
    short field_2_3;
    unsigned char random_id[8];
    unsigned long field_8_4;
    long unknown14;
    long unknown18;
    unsigned char unknown1c[0x124 - 0x1c];
    bool unknown124;
    unsigned char unknown125[0x128 - 0x125];
    unsigned char unknown128;
    unsigned char unknown129;
    short unknown12a;
    unsigned char remaining[0x1118 - 0x12c];
};
static_assert(sizeof(s_game_options) == 0x1118, "game options layout");
void function_138110(s_game_options *options);
unsigned long function_1462b0();
extern short g_485ac0;
extern unsigned char g_510c49;

extern "C" int h2_main_menu_guest_game_options_probe()
{
    s_game_options options;
    function_138110(&options);
    const short ticks = g_485ac0 > 0 ? g_485ac0 : 60;
    if (options.state != 1 || options.unknown04 != 1 || options.unknown05 != 0 ||
        options.field_2_3 != ticks / 2 || options.field_8_4 != 0x78a8 ||
        options.unknown14 != NONE || options.unknown18 != NONE ||
        options.unknown128 != g_510c49 || options.unknown12a != 1)
        return 1;
    volatile unsigned long seed = function_1462b0();
    (void)seed;
    return 0;
}

extern "C" int h2_main_menu_guest_map_location_probe()
{
    char const main_menu[] = "mainmenu";
    char const campaign[] =
        "scenarios\\solo\\00a_introduction\\00a_introduction";
    if (map_location_get(main_menu) != 3 ||
        map_location_progress_get(main_menu) != 2)
        return 1;
    if (map_location_get(campaign) != 0 ||
        map_location_progress_get(campaign) != 0)
        return 2;
    if (!host_h2_map_select(campaign))
        return 3;
    if (map_location_get(campaign) != 3 ||
        map_location_progress_get(campaign) != 2)
        return 4;
    return map_location_get(main_menu) == 0 &&
        map_location_progress_get(main_menu) == 0 ? 0 : 5;
}

struct s_native_machine_address
{
    unsigned char bytes[6];
};

struct s_native_local_player_entry
{
    bool valid;
    unsigned char unknown01;
    short value02;
    long index;
    s_native_machine_address address;
    unsigned char xuid[0x1c - 0xe];
    unsigned char record[0xe4 - 0x1c];
};
static_assert(sizeof(s_native_local_player_entry) == 0xe4,
              "local player entry layout");
void function_23643f();
void function_23654b(void *entries, void *address, void *count);
extern long g_54e7c0;
extern long g_54e7c4;
extern long g_54e7c8;

static char const *g_native_expected_campaign_map;
static long g_native_campaign_options_status = NONE;

extern "C" void h2_native_campaign_options_ready(s_game_options const *options)
{
    g_native_campaign_options_status = 1;
    if (!options || !g_native_expected_campaign_map)
        return;
    unsigned char const *const bytes =
        reinterpret_cast<unsigned char const *>(options);
    char const *const map_name = reinterpret_cast<char const *>(bytes + 0x1c);
    long character = 0;
    for (; character < 0x104 && map_name[character] &&
           map_name[character] == g_native_expected_campaign_map[character];
         ++character)
    {
    }
    if (character == 0x104 || map_name[character] != 0 ||
        g_native_expected_campaign_map[character] != 0)
        return;
    if (*reinterpret_cast<long const *>(bytes) != 1 ||
        *reinterpret_cast<long const *>(bytes + 0x14) != g_54e7c0 ||
        *reinterpret_cast<long const *>(bytes + 0x18) != g_54e7c4 ||
        *reinterpret_cast<short const *>(bytes + 0x12a) != (short)g_54e7c8 ||
        bytes[0x12c] || !bytes[0x12d] ||
        *reinterpret_cast<long const *>(bytes + 0x268) != 1 ||
        !bytes[0x2cc])
        return;
    s_native_local_player_entry const *const players =
        reinterpret_cast<s_native_local_player_entry const *>(bytes + 0x2d4);
    if (!players[0].valid || players[0].index != 0 ||
        (players[0].record[0] == 0 && players[0].record[1] == 0))
        return;
    g_native_campaign_options_status = 0;
}

extern "C" int h2_main_menu_guest_load_selected_campaign()
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
    bool difficulty_open = false;
    for (long index = 0; index < 7; ++index)
        difficulty_open = difficulty_open ||
            (channels[index]->current && channels[index]->current->screen_id == 0xc);
    if (!difficulty_open)
        return 2;
    char *const map_name = function_148d73();
    if (!map_name)
        return 3;
    s_native_local_player_entry players[16];
    s_native_machine_address address;
    long player_count = 0;
    function_23654b(players, &address, &player_count);
    if (player_count != 1)
        return 6;
    if (!players[0].valid)
        return 7;
    if (players[0].index != 0)
        return 8;
    if (players[0].record[0] == 0 && players[0].record[1] == 0)
        return 9;
    if (!host_h2_map_select(map_name))
        return 4;
    g_native_expected_campaign_map = map_name;
    g_native_campaign_options_status = NONE;
    function_23643f();
    g_native_expected_campaign_map = 0;
    if (g_native_campaign_options_status != 0)
        return 10;
    if (!h2_native_load_campaign_map(map_name))
        return 5;
    if (h2_native_campaign_bsp_metadata() <= 0)
        return 12;
    return h2_main_menu_guest_load_strings() == 0 ? 0 : 11;
}

extern "C" int h2_main_menu_guest_up()
{
    return h2_native_main_menu_move(1);
}

extern "C" int h2_main_menu_guest_down()
{
    return h2_native_main_menu_move(3);
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
