// @flags /O1 /Oi /Gr
/* ONLINE_Y_MENU_LISTS.CPP: the online Y menu's friends list, the base it
   shares with the players list, and the list of the players of the recent
   games, copied from the player configuration cache when the list is built
   (its item filler is SSE code: online_y_menu_recent_players_item.cpp) */

#include "unknown_4cf790.h"
#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "data_array.h"
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "unknown_2312b4.h"
#include "unknown_2b116a.h"
#include "unknown_x8d43e5.h"
#include "online_friends.h"
#include "online_y_menu_lists.h"

struct s_message;
void function_149f49(s_message *message, word a, dword *id, short b, long c, long d, long e);
void function_238c21(long controller, long type, word *name, long maximum_count);
struct s_friend;
void friend_get_online_friend(s_friend const *player, XONLINE_FRIEND *result);
bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
c_class_1473c9 *__stdcall function_2b71f0(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7212(s_screen_parameters *parameters);
void unicode_string_to_ascii(const word *source, char *destination, long maximum_count);
bool function_6c7e0();
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);
void __stdcall function_2b2ed4(c_y_menu_list *list, bool friends, bool empty,
    c_class_1a2c81 *item, s_friend *datum);
struct s_screen_view_2b2d;
long function_2b2d81(s_screen_view_2b2d *screen);

/* the next player of the player configuration cache after the iterator's */
bool player_configuration_cache_next_recent_player(s_recent_player *player, long *iterator);

/* the scenario's type at +0x10 (2 is the main menu) */
struct s_scenario_type_view
{
	byte unknown00[0x10];
	short type;
};

/* the friends list: registers its item handler */
// @retail 0x2b2d9a
c_y_menu_friends_list::c_y_menu_friends_list(word user_flags) :
	c_y_menu_list(user_flags),
	item_count(NONE),
	handler(this, (list_item_method)&c_y_menu_friends_list::handle_item)
{
	delegate_register(&item_handlers, &handler);
}

/* marks a friends or players list existing outside the main menu */
// @retail 0x2b2e6b
c_y_menu_list::c_y_menu_list(word user_flags) :
	c_list_widget_with_items(user_flags),
	value88(NONE)
{
	if (g_4e0350 && ((s_scenario_type_view *)g_4e0350)->type != 2)
	{
		g_4e647b = true;
	}
}

/* clears that mark */
// @retail 0x2b2ebd
c_y_menu_list::~c_y_menu_list()
{
	g_4e647b = false;
}

// @retail 0x2b2e9f deleting c_y_menu_list

/* the players list's deleting destructor is this one */
// @retail 0x2b2dfd deleting c_y_menu_friends_list

/* slot 1 of the list (c_widget::v9 in unknown_19b516.h's view) */
// @retail 0x2b2e19
void c_y_menu_friends_list::v1()
{
	data = g_global_4acf62.field_4_4;
	item_count = NONE;
	((c_widget *)this)->m7f = 0;
	((c_widget *)this)->c_widget::v9();
}

/* gives the items new data when the friends list's count changed */
// @retail 0x2b2e31
void c_y_menu_friends_list::v3()
{
	data = g_global_4acf62.field_4_4;
	if (g_global_4acf62.field_4_4)
	{
		if (item_count != g_global_4acf62.field_4_4->actual_count)
		{
			item_count = g_global_4acf62.field_4_4->actual_count;
			function_24c0c4((c_widget *)this);
		}
	}
	else
	{
		item_count = NONE;
	}
	((c_widget *)this)->c_widget::v11();
}

/* the items' widgets */
// @retail 0x2b3ef5
void *c_y_menu_friends_list::get_item_data()
{
	return items;
}

/* the friends list shows eight items at a time */
// @retail 0x2b2d7d
long c_y_menu_friends_list::get_item_count()
{
	return 8;
}

/* the user's pending online messages, held by the menu three levels up */
// @retail 0x2b419f
void *c_y_menu_friends_list::get_items(long *count)
{
	c_online_y_menu_screen *screen = (c_online_y_menu_screen *)parent->parent->parent;

	*count = screen->message_count;
	return screen->messages;
}

/* a friend opens the friend's screen; the empty item asks for a gamertag to
   send a friend request to */
