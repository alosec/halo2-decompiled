/* GAME_VARIANT_SETTINGS.H: the game variant settings tag, which the shared
   globals of each game mode name (s_user_interface_shared_globals,
   ui_definition.h): the settings a game variant has, grouped in categories,
   each with the values it can take; the preset variants; and the default
   settings of each game engine type. The lookups are in
   src/game_variant_settings.cpp. */

#ifndef GAME_VARIANT_SETTINGS_H
#define GAME_VARIANT_SETTINGS_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_234c64.h"

/* a value a setting can take, and the string that names it */
struct s_game_variant_preset;

struct s_game_variant_setting_option
{
	byte unknown00[4];
	long value;
	long name_string;
};
typedef char assert_game_variant_setting_option_size[(sizeof(s_game_variant_setting_option) == 0xc) ? 1 : -1];

/* a setting (a tag of its own): the variant field it edits (by 0x2374f0
   and 0x2373be), its strings and its values. Only its first 0x24 bytes are
   known. */
struct s_game_variant_setting_definition
{
	long setting_index;
	byte unknown04[8];
	/* the string list its strings are in, and their handles */
	long string_list_index;
	long name_string;
	long title_text;
	long description_string;
	long option_count;
	s_game_variant_setting_option *options;
};
typedef char assert_game_variant_setting_definition_size[(sizeof(s_game_variant_setting_definition) == 0x24) ? 1 : -1];
typedef char assert_game_variant_setting_definition_options[(offsetof(s_game_variant_setting_definition, option_count) == 0x1c) ? 1 : -1];

/* a category of settings: its id and the setting tags it lists */
struct s_game_variant_setting_category
{
	long category_id;
	byte unknown04[4];
	long setting_count;
	s_tag_reference_8 *settings;
	byte unknown10[8];
};
typedef char assert_game_variant_setting_category_size[(sizeof(s_game_variant_setting_category) == 0x18) ? 1 : -1];

/* one default setting of a game variant: the setting's index and its value
   (stored by 0x2373be) */
struct s_game_variant_default_setting
{
	long setting_index;
	long value;
};
typedef char assert_game_variant_default_setting_size[(sizeof(s_game_variant_default_setting) == 0x8) ? 1 : -1];

/* the default settings of one game engine type (0x14 bytes) */
struct s_game_engine_default_settings
{
	byte unknown00[8];
	long setting_count;
	s_game_variant_default_setting *settings;
	byte unknown10[4];
};
typedef char assert_game_engine_default_settings_size[(sizeof(s_game_engine_default_settings) == 0x14) ? 1 : -1];

/* the game variant settings tag (the part that is read): the categories of
   settings, the preset variants (read by 0x149f88) and the default settings
   of each game engine type, indexed by engine type - 1 (the engine types run
   from 1 to 9) */
struct s_game_variant_settings
{
	byte unknown00[0x18];
	long num_categories;
	s_game_variant_setting_category *categories;
	byte unknown20[4];
	long preset_string_list_index;
	long preset_count;
	s_game_variant_preset *presets;
	s_game_engine_default_settings engines[9];
};
typedef char assert_game_variant_settings_categories[(offsetof(s_game_variant_settings, num_categories) == 0x18) ? 1 : -1];
typedef char assert_game_variant_settings_engines[(offsetof(s_game_variant_settings, engines) == 0x30) ? 1 : -1];
typedef char assert_game_variant_settings_size[(sizeof(s_game_variant_settings) == 0xe4) ? 1 : -1];

/* game_variant_settings.cpp */
s_game_variant_settings *game_variant_settings_table(void);
s_game_variant_setting_definition *game_variant_settings_find_setting(long setting_index);
s_game_variant_setting_category *game_variant_settings_find_category(long category_id);
s_game_variant_setting_option *game_variant_setting_find_option(s_game_variant_setting_definition *setting, long value);

#endif
