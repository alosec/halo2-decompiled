// @flags /O1 /Oi /Gr /GL-
/* PLAYER_SLOT_GET.CPP: the slot of g_54e8e0 at an index. Retail compiled it
   without LTCG: its only caller (0x18fc44) does not inline it and keeps the
   profile index it needs after the call in ebx, as it must around a
   standard __fastcall callee that may clobber edx (lane H) */

#include "unknown_11c920.h"
#include "globals.h"

/* the slot (0xc70 bytes; player_slots.cpp) */
struct s_player_slot_view;

// @retail 0x18f576
s_player_slot_view *player_slot_get(long index)
{
	s_player_slot_view *slot = NULL;

	if (index != NONE)
	{
		slot = (s_player_slot_view *)&g_54e8e0[index];
	}
	return slot;
}