// @retail 0x2b386f
void c_y_menu_friends_list::handle_item(s_controller_reference **controller, long *item)
{
	s_friend_list_entry *entry = data ? (s_friend_list_entry *)record_pool_lookup(data, *item) : 0;

	if (entry)
	{
		if (entry->xuid == 0)
		{
			function_148893(0, 1);
			name[0] = 0;
			function_238c21((*controller)->controller_index, 0xc, name, 0x10);
		}
		else
		{
			s_name_request request;
			s_screen_parameters parameters;

			parameters.field_c = 0;
			request.type = 2;
			friend_get_online_friend((s_friend const *)entry, &request.friend_);
			function_148893(&request, 1);
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b71f0);
			parameters.load(&parameters);
		}
	}
}

/* fills in a friend's item while the friends tab is shown */
// @retail 0x2b37fd
void c_y_menu_friends_list::v20(c_class_1a2c81 *item, long unused)
{
    if ((byte)function_2b2d81((s_screen_view_2b2d *)parent->parent->parent))
    {
        long index = function_6c7e0() ? widget_item(item)->value70 : NONE;
        s_friend *entry = data ? (s_friend *)record_pool_lookup(data, index) : 0;
        if (entry)
        {
            bool empty = true;
            if (((s_friend_list_entry *)entry)->xuid != 0)
                empty = false;
            function_2b2ed4(this, true, empty, item, entry);
        }
        else
            function_22f042(0, item, 0);
    }
}

/* the request's player or friend, when it is one */
static __forceinline unsigned __int64 *name_request_xuid(s_name_request *request)
{
	unsigned __int64 *xuid = 0;

	switch (request->type)
	{
	case 1:
		xuid = (unsigned __int64 *)request->player.id;
		break;
	case 2:
		xuid = &request->friend_xuid;
		break;
	}
	return xuid;
}

/* builds the list's data from the player configuration cache (100 players
   at most) */
// @retail 0x2b393c
c_y_menu_recent_players_list::c_y_menu_recent_players_list(word user_flags) :
	c_class_1474e8(user_flags),
	value88(NONE),
	handler(this, (list_item_method)&c_y_menu_recent_players_list::handle_item)
{
	s_recent_player player;
	long iterator;
	long i;

	data = user_interface_data_new("recent players list", 100, sizeof(s_recent_player_datum));
	function_16b790(data);
	iterator = g_4cf984;
	for (i = 0; i < 100 && player_configuration_cache_next_recent_player(&player, &iterator); i++)
	{
		((s_recent_player_datum *)data->data)[record_pool_allocate(data) & 0xffff].player = player;
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b3a0a deleting c_y_menu_recent_players_list

/* a recent player's id and gamertag as a name request takes them */
// @retail 0x2b3e26
void recent_player_get_name_request(s_recent_player const *player, s_player_name_request *request)
{
	memset(request, 0, sizeof(*request));
	memcpy(request->id, player->xuid, sizeof(request->id));
	unicode_string_to_ascii(player->name, request->name, 16);
	request->name[15] = 0;
}

/* a recent player opens the player's screen */
// @retail 0x2b3e4d
void c_y_menu_recent_players_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		s_recent_player_datum *datum = &((s_recent_player_datum *)data->data)[*item & 0xffff];
		byte zero[0xc] = { 0 };

		if (memcmp(datum, zero, sizeof(zero)) != 0)
		{
			s_name_request request;
			s_screen_parameters parameters;
			unsigned __int64 *xuid;

			parameters.field_c = 0;
			request.type = 1;
			recent_player_get_name_request(&datum->player, &request.player);
			function_148893(&request, 1);
			xuid = name_request_xuid(&request);
			if (xuid && *xuid)
			{
				function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b7212);
				parameters.load(&parameters);
			}
		}
	}
}

/* the online flags of the friend with this xuid (0 when not a friend) */
// @retail 0x2b3a28
dword friend_get_flags(XUID const *xuid)
{
	dword result = 0;

	if (g_global_4acf62.field_4_4)
	{
		s_list_item_iterator iterator;

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = g_global_4acf62.field_4_4;
		while (function_2b2327(&iterator))
		{
			if (xuid_equal((XUID const *)&((s_friend_list_entry *)iterator.item)->xuid, xuid, false))
			{
				XONLINE_FRIEND friend_info;
				s_online_friend copy;

				friend_get_online_friend((s_friend const *)iterator.item, &friend_info);
				online_friend_copy(&friend_info, &copy);
				result = copy.flags;
				break;
			}
		}
	}
	return result;
}
