/* UNKNOWN_01E930.H: the sound settings Bink plays through (unknown_01e930.cpp
   defines g_51ebe4; the voice effects of unknown_191270.cpp and the sound
   streams of unknown_2ae170.cpp use its DirectSound object), and the movie
   head the Bink files (unknown_01e930.cpp, unknown_1564e0.cpp) read */

#ifndef UNKNOWN_01E930_H
#define UNKNOWN_01E930_H

#include "unknown_11c920.h"

struct s_bink_sound_settings
{
	byte unknown0000;
	bool surround;
	byte unknown0002[0x2ab0 - 0x2];
	void *direct_sound;
};

extern s_bink_sound_settings *g_51ebe4;

/* the head of a Bink movie (HBINK) */
struct s_bink_movie
{
	dword width;
	dword height;
	dword frames;
	dword frame_number;
	dword last_frame_number;
	dword frame_rate;
	dword frame_rate_divisor;
};

#endif
