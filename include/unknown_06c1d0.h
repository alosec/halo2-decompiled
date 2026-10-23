#pragma once
#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>

static inline XUID *online_user_get_xuid(XONLINE_USER *user)
{
	XUID *xuid = 0;
	if (user)
		xuid = &user->xuid;
	return xuid;
}

static inline long controller_index_next(long index)
{
	long next = NONE;
	if (index >= 0 && index < XONLINE_MAX_LOGON_USERS - 1)
		next = index + 1;
	return next;
}

