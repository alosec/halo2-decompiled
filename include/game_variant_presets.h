/* GAME_VARIANT_PRESETS.H: the preset game variants of the game variant
   settings tag (s_game_variant_settings, game_variant_settings.h), which
   game_variant_settings_add_presets (src/game_variant_presets.cpp) adds to
   the saved game files. */
#ifndef GAME_VARIANT_PRESETS_H
#define GAME_VARIANT_PRESETS_H

#include <stddef.h>
#include "game_variant_settings.h"

/* one preset variant (0x14 bytes): the string naming it (in the tag's
   preset string list), its engine (0 to 6, turned into the game engine type
   by 0x216ab0), the settings it changes from that engine's defaults, and a
   percentage (0 to 100) stored in the variant's byte 3 */
struct s_game_variant_preset
{
	long name_string;
	long engine_index;
	long setting_count;
	s_game_variant_default_setting *settings;
	char percentage;
	byte unknown11[3];
};
typedef char assert_game_variant_preset_size[(sizeof(s_game_variant_preset) == 0x14) ? 1 : -1];
typedef char assert_game_variant_preset_percentage[(offsetof(s_game_variant_preset, percentage) == 0x10) ? 1 : -1];
typedef char assert_game_variant_settings_presets[(offsetof(s_game_variant_settings, presets) == 0x2c) ? 1 : -1];
typedef char assert_game_variant_settings_preset_strings[(offsetof(s_game_variant_settings, preset_string_list_index) == 0x24) ? 1 : -1];

/* src/game_variant_presets.cpp */
void __stdcall game_variant_settings_add_presets(void *settings_tag);

#endif
