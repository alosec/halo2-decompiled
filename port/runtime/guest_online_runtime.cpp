#include "online_tasks.h"
#include <string.h>
#include <xonline.h>
#include <xtl.h>

/* Native offline compatibility for the recovered task-screen core.  The
 * selected UI path still owns and looks up its retail-format task records,
 * while Xbox Live task handles cannot exist in the native port yet. */

// @retail 0x6ced0, native offline boundary
void online_get_logon_users(XONLINE_USER *users)
{
    /* The native port has no Xbox Live identity provider. */
    memset(users, 0, sizeof(XONLINE_USER) * XONLINE_MAX_LOGON_USERS);
}

// @retail 0x6c7c0, native offline boundary
bool online_get_logon_status(long *status)
{
    *status = 0;
    return false;
}

// Native offline builds do not start the Xbox Live voice-chat engine.
void voice_start_engine()
{
}

// @retail 0x6c7e0, native offline boundary
bool function_6c7e0()
{
    return false;
}

struct s_channel_message;

// @retail 0x6d080, native offline boundary
bool function_6d080(long, s_channel_message *)
{
    return false;
}

void online_team_get_details(long, XUID const *, XONLINE_TEAM *team)
{
    memset(team, 0, sizeof(*team));
}

struct _XUID;

// @retail 0xabc70, native offline boundary
long function_abc70(long, _XUID const *)
{
    return NONE;
}

long online_team_members_enumerate(long, XUID const *)
{
    return NONE;
}

void online_team_member_get_details(long, XUID const *, XONLINE_TEAM_MEMBER *member)
{
    memset(member, 0, sizeof(*member));
}

void online_team_members_enumerate_get_results(long, DWORD *count, XUID *)
{
    *count = 0;
}

void online_teams_enumerate_get_results(long, DWORD *count, XUID *)
{
    *count = 0;
}

/* The native offline bootstrap cannot own either Xbox network session.  The
 * recovered leave-both-sessions router may still call these while unwinding
 * UI state, so treat both leaves as already complete rather than fabricating
 * a session manager or transport. */
void network_session_manager_leave_session_a(bool)
{
}

void network_session_manager_leave_session_b(bool)
{
}

/* No peer list or network-session-backed screen can exist in native offline
 * bootstrap state, so their per-frame updates have no state to advance. */
/* Native offline state has no Live tasks, transport reset callbacks, or
 * memory-unit devices to tear down. */
void function_6cb60()
{
}

void function_18f62b()
{
    /* No native controller-event or online-team task source exists yet. */
}

void function_199893()
{
}

void function_199b08()
{
}

// @retail 0x6b5d0
long online_task_poll(long task_index)
{
    s_type_9df9da *task = online_task_try_get(task_index);
    if (!task)
        return 5;
    if (task->flag_bits.finished)
        return task->flag_bits.failed ? 3 : 2;
    if (task->flag_bits.running)
        return 1;
    return task->flag_bits.started ? 0 : 4;
}

// @retail 0x6b910
s_type_9df9da *function_6b910(long task_index)
{
    return online_task_try_get(task_index);
}

// @retail 0x6c670
HRESULT online_task_continue(s_type_9df9da *task)
{
    if (!task || !task->handle || task->handle == (void *)NONE)
        return E_FAIL;
    return (HRESULT)0x80151000L;
}

// @retail 0x6b640
void function_6b640(long task_index)
{
    s_type_9df9da *task = online_task_try_get(task_index);
    if (task)
    {
        /* Native task handles cannot own Xbox Live resources. */
        task->handle = 0;
        record_pool_release(g_4cf78c, task_index);
    }
}
