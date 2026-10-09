// @flags /O1 /Oi /Gr
/* UNKNOWN_24B5BC.CPP: signing a player slot in with a
   profile and a Live user */

#include "unknown_11c920.h"
#include <string.h>
#include "unknown_24b5bc.h"
#include "unknown_19b510.h"

bool function_1a0540(s_player_profile_settings *settings, long profile_index);
void __stdcall function_18fb34(long player, s_player_profile_settings *settings, long profile_index);
void function_120df0(long index, wchar_t const *name);
void function_190001(long index, char const *name);
bool window_manager_channel_in_use(long channel);
void voice_start_engine(void);
void voice_stop_engine(void);
long function_6c8b0(XONLINE_USER *user, long player);
void function_1487c3(long controller_index, long task_index, long callback, long value, long context);
void __stdcall function_24b869(c_class_1473c9 *screen);
bool function_6c7e0();
short player_slot_count_active(void);
void function_1906b4(void);
void function_18fc08(long index);
void function_190074(long index, bool active);
void function_190d4b(long index);
void online_mutelist_dispose(long controller_index);
long __stdcall function_6cc10(long controller_index);
void function_190186(long controller);
void function_190728(long index);
void function_18fee9(XONLINE_USER const *user, long index);
void online_get_logon_users(XONLINE_USER *users);
bool __stdcall function_236937(long controller_index);
extern bool g_50944f;
bool __stdcall function_24ba7d(long controller_index);

/* the online task screen's (not decompiled yet) */
long g_475338;

/* whether two strings are equal */
// @retail 0x24b5bc
bool function_24b5bc(char const *a, char const *b)
{
	bool result = strcmp(a, b) == 0;

	return result;
}

// @retail 0x24b5ed
s_player_slot_profile::s_player_slot_profile()
{
	player = NONE;
	profile_index = NONE;
	memset(&settings, 0, sizeof(settings));
	memset(&user, 0, sizeof(user));
	memset(&value258, 0, sizeof(value258));
	callback = 0;
	task_type = 0;
}

// @retail 0x24b621
void s_player_slot_profile::initialize(long player)
{
	profile_index = NONE;
	this->player = player;
	memset(&settings, 0, sizeof(settings));
	memset(&user, 0, sizeof(user));
	memset(&value258, 0, sizeof(value258));
	callback = 0;
	task_type = 0;
}

// @retail 0x24b652
void s_player_slot_profile::set_profile_index(long profile_index)
{
	if (function_1a0540(&settings, profile_index))
	{
		this->profile_index = profile_index;
	}
}

/* whether a Live user is set */
inline bool xuid_valid(XUID const *xuid)
{
	return xuid && xuid->qwUserID != 0;
}

/* signs the slot in with a profile, with the gamertag of its Live user when
   it has one */
// @retail 0x24b70d
void s_player_slot_profile::set_profile(long profile_index)
{
	char name[16];

	if (function_1a0540(&settings, profile_index))
	{
		name[0] = 0;
		this->profile_index = profile_index;
		if (xuid_valid(&user.xuid))
		{
			strncpy(name, user.szGamertag, 16);
			name[15] = 0;
		}
		function_18fb34(player, &settings, this->profile_index);
		function_120df0(player, (wchar_t const *)settings.name);
		function_190001(player, name);
	}
}

/* signs the slot in with its profile, or with its Live user when it has one */
// @retail 0x24b664
void s_player_slot_profile::sign_in(player_sign_in_callback callback)
{
	if (profile_index != NONE)
	{
#if defined(H2_NATIVE_PORT)
		char name[16] = { 0 };
		function_18fb34(player, &settings, profile_index);
		function_120df0(player, (wchar_t const *)settings.name);
		function_190001(player, name);
		if (callback)
			callback(player, true);
#else
		if (xuid_valid(&user.xuid))
		{
			task_type = 1;
			this->callback = callback;
			sign_in_live();
		}
		else
		{
			char name[16] = { 0 };

			task_type = 0;
			function_18fb34(player, &settings, profile_index);
			function_120df0(player, (wchar_t const *)settings.name);
			function_190001(player, name);
			if (callback)
			{
				callback(player, true);
			}
		}
#endif
	}
	else if (callback)
	{
		callback(player, false);
	}
}

