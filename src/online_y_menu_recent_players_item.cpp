// @flags /O1 /Oi /arch:SSE /Gr
/* ONLINE_Y_MENU_RECENT_PLAYERS_ITEM.CPP: filling in one item of the online Y
   menu's list of the players of the recent games: gamertag, status, voice,
   session and invitation bitmaps (built with SSE, apart from the rest of the
   list in online_y_menu_lists.cpp) */

#include "unknown_11c920.h"
#include <string.h>
#include "data_array.h"
#include "screen_widgets.h"
#include "unknown_2312b4.h"
#include "unknown_2b116a.h"
#include "unknown_x8d43e5.h"
#include "online_friends.h"
#include "online_y_menu_lists.h"

bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);

#include <wchar.h>
struct s_screen_view_2b39;
inline __declspec(noinline) long function_2b3923(s_screen_view_2b39 *screen);
struct s_name_buffer;
void function_08cc20(s_name_buffer *buffer, wchar_t const *name);
bool network_session_interface_has_user(XUID const *xuid);
long function_19ad9c(XUID const *xuid);
long voice_find_player(long controller);
bool voice_port_flag0_only(long port);
bool voice_port_flag1(long port);
bool voice_test_unknownF0(long port, long bit);
bool function_53750(long player);
bool function_19acc6(XUID const *xuid);
long function_19adca(XUID const *xuid);
bool function_19abe4(long player_index);
struct s_widget_view_2b0a;
void function_2b0a14(s_widget_view_2b0a *widget, short index);
struct s_player_status_values;
struct s_player_status_source;
void function_080de0(s_player_status_values *values, s_player_status_source const *source);
void function_2b01a2(long value, s_widget_item *item);
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);

/* fills in a recent player's item */
// @retail 0x2b3a9d
void c_y_menu_recent_players_list::v20(c_class_1a2c81 *item, long index)
{
    if (!(byte)function_2b3923((s_screen_view_2b39 *)parent->parent->parent) || !data)
        return;
    c_class_1a2c81 *name = item->find_child(6, 0, false);
    c_text_widget_45a5e0 *status = (c_text_widget_45a5e0 *)item->find_child(6, 1, false);
    c_class_1a2c81 *invite = item->find_child(8, 3, false);
    c_class_1a2c81 *session = item->find_child(8, 5, false);
    c_class_1a2c81 *voice = item->find_child(8, 4, false);
    long datum_index = widget_item(item)->value70;
    if (datum_index != NONE)
    {
        s_recent_player_datum *datum = &((s_recent_player_datum *)data->data)[datum_index & 0xffff];
        byte zero[12] = { 0 };
        word display_name[256];
        display_name[0] = 0;
        bool has_identifier = memcmp(&datum->player, zero, sizeof(zero)) != 0;
        XUID const *xuid = (XUID const *)&datum->player;
        bool in_session = false;
        dword friend_flags = 0;
        if (has_identifier)
        {
            function_08cc20((s_name_buffer *)display_name, (wchar_t const *)datum->player.name);
            in_session = network_session_interface_has_user(xuid);
            friend_flags = friend_get_flags(xuid);
        }
        else
        {
            wcsncpy((wchar_t *)display_name, L"", 255);
            display_name[255] = 0;
        }
        long player = in_session ? function_19ad9c(xuid) : NONE;
        long local_player = voice_find_player(value88);
        bool has_voice = player != NONE ? voice_port_flag0_only(player) : false;
        bool unavailable = !has_voice || (player != NONE ? voice_port_flag1(player) : false);
        bool muted = player != NONE && local_player != NONE ? voice_test_unknownF0(local_player, player) : false;
        bool speaking = player != NONE ? function_53750(player) : false;
        bool blocked = false;
        bool active = false;
        bool joinable = false;
        bool invitation = (friend_flags >> 5) & 1;
        if (in_session)
        {
            active = function_19acc6(xuid);
            if (active)
            {
                long player_index = function_19adca(xuid);
                if (player_index != NONE && function_19abe4(player_index))
                    joinable = true;
            }
        }
        c_online_y_menu_screen *screen = (c_online_y_menu_screen *)parent->parent->parent;
        long count = screen->message_count;
        s_entry *messages = screen->messages;
        for (long i = 0; i < count; ++i)
        {
            s_entry *message = &messages[i];
            XUID const *sender = 0;
            if (message)
                sender = (XUID const *)message;
            if (xuid_equal(xuid, sender, false) && (message->flags & 0x800))
            {
                blocked = true;
                break;
            }
        }
        if (name)
            name->function_22f52e()->set_text(display_name);
        if (status)
        {
            if (blocked) status->function_253b1a(0x15000269);
            else if (joinable) status->function_253b1a(0x18000250);
            else if (active) status->function_253b1a(0x14000251);
            else if (in_session) status->function_253b1a(0x13000252);
            else status->function_253b1a(0xd00027f);
        }
        if (voice)
        {
            if (in_session && muted)
            {
                voice->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)voice, 3);
            }
            else if (in_session && speaking)
            {
                voice->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)voice, 30);
            }
            else if (in_session && unavailable)
            {
                voice->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)voice, 5);
            }
            else if (in_session && has_voice)
            {
                voice->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)voice, 4);
            }
            else voice->value6e = false;
        }
        if (session)
        {
            if (active)
            {
                session->value6e = true;
                if (joinable)
                    function_2b0a14((s_widget_view_2b0a *)session, 29);
                else
                    function_2b0a14((s_widget_view_2b0a *)session, 19);
            }
            else if (in_session)
            {
                session->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)session, 0);
            }
            else session->value6e = false;
        }
        if (invite)
        {
            if (blocked)
            {
                invite->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)invite, 18);
            }
            else if (invitation)
            {
                invite->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)invite, 2);
            }
            else invite->value6e = false;
        }
        s_widget_item layout;
        if (has_identifier)
        {
            dword values[4];
            function_080de0((s_player_status_values *)values, (s_player_status_source const *)xuid);
            function_2b01a2(datum->player.range_value, &layout);
            word *name_text = datum->player.name;
            memcpy(layout.value48, values, sizeof(values));
            layout.flags |= 2;
            layout.value4 = (long)name_text;
            layout.flags |= 1;
        }
        else
        {
            layout.value5e = true;
            layout.flags = 0x20;
        }
        function_22f042(&layout, item, 1);
    }
    else
        function_22f042(0, item, 0);
}
