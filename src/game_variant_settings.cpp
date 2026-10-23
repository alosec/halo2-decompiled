// @flags /O1 /Gr
/* GAME_VARIANT_SETTINGS.CPP: lookups in the game variant settings tag: its
   categories by id, its setting tags by setting index, and a setting's
   options by value */

#include "unknown_11c920.h"
#include "globals.h"
#include "ui_definition.h"
#include "game_variant_settings.h"

/* the game mode's game variant settings tag, or none */
// @retail 0x236a7f
s_game_variant_settings *game_variant_settings_table(void)
{
	s_game_variant_settings *result = 0;
	s_user_interface_shared_globals *shared_globals = ui_mode_definition_get();

	if (shared_globals && shared_globals->game_variant_settings.tag_index != NONE)
		result = (s_game_variant_settings *)g_4e3b44[shared_globals->game_variant_settings.tag_index & 0xffff].bytes;
	return result;
}

/* the setting tag that edits a setting index, searched in every category */
// @retail 0x2369b3
s_game_variant_setting_definition *game_variant_settings_find_setting(long setting_index)
{
	s_game_variant_setting_definition *result = 0;
	s_game_variant_settings *settings = game_variant_settings_table();

	if (settings)
	{
		for (long i = 0; i < settings->num_categories && !result; i++)
		{
			s_game_variant_setting_category *category = &settings->categories[i];
			for (long j = 0; j < category->setting_count && !result; j++)
			{
				s_tag_reference_8 *reference = &category->settings[j];

				if (reference->tag_index != NONE)
				{
					s_game_variant_setting_definition *setting = (s_game_variant_setting_definition *)g_4e3b44[reference->tag_index & 0xffff].bytes;
					if (setting->setting_index == setting_index)
						result = setting;
				}
			}
		}
	}
	return result;
}

/* the category with an id */
// @retail 0x236a3a
s_game_variant_setting_category *game_variant_settings_find_category(long category_id)
{
	s_game_variant_setting_category *result = 0;
	s_game_variant_settings *settings = game_variant_settings_table();

	if (settings)
	{
		for (long i = 0; i < settings->num_categories && !result; i++)
		{
			s_game_variant_setting_category *category = &settings->categories[i];

			if (category->category_id == category_id)
				result = category;
		}
	}
	return result;
}

/* a setting's option that stands for a value */
// @retail 0x236aa9
s_game_variant_setting_option *game_variant_setting_find_option(s_game_variant_setting_definition *setting, long value)
{
	s_game_variant_setting_option *result = 0;

	for (long i = 0; i < setting->option_count && !result; i++)
	{
		s_game_variant_setting_option *option = &setting->options[i];

		if (option->value == value)
			result = option;
	}
	return result;
}
