// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_21E230.CPP: DirectSound mix bin and filter helpers */

#include "unknown_11c920.h"
#include <xtl.h>
#include <math.h>
#include "unknown_21e230.h"

// score 8 (was 8): retail stores the mix bin before it loads the decibels for the push and keeps
// the count in eax and the volume in edx after the call; ours loads the decibels first (edx) and uses
// ecx/eax after the call. Tried: the decibels read through a pointer 8; the volume in a local 8;
// the conversion written out 8; a pairs pointer 8; a by-reference conversion helper 8.
// @retail 0x21e230
void sound_mixbins_add(s_mixbin_settings *settings, long mixbin, real decibels)
{
	settings->pairs[settings->mixbins.dwMixBinCount].dwMixBin = mixbin;
	settings->pairs[settings->mixbins.dwMixBinCount].lVolume = sound_decibels_to_volume(decibels);
	settings->mixbins.dwMixBinCount++;
}
