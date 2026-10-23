// @flags /O2 /Ob1 /Gr
/* UNKNOWN_06DEC0.CPP: one update of the session manager's state machine:
   the owner's helper first, then every requested change of state (the old
   state leaves, the new one enters with the request's data), then the
   current state's update, until it is done and no change is pending */

#include "unknown_11c920.h"
#include <xtl.h>
#include "unknown_058dd0.h"
#include "unknown_058ee0.h"

void __stdcall function_6d4c0(void *object);
void function_6dec0(s_session_owner_view *owner);

// @retail 0x6dec0
void function_6dec0(s_session_owner_view *owner)
{
	function_6d4c0(owner->unknown3c);
	for (;;)
	{
		if (owner->failed)
		{
			owner->failed = false;

			long data_size = owner->data_size;
			void *data = data_size > 0 ? owner->data : NULL;
			long next = owner->error_code;
			long current = owner->mode;

			owner->unknown48 = true;
			if (current != next)
			{
				c_session_state *from = owner->states[current];
				c_session_state *to = owner->states[next];

				from->leave((long)to);
				owner->mode = next;
				to->enter((long)from, data_size, (long)data);
			}
			owner->unknown48 = false;
		}
		else
		{
			owner->unknown49 = true;

			bool busy = owner->states[owner->mode]->update();

			owner->unknown49 = false;
			if (!busy && !owner->failed)
				break;
		}
	}
}
