// @flags /O2 /Gr
/* UNKNOWN_137D00.CPP: leaving a game: the subsystems' game-dispose
   callbacks, last to first */

#include "unknown_440dd8.h"
#include "globals.h"

struct s_440de4
{
	void (*field_0)(void);
	dword field_4[8];
};

#define g_440de4 (*(s_440de4 const (*)[68])((byte const *)g_440dd8 + 0xc))

// @retail 0x137d00
void function_137d00(void)
{
	g_4e6948->flag1120 = 0;
	s_440de4 const *entry = &g_440de4[67];
	do
	{
		if (entry->field_0)
			entry->field_0();
		entry--;
	}
	while ((long)entry >= (long)g_440de4);
	g_4e6948->flag = 0;
}
