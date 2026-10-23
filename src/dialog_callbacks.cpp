// @flags /O1 /Gr
/* DIALOG_CALLBACKS.CPP: the callbacks of the choice dialogs that leave the
   game: for one of the dashboard's pages, for the main menu, or for the four
   way sign in */

#include "unknown_11c920.h"
#include <xtl.h>
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "saved_game_storage.h"
#include "dialog_callbacks.h"

/* unknown_22376b.cpp: launches the dashboard at a page; never returns */
void function_2238f4(long page, dword context, dword parameter1, dword parameter2);
long saved_game_file_type_size_in_blocks(long type);
long minimal_storage_size_in_blocks();
/* loads the main menu's scenario (unknown_18ec20.cpp) */
void __stdcall function_18f1c0(long reason);
/* leaves both network sessions and their tasks (unknown_147f6d.cpp) */
void function_148823();
/* signs every controller out (unknown_190001.cpp) */
void function_1906b4(void);

/* the drive the dashboard's memory page is asked to make room on: the
   letter of the user data drive */
#define k_dashboard_memory_drive 'u'

/* back to the main menu (the callback of 0x19ae0f's dialog) */
// @retail 0x23690b
bool __stdcall dialog_callback_return_to_main_menu(long controller)
{
	function_18f1c0(0);
	return true;
}

/* the dashboard's network configuration */
// @retail 0x236917
bool __stdcall dialog_callback_dashboard_network_configuration(long controller)
{
	function_2238f4(_dashboard_page_network_configuration, 0, 0, 0);
	return true;
}

/* gives up the processor once, then leaves the sessions for the main menu */
// @retail 0x236926
bool __stdcall dialog_callback_leave_sessions(long controller)
{
	Sleep(0);
	dialog_callback_leave_sessions_to_main_menu();
	return true;
}

/* the dashboard's online menu */
// @retail 0x236937
bool __stdcall dialog_callback_dashboard_online_menu(long controller)
{
	function_2238f4(_dashboard_page_online_menu, 0, 0, 0);
	return true;
}

/* leaves the sessions and their tasks, then goes back to the main menu */
// @retail 0x236946
void dialog_callback_leave_sessions_to_main_menu()
{
	function_148823();
	function_18f1c0(0);
}

/* signs every controller out and goes back to the main menu */
// @retail 0x236953
bool __stdcall dialog_callback_sign_out_to_main_menu(long controller)
{
	function_1906b4();
	function_18f1c0(0);
	return true;
}

/* the dashboard's memory page */
// @retail 0x236964
bool __stdcall dialog_callback_dashboard_memory(long controller)
{
	function_2238f4(_dashboard_page_memory, 0, 0, 0);
	return true;
}

/* the dashboard's memory page, asking for room for a profile */
// @retail 0x236973
bool __stdcall dialog_callback_dashboard_memory_for_profile(long controller)
{
	function_2238f4(_dashboard_page_memory, 0, k_dashboard_memory_drive, saved_game_file_type_size_in_blocks(_saved_game_type_profile));
	return true;
}

/* the dashboard's memory page, asking for room for a game variant */
// @retail 0x236989
bool __stdcall dialog_callback_dashboard_memory_for_game_variant(long controller)
{
	function_2238f4(_dashboard_page_memory, 0, k_dashboard_memory_drive, saved_game_file_type_size_in_blocks(k_saved_game_file_first_variant_type));
	return true;
}

/* the dashboard's memory page, asking for the room the game needs */
// @retail 0x23699f
bool __stdcall dialog_callback_dashboard_memory_for_minimum_storage(long controller)
{
	function_2238f4(_dashboard_page_memory, 0, k_dashboard_memory_drive, minimal_storage_size_in_blocks());
	return true;
}

/* lets go of the online tasks and each controller's online state
   (unknown_06b590.cpp) */
void function_6cb60(void);
short player_slot_count_active(void);
/* the signed in controllers' mask (unknown_190001.cpp) */
word function_1901fc(void);
void __stdcall function_1483c3(long reason);
/* the four way sign in screen, for split screen (mode 1) and for system
   link (mode 2) (unknown_2523e3.cpp) */
c_class_1473c9 *__stdcall function_25240c(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_252433(s_screen_parameters *parameters);

/* split screen, after letting go of the online state: the four way sign in, or
   0x1483c3 when no player is active */
// @retail 0x236877
bool __stdcall dialog_callback_split_screen_sign_in(long controller)
{
	function_6cb60();
	if (player_slot_count_active() > 0)
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, function_1901fc(), 5, 4, (long)function_25240c);
		parameters.load(&parameters);
	}
	else
	{
		function_1483c3(0);
	}
	return true;
}

/* system link, after letting go of the online state: the four way sign in, or
   0x1483c3 when no player is active */
// @retail 0x2368c1
bool __stdcall dialog_callback_system_link_sign_in(long controller)
{
	function_6cb60();
	if (player_slot_count_active() > 0)
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, function_1901fc(), 5, 4, (long)function_252433);
		parameters.load(&parameters);
	}
	else
	{
		function_1483c3(0);
	}
	return true;
}
