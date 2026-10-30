// @flags /O2 /arch:SSE /Gr
#include "pool_storage.h"
#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "unknown_123b30.h"
#include "crc.h"
#include "globals.h"
#include "unknown_21e230.h"
#include "unknown_221810.h"
#include "sound_driver.h"
#include <math.h>

enum
{
	k_sound_class_count = 54,
	k_silence_buffer_size = 1000
};

/* the wave format with natural alignment (the SDK header packs it) */
struct s_wave_format
{
	word format_tag;
	word channels;
	dword samples_per_second;
	dword average_bytes_per_second;
	word block_align;
	word bits_per_sample;
	word extra_size;
};

/* a sound class volume fade, 16 bytes; target and current are real values
   handled as raw bits */
struct s_sound_class_fade
{
	dword target;
	dword current;
	real time;
	byte flags;
	byte unknownd[3];
};

/* a mix bin table: a count followed by that many pairs */
struct s_mixbin_list
{
	long count;
	DSMIXBINVOLUMEPAIR pairs[8];
};


/* the volumes the game sets */
struct s_sound_driver_volumes
{
	real volume_a;
	real volume_b;
	real volume_c;
	real volume_d;
	long unknown10;
	real unknown14;
};

s_sound_effect_parameters g_47005c =
{
	true, {0, 0, 0},
	0, 0, 0, 0,
	0.0f, 0, 0.0f, 0, 0.25f, 8000, 0.0f
};

#define PIN(n, floor, ceiling) ((n) < (floor) ? (floor) : ((n) > (ceiling) ? (ceiling) : (n)))

/* a gain in [0, 1] as a direct sound volume in hundredths of decibels */
PRIVATE inline long sound_gain_to_volume(real gain)
{
	long volume;

	if (gain == 0.0f)
	{
		volume = -6400;
	}
	else
	{
		volume = (long)(log10(gain) * 2000.0f);
		if (volume < -6400)
			volume = -6400;
		else if (volume > 0)
			volume = 0;
	}
	return volume;
}


s_sound_class_fade *g_502118;
bool g_50211c;
char const *g_470090[k_sound_class_count];

PRIVATE __forceinline void function_221491(long arg_0, long arg_1, LPDIRECTSOUNDBUFFER *arg_2)
{
	s_wave_format format;
	DSBUFFERDESC description;
	DSMIXBINVOLUMEPAIR pair;
	DSMIXBINS mixbins;
	long aligned_size;
	byte *memory;
	byte *top;

	memset(&format, 0, 0x12);
	memset(&description, 0, sizeof(description));

	LPDIRECTSOUNDBUFFER buffer = (LPDIRECTSOUNDBUFFER)arg_1;

	format.format_tag = (word)arg_0;
	format.channels = (word)arg_0;
	format.samples_per_second = k_silence_buffer_size;
	format.average_bytes_per_second = k_silence_buffer_size;
	format.block_align = (word)arg_0;
	format.bits_per_sample = 8;
	format.extra_size = (word)arg_1;

	description.dwSize = sizeof(description);
	description.dwBufferBytes = (DWORD)arg_1;
	description.lpwfxFormat = (LPWAVEFORMATEX)&format;
	description.lpMixBins = &mixbins;

	mixbins.dwMixBinCount = (DWORD)arg_0;
	mixbins.lpMixBinVolumePairs = &pair;
	pair.dwMixBin = 0xe;
	pair.lVolume = arg_1;


	DirectSoundCreateBuffer(&description, &buffer);

	top = (byte *)g_510800_pool_base + g_510804_pool_size;
	memory = (byte *)(((dword)top + 3) & ~3);
	aligned_size = (memory - top) + k_silence_buffer_size;
	g_510804_pool_size += aligned_size;
	function_163ba0(&g_510808_pool_checksum, &aligned_size, sizeof(aligned_size));

	memset(memory, 0, k_silence_buffer_size);
	IDirectSoundBuffer_SetBufferData(buffer, memory, k_silence_buffer_size);
	IDirectSoundBuffer_SetHeadroom(buffer, (DWORD)arg_1);
	IDirectSoundBuffer_SetVolume(buffer, -10000);
	IDirectSoundBuffer_Play(buffer, (DWORD)arg_1, (DWORD)arg_1, (DWORD)arg_0);

	*arg_2 = buffer;
}

