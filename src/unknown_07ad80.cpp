// @flags /O2 /Ob1 /Gr
/* UNKNOWN_07AD80.CPP: network random bytes and address helpers */

#include "unknown_11c920.h"
#include <xtl.h>
#include <stdlib.h>
#include <time.h>

byte g_4cf790;

// @retail 0x7ad80
void function_07ad80(long count, byte *buffer)
{
#ifndef H2_NATIVE_PORT
	if (g_4cf790)
	{
		XNetRandom(buffer, count);
		return;
	}
#endif

#ifdef H2_NATIVE_PORT
	dword seed = GetTickCount() ^ 0x3c6ef35f;
#else
	time_t now = time(0);
	long r = rand();
	dword ticks = GetTickCount();
	dword seed = r ^ ticks ^ (dword)now;
#endif
	for (long i = 0; i < count; i++)
	{
		seed = seed * 0x19660d + 0x3c6ef35f;
		word r = (word)(seed >> 16);
		dword scaled = r * 256u;
		buffer[i] = (byte)(scaled >> 16);
	}
}
