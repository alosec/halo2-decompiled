// @flags /O2 /Gr
/* ONLINE_PRESENCE.CPP: the Live presence task (online task type 33): the
   users it watches and the state they publish (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "online_tasks.h"
#include "globals.h"
#include "unknown_0662e0.h"
#include <wchar.h>

struct s_presence_name_cache_entry
{
	XUID user;
	XUID team;
	wchar_t name[16];
	dword time;
	long state;
};

static inline dword presence_cache_time(void)
{
	return g_510548 ? g_51054c : GetTickCount();
}

struct s_presence_name_cache
{
	s_presence_name_cache_entry entries[150];
	long task_index;
	s_presence_name_cache_entry *pending;
};

void online_teams_enumerate_get_results(long task_index, DWORD *count, XUID *teams);
void online_team_get_details(long task_index, XUID const *team, XONLINE_TEAM *details);
long function_abc70(long controller_index, XUID const *xuid);
long player_slot_find_signed_in(long mode);

// @retail 0x8bf70
void function_8bf70(s_presence_name_cache *cache)
{
	if (cache->task_index != NONE)
	{
		long state = online_task_poll(cache->task_index);
		switch (state)
		{
		case 0:
			break;
		case 1:
		case 2:
		{
			DWORD count = 8;
			XUID teams[8];
			online_teams_enumerate_get_results(cache->task_index, &count, teams);
			if ((long)count > 0)
			{
				XONLINE_TEAM details;
				online_team_get_details(cache->task_index, teams, &details);
				wchar_t *name = cache->pending->name;
				wcsncpy(name, details.TeamProperties.wszTeamName, 15);
				name[15] = 0;
				cache->pending->team = teams[0];
			}
			else
			{
				cache->pending->name[0] = 0;
				cache->pending->team.qwUserID = 0;
			}
			cache->pending->time = presence_cache_time();
			cache->pending->state = 3;
			cache->pending = 0;
			function_6b640(cache->task_index);
			cache->task_index = NONE;
			break;
		}
		default:
		{
			cache->pending->name[0] = 0;
			cache->pending->team.qwUserID = 0;
			cache->pending->time = presence_cache_time();
			cache->pending->state = 3;
			cache->pending = 0;
			function_6b640(cache->task_index);
			cache->task_index = NONE;
			break;
		}
		}
	}
	if (cache->task_index == NONE)
	{
		long controller = player_slot_find_signed_in(1);
		if (controller >= 0 && controller < 4)
		{
			for (long i = 0; i < 150; i++)
			{
				if (cache->entries[i].state == 1)
				{
					cache->task_index = function_abc70(controller, &cache->entries[i].user);
					if (cache->task_index != NONE)
					{
						cache->pending = &cache->entries[i];
						cache->entries[i].state = 2;
					}
					break;
				}
			}
		}
	}
}

// @retail 0x8c150
bool __stdcall function_8c150(void *cache, long allow_stale, unsigned __int64 *user_id,
	void *name, unsigned __int64 *team_id)
{
	s_presence_name_cache_entry *entries = (s_presence_name_cache_entry *)cache;
	bool found = false;
	bool result = false;
	s_presence_name_cache_entry *oldest = 0;
	*(wchar_t *)name = 0;
	long i = 0;
	do
	{
		s_presence_name_cache_entry *entry = &entries[i];
		if ((!oldest || entry->time < oldest->time) && (entry->state == 3 || entry->state == 0))
			oldest = entry;
		if (user_id && *user_id == entry->user.qwUserID)
		{
			found = true;
			if (entry->state == 3)
			{
				long duration = g_network_configuration.value1728 * 1000;
				if (!allow_stale)
				{
					dword time = entry->time;
					if ((long)(presence_cache_time() - time) > duration)
					{
						entry->state = 1;
						break;
					}
				}
				wcsncpy((wchar_t *)name, entry->name, 15);
				((wchar_t *)name)[15] = 0;
				*(XUID *)team_id = entry->team;
				result = true;
			}
			break;
		}
		i++;
	} while (i < 150);
	if (!allow_stale && !found && oldest)
	{
		oldest->user = *(XUID *)user_id;
		oldest->state = 1;
	}
	return result;
}

/* a user's presence state as the game's friend flags */
// @retail 0x8c280
dword online_presence_get_flags(DWORD state, DWORD title_id)
{
	dword flags = 0;
	bool same_title = false;

	if (state & XONLINE_PRESENCE_FLAG_ONLINE)
	{
		if (XOnlineTitleIdIsSameTitle(title_id) && online_logon_connected())
			same_title = true;
		else
			same_title = false;
		flags = 1;
	}
	if (state & XONLINE_PRESENCE_FLAG_JOINABLE)
		flags |= 8;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDREQUEST)
		flags |= 0x10;
	if (state & XONLINE_PRESENCE_FLAG_PLAYING)
		flags |= 4;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDINVITE)
	{
		flags |= 0x80;
		if (same_title)
			flags |= 0x40;
		else
			flags &= ~0x40;
	}
	if (state & XONLINE_PRESENCE_FLAG_VOICE)
		flags |= 2;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDTEAMRECRUIT)
		flags |= 0x400;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDCOMPREMINDER)
		flags |= 0x1000;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDCOMPREQUEST)
		flags |= 0x2000;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDTITLECUSTOM)
		flags |= 0x200;
	return flags;
}

