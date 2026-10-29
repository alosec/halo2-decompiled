// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1564E0.CPP: the Bink playback functions with float arithmetic
(unknown_01e930.cpp holds the rest). Retail builds them with SSE: the
remaining ticks convert and scale through cvtsi2ss/mulss, and the sound
callback selects its volume through a movss stack slot. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_01e930.h"
#include "slot_handler.h"

static __forceinline long real_to_long(real value);

/* per-track sound settings for the Bink sound callback */
struct s_bink_sound_track
{
	dword unknown00;
	real volume;
	byte unknown08[0x18];
};

s_bink_sound_track g_4e9300[8];

// @retail 0x1564e0
long bink_playback_ticks_remaining(void)
{
	s_bink_movie *movie = (s_bink_movie *)g_4e9188.movie;
	long result = 0;

	if (movie)
	{
		dword remaining = movie->frames - movie->frame_number;
		dword milliseconds = movie->frame_rate_divisor * movie->frames * 1000 / movie->frame_rate;
		real seconds;
		real ticks;

		milliseconds = milliseconds * remaining / (movie->frames > 1 ? movie->frames : 1);
		seconds = (long)milliseconds * 0.001f;
		ticks = seconds * 30.0f;
		result = real_to_long(ticks);
		result = result > 0 ? result : 0;
	}
	return result;
}

/* the Bink sound callback: a track's volume */
// @retail 0x156b30
real __stdcall function_156b30(long track, long type)
{
	real result = 0.0f;

	if (type == 0x3000577)
		result = g_4e9300[track].volume;
	return result;
}

/* the screen rectangles (unknown_0167a0.cpp) */
struct short_rect
{
	short v0, v1, v2, v3;
};

struct short_rect_pair
{
	short_rect a, b;
};

struct s_unknown_485a88
{
    byte flag;
    short_rect_pair rects;
};
extern s_unknown_485a88 g_unknown_485a88;
#define g_485a8a (g_unknown_485a88.rects)
extern byte g_4c1a18;

struct s_bink_vertex
{
	real x;
	real y;
	real u;
	real v;
	dword color;
};


void __stdcall function_35b90(void *material);
void __stdcall function_363a0(void *vertices);
void __stdcall function_1e930(long a);

/* draws the movie frame: centered in the screen rectangle at the movie's
   size, or over the whole of it */
// @retail 0x156960
void function_156960(void)
{
	short_rect screen = g_485a8a.a;
	s_bink_vertex vertices[4];

	if (!(g_4e9188.flags & 0x10))
	{
		short screen_width = screen.v3 - screen.v1;
		short screen_height = screen.v2 - screen.v0;

		screen.v1 = (screen_width - g_4e9188.width) / 2;
		screen.v3 = (screen_width + g_4e9188.width) / 2;
		screen.v0 = (screen_height - g_4e9188.height) / 2;
		screen.v2 = (screen_height + g_4e9188.height) / 2;
	}

	for (short i = 0; i < 4; i++)
	{
		long corner = i + 1;
		real u, v, x, y;

		if (corner & 2)
			u = g_4e9188.width;
		else
			u = 0.0f;
		if (i > 1)
			v = g_4e9188.height;
		else
			v = 0.0f;
		if (corner & 2)
			x = screen.v3;
		else
			x = screen.v1;
		if (i > 1)
			y = screen.v2;
		else
			y = screen.v0;
		vertices[i].x = x;
		vertices[i].y = y;
		vertices[i].u = u;
		vertices[i].v = v;
		vertices[i].color = 0xffffffff;
	}

	function_35b90(g_4e9188.material);
	function_363a0(vertices);
	if (g_4c1a18)
	{
		function_1e930(1);
		g_4c1a18 = 0;
	}
}
