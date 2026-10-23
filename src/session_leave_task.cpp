// @flags /O1 /arch:SSE /Gr
/* SESSION_LEAVE_TASK.CPP: the online task screen's procedure for leaving the
   network sessions (opened by function_199e3c on the way back to the main
   menu) */

#include "unknown_11c920.h"

/* the online task screen (declared in full by unknown_1a2ca7.cpp; only the
   method called here) */
class c_online_task_screen
{
public:
	void set_description(long string_handle);
};

void online_task_screen_finish(c_online_task_screen *screen);
long function_19a279(void);

/* starts the task screen of leaving the squad: shows its description, and
   finishes at once when there was nothing to leave */
// @retail 0x2523bc
void __stdcall session_leave_task_screen_begin(c_online_task_screen *screen)
{
	long leave_state = function_19a279();

	screen->set_description(0xd000443);
	if (!leave_state)
	{
		online_task_screen_finish(screen);
	}
}
