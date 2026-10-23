// @flags /O1 /Oi /Gr
/* MAIN_MENU.CPP: the legal notice and main menu screens, which the window
   manager loads itself, and their lists */

#include "unknown_11c920.h"
#include "globals.h"
#include "screen_widgets.h"
#include "user_interface_lists.h"
#include "unknown_24b5bc.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "main_menu.h"

/* the legal notice screen (vtable 0x4537b8; its deleting destructor is
   folded with the appear offline screen's) */
class c_legalese_screen : public c_screen_with_menu
{
public:
	c_legalese_screen(long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual screen_load_proc get_load_proc();

	c_legalese_acceptance_list list;
};

/* the main menu screen (vtable 0x453828; its deleting destructor is folded
   with the multiplayer controller settings screen's) */
class c_main_menu_screen : public c_screen_with_menu
{
public:
	c_main_menu_screen(long a, long b, word user_flags);

	/* finishes a sign in the screen was waiting for, and shows the feedback
	   dialog once */
	virtual void v3();
	virtual bool v10(s_widget_event *event);
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_main_menu_list list;
	long pending_live_controller_index;
};

/* creates the legal notice screen */
// @retail 0x14741b
c_class_1473c9 *__stdcall legalese_open(s_screen_parameters *parameters)
{
	c_legalese_screen *screen = new c_legalese_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

/* the legal notice screen shows its acceptance list */
// @retail 0x147459
c_legalese_screen::c_legalese_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xcb, a, b, user_flags, &list),
	list(user_flags)
{
}

/* the legal notice screen's load procedure */
// @retail 0x14748e
screen_load_proc c_legalese_screen::get_load_proc()
{
	return legalese_open;
}

/* creates the main menu screen */
// @retail 0x14752c
c_class_1473c9 *__stdcall main_menu_open(s_screen_parameters *parameters)
{
	c_main_menu_screen *screen = new c_main_menu_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

/* the main menu screen, waiting for no controller's sign in */
// @retail 0x14756a
c_main_menu_screen::c_main_menu_screen(long a, long b, word user_flags) :
	c_screen_with_menu(6, a, b, user_flags, &list),
	list(user_flags),
	pending_live_controller_index(NONE)
{
}

/* the main menu screen's load procedure */
// @retail 0x1475a3
screen_load_proc c_main_menu_screen::get_load_proc()
{
	return main_menu_open;
}

/* the lists (in 0x2304d2..0x232708 in retail) */

/* the legal notice's two items: decline and accept */
// @retail 0x2304d2
c_legalese_acceptance_list::c_legalese_acceptance_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_legalese_acceptance_list::handle_item)
{
	data = user_interface_data_new("legalese acceptance list", 2, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
	accepted = false;
}

/* the main menu's five items */
// @retail 0x230738
c_main_menu_list::c_main_menu_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_main_menu_list::handle_item)
{
	data = user_interface_data_new("main menu list", 5, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

/* the text of a legal notice item */
// @retail 0x230591
void c_legalese_acceptance_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0xe000223;
			break;
		case 1:
			string_handle = 0x7000224;
			break;
		default:
			string_handle = 0;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

void __stdcall function_1483c3(long reason);
void function_2238f4(long page, dword context, dword parameter1, dword parameter2);

/* a "no" launches the dashboard; a "yes" goes on to the main screen */
// @retail 0x2305d0
void c_legalese_acceptance_list::handle_item(s_controller_reference **controller, long *item)
{
	switch ((short)*item)
	{
	case 0:
		function_2238f4(0, 0, 0, 0);
		break;
	case 1:
		function_1483c3(0);
		break;
	}
}

/* event 6 accepts the legal notice too */
// @retail 0x2305f5
bool c_legalese_screen::v10(s_widget_event *event)
{
	bool event_consumed_flag;
	if (event->type == 6)
	{
		function_1483c3(0);
		event_consumed_flag = true;
	}
	else
	{
		event_consumed_flag = c_class_1473c9::v10(event);
	}
	return event_consumed_flag;
}

/* vtable slots 18/19: retail shares the common +0x88 item accessor
   (0x19b516, unknown_19b516.cpp) and a constant-five count (0x230612,
   user_interface_list_methods.cpp), which are matched there */
void *c_main_menu_list::get_item_data()
{
	return (byte *)this + 0x88;
}

long c_main_menu_list::get_item_count()
{
	return 5;
}

/* the text of a main menu item */
// @retail 0x2307c8
void c_main_menu_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0x800010b;
			break;
		case 1:
			string_handle = 0x9000283;
			break;
		case 2:
			string_handle = 0xb000284;
			break;
		case 3:
			string_handle = 0xa000285;
			break;
		case 4:
			string_handle = 0x8000286;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

struct s_player_profile
{
	dword data[0x78];
};

bool function_19028d(void);
bool function_8d7c0(void);
bool function_6c7e0();
word function_1901fc(void);
void function_1906b4(void);
void __stdcall function_18f1c0(long a);
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index);
c_class_1473c9 *__stdcall function_230616(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_230691(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_24b4a9(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_25240c(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_252433(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_25245a(s_screen_parameters *parameters);
bool __stdcall dialog_callback_split_screen_sign_in(long controller_index);
bool __stdcall dialog_callback_system_link_sign_in(long controller_index);
bool __stdcall dialog_callback_dashboard_network_configuration(long controller_index);

extern bool g_54d5a0;

/* the main menu's last choice */
long g_main_menu_last_item;
bool g_510819;
bool g_main_menu_campaign_chosen;

/* the campaign */
// @retail 0x230888
void main_menu_list_choose_campaign(s_controller_reference **controller)
{
	if (g_54d5a0)
	{
		dialog_ok_show(1, 0x32, 4, 1 << (*controller)->controller_index, 0, 0);
	}
	else
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_230616);
		parameters.load(&parameters);
		g_main_menu_campaign_chosen = true;
	}
}

/* Xbox Live: signs the controller's player in first when it has a profile */
// @retail 0x2308e0
void __stdcall main_menu_list_choose_xbox_live(c_main_menu_list *list, s_controller_reference **controller)
{
	s_player_profile profile;
	s_screen_parameters parameters;

	parameters.field_c = 0;
	if (function_19028d())
	{
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_25245a);
		parameters.load(&parameters);
	}
	else if (function_8d7c0())
	{
		c_main_menu_screen *screen = (c_main_menu_screen *)list->get_screen();
		s_controller_reference *reference = *controller;
		s_player_slot_profile *slot_profile = player_slot_profile_get(reference->controller_index);
		long profile_index;

		player_slot_get_profile(reference->controller_index, &profile, &profile_index);
		if (profile_index != NONE)
		{
			slot_profile->initialize(reference->controller_index);
			slot_profile->set_profile_index(profile_index);
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_24b4a9);
			parameters.load(&parameters);
			screen->pending_live_controller_index = (*controller)->controller_index;
		}
		else
		{
			function_1906b4();
			function_18f1c0(0);
		}
	}
	else
	{
		dialog_choice_show(1, 0x23, 4, 1 << (*controller)->controller_index, dialog_callback_dashboard_network_configuration, 0, 0);
	}
}

/* split screen */
// @retail 0x2309ec
void main_menu_list_choose_split_screen(s_controller_reference **controller)
{
	if (g_54d5a0)
	{
		dialog_ok_show(1, 0x32, 4, 1 << (*controller)->controller_index, 0, 0);
	}
	else
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		if (function_6c7e0())
		{
			dialog_choice_show(3, 0x78, 4, 1 << (*controller)->controller_index, dialog_callback_split_screen_sign_in, 0, 0);
		}
		else
		{
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_25240c);
			parameters.load(&parameters);
		}
	}
}

/* system link */
// @retail 0x230a64
void main_menu_list_choose_system_link(s_controller_reference **controller)
{
	bool connected = function_8d7c0();

	if (g_54d5a0)
	{
		dialog_ok_show(1, 0x32, 4, 1 << (*controller)->controller_index, 0, 0);
	}
	else
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		if (function_6c7e0())
		{
			dialog_choice_show(3, 0x78, 4, 1 << (*controller)->controller_index, dialog_callback_system_link_sign_in, 0, 0);
		}
		else if (connected)
		{
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_252433);
			parameters.load(&parameters);
		}
		else
		{
			dialog_ok_show(1, 0x3b, 4, function_1901fc(), 0, 0);
		}
	}
}