// @retail 0x8c320
long online_presence_task_new(DWORD controller_index)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get(task_index);
		if (task)
		{
			if (SUCCEEDED(XOnlinePresenceInit(controller_index, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 33;
				task->controller_index = controller_index;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}
	return task_index;
}

PRIVATE __forceinline s_type_9df9da *function_8c3a4(long arg_0, s_record_pool *arg_1)
{
 s_type_9df9da *local_0 = 0;
 long local_1 = arg_0 & 0xffff;
 if (local_1 < arg_1->high_water_index)
 {
  byte *local_2 = arg_1->data + arg_1->size * local_1;
  short local_3 = *(short *)local_2;
  if (local_3 != 0 && local_3 == (arg_0 >> 16)) local_0 = (s_type_9df9da *)local_2;
 }
 return local_0;
}
// @retail 0x8c3a0
void online_presence_add(long task_index, DWORD group_id, DWORD user_count, XUID *users)
{
	s_type_9df9da *task = 0;
	if (task_index != NONE) task = function_8c3a4(task_index, g_4cf78c);

	if (task && online_logon_connected())
		XOnlinePresenceAdd((XONLINETASK_HANDLE)task->handle, group_id, user_count, users);
}

// @retail 0x8c410
void online_presence_submit(long task_index)
{
	s_type_9df9da *task = online_task_try_get(task_index);

	if (task && online_logon_connected())
		XOnlinePresenceSubmit((XONLINETASK_HANDLE)task->handle);
}

// @retail 0x8c470
void online_presence_get_latest(long task_index, DWORD group_id, DWORD count, XONLINE_PRESENCE *presences)
{
	s_type_9df9da *task = online_task_try_get(task_index);

	memset(presences, 0, count * sizeof(XONLINE_PRESENCE));
	if (task && online_logon_connected())
	{
		long status = online_task_poll(task_index);
		if ((status == 1 || status == 2) && SUCCEEDED(XOnlinePresenceGetLatest((XONLINETASK_HANDLE)task->handle, group_id, count, presences)))
		{
			for (DWORD i = 0; i < count; i++)
				presences[i].dwUserState = online_presence_get_flags(presences[i].dwUserState, presences[i].dwTitleID);
		}
	}
}

// @retail 0x8c540
bool online_title_is_this_title(DWORD title_id)
{
	return (bool)XOnlineTitleIdIsSameTitle(title_id);
}

// @retail 0x8c550
void online_presence_task_clear(long task_index)
{
	s_type_9df9da *task = online_task_try_get(task_index);

	if (task && online_logon_connected())
		XOnlinePresenceClear((XONLINETASK_HANDLE)task->handle);
}