// @retail 0x221490
void function_221490(
	LPDIRECTSOUNDBUFFER *buffer_reference)
{
    function_221491(1, 0, buffer_reference);
}

// score 83 (was 83): Frame 0xdc agrees; list EAX/ECX, settings-side induction, seen/output-count registers and descriptor homes differ.
// Earlier: the prologue and the loop's registers differ: retail pushes ebp, esi
// and edi first and keeps the pair count in ebp and the pair pointer in ebx, ours loads the
// pointer into ebx before the pushes and counts in ecx.
// Round 81: retail strength-reduces &settings->pairs[i].lVolume into the pointer (esi) and
// indexes the list's dwMixBin, which leaves the list in eax and the seen mask in ebp; ours
// walks the list's pairs and indexes settings. Both compares reading both arrays 96; volume
// read after the mixbin compare 136 (settings becomes the pointer, as retail); the mixbin
// compare reading both arrays 115; an explicit settings pair pointer 103; volume before
// mixbin 83; element pointer locals and declared parameter orders compile the same.
// Round 276: Actual setting/previous/both pair values 104/96/108; native changed byte 83;
// Round 276: the same pair values with byte state 104/96/108; complete graph with descriptor aggregate 108.
// Round 276: Original scalars, array observations and descriptor restored; mirrored induction remains.
// Round 216: SDK descriptor initializer 83; append-local count 87; later mask zeros 98;
// direct SDK count 87; completed-result descriptor via aggregate/fields/DWORD count 88/88/88
// (281 bytes, frame 0xd8); early descriptor initializer with DWORD count 83.
// Original descriptor, scalar mirror, input observations and all output updates restored.
// Round 20: the list arrives in ecx (retail eax): the all-stack first pass puts the shared zero
// in eax. The index declared first 83; no count local 83; the entry tests combined 85; `changed`
// initialised at its declaration 90 (retail's length, seen mask in ebp); that with the volume
// read after the mixbin compare 135.
// @retail 0x2215d0
void function_2215d0(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	LPDIRECTSOUNDSTREAM stream)
{
	DSMIXBINS out_mixbins;
	DSMIXBINVOLUMEPAIR out_pairs[8];
	long values[32];
	long out_count = 0;
	dword changed_mask = 0;
	dword seen_mask = 0;
	bool changed;
	long i;

	out_mixbins.dwMixBinCount = 0;
	out_mixbins.lpMixBinVolumePairs = out_pairs;

	if (!list)
		goto set_all;
	if (list->count != (long)settings->mixbins.dwMixBinCount)
		goto set_all;
	{
		long count = list->count;
		changed = false;

		for (i = 0; i < count; i++)
		{
			long mixbin = settings->pairs[i].dwMixBin;
			long volume = settings->pairs[i].lVolume;
			dword bit;

			if (list->pairs[i].dwMixBin != (dword)mixbin)
				goto set_all;

			if (list->pairs[i].lVolume != volume)
			{
				bit = 1 << mixbin;
				if (seen_mask & bit)
				{
					if (volume != values[i])
						goto set_all;
					if (changed_mask & bit)
						goto set_all;
				}
				changed_mask |= bit;
				out_count = out_mixbins.dwMixBinCount;
				changed = true;
			}

			bit = 1 << mixbin;
			if (!(seen_mask & bit))
			{
				out_pairs[out_count].lVolume = volume;
				seen_mask |= bit;
				out_pairs[out_count].dwMixBin = mixbin;
				out_count++;
				values[mixbin] = volume;
				out_mixbins.dwMixBinCount = out_count;
			}
		}

		if (changed)
			IDirectSoundStream_SetMixBinVolumes(stream, &out_mixbins);
		return;
	}

set_all:
	IDirectSoundStream_SetMixBins(stream, &settings->mixbins);
}

