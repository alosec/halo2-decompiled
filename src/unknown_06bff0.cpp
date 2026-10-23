// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_06BFF0.CPP: finishing an Xbox Live logon once its connection is
   up (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "online_tasks.h"
#include "pending_messages.h"

/* the services the logon asks for (unknown_06b590.cpp) */
struct s_online_service
{
	DWORD service_id;
	long unknown04;
	bool connect;
};

#define k_online_service_count 13
#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

extern s_online_service g_467178[k_online_service_count];
extern bool g_50944f;

/* the online files a logon requests, each starting with a request header:
   the network configuration, the hoppers and the message of the day */
struct s_online_file;
struct s_game_variant_globals;
extern s_online_file g_477058;
extern s_game_variant_globals g_47d8f4;
extern s_pending_message_header g_479780;

long online_get_logon_users_status(void);
long online_task_get_change_logon_status(s_type_9df9da *task);
void online_mutelists_refresh(XONLINE_USER *users);
void online_presence_clear(XONLINE_USER *users);
void voice_start_engine(void);
long player_slot_find_online_user(XONLINE_USER *users);
void function_8c660(void);
bool pending_message_request_receive(s_pending_message_header *header, long value10, s_pending_message_values values, long value06);
long online_friends_startup(void);
long online_offering_check_new_content(void);

/* asks for an online file for a controller, with no values */
static inline void online_file_request_receive(s_pending_message_header *header, long controller)
{
	s_pending_message_values values;

	memset(&values, 0, sizeof(values));
	pending_message_request_receive(header, controller, values, 0);
}

/* finishes a logon task once its connection is up; mapped in docs/unknown_06bff0.md */
// @retail 0x6bff0
bool __stdcall function_6bff0(s_type_9df9da *task)
{
	long result;

	if (online_logon_connected())
	{
		XONLINE_USER *users = XOnlineGetLogonUsers();

		if (users)
		{
			long status;
			long controller;

			if (task->type == 0)
				status = online_get_logon_users_status();
			else
				status = online_task_get_change_logon_status(task);
			result = 1;
			if (status == 1)
				g_50944f = true;
			else if (status == 2)
			{
				task->flag_bits.failed = true;
				result = status;
				task->result = status;
				goto done;
			}
			for (long i = 0; i < NUMBEROF(g_467178); i++)
			{
				if (SUCCEEDED(XOnlineGetServiceInfo(g_467178[i].service_id, NULL)))
					g_467178[i].connect = true;
				else
				{
					g_467178[i].connect = false;
					if (g_467178[i].unknown04)
					{
						result = 10;
						goto done;
					}
				}
			}
			online_mutelists_refresh(users);
			online_presence_clear(users);
			voice_start_engine();
			controller = player_slot_find_online_user(users);
			function_8c660();
			if (task->type == 0)
			{
				online_file_request_receive((s_pending_message_header *)&g_477058, controller);
				online_file_request_receive((s_pending_message_header *)&g_47d8f4, controller);
				online_file_request_receive(&g_479780, controller);
				online_friends_startup();
				online_offering_check_new_content();
			}
		}
		else
			result = 10;
	}
	else
		result = 5;
done:
	task->result = result;
	return result == 1;
}
