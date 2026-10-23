// @flags /O1 /Oi /Gr
/* MP_PAUSE_GAME.CPP: the multiplayer pause screen, which the window manager
   loads itself, and its list */

#include "unknown_11c920.h"
#include "globals.h"
#include "screen_widgets.h"
#include "user_interface_lists.h"
#include "unknown_24b5bc.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "mp_pause_game.h"

/* the multiplayer pause screen (vtable 0x453898) */
class c_mp_pause_game_screen : public c_screen_with_menu
{
public:
	c_mp_pause_game_screen(long a, long b, word user_flags);

	virtual void v3();
	virtual screen_load_proc get_load_proc();

	c_mp_pause_game_list list;
};

/* creates the multiplayer pause screen */
// @retail 0x1475c7
c_class_1473c9 *__stdcall mp_pause_game_open(s_screen_parameters *parameters)
{
	c_mp_pause_game_screen *screen = new c_mp_pause_game_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

/* the multiplayer pause screen shows its list */
// @retail 0x147605
c_mp_pause_game_screen::c_mp_pause_game_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xc3, a, b, user_flags, &list),
	list(user_flags)
{
}

/* the multiplayer pause screen's load procedure */
// @retail 0x14763a
screen_load_proc c_mp_pause_game_screen::get_load_proc()
{
	return mp_pause_game_open;
}

// @retail 0x232091 deleting c_mp_pause_game_screen
// @retail 0x2320af destructor c_mp_pause_game_screen
// @retail 0x231f36 destructor c_mp_pause_game_list
// @retail 0x147640 destructor c_mp_pause_game_list_item

// @retail 0x231f18 deleting c_mp_pause_game_list

/* the text of a pause list item */
// @retail 0x23284e
void c_mp_pause_game_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[6];

	table[0].item = 0;
	table[0].string_handle = 0xa0001b7;
	table[1].item = 1;
	table[1].string_handle = 0x8000286;
	table[2].item = 2;
	table[2].string_handle = 0xc0001b9;
	table[3].item = 3;
	table[3].string_handle = 0xc0001ba;
	table[4].item = 4;
	table[4].string_handle = 0x1c0001bb;
	table[5].item = 5;
	table[5].string_handle = 0x80001bd;
	function_24c75c(this, item, table, 0, 6);
}

long function_199ebc(void);
long function_199f34(void);
bool function_1999f9(void);
bool function_1900a5(long index);
short function_1900be(void);
short function_1900ff(long controller);
bool function_199971(void);
bool function_19a0c5(void);
long function_19989d(void);
void function_199e3c(long controller);
long function_199f6d(void);
bool function_1906da(long index);
bool function_199967(void);
bool function_19a1bd(void);
bool function_592f0(void);
bool function_6c7e0();
bool function_1999b3(void);
byte *network_session_interface_get_data_4db0(void);
c_class_1473c9 *__stdcall function_231db5(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_23252e(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2323c3(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_23246a(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2325fb(s_screen_parameters *parameters);

/* quits the game: the player leaves, or the whole game ends */
// @retail 0x232c9e
bool __stdcall mp_pause_game_quit_confirmed(long controller_index)
{
	bool alone = function_199f6d() == 1;

	if (!function_1900a5(controller_index) && (alone || function_1900be() == 1))
	{
		function_199e3c(controller_index);
		g_4ee4e0 = true;
	}
	else
	{
		s_player_slot_profile *profile = player_slot_profile_get(controller_index);

		profile->callback = 0;
		profile->sign_out();
	}
	return true;
}

/* ends the game */
// @retail 0x232cfc
bool __stdcall mp_pause_game_end_game_confirmed(long controller_index)
{
	function_19a1bd();
	return true;
}

/* asks whether to quit, in the words that fit the game */
// @retail 0x2329a7
void mp_pause_game_list_choose_quit(c_class_1473c9 *screen, s_controller_reference **controller)
{
	bool host = function_199ebc() == 1;
	bool live = function_592f0();
	bool alone = function_199f34() == 1;
	long dialog_id;

	if (function_1999f9())
	{
		long controller_index = (*controller)->controller_index;

		if (function_1900a5(controller_index))
		{
			dialog_id = 0xba;
		}
		else if (function_1900be() != 1)
		{
			if (function_1900ff(controller_index) > 0)
			{
				dialog_ok_show(screen->v20(), 0x88, screen->v21(), 1 << controller_index, 0, 0);
				return;
			}
			dialog_id = 7;
		}
		else if (function_199971())
		{
			if (function_19a0c5())
			{
				if (host)
				{
					dialog_id = 0xbb;
				}
				else
				{
					dialog_id = live ? 0xb1 : 0xb2;
				}
			}
			else if (!host && !alone)
			{
				dialog_id = live ? 0xbd : 0xbe;
			}
			else
			{
				dialog_id = 0xbc;
			}
		}
		else if (!host && !alone)
		{
			dialog_id = live ? 0xb3 : 0xb5;
		}
		else
		{
			dialog_id = 0xb4;
		}
	}
	else if (host)
	{
		dialog_id = 0xb0;
	}
	else
	{
		dialog_id = live ? 0xa1 : 0x37;
	}
	dialog_choice_show_default(screen->v20(), screen->v21(), 1 << (*controller)->controller_index, mp_pause_game_quit_confirmed, dialog_id);
	screen->start_animation(3);
}

/* whether the player may change its settings in this game */
// @retail 0x2b505c
bool __stdcall mp_pause_game_controller_settings_only(long controller_index)
{
	bool result = true;

	if (!function_1900a5(controller_index))
	{
		if (function_1906da(controller_index))
		{
			result = false;
		}
		if (function_6c7e0())
		{
			result = false;
		}
	}
	return result;
}

/* the player settings */
// @retail 0x232aff
void mp_pause_game_list_choose_settings(c_class_1473c9 *screen, s_controller_reference **controller)
{
	s_screen_parameters parameters;
	long window;
	long channel;

	parameters.field_c = 0;
	if (mp_pause_game_controller_settings_only((*controller)->controller_index))
	{
		window = screen->v21();
		channel = screen->v20();
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_231db5);
	}
	else
	{
		window = screen->v21();
		channel = screen->v20();
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_23252e);
	}
	parameters.load(&parameters);
}

/* the handicap */
// @retail 0x232b63
void mp_pause_game_list_choose_handicap(c_class_1473c9 *screen, s_controller_reference **controller)
{
	s_screen_parameters parameters;
	long window;
	long channel;

	screen->start_animation(3);
	parameters.field_c = 0;
	window = screen->v21();
	channel = screen->v20();
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_2323c3);
	parameters.load(&parameters);
}

/* whether the teams may be changed */
// @retail 0x232d06
bool mp_pause_game_can_change_teams()
{
	bool result = false;
	s_mp_pause_session_view *session = (s_mp_pause_session_view *)network_session_interface_get_data_4db0();

	if (session && function_199967())
	{
		if ((TEST_FIELD_BIT(session->flag0) || TEST_FIELD_BIT(session->flag5)) && TEST_FIELD_BIT(session->flag6))
		{
			result = true;
		}
	}
	return result;
}

/* change teams */
// @retail 0x232bb4
void mp_pause_game_list_choose_change_teams(c_class_1473c9 *screen, s_controller_reference **controller)
{
	if (mp_pause_game_can_change_teams())
	{
		s_screen_parameters parameters;
		long window;
		long channel;

		parameters.field_c = 0;
		window = screen->v21();
		channel = screen->v20();
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_23246a);
		parameters.load(&parameters);
	}
}