// score 83 (was 83): the prologue and the loop's registers differ: retail pushes ebp, esi
// and edi first and keeps the pair count in ebp and the pair pointer in ebx, ours loads the
// pointer into ebx before the pushes and counts in ecx.
// Round 81: written as 0x2215d0 is (the two entry tests as early exits to set_all, no
// volatile on changed_mask) 83; the volatile removed from the if form alone 85; volume
// read after the mixbin compare 136/145; volume before mixbin 83. Same pairing cause as
// 0x2215d0.
// Native desired-pair reference, alone/both element references and nested/combined
// duplicate guards 102/102/96/96; complete pair snapshots 104/104/104/104.
// Settings/list induction and mask/count homes remain; all executable forms restored.
// Round 189: Complete volume/bin/both field observations 107/106/86; read-only input contracts
// alone/volume/bin/both 83/107/106/86; both with native output-pair assignment 112. Both
// observations recover length but shrink the frame and change cleanup to four bytes. Every body
// and declaration restored.
// @retail 0x2216f0
void function_2216f0(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	LPDIRECTSOUNDBUFFER buffer)
{
	DSMIXBINS out_mixbins;
	DSMIXBINVOLUMEPAIR out_pairs[8];
	long values[32];
	long out_count = 0;
	dword changed_mask = 0;
	dword seen_mask = 0;
	bool changed;
	long i;

	out_mixbins.dwMixBinCount = 0;
	out_mixbins.lpMixBinVolumePairs = out_pairs;

	if (!list)
		goto set_all;
	if (list->count != (long)settings->mixbins.dwMixBinCount)
		goto set_all;
	{
		long count = list->count;
		changed = false;

		for (i = 0; i < count; i++)
		{
			long mixbin = settings->pairs[i].dwMixBin;
			long volume = settings->pairs[i].lVolume;
			dword bit;

			if (list->pairs[i].dwMixBin != (dword)mixbin)
				goto set_all;

			if (list->pairs[i].lVolume != volume)
			{
				bit = 1 << mixbin;
				if (seen_mask & bit)
				{
					if (volume != values[i])
						goto set_all;
					if (changed_mask & bit)
						goto set_all;
				}
				changed_mask |= bit;
				out_count = out_mixbins.dwMixBinCount;
				changed = true;
			}

			bit = 1 << mixbin;
			if (!(seen_mask & bit))
			{
				out_pairs[out_count].lVolume = volume;
				seen_mask |= bit;
				out_pairs[out_count].dwMixBin = mixbin;
				out_count++;
				values[mixbin] = volume;
				out_mixbins.dwMixBinCount = out_count;
			}
		}

		if (changed)
			IDirectSoundBuffer_SetMixBinVolumes(buffer, &out_mixbins);
		return;
	}

set_all:
	IDirectSoundBuffer_SetMixBins(buffer, &settings->mixbins);
}

/* a sound class of the sound classes tag. Retail calls it out of line
   everywhere, but moving it into a file of its own built /Ob1 (which matched
   0x218d30) changed 0x189fe0's convention and lost 0x189650 and 0x189760 */
// @retail 0x221810
s_unknown_5c *function_221810(
	short index)
{
	return sound_class_definition_get(index);
}

// @retail 0x221850
void function_221850(void)
{
	g_502118 = (s_sound_class_fade *)function_123d40("", "", k_sound_class_count * sizeof(s_sound_class_fade));
	memset(g_502118, 0, k_sound_class_count * sizeof(s_sound_class_fade));
}