// @retail 0x24b6ea
void s_player_slot_profile::show_dialog(player_sign_in_callback callback, long dialog_id)
{
	this->callback = callback;
	dialog_choice_show_default(3, 4, 1 << player, function_24ba7d, dialog_id);
}

/* starts the Live sign in task, with a screen that waits for it */
// @retail 0x24b824
void s_player_slot_profile::sign_in_live()
{
	if (!window_manager_channel_in_use(1))
	{
		long task;

		voice_stop_engine();
		task = function_6c8b0(&user, player);
		if (task != NONE)
		{
			g_475338 = NONE;
			function_1487c3(player, task, (long)function_24b869, 0, (long)this);
		}
		else
		{
			sign_in_failed();
		}
	}
}

// @retail 0x24ba33
void s_player_slot_profile::sign_in_failed()
{
	voice_start_engine();
	task_type = 0;
	if (callback)
	{
		callback(player, false);
	}
}

/* signs the slot out (of Live first, with a screen that waits for it) */
// @retail 0x24b779
void s_player_slot_profile::sign_out()
{
	if (TEST_FIELD_BIT(((s_player_slot_sign_in_view *)g_54e8e0)[player].live))
	{
		if (window_manager_channel_in_use(1))
		{
			return;
		}
		task_type = 2;
		if (function_6c7e0() && player_slot_count_active() <= 1)
		{
			function_1906b4();
		}
		else
		{
			long task;

			function_18fc08(player);
			function_190074(player, false);
			voice_stop_engine();
			online_mutelist_dispose(player);
			task = function_6cc10(player);
			if (task != NONE)
			{
				g_475338 = NONE;
				function_1487c3(player, task, (long)function_24b869, 0, (long)this);
			}
			return;
		}
	}
	else
	{
		function_190d4b(player);
		task_type = 0;
	}
	if (callback)
	{
		callback(player, true);
	}
}

/* the sign out task ended */
// @retail 0x24ba50
void s_player_slot_profile::signed_out()
{
	function_190186(player);
	function_190d4b(player);
	voice_start_engine();
	task_type = 0;
	if (callback)
	{
		callback(player, false);
	}
}

/* the dialog's choice: sign out */
// @retail 0x24ba7d
bool __stdcall function_24ba7d(long controller_index)
{
	player_slot_profile_get(controller_index)->sign_out();
	return true;
}

/* the online task screen closed before its task ended (called from lane M's
   unknown_1a2ca7.cpp, which passes the slot profile untyped) */
// @retail 0x24bac5
void function_24bac5(void *data)
{
	s_player_slot_profile *profile = (s_player_slot_profile *)data;

	if (profile->task_type == 1)
	{
		profile->sign_in_failed();
	}
	else if (profile->task_type == 2)
	{
		profile->signed_out();
	}
}

/* the online task succeeded */
// @retail 0x24b971
void s_player_slot_profile::task_succeeded()
{
	XONLINE_USER users[4];

	online_get_logon_users(users);
	if (task_type == 1)
	{
		task_type = 0;
		function_190074(player, true);
		function_18fb34(player, &settings, profile_index);
		function_120df0(player, (wchar_t const *)settings.name);
		function_18fee9(&user, player);
		function_190001(player, user.szGamertag);
		function_190728(player);
		if (g_50944f)
		{
			dialog_choice_show(1, 0x28, 4, 1 << player, function_236937, 0, 0);
			g_50944f = false;
		}
	}
	else if (task_type == 2)
	{
		task_type = 0;
		function_190186(player);
		function_190d4b(player);
	}
	if (callback)
	{
		callback(player, false);
	}
}
