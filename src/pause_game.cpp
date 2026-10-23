// @flags /O1 /Gr
/* PAUSE_GAME.CPP: the campaign pause screen's list: resume, revert to the
   last checkpoint, restart the level, the controller settings and quit,
   with the dialogs that confirm the last two */

#include "unknown_11c920.h"
#include "main_globals.h"
#include "unknown_19b516.h"
#include "unknown_19b510.h"
#include "screen_widgets.h"
#include "user_interface_lists.h"

short player_slot_count_active(void);
c_class_1473c9 *__stdcall function_231db5(s_screen_parameters *parameters);

/* the pause list's items, in order */
enum e_pause_game_item
{
	_pause_game_item_resume,
	_pause_game_item_revert,
	_pause_game_item_restart_level,
	_pause_game_item_settings,
	_pause_game_item_quit,

	k_pause_game_item_count
};

/* the dialogs' choices: quit the game, restart the level */
// @retail 0x2323ab
bool __stdcall pause_game_quit_confirmed(long controller_index)
{
	main_globals.quit_game = true;
	return true;
}

// @retail 0x2323b7
bool __stdcall pause_game_restart_level_confirmed(long controller_index)
{
	main_globals.reset_map = true;
	return true;
}

/* reverts the game to its last checkpoint (main_globals.unknown6f, the
   revert the main loop handles) and closes the screen */
// @retail 0x23216c
void pause_game_list_choose_revert(c_class_1a2c81 *screen)
{
	main_globals.unknown6f = true;
	screen->start_animation(3);
}

/* asks whether to restart the level */
// @retail 0x23217b
void pause_game_list_choose_restart_level(c_class_1a2c81 *screen, s_controller_reference **controller)
{
	dialog_choice_show(3, 0xaa, 4, 1 << (*controller)->controller_index, pause_game_restart_level_confirmed, 0, 0);
	screen->start_animation(3);
}

/* opens the controller settings */
// @retail 0x2321ab
void pause_game_list_choose_settings(c_class_1473c9 *screen, s_controller_reference **controller)
{
	s_screen_parameters parameters;
	long window;
	long channel;

	screen->start_animation(3);
	parameters.field_c = 0;
	window = screen->v21();
	channel = screen->v20();
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_231db5);
	parameters.load(&parameters);
}

/* asks whether to quit the game (another dialog when several players are in it) */
// @retail 0x2321fc
void pause_game_list_choose_quit(c_class_1a2c81 *screen, s_controller_reference **controller)
{
	long dialog_id = 0xa8;

	if (player_slot_count_active() > 1)
	{
		dialog_id += 0xf;
	}
	dialog_choice_show_default(3, 4, 1 << (*controller)->controller_index, pause_game_quit_confirmed, dialog_id);
	screen->start_animation(3);
}

/* the list's handler: resume closes the screen */
// @retail 0x231fe3
void c_pause_game_list::handle_item(s_controller_reference **controller, long *item)
{
	c_class_1473c9 *screen = get_screen();

	switch (*item & 0xffff)
	{
	case _pause_game_item_resume:
		screen->start_animation(3);
		break;
	case _pause_game_item_revert:
		pause_game_list_choose_revert(screen);
		break;
	case _pause_game_item_restart_level:
		pause_game_list_choose_restart_level(screen, controller);
		break;
	case _pause_game_item_settings:
		pause_game_list_choose_settings(screen, controller);
		break;
	default:
		pause_game_list_choose_quit(screen, controller);
		break;
	}
}