// @retail 0x2218a0
void function_2218a0(void)
{
	long i;

	g_50211c = true;
	for (i = 0; i < k_sound_class_count; i++)
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

// @retail 0x221900
void function_221900(
	real delta)
{
	if (delta > 0.0f)
	{
		real volatile *class_fade_time = &g_502118->time;
		for (long i = 0; i < k_sound_class_count; i++, class_fade_time += 4)
		{
			real time_left = *class_fade_time;
			if (time_left > delta)
			{
				dword target_bits = ((dword *)class_fade_time)[-2];
				dword current_bits = ((dword *)class_fade_time)[-1];
				real target = *(real *)&target_bits;
				real current = *(real *)&current_bits;
				/* the same local holds this step's fraction */
				time_left = delta / time_left;
				real result = (target - current) * time_left + current;
				((dword *)class_fade_time)[-1] = *(dword *)&result;
				time_left = *class_fade_time - delta;
				*class_fade_time = time_left;
			}
			else
			{
				((dword *)class_fade_time)[-1] = ((dword *)class_fade_time)[-2];
				*class_fade_time = 0.0f;
			}
		}
	}
}

// @retail 0x221980
void __stdcall function_221980(
	char const *name,
	long value_bits,
	real time)
{
	char const *local_0 = name;
	long i;

	for (i = 0; i < k_sound_class_count; i++)
	{
		if (*g_470090[i] && strstr(g_470090[i], local_0))
		{
			s_sound_class_fade *fade = &g_502118[i];
			*(long volatile *)&name = value_bits;
			real value = *(real *)&name;
			dword target;

			if (value < -64.0f)
				target = 0xc2800000;
			else if (value > 0.0f)
				target = 0;
			else
				target = value_bits;

			fade->target = target;
			fade->time = time < 0.0f ? 0.0f : time;
			if (time == 0.0f)
				fade->current = target;
		}
	}
}

struct s_221a20
{
	byte field_0[0xc];
	word field_c : 1;
	word field_d : 15;
};

PRIVATE inline void set_fade_flag(
	s_221a20 &arg_0,
	bool arg_1)
{
	if (arg_1)
		arg_0.field_c = true;
	else
		arg_0.field_c = false;
}

// @retail 0x221a20
void function_221a20(
	char const *name,
	bool set)
{
	short i;

	for (i = 0; i < k_sound_class_count; ++i)
	{
		if (*g_470090[i] && strstr(g_470090[i], name))
			{
			s_221a20 *local_0 = (s_221a20 *)&g_502118[i];
			set_fade_flag(*local_0, !set);
		}
	}
}

void (__stdcall *const g_221980_callbacks[])(char const *, long, real) =
{
	function_221980
};

// @retail 0x220a70
bool sound_driver_voice_create_buffer(
	long voice_index)
{
	bool result = false;
	s_sound_driver_voice *voice = &SOUND_DRIVER_GLOBALS->voices[voice_index];
	LPDIRECTSOUND local_0 = SOUND_DRIVER_GLOBALS->direct_sound;
	DSBUFFERDESC description;
	memset(&description, 0, sizeof(description));

	description.dwSize = sizeof(description);
	description.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_MIXIN;
	if (SUCCEEDED(IDirectSound_CreateSoundBuffer(local_0, &description, &voice->buffer, NULL)))
	{
		DSMIXBINVOLUMEPAIR pairs[5];
		DSMIXBINS mixbins;

		pairs[0].dwMixBin = 6;
		pairs[0].lVolume = 0;
		pairs[1].dwMixBin = 8;
		pairs[1].lVolume = 0;
		pairs[2].dwMixBin = 7;
		pairs[2].lVolume = 0;
		pairs[3].dwMixBin = 9;
		pairs[3].lVolume = 0;
		pairs[4].dwMixBin = 10;
		pairs[4].lVolume = 0;
		mixbins.dwMixBinCount = 5;
		mixbins.lpMixBinVolumePairs = pairs;
		IDirectSoundBuffer_SetMixBins(voice->buffer, &mixbins);
		voice->unknown00 = 0x1f;
		IDirectSoundBuffer_SetMaxDistance(voice->buffer, FLT_MAX, DS3D_DEFERRED);
		IDirectSoundBuffer_SetMinDistance(voice->buffer, FLT_MAX, DS3D_DEFERRED);
		IDirectSoundBuffer_SetRolloffFactor(voice->buffer, 0.0f, DS3D_DEFERRED);
		IDirectSoundBuffer_SetDopplerFactor(voice->buffer, 0.0f, DS3D_DEFERRED);
		IDirectSoundBuffer_SetConeOutsideVolume(voice->buffer, 0, DS3D_DEFERRED);
		result = true;
	}
	voice->submix = NULL;
	return result;
}

// score 183 (was 98): the second buffer's flags are now retail's play ? DSBCAPS_FXIN2 : DSBCAPS_FXIN
// (0x100010 when play is set; the old body set 0x180010, a behaviour difference, fixed at review in lane 15
// round 13). Every 98-105 form below carried the wrong flags; with the correct select see the 155-197 list.
// Old notes: frame/length agree; early saves and descriptor zero/input scheduling differ.
// Descriptor input first 105; flags before clear 105; spatial byte 105; unsigned result 105;
// pair clear plus direct loop 185; input mixbin snapshot 105; real voice reference 98 (plain);
// pair clear plus descriptor input first 183.
// Early saves and descriptor zero/input scheduling still differ.
// Spatial-table/both plain loops 98/98; first/second/both complete pair copies
// 230/186/264; native first/second/both descriptors 136/112/136.
// First/second/both pair aggregates 125/132/160; scoped mixbin aggregates 120;
// complete descriptor aggregates 229; reused descriptor 171; direct count increment 98;
// success-path result assignments 110 (caller differs).
// early saves and descriptor zero/input scheduling remain;
// voice before modes/result after count/next count word/spatial signed byte 98 each;
// spatial word 151; play signed byte 98; spatial word with play byte 151; play long 101.
// the old body set the second buffer's flags to 0x180010 when play was set;
// retail computes play ? DSBCAPS_FXIN2 : DSBCAPS_FXIN (0x100010). The 98 allocation needs
// 0x100000 used twice (first buffer and the select): the correct select 183, or
// (play ? FXIN : 0) + FXIN 183. With the correct select: index returned directly 155 (pushes
// sink as retail's, mov ax return, but flags arrive in eax), no result 155, long result 155,
// result set after the count 159, voice_index = NONE in an else 169, failure through the
// final return 173, the plain-locals body 197, select then |= CTRL3D 175, if/else select 189,
// parameters reordered (flags last) no change.
// Round 263: first/second/both pair-volume zero chains after identifiers 109/107/118,
// before identifiers 110/107/119; shared creation status 98, with both late chains 118. All restored.
// Round 208: disjoint native descriptor phases, shared/per-phase mixbins,
// pair arrays at success/phase entry and both descriptor name orders: 98 in all eight.
// Original storage and body restored; conditional saves and zero/input residency remain.
// Round 13: retail saves only esi before the count test and gives that exit its own epilogue
// (ours shares the full one, so every save stays at entry); it also stores the new count before
// the descriptor clear. The count stored before the clear or before the voice reference 98/98;
// the count test as a guard clause returning result or NONE 164/164 (every save sinks, but flags
// then arrive in eax and the exit is placed first); the failure path returning NONE 98.
// Round 219: Native descriptor phases with direct success short/long/no sentinel result 178/178/178;
// test-assigned result 181; common short/long exit 149/180; direct success and early count,
// with/without sentinel 179/179. All restored; early saves and zero/input scheduling remain.
// @retail 0x220b90
short sound_driver_voice_new(
	dword flags,
	dword input_mixbin)
{
	short result = NONE;
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	short voice_index = globals->voice_count;

	if (voice_index < 0x40)
	{
		bool three_d = flags & 1;
		bool play = (flags >> 1) & 1;
		s_sound_driver_voice &voice = globals->voices[voice_index];
		struct
		{
			DSMIXBINS mixbins;
			union { DSBUFFERDESC first; DSMIXBINVOLUMEPAIR pairs_second[5]; };
			DSBUFFERDESC second;
			DSMIXBINVOLUMEPAIR pairs_first[6];
		} storage;
		DSBUFFERDESC &description = storage.first;
		memset(&description, 0, sizeof(description));

		globals->voice_count = voice_index + 1;
		description.dwSize = sizeof(description);
		description.dwFlags = DSBCAPS_FXIN2;
		description.dwInputMixBin = input_mixbin;
		if (SUCCEEDED(IDirectSound_CreateSoundBuffer(globals->direct_sound, &description, &voice.submix, NULL)))
		{
			DSMIXBINVOLUMEPAIR (&pairs)[6] = storage.pairs_first;
			DSMIXBINS &mixbins = storage.mixbins;

			IDirectSoundBuffer_SetVolume(voice.submix, 0);
			pairs[0].dwMixBin = 0;
			pairs[0].lVolume = 0;
			pairs[1].dwMixBin = 1;
			pairs[1].lVolume = 0;
			pairs[2].dwMixBin = 2;
			pairs[2].lVolume = 0;
			pairs[3].dwMixBin = 3;
			pairs[3].lVolume = 0;
			pairs[4].dwMixBin = 4;
			pairs[4].lVolume = 0;
			pairs[5].dwMixBin = 5;
			pairs[5].lVolume = 0;
			mixbins.dwMixBinCount = 6;
			mixbins.lpMixBinVolumePairs = pairs;
			IDirectSoundBuffer_SetMixBins(voice.submix, &mixbins);
			IDirectSoundBuffer_Play(voice.submix, 0, 0, 0);
		}
		if (three_d)
		{
			DSBUFFERDESC &buffer_description = storage.second;
			memset(&buffer_description, 0, sizeof(buffer_description));

			buffer_description.dwSize = sizeof(buffer_description);
			buffer_description.dwFlags = (play ? DSBCAPS_FXIN2 : DSBCAPS_FXIN) | DSBCAPS_CTRL3D;
			buffer_description.dwInputMixBin = input_mixbin;
			if (SUCCEEDED(IDirectSound_CreateSoundBuffer(SOUND_DRIVER_GLOBALS->direct_sound, &buffer_description, &voice.buffer, NULL)))
			{
				DSMIXBINVOLUMEPAIR (&pairs)[5] = storage.pairs_second;
				DSMIXBINS &mixbins = storage.mixbins;

				pairs[0].dwMixBin = 6;
				pairs[0].lVolume = 0;
				pairs[1].dwMixBin = 8;
				pairs[1].lVolume = 0;
				pairs[2].dwMixBin = 7;
				pairs[2].lVolume = 0;
				pairs[3].dwMixBin = 9;
				pairs[3].lVolume = 0;
				pairs[4].dwMixBin = 10;
				pairs[4].lVolume = 0;
				mixbins.dwMixBinCount = 5;
				mixbins.lpMixBinVolumePairs = pairs;
				IDirectSoundBuffer_SetMixBins(voice.buffer, &mixbins);
				voice.unknown00 = input_mixbin;
				IDirectSoundBuffer_SetMaxDistance(voice.buffer, FLT_MAX, DS3D_DEFERRED);
				IDirectSoundBuffer_SetMinDistance(voice.buffer, FLT_MAX, DS3D_DEFERRED);
				if (play)
					IDirectSoundBuffer_Play(voice.buffer, 0, 0, 0);
			}
			else
			{
				SOUND_DRIVER_GLOBALS->voice_count--;
				return result;
			}
		}
		else
		{
			voice.buffer = NULL;
		}
		result = voice_index;
	}
	return result;
}

// @retail 0x220fd0
void function_220fd0(
	s_sound_driver_volumes const *volumes)
{
	s_sound_driver_globals *globals;
	long volume;
	long i;

	(globals = SOUND_DRIVER_GLOBALS)->volume_a = PIN(volumes->volume_a, 0.0f, 1.0f);
	globals->volume_b = PIN(volumes->volume_b, 0.0f, 1.0f);
	globals->volume_c = PIN(volumes->volume_c, 0.0f, 1.0f);
	globals->volume_d = PIN(volumes->volume_d, 0.0f, 1.0f);
	globals->unknown2acc = volumes->unknown10;
	globals->unknown2ad0 = volumes->unknown14;
	g_47005c.room = sound_gain_to_volume(globals->volume_a);
	g_47005c.room_hf = sound_gain_to_volume(globals->volume_b);
	g_47005c.unknown70 = 0.0f;
	g_47005c.unknown78 = 0.0f;
	g_47005c.unknown80 = 0.25f;
	volume = sound_gain_to_volume(globals->volume_c);
	volume = PIN(volume, -10000, 0);
	g_47005c.direct = volume;
	g_47005c.direct_hf = volume;
	g_47005c.unknown84 = globals->unknown2acc;
	g_47005c.unknown88 = globals->unknown2ad0;
	for (i = 0; i < globals->voice_count; i++)
		globals->voices[i].flags &= ~2;
	g_47005c.dirty = true;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// score 21 (was 21): the truncated float is kept in ecx where retail keeps it in edx, and
// the flag byte follows it.
// @retail 0x2211e0
void sound_driver_voice_environment_set(
	long voice_index,
	real decibels)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	s_sound_driver_voice *voice = &globals->voices[voice_index];
	DSI3DL2BUFFER parameters;
	DSFILTERDESC filter;
	real attenuation;

	memset(&parameters, 0, sizeof(parameters));

	parameters.lDirect = sound_gain_to_volume(globals->volume_a);
	parameters.lDirectHF = sound_gain_to_volume(globals->volume_b);
	_ReadWriteBarrier();
	parameters.flRoomRolloffFactor = 0.0f;
	parameters.Obstruction.flLFRatio = 0.0f;
	parameters.Occlusion.flLFRatio = 0.25f;
	attenuation = function_12aff0(-64.0f, 0.0f, decibels, true);
	long room = sound_gain_to_volume(globals->volume_c) - (long)(6400.0f - attenuation * 6400.0f);
	parameters.lRoom = PIN(room, -10000, 0);
	parameters.lRoomHF = parameters.lRoom;
	parameters.Obstruction.lHFLevel = sound_gain_to_volume(1.0f - voice->unknown30);
	parameters.Occlusion.lHFLevel = sound_gain_to_volume(1.0f - voice->unknown2c);
	_ReadWriteBarrier();
	IDirectSoundBuffer_SetI3DL2Source(voice->buffer, &parameters, DS3D_DEFERRED);
	filter.dwMode = DSFILTER_MODE_DLS2;
	filter.dwQCoefficient = 0;
	filter.adwCoefficients[0] = sound_filter_frequency_coefficient((real)g_47005c.unknown84);
	filter.adwCoefficients[1] = sound_filter_gain_coefficient(g_47005c.unknown88);
	filter.adwCoefficients[2] = 0;
	filter.adwCoefficients[3] = 0;
	IDirectSoundBuffer_SetFilter(voice->buffer, &filter);
}
#pragma function(_ReadWriteBarrier)

/* what the game asks of a voice each update */
struct s_sound_driver_voice_parameters
{
	byte flags;
	byte unknown01[3];
	point3f position;
	real obstruction;
	real occlusion;
	real decibels;
	real occlusion_rate;
	real obstruction_rate;
};

/* the voice's flag byte as 0x220e00 writes it */
struct s_voice_flags_220e00
{
	byte bit0 : 1;
	byte unknown_1 : 7;
};

/* a change limited to plus or minus a rate (0x220e00) */
static __forceinline real pin_rate_220e00(real delta, real rate)
{
	return PIN(delta, -rate, rate);
}

// @retail 0x220e00
void sound_driver_voice_update(
	long voice_index,
	s_sound_driver_voice_parameters const *parameters)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	s_sound_driver_voice *voice = &globals->voices[voice_index];
	bool force = !(voice->flags & 2) || !globals->unknown0000;
	byte flag = parameters->flags & 1;
	real occlusion = parameters->obstruction;
	real obstruction = parameters->occlusion;

	if (force ||
		!(fabs(parameters->position.x - voice->position.x) < 0.05f) ||
		!(fabs(parameters->position.y - voice->position.y) < 0.05f) ||
		!(fabs(parameters->position.z - voice->position.z) < 0.05f))
	{
		/* DirectSound's y axis is up */
		IDirectSoundBuffer_SetPosition(voice->buffer, parameters->position.x, parameters->position.z, parameters->position.y, DS3D_DEFERRED);
		voice->position = parameters->position;
	}
	if (!force && fabs(occlusion - voice->unknown2c) < 0.001f &&
		fabs(obstruction - voice->unknown30) < 0.001f &&
		(voice->flags & 1) == flag)
	{
		goto local_0;
	}
	if (!force && (voice->flags & 1) == flag && !(0.0f >= parameters->obstruction_rate))
	{
		real rate = parameters->obstruction_rate;
		real *value = &voice->unknown30;
		*value += PIN(obstruction - *value, -rate, rate);
	}
	else
	{
		voice->unknown30 = obstruction;
	}
	if (!force && (voice->flags & 1) == flag && !(0.0f >= parameters->occlusion_rate))
	{
		voice->unknown2c += pin_rate_220e00(occlusion - voice->unknown2c, parameters->occlusion_rate);
	}
	else
	{
		voice->unknown2c = occlusion;
	}
	((s_voice_flags_220e00 *)&voice->flags)->bit0 = flag;
	sound_driver_voice_environment_set(voice_index, parameters->decibels);
local_0:
	voice->flags |= 2;
}