/* the settings */
// @retail 0x230aff
void main_menu_list_choose_settings(s_controller_reference **controller)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_230691);
	parameters.load(&parameters);
}

/* runs the chosen main menu item and remembers it */
// @retail 0x230827
void c_main_menu_list::handle_item(s_controller_reference **controller, long *item)
{
	long index = *item & 0xffff;

	g_main_menu_last_item = index;
	g_510819 = true;
	switch (index)
	{
	case 0:
		main_menu_list_choose_campaign(controller);
		break;
	case 1:
		main_menu_list_choose_xbox_live(this, controller);
		break;
	case 2:
		main_menu_list_choose_split_screen(controller);
		break;
	case 3:
		main_menu_list_choose_system_link(controller);
		break;
	case 4:
		main_menu_list_choose_settings(controller);
		break;
	}
}

void __stdcall function_1483c3(long reason);

/* after the sign out, back to the main screen */
// @retail 0x230c7d
void __stdcall main_menu_screen_sign_out_done(long player, bool signed_in)
{
	function_1483c3(1);
}

extern bool g_54e7f8;

void function_199a03(long mode);
short player_slot_count_active(void);
void function_148823();
#include "unknown_234c64.h"

/* the main menu shows again: ends any profile edit, resets the session menu
   and selects the last item chosen */
