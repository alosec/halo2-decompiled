/* USER_INTERFACE_LISTS.H: the lists of 0x2b0000..0x2cb8c0 that the screens
   of other files hold as members */

#ifndef USER_INTERFACE_LISTS_H
#define USER_INTERFACE_LISTS_H

#include "unknown_11c920.h"
#include "screen_widgets.h"

/* "settings list" (vtable 0x45b060; unknown_2b116a.cpp) */
class c_class_232671 : public c_class_1474e8
{
public:
	c_class_232671(word user_flags);

	virtual long get_item_count();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[3];
	c_list_item_handler handler;
	bool extended;
};

/* "squad privacy setting list" (vtable 0x45ba38; unknown_2b116a.cpp) */
class c_squad_privacy_setting_list : public c_class_1474e8
{
public:
	c_squad_privacy_setting_list(word user_flags);

	/* focuses the squad's privacy setting */
	virtual void v1();
	virtual long get_item_count();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[3];
	c_list_item_handler handler;
};

/* "xbox live menu list" (vtable 0x458b48; unknown_23068b.cpp) */
class c_xbox_live_menu_list : public c_class_1474e8
{
public:
	c_xbox_live_menu_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	c_list_item_handler handler;
};

/* "mp controller settings game list" (vtable 0x45b778; unknown_2b116a.cpp) */
class c_mp_controller_settings_game_list : public c_class_1474e8
{
public:
	c_mp_controller_settings_game_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[5];
	c_list_item_handler handler;
};

/* "handicap settings edit list" (vtable 0x45b8d8; unknown_2b116a.cpp) */
class c_handicap_settings_edit_list : public c_class_1474e8
{
public:
	c_handicap_settings_edit_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	c_list_item_handler handler;
};

/* "variant editing options list" (vtable 0x45bdb8; unknown_2b116a.cpp) */
class c_variant_editing_options_list : public c_class_1474e8
{
public:
	c_variant_editing_options_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[6];
	c_list_item_handler handler;
};

/* "player profile edit list" (vtable 0x45bea0; unknown_2b116a.cpp) */
class c_player_profile_edit_list : public c_class_1474e8
{
public:
	c_player_profile_edit_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[6];
	c_list_item_handler handler;
};

/* "friends options list" (vtable 0x45b370; unknown_2b116a.cpp) */
class c_friends_options_list : public c_class_1474e8
{
public:
	c_friends_options_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[6];
	/* the online Y menu's tab fills these in (online_y_menu_screen.cpp) */
	word *name;
	void *entries;
	long entry_count;
	s_record_pool *source;
	c_list_item_handler handler;
};

/* the friends options screen (vtable 0x458de8; unknown_23068b.cpp) */
class c_friends_options_screen : public c_screen_with_menu
{
public:
	c_friends_options_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	c_friends_options_list list;
};

/* the items of the list at 0x459018 (vtable 0x459070): a press of start
   animates the screen */
class c_pause_game_list_item : public c_class_14750b
{
public:
	c_pause_game_list_item();

	virtual bool v10(s_widget_event *event);
};

/* "pause game list" (vtable 0x459018; unknown_23068b.cpp) */
class c_pause_game_list : public c_class_1474e8
{
public:
	c_pause_game_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_pause_game_list_item items[5];
	c_list_item_handler handler;
};

/* "mp player settings game list" (vtable 0x45b9c0; unknown_2b116a.cpp) */
class c_mp_player_settings_game_list : public c_class_1474e8
{
public:
	c_mp_player_settings_game_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	c_list_item_handler handler;
};

/* "clan options list" (vtable 0x45b1b0; unknown_2b116a.cpp) */
class c_clan_options_list : public c_class_1474e8
{
public:
	c_clan_options_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	/* the online Y menu's tab fills these in (online_y_menu_screen.cpp) */
	word *name;
	void *entries;
	long entry_count;
	s_record_pool *source;
	c_list_item_handler handler;
};

