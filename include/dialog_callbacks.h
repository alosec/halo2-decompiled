/* DIALOG_CALLBACKS.H: the callbacks of the choice dialogs that leave the
   game: for the dashboard, for the main menu, or for the four way sign in
   (src/dialog_callbacks.cpp). Each takes the controller that chose and
   returns true. */

#ifndef DIALOG_CALLBACKS_H
#define DIALOG_CALLBACKS_H

#include "unknown_11c920.h"

/* the dashboard pages 0x2238f4 (unknown_22376b.cpp) launches the dashboard
   at: the index of its XLD_LAUNCH_DASHBOARD_* reason */
enum e_dashboard_page
{
	_dashboard_page_main_menu = 0,
	_dashboard_page_memory,
	_dashboard_page_network_configuration,
	_dashboard_page_new_account_signup,
	_dashboard_page_account_management,
	_dashboard_page_online_menu
};

bool __stdcall dialog_callback_return_to_main_menu(long controller);
bool __stdcall dialog_callback_dashboard_network_configuration(long controller);
bool __stdcall dialog_callback_leave_sessions(long controller);
bool __stdcall dialog_callback_dashboard_online_menu(long controller);
void dialog_callback_leave_sessions_to_main_menu();
bool __stdcall dialog_callback_sign_out_to_main_menu(long controller);
bool __stdcall dialog_callback_dashboard_memory(long controller);
bool __stdcall dialog_callback_dashboard_memory_for_profile(long controller);
bool __stdcall dialog_callback_dashboard_memory_for_game_variant(long controller);
bool __stdcall dialog_callback_dashboard_memory_for_minimum_storage(long controller);
bool __stdcall dialog_callback_split_screen_sign_in(long controller);
bool __stdcall dialog_callback_system_link_sign_in(long controller);

#endif