// @retail 0x230b32
void c_main_menu_screen::v19()
{
	if (g_54e5d0.profile_index != NONE)
		profile_edit_end();
	function_199a03(NONE);
	g_54e7c0 = NONE;
	g_54e7c4 = NONE;
	set_user_flags(function_1901fc());
	c_class_1473c9::v19();
	if (player_slot_count_active() != 1)
	{
		function_148823();
		function_18f1c0(0);
	}
	if (g_main_menu_last_item >= 0 && g_main_menu_last_item < 5)
		list.select_item((short)g_main_menu_last_item);
	g_main_menu_campaign_chosen = false;
}

// @retail 0x230ba6
void c_main_menu_screen::v3()
{
	c_class_1a2c81::v3();
	long controller_index = pending_live_controller_index;
	if (controller_index != NONE)
	{
		if (!function_8d7c0())
		{
			pending_live_controller_index = NONE;
		}
		else if (function_19028d())
		{
			s_main_menu_selection press;
			s_main_menu_selection *reference = &press;

			press.type = 5;
			press.controller_index = controller_index;
			press.param = 0;
			press.source = 0xff;
			main_menu_list_choose_xbox_live(&list, (s_controller_reference **)&reference);
			pending_live_controller_index = NONE;
		}
	}
	if (g_54e7f8)
	{
		dialog_ok_show(3, 0x2c, 4, function_1901fc(), 0, 0);
		g_54e7f8 = false;
	}
}

/* B or back asks a signed in player whether to sign out */
// @retail 0x230c2b
bool c_main_menu_screen::v10(s_widget_event *event)
{
	long controller_index = event->controller_index;
	if (TEST_FIELD_BIT(((s_player_slot_sign_in_view *)g_54e8e0)[controller_index].signed_in) && event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 13:
			((s_player_slot_sign_in_view *)g_54e8e0)[controller_index].profile.show_dialog(main_menu_screen_sign_out_done, 0x30);
			return true;
		}
	}
	return c_class_1473c9::v10(event);
}
