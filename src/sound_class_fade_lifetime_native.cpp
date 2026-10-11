// Source-local recovered lifetime for the 54 sound-class fade records. The
// lifecycle table initializes, resets, deactivates, and finally clears this
// publication independently of the static sound-system arena.

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include <string.h>
#include <stddef.h>

enum
{
	k_sound_class_count = 54
};

struct s_sound_class_fade
{
	dword target;
	dword current;
	real time;
	byte flags;
	byte unknownd[3];
};

typedef char check_sound_class_fade_size[
	sizeof(s_sound_class_fade) == 0x10 ? 1 : -1];
typedef char check_sound_class_fade_time[
	offsetof(s_sound_class_fade, time) == 0x08 ? 1 : -1];
typedef char check_sound_class_fade_flags[
	offsetof(s_sound_class_fade, flags) == 0x0c ? 1 : -1];

s_sound_class_fade *g_502118;
bool g_50211c;

// @retail 0x221850
void function_221850(void)
{
	g_502118 = (s_sound_class_fade *)function_123d40(
		"", "", k_sound_class_count * sizeof(s_sound_class_fade));
	memset(g_502118, 0, k_sound_class_count * sizeof(s_sound_class_fade));
}

// @retail 0x2218a0
void function_2218a0(void)
{
	g_50211c = true;
	for (long i = 0; i < k_sound_class_count; i++)
	{
		g_502118[i].current = 0;
		g_502118[i].target = 0;
		g_502118[i].time = 0.0f;
	}
}

// @retail 0x2218e0
void function_2218e0(void)
{
	g_50211c = false;
}

// @retail 0x2218f0
void function_2218f0(void)
{
	g_502118 = NULL;
}