/* the clan options screen (vtable 0x458d08; unknown_23068b.cpp) */
class c_clan_options_screen : public c_screen_with_menu
{
public:
	c_clan_options_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	c_clan_options_list list;
};

/* "mp change teams list" (vtable 0x45b950; unknown_2b116a.cpp) */
class c_mp_change_teams_list : public c_class_1474e8
{
public:
	c_mp_change_teams_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[5];
	c_list_item_handler handler;
};

/* "custom game profile list" (vtable 0x45d768; unknown_2c9ddb.cpp): the
   saved variants of one game type */
class c_class_2c9e69 : public c_class_1474e8
{
public:
	c_class_2c9e69(word user_flags);

	virtual void v1();
	virtual void v3();
	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);
	/* rebuilds the items from the saved variants */
	void fill();
	void fill_and_select_first();
	void fill_and_keep_focus();
	void select_last_variant();

	c_class_14750b items[15];
	long variants[0x1065];
	long game_type;
	long value49a0;
	long value49a4;
	bool flag_a;
	bool flag_b;
	bool flag_c;
	c_list_item_handler handler;
};

/* "voice mask list" (vtable 0x45cbd0; unknown_2c4e9c.cpp) */
class c_voice_mask_list : public c_class_1474e8
{
public:
	c_voice_mask_list(word user_flags);

	virtual void v1();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[2];
	bool value188;
	byte unknown189[0x18c - 0x189];
	c_list_item_handler handler;
};

/* "voice through tv list" (vtable 0x45cc38; unknown_2c4e9c.cpp) */
class c_voice_through_tv_list : public c_class_1474e8
{
public:
	c_voice_through_tv_list(word user_flags);

	virtual void v1();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	bool value288;
	byte unknown289[0x28c - 0x289];
	c_list_item_handler handler;
};

/* "thumbstick settings edit list" (vtable 0x45cca8; unknown_2c4e9c.cpp) */
class c_thumbstick_settings_edit_list : public c_class_1474e8
{
public:
	c_thumbstick_settings_edit_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	bool value288;
	byte unknown289[0x28c - 0x289];
	c_list_item_handler handler;
};

/* "look sensitivity settings edit list" (vtable 0x45cd20; unknown_2c4e9c.cpp) */
class c_look_sensitivity_settings_edit_list : public c_class_1474e8
{
public:
	c_look_sensitivity_settings_edit_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	bool value288;
	byte unknown289[0x28c - 0x289];
	c_list_item_handler handler;
};

/* "invert look settings edit list" (vtable 0x45ce38; unknown_2c4e9c.cpp) */
class c_invert_look_settings_edit_list : public c_class_1474e8
{
public:
	c_invert_look_settings_edit_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[2];
	bool value188;
	byte unknown189[0x18c - 0x189];
	c_list_item_handler handler;
};

/* "button settings edit list" (vtable 0x45cdc0; unknown_2c4e9c.cpp) */
class c_button_settings_edit_list : public c_class_1474e8
{
public:
	c_button_settings_edit_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	bool value288;
	byte unknown289[0x28c - 0x289];
	c_list_item_handler handler;
};

/* "auto level settings edit list" (vtable 0x45ce38; unknown_2c4e9c.cpp) */
class c_auto_level_settings_edit_list : public c_class_1474e8
{
public:
	c_auto_level_settings_edit_list(word user_flags);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[2];
	bool value188;
	byte unknown189[0x18c - 0x189];
	c_list_item_handler handler;
};

/* "vibration settings edit list" (vtable 0x45ceb0; unknown_2c4e9c.cpp) */
class c_vibration_settings_edit_list : public c_class_1474e8
{
public:
	c_vibration_settings_edit_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[2];
	bool value188;
	byte unknown189[0x18c - 0x189];
	c_list_item_handler handler;
};

/* "subtitle setting list" (vtable 0x45d4f0; unknown_2c4e9c.cpp) */
class c_subtitle_setting_list : public c_class_1474e8
{
public:
	c_subtitle_setting_list(word user_flags);

