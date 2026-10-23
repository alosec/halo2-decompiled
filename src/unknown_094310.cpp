// @flags /O2 /Gr
/* UNKNOWN_094310.CPP: the handler for a session's disband message: looks
   the session up in the handler's session manager and passes the message
   on. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_0820f0.h"

struct s_network_session_list;
c_class_58d20 *network_session_manager_find_session(s_network_session_list *manager, const s_session_id *session_id);
bool network_session_handle_session_disband(c_class_58d20 *session, const s_type_99af70 *address);

// @retail 0x94310
void __stdcall function_094310(c_class_938e0 *handler, s_session_id const *message, s_type_99af70 const *address)
{
	s_network_session_list *manager = *(s_network_session_list *const *)((byte const *)handler + 0x14);
	c_class_58d20 *session = network_session_manager_find_session(manager, message);
	if (session)
		network_session_handle_session_disband(session, address);
}