/* the controller settings */
// @retail 0x232c05
void mp_pause_game_list_choose_squad_privacy(c_class_1473c9 *screen, s_controller_reference **controller)
{
	s_screen_parameters parameters;
	long window;
	long channel;

	parameters.field_c = 0;
	window = screen->v21();
	channel = screen->v20();
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_2325fb);
	parameters.load(&parameters);
}

/* asks whether to end the game */
// @retail 0x232c4d
void mp_pause_game_list_choose_end_game(c_class_1473c9 *screen, s_controller_reference **controller)
{
	long state = function_19989d();

	if (function_592f0() && state != 6)
	{
		dialog_choice_show_default(screen->v20(), screen->v21(), 1 << (*controller)->controller_index, mp_pause_game_end_game_confirmed, 0xaf);
		screen->start_animation(3);
	}
}

/* a pause list item */
// @retail 0x2326bc
c_mp_pause_game_list_item::c_mp_pause_game_list_item()
{
}

/* B, back or start closes the pause screen */
// @retail 0x2326ce
bool c_mp_pause_game_list_item::v10(s_widget_event *event)
{
	if (event->type == 5 && (event->param == 0xc || event->param == 0xd || event->param == 1))
	{
		get_screen()->start_animation(3);
		return true;
	}
	return c_class_14750b::v10(event);
}

/* the pause list's items: quit, settings, handicap, change teams (when
   allowed), squad privacy (on Xbox Live) and end game (when it may end) */
// @retail 0x232708
c_mp_pause_game_list::c_mp_pause_game_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_mp_pause_game_list::handle_item)
{
	long state = function_19989d();
	bool can_end = function_592f0() && state != 6;

	data = user_interface_data_new("mp pause game list", 6, 4);
	function_16b790(data);
	((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 0;
	((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 1;
	((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 2;
	if (mp_pause_game_can_change_teams())
	{
		((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 3;
	}
	if (function_592f0() && !function_1999b3())
	{
		((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 4;
	}
	if (can_end)
	{
		((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 5;
	}
	delegate_register(&item_handlers, &handler);
}

/* runs the chosen pause list item */
// @retail 0x2328b5
void c_mp_pause_game_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		c_class_1473c9 *screen = get_screen();
		s_list_item_datum *datum = &((s_list_item_datum *)data->data)[*item & 0xffff];

		s_list_item_datum *const *datum_reference = &datum;
		switch ((*datum_reference)->item)
		{
		case 0:
			mp_pause_game_list_choose_quit(screen, controller);
			break;
		case 1:
			mp_pause_game_list_choose_settings(screen, controller);
			break;
		case 2:
			mp_pause_game_list_choose_handicap(screen, controller);
			break;
		case 3:
			mp_pause_game_list_choose_change_teams(screen, controller);
			break;
		case 4:
			mp_pause_game_list_choose_squad_privacy(screen, controller);
			break;
		default:
			mp_pause_game_list_choose_end_game(screen, controller);
			break;
		}
	}
}

/* shows or hides bitmap 4 by the player slot's flag */
// @retail 0x23296e
void c_mp_pause_game_screen::v3()
{
	c_class_1a2c81 *bitmap = find_child(8, 4, false);

	if (bitmap)
	{
		long controller = get_controller_index();

		if (controller != NONE)
		{
			bitmap->value6e = ((s_player_slot_pause_view *)g_54e8e0)[controller].bitmap_shown;
		}
	}
	c_class_1a2c81::v3();
}