	virtual void v1();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[3];
	c_list_item_handler handler;
};

/* "choose model list" (vtable 0x45d8c8; unknown_2c4e9c.cpp) */
class c_choose_model_list : public c_class_1474e8
{
public:
	c_choose_model_list(word user_flags);

	/* shows the model's name */
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[2];
	c_list_item_handler handler;
};

/* "xbox live appear offline list" (vtable 0x45cb58; unknown_2c4e9c.cpp) */
class c_xbox_live_appear_offline_list : public c_class_1474e8
{
public:
	c_xbox_live_appear_offline_list(word user_flags);

	virtual void v1();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[2];
	c_list_item_handler handler;
};

/* "controller settings edit list" (vtable 0x45d240; unknown_2c4e9c.cpp) */
class c_controller_settings_edit_list : public c_class_1474e8
{
public:
	c_controller_settings_edit_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[6];
	c_list_item_handler handler;
};

/* "multiplayer settings edit list" (vtable 0x45d478; unknown_2c4e9c.cpp) */
class c_multiplayer_settings_edit_list : public c_class_1474e8
{
public:
	c_multiplayer_settings_edit_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[7];
	c_list_item_handler handler;
};

/* "choose player color list" (vtable 0x45d850; unknown_2c4e9c.cpp) */
class c_choose_player_color_list : public c_class_1474e8
{
public:
	c_choose_player_color_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	c_list_item_handler handler;
	long value2a0;
};

/* "choose emblem list" (vtable 0x45d7e0; unknown_2c4e9c.cpp): the emblems
   of one of the two kinds */
class c_choose_emblem_list : public c_class_1474e8
{
public:
	c_choose_emblem_list(word user_flags, long mode);

	/* shows the emblem's name */
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	c_list_item_handler handler;
	long mode;
};

/* "legalese acceptance list" (vtable 0x458988; main_menu.cpp) */
class c_legalese_acceptance_list : public c_class_1474e8
{
public:
	c_legalese_acceptance_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[2];
	c_list_item_handler handler;
	bool accepted;
};

/* "main menu list" (vtable 0x458a70; main_menu.cpp) */
class c_main_menu_list : public c_class_1474e8
{
public:
	c_main_menu_list(word user_flags);

	virtual void *get_item_data();
	virtual long get_item_count();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[5];
	c_list_item_handler handler;
};

/* the multiplayer pause list's item (vtable 0x4592f0): B, back or start
   closes the menu */
class c_mp_pause_game_list_item : public c_class_14750b
{
public:
	c_mp_pause_game_list_item();

	virtual bool v10(s_widget_event *event);
};

/* "mp pause game list" (vtable 0x459298; mp_pause_game.cpp) */
class c_mp_pause_game_list : public c_class_1474e8
{
public:
	c_mp_pause_game_list(word user_flags);

	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_mp_pause_game_list_item items[5];
	c_list_item_handler handler;
};

/* "difficulty list" (vtable 0x45d688; unknown_2c4e9c.cpp) */
class c_difficulty_list : public c_class_1474e8
{
public:
	c_difficulty_list(word user_flags);

	virtual void v1();
	/* Retail vtable slots 18/19 use the common +0x88 item accessor and a
	 * constant-four count (0x19b516/0x334304). */
	virtual void *get_item_data() { return items; }
	virtual long get_item_count() { return 4; }
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	c_list_item_handler handler;
	bool alternate;
	bool value2a1;
};

/* "custom game maps" (vtable 0x45d618; unknown_2c4e9c.cpp): the maps a
   custom game, or a coop game, can be played on */
class c_custom_game_maps_list : public c_class_1474e8
{
public:
	c_custom_game_maps_list(word user_flags);

	virtual void v1();
	/* folded with c_widget's v2 and c_list_45cf40's item count */
	virtual void *get_item_data() { return items; }
	virtual long get_item_count() { return 14; }
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);
	void select_last_map();

	c_class_14750b items[14];
	bool coop;
	c_list_item_handler handler;
};

