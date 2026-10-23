#include "unknown_4cf790.h"
// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_06B590.CPP: Xbox Live logon users, services and passcodes
   (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "online_tasks.h"
#include "online_presence.h"
#include "globals.h"
#include "unknown_075870.h"
#include "unknown_0662e0.h"
#include "pending_messages.h"

/* the services the logon asks for; the last one is not a Live service */
struct s_online_service
{
	DWORD service_id;
	long unknown04;
	bool connect;
};

#define k_online_service_count 13

s_online_service g_467178[k_online_service_count] =
{
	{ 0x2, 1, false },
	{ 0x4, 1, false },
	{ 0x6, 1, false },
	{ 0x7, 1, false },
	{ 0x8, 1, false },
	{ 0x9, 1, false },
	{ 0xd, 1, false },
	{ 0xf, 1, false },
	{ 0x10, 1, false },
	{ 0x12, 1, false },
	{ 0x13, 1, false },
	{ 0x14, 1, false },
	{ 0x4d530064, 0, false },
};

// @retail 0x6b590
void online_get_title_name(DWORD title_id, WCHAR *name, long name_length)
{
	DWORD language = XGetLanguage();
	if (FAILED(XOnlineFriendsGetTitleName(title_id, language, name_length, name)))
	{
		wcsncpy(name, L"?", name_length - 1);
		name[name_length - 1] = 0;
	}
}

// @retail 0x6ced0
void online_get_logon_users(XONLINE_USER *users)
{
	memset(users, 0, sizeof(XONLINE_USER) * XONLINE_MAX_LOGON_USERS);
	XONLINE_USER *logon_users = XOnlineGetLogonUsers();
	if (logon_users)
		memcpy(users, logon_users, sizeof(XONLINE_USER) * XONLINE_MAX_LOGON_USERS);
}

#include "unknown_06c1d0.h"

/* the mute lists (unknown_058cb0.cpp) */
void online_mutelist_startup(long controller_index);
void online_mutelist_dispose(long controller_index);
void online_mutelist_get(long controller_index);

/* restarts the mute list of every signed-in, non-guest user */
// @retail 0x6c170
void online_mutelists_refresh(XONLINE_USER *users)
{
	for (long i = 0; i != NONE; i = controller_index_next(i))
	{
		XONLINE_USER *user = &users[i];
		if (user->xuid.qwUserID != 0 && user->szGamertag[0] && SUCCEEDED(user->hr))
		{
			XUID *xuid = online_user_get_xuid(users + i);
			if (!XOnlineIsUserGuest(xuid->dwUserFlags))
			{
				online_mutelist_dispose(i);
				online_mutelist_startup(i);
				online_mutelist_get(i);
			}
		}
	}
}

void online_presence_clear(XONLINE_USER *users);

// @retail 0x6c6c0
long online_get_logon_users_status(void)
{
	XONLINE_USER users[XONLINE_MAX_LOGON_USERS];
	long status = 0;

	online_get_logon_users(users);
	for (long i = 0; i < XONLINE_MAX_LOGON_USERS; i++)
	{
		XONLINE_USER *const &user = &users[i];
		HRESULT hr = user->hr;
		if (user && user->xuid.qwUserID != 0 && hr != S_OK)
		{
			if (hr == XONLINE_S_LOGON_USER_HAS_MESSAGE)
				status = 1;
			else if (hr == XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT)
				return 2;
		}
	}
	return status;
}

// @retail 0x6c750
long online_task_get_change_logon_status(s_type_9df9da *task)
{
	HRESULT results[XONLINE_MAX_LOGON_USERS];
	long status = 0;

	XOnlineChangeLogonUsersTaskGetResults((XONLINETASK_HANDLE)task->handle, results);
	for (long i = 0; i < XONLINE_MAX_LOGON_USERS; i++)
	{
		HRESULT hr = results[i];
		if (hr != S_OK)
		{
			if (hr == XONLINE_S_LOGON_USER_HAS_MESSAGE)
				status = 1;
			else if (hr == XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT)
				return 2;
		}
	}
	return status;
}

// @retail 0x6c7a0
void online_get_service_ids(DWORD *service_ids)
{
	for (long i = 0; i < k_online_service_count; i++)
		service_ids[i] = g_467178[i].service_id;
}

// @retail 0x6c800
DWORD online_get_users(XONLINE_USER *users)
{
	DWORD user_count;

	GetTickCount();
	HRESULT hr = XOnlineGetUsers(users, &user_count);
	GetTickCount();
	if (FAILED(hr))
		user_count = 0;
	return user_count;
}

// @retail 0x6c830
bool online_user_requires_passcode(const XONLINE_USER *user)
{
	bool result = false;
	if (!XOnlineIsUserGuest(user->xuid.dwUserFlags) && (user->dwUserOptions & XONLINE_USER_OPTION_REQUIRE_PASSCODE))
		result = true;
	return result;
}

// @retail 0x6c850
byte function_6c850(byte button)
{
	switch (button)
	{
	case 2:
		return XONLINE_PASSCODE_GAMEPAD_X;
	case 3:
		return XONLINE_PASSCODE_GAMEPAD_Y;
	case 6:
		return XONLINE_PASSCODE_GAMEPAD_LEFT_TRIGGER;
	case 7:
		return XONLINE_PASSCODE_GAMEPAD_RIGHT_TRIGGER;
	case 8:
		return XONLINE_PASSCODE_DPAD_UP;
	case 9:
		return XONLINE_PASSCODE_DPAD_DOWN;
	case 10:
		return XONLINE_PASSCODE_DPAD_LEFT;
	case 11:
		return XONLINE_PASSCODE_DPAD_RIGHT;
	default:
		return 0;
	}
}

/* minutes in a byte: exact below an hour, then 5- and 10-minute steps */
#define ENCODE_MINUTES(result, minutes) \
	if ((minutes) < 60) \
		(result) = (byte)(minutes); \
	else if ((minutes) < 180) \
		(result) = (byte)(((minutes) - 60) / 5 + 60); \
	else if ((minutes) < 600) \
		(result) = (byte)(((minutes) - 180) / 10 + 84); \
	else \
		(result) = 126

// @retail 0x6cf00
void online_presence_build(s_online_presence *presence, const s_online_presence_source *source)
{
	*(dword *)presence = 0;
	presence->magic = 0xfcf1;

	byte time = 0;
	if (source->minutes_a > 0)
	{
		ENCODE_MINUTES(time, source->minutes_a);
	}
	else if (source->minutes_b > 0)
	{
		ENCODE_MINUTES(time, source->minutes_b);
		time += 127;
	}
	presence->time = time;

	byte state = 0;
	switch (source->state)
	{
	case 1:
		state = 0;
		break;
	case 2:
		state = 4;
		break;
	case 3:
		state = 8;
		break;
	case 4:
		state = 12;
		break;
	case 5:
		state = 1;
		break;
	case 6:
		state = 3;
		break;
	}
	presence->state = state;
	presence->unknown08 = source->unknown08;
	presence->unknown04 = source->unknown04;
}

// @retail 0x6d210
bool online_get_accepted_game_invite(XONLINE_ACCEPTED_GAMEINVITE *invite)
{
	return SUCCEEDED(XOnlineFriendsGetAcceptedGameInvite(invite));
}

// @retail 0x6d220
void online_connect_to_service_20(void)
{
	bool connect = false;

	for (long i = 0; i < k_online_service_count; i++)
	{
		if (g_467178[i].service_id == 0x14)
			connect = g_467178[i].connect;
	}
	if (connect)
	{
		XONLINE_SERVICE_INFO info;
		if (SUCCEEDED(XOnlineGetServiceInfo(0x14, &info)))
			XNetConnect(info.serviceIP);
	}
}

extern bool g_50944f;

// @retail 0x6c8b0
long function_6c8b0(long controller, XONLINE_USER *user)
{
 long result = NONE;
 if (g_transport_globals.initialized && g_transport_globals.started)
 {
  if (g_467214 == NONE)
  {
   XONLINE_USER users[XONLINE_MAX_LOGON_USERS];
   memset(users, 0, sizeof(users));
   users[controller] = *user;
   g_467214 = online_task_new();
   s_type_9df9da *task = function_6b910(g_467214);
   if (task)
   {
    DWORD services[k_online_service_count];
    online_get_service_ids(services);
    g_50944f = false;
    if (SUCCEEDED(XOnlineLogon(users, services, k_online_service_count, 0,
     (XONLINETASK_HANDLE *)&task->handle)))
    {
     task->flags = 1;
     task->type = 0;
     task->controller_index = NONE;
     result = g_467214;
    }
    else
    {
     function_6b640(g_467214);
     g_467214 = NONE;
    }
   }
  }
  else
  {
   XONLINE_USER users[XONLINE_MAX_LOGON_USERS];
   memset(users, 0, sizeof(users));
   users[controller] = *user;
   for (long i = 0; i < XONLINE_MAX_LOGON_USERS; i++)
   {
    if (i != controller && (bool)((*(dword *)&g_54e8e0[i] >> 5) & 1))
     users[i] = *(const XONLINE_USER *)((const byte *)&g_54e8e0[i] + 0x470);
   }
   long index = online_task_new_if_logged_on();
   s_type_9df9da *task = online_task_try_get(index);
   if (task)
   {
    g_50944f = false;
    online_connect_to_service_20();
    if (SUCCEEDED(XOnlineChangeLogonUsers(users, 0, (XONLINETASK_HANDLE *)&task->handle)))
    {
     task->flags = 1;
     task->type = 1;
     task->controller_index = controller;
     result = index;
    }
    else
     function_6b640(index);
   }
  }
 }
 return result;
}



struct s_game_variant_globals
{
 dword unknown0;
 union { dword flags; word field_4; };
 word count;
 word state;
};
extern s_pending_message_header g_479780;
extern s_game_variant_globals g_47d8f4;
void __stdcall function_8e0f0(long index, long result);
void online_tasks_dispose_all(void);
void function_190da5(long index);
void function_7f410(void);

// @retail 0x6cb60
void function_6cb60(void)
{
 if (g_467214 != NONE)
 {
  for (long i = 0; i < 32; i++)
   function_8e0f0(i, 13);
  online_tasks_dispose_all();
  g_467214 = NONE;
 }
 for (long controller = 0; controller != NONE; controller = controller_index_next(controller))
  function_190da5(controller);
 g_4cf8e4->flag4e00 = false;
 g_4cf95c = false;
 function_7f410();
 for (long i = 0; i < g_transport_globals.transition_function_count; i++)
  if (g_transport_globals.reset_functions[i])
   g_transport_globals.reset_functions[i](g_transport_globals.contexts[i]);
 g_479780.flags &= (word)~2;
 g_47d8f4.field_4 &= (word)~2;
 g_477058.field_4 &= (word)~2;
}

void function_8e5e0(long status, long task_index);

// @retail 0x6c4a0
void function_6c4a0(long result, long task_index)
{
 switch (result)
 {
 case 0:
 case 0x1500f0:
  function_8e5e0(0, task_index); break;
 case (long)0x8007000e:
 case (long)0x80150002:
 case (long)0x80150003:
 case (long)0x80150005:
 case (long)0x80150006:
 case (long)0x80150008:
  function_8e5e0(2, task_index); break;
 case (long)0x80150004:
 case (long)0x8015c002:
  function_8e5e0(3, task_index); break;
 case (long)0x8015c004:
 case (long)0x8015c007:
  function_8e5e0(5, task_index); break;
 case (long)0x8015c006:
 case (long)0x8015c008:
 case (long)0x8015c009:
  function_8e5e0(4, task_index); break;
 default:
  function_8e5e0(1, task_index); break;
 }
}

void function_190d0a(long index);

#pragma inline_depth(0)
static __forceinline void function_6cc12(long controller)
{
 function_190d0a(controller);
}
#pragma inline_depth(255)

static __forceinline s_type_9df9da *function_6cc14(long index)
{
 return online_task_try_get(index);
}

// @retail 0x6cc10
long __stdcall function_6cc10(long controller)
{
 long const *controller_reference = &controller;
 controller = *controller_reference;
 long result = NONE;
 if (g_467214 != result)
 {
  long previous = online_task_find(3, controller);
  if (previous != result)
   function_6b640(previous);
  function_6cc12(controller);
  XONLINE_USER users[XONLINE_MAX_LOGON_USERS];
  memset(users, 0, sizeof(users));
  long count = 0;
  long i = 0;
  do
  {
   const s_player_slot &slot = g_54e8e0[i];
   if (i != controller && (bool)((*(const dword *)&slot >> 5) & 1))
   {
    users[i] = *(const XONLINE_USER *)((const byte *)&slot + 0x470);
    count++;
   }
   i++;
  } while (i < XONLINE_MAX_LOGON_USERS);
  if (count > 0)
  {
   long index = online_task_new_if_logged_on();
   s_type_9df9da *task = function_6cc14(index);
   if (task)
   {
    online_connect_to_service_20();
    if (SUCCEEDED(XOnlineChangeLogonUsers(users, 0, (XONLINETASK_HANDLE *)&task->handle)))
    {
     task->flags = 1;
     task->type = 1;
     task->controller_index = controller;
     result = index;
     goto done;
    }
    function_6b640(index);
   }
  }
  else
   function_6cb60();
 }
done:
 return result;
}
