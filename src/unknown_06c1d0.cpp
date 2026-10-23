#include "unknown_4cf790.h"
// @flags /O2 /arch:SSE /Gr
#include "unknown_06c1d0.h"
#include "online_tasks.h"
#include "online_presence.h"

/* publishes an empty presence for every signed-in, non-guest user */
// @retail 0x6c1d0
void online_presence_clear(XONLINE_USER *users)
{
	XONLINE_USER *const *users_reference = &users;
	for (long i = 0; i != NONE; i = controller_index_next(i))
	{
		XONLINE_USER *user = &(*users_reference)[i];
		if (user->xuid.qwUserID != 0 && user->szGamertag[0] && SUCCEEDED(user->hr))
		{
			XUID *xuid = online_user_get_xuid(&(*users_reference)[i]);
			if (!XOnlineIsUserGuest(xuid->dwUserFlags))
			{
				s_online_presence presence;
				XNKID session_id = { 0 };
				s_online_presence_source source;

				source.state = 1;
				source.minutes_a = 0;
				source.minutes_b = 0x7fff;
				source.unknown04 = 0;
				source.unknown08 = 0;
				online_presence_build(&presence, &source);
				if (online_logon_connected())
					XOnlineNotificationSetState(i, 0, session_id, sizeof(presence), (BYTE *)&presence);
			}
		}
	}
}