/* "campaign level handles" (vtable 0x45ca68; unknown_2c4e9c.cpp): the
   campaign's levels, or (alternate) the levels from 0x69 on */
class c_campaign_level_handles_list : public c_class_1474e8
{
public:
	c_campaign_level_handles_list(word user_flags, bool alternate);

	/* selects the level played last */
	virtual void v1();
	/* folded with c_widget's v2 */
	virtual void *get_item_data() { return items; }
	virtual long get_item_count();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[15];
	c_list_item_handler handler;
	bool alternate;
	bool unlocked;
	long last_map_id;
};

/* "game engine variant category list" (vtable 0x45cad8; unknown_2c4e9c.cpp):
   the game engines a variant can be made of; it opens the engine's settings
   (edit_settings, edit_alternate, or the plain ones) or creates a variant of
   it (create) */
class c_game_engine_variant_category_list : public c_class_1474e8
{
public:
	c_game_engine_variant_category_list(word user_flags);

	/* selects the engine of the variant being edited */
	virtual void v1();
	/* folded with c_widget's v2 */
	virtual void *get_item_data() { return items; }
	virtual long get_item_count();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);
	void select_variant_engine();

	c_class_14750b items[9];
	bool edit_settings;
	bool create;
	bool edit_alternate;
	c_list_item_handler handler;
};

/* the variant game engine type screen (vtable 0x45aff0; unknown_2b116a.cpp) */
class c_variant_game_engine_type_screen : public c_screen_with_menu
{
public:
	c_variant_game_engine_type_screen(long a, long b, word user_flags, long screen_id, bool edit_settings, bool create, bool edit_alternate);

	virtual void v17();
	virtual screen_load_proc get_load_proc();

	c_game_engine_variant_category_list list;
	bool edit_settings;
	bool create;
	bool edit_alternate;
};

/* "clan member privileges" (vtable 0x45bab0; unknown_2c4e9c.cpp): the
   privilege to give a clan member, confirmed by a dialog */
class c_clan_member_privileges_list : public c_class_1474e8
{
public:
	c_clan_member_privileges_list(word user_flags);
	virtual ~c_clan_member_privileges_list();

	/* focuses the member's current privilege */
	virtual void v1();
	/* folded with c_widget's v2 */
	virtual void *get_item_data() { return items; }
	virtual long get_item_count() { return 4; }
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);
	void select_current_privilege();

	c_class_14750b items[4];
	c_list_item_handler handler;
	long privilege;
};

/* the clan member privileges screen (vtable 0x45bb08; unknown_2b116a.cpp) */
class c_clan_member_privileges_screen : public c_screen_with_menu
{
public:
	c_clan_member_privileges_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	c_clan_member_privileges_list list;
};

/* "xbox live message list" (vtable 0x45cf40; unknown_2c4e9c.cpp): what can
   be done with a message */
class c_xbox_live_message_list : public c_class_1474e8
{
public:
	c_xbox_live_message_list(word user_flags);
	~c_xbox_live_message_list();

	/* button event 1 makes the screen go back; 2 plays the voice mail again */
	virtual bool v10(s_widget_event *event);
	/* folded with c_widget's v2 */
	virtual void *get_item_data() { return items; }
	virtual long get_item_count();
	/* shows the item's string */
	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);
	/* fills the items with what can be done with the message */
	void set_message(struct s_online_message_view *message);

	c_class_14750b items[14];
	/* the message the screen shows */
	struct s_online_message_view *message;
	c_list_item_handler handler;
};

/* the campaign level select screen (vtable 0x45ae38; unknown_2b116a.cpp) */
class c_level_select_screen : public c_screen_with_menu
{
public:
	virtual void v2();
	virtual void v18(void *parameters);
	c_level_select_screen(long a, long b, word user_flags, bool alternate);
 __forceinline c_level_select_screen(long a, long b, word user_flags);

	/* shows the focused level's picture and description */
	virtual void v3();
	virtual screen_load_proc get_load_proc();

	c_campaign_level_handles_list list;
};

#endif
