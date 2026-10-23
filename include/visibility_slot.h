/* VISIBILITY_SLOT.H: the table of visibility test slots (g_51f40c; 020560,
   020670) */

#ifndef VISIBILITY_SLOT_H
#define VISIBILITY_SLOT_H

#include "unknown_11c920.h"

/* a slot of the visibility table (g_51f40c, 32 bytes each; the data begins at
   +8, the release is 020e50) */
struct s_slot
{
	dword a : 1;
	dword b : 2;
	dword c : 3;
	dword d : 12;
	dword e : 6;
	dword f : 8;
	dword used : 1;
	dword valid : 1;
	dword size : 5;
	dword j : 9;
	dword k : 16;
	byte data[24];
};

/* The count and the three bitsets follow the 511 slots in retail. */
struct s_visibility_storage
{
	s_slot slots[511];
	long count;
	dword bitsets[3][16];
};


#include "render_state_storage.h"
#endif
