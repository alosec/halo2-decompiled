#include "online_tasks.h"
#include "unknown_24b5bc.h"
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

bool controller_is_connected(short)
{
    return false;
}

long function_1249d0(short)
{
    return 0;
}

void function_1905bf(long, bool)
{
    /* No controller event can request profile sign-in yet. */
}

bool function_19a76d(short)
{
    return false;
}

void function_199a03(long)
{
    /* No native network session exists to receive a mode change. */
}

void function_199a57()
{
    /* Session variant initialization is unreachable without session hosting. */
}

long function_199d7c()
{
    return 0;
}

bool function_199df9(bool, bool)
{
    return false;
}

bool function_19a0c5()
{
    return false;
}

bool function_19a1bd()
{
    /* There is no native network session to mark ready. */
    return false;
}

void function_121100(long *value)
{
    /* Native bootstrap has no persisted preferred session mode yet. */
    *value = 0;
}

void __stdcall function_238c21(long, long, unsigned short *, long)
{
    /* Native text entry has no controller/keyboard screen yet. */
}

void __stdcall function_238ea7(long)
{
    /* No Xbox Live friend selection exists offline. */
}

void __stdcall function_238eb5(long, long)
{
    /* No Xbox Live friend selection exists offline. */
}

void __stdcall function_238f3f(long, void *, unsigned __int64)
{
    /* Xbox Live invitation dispatch is unavailable offline. */
}

struct s_screen_parameters;
class c_class_1473c9;

c_class_1473c9 *__stdcall function_253185(s_screen_parameters *)
{
    /* The network squad browser has no native-offline screen. */
    return 0;
}

void s_player_slot_profile::set_profile_index(long)
{
    /* The native virtual save list is empty until persistence exists. */
    profile_index = NONE;
}

void s_player_slot_profile::show_dialog(player_sign_in_callback next, long)
{
    /* There is no native controller source to answer profile dialogs yet. */
    callback = next;
}

void s_player_slot_profile::sign_out()
{
    /* Native bootstrap cannot have a Live or local profile signed in. */
    profile_index = NONE;
    task_type = 0;
    if (callback)
        callback(player, true);
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
