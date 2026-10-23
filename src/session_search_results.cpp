// @flags /O2 /Gr
/* SESSION_SEARCH_RESULTS.CPP: the sessions a search found
   (include/session_search_results.h): fills an entry from a Live match
   search result, and looks an entry up in the list */

#include "unknown_11c920.h"
#include "globals.h"
#include "session_search_results.h"
#include <xtl.h>
#include <string.h>

/* the list's capacity and entries (allocated by 0xb3610 in
   src/unknown_0b3570.cpp, which shares them) */
long g_4d8f14;
s_0b35e0_entry *g_4d8f18;

/* fills an entry from a Live match search result, found now */
// @retail 0xb3500
void session_search_result_initialize(s_online_match_session_info const *match, s_0b35e0_entry *entry)
{
	memset(entry, 0, sizeof(*entry));
	entry->used = true;
	entry->time_found = g_510548 ? g_51054c : GetTickCount();
	entry->session_id = match->session_id;
	entry->key = match->key;
	entry->address = match->address;
}

/* the entry at an index of the list, or none when it is unused or not of
   kind 0 */
// @retail 0xb35e0
s_0b35e0_entry *session_search_results_get(long index)
{
	s_0b35e0_entry *entry = 0;

	if (index >= 0 && index < g_4d8f14)
	{
		s_0b35e0_entry *candidate = &g_4d8f18[index];
		if (candidate->used && candidate->description.kind == 0)
			entry = candidate;
	}

	return entry;
}
