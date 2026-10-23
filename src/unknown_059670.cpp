// @flags /O2 /Gr
/* UNKNOWN_059670.CPP: the current and the other session (the session
   manager's session_a and session_b) as the menus query them (outside lane H's region, decompiled
   by lane H because its menus call these with register arguments) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058ee0.h"

struct s_network_session_membership;

#define SESSION_STATE_IS_LIVE(state) ((state) > 2 && (state) <= 8)

PRIVATE __forceinline bool function_592f1(const c_class_58d20 *arg_0)
{
	bool local_0 = false;
	long local_1 = arg_0->state;
	if (local_1 && SESSION_STATE_IS_LIVE(local_1))
		local_0 = arg_0->current_member == arg_0->value50;
	return local_0;
}

// @retail 0x592f0
bool function_592f0(void)
{
	bool result = false;
	if (g_527330.initialized)
		result = function_592f1((const c_class_58d20 *)g_527330.session_a);
	return result;
}

// @retail 0x5a680
s_network_session_membership *function_5a680(c_class_58d20 *session, long *current_member, long *member_index)
{
	if (current_member)
	{
		*current_member = session->current_member;
	}
	if (member_index)
	{
		*member_index = session->member_index;
	}
	return (s_network_session_membership *)&session->value4c;
}
