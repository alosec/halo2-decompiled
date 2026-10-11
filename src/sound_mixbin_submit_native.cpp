// Exact mixbin construction and changed-volume submission isolated from the
// broad Xbox DirectSound unit.  Stream and buffer updates remain genuine
// DirectSound operations.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSoundStream;
struct IDirectSoundBuffer;

struct s_mixbin_volume_pair
{
	dword mixbin;
	long volume;
};

struct s_mixbins
{
	dword count;
	s_mixbin_volume_pair *pairs;
};

struct s_mixbin_settings
{
	s_mixbin_volume_pair pairs[8];
	s_mixbins mixbins;
};

struct s_mixbin_list
{
	long count;
	s_mixbin_volume_pair pairs[8];
};

real function_12aff0(real lower, real upper, real value, bool clamp);

extern "C" long __stdcall IDirectSoundStream_SetMixBins(
	IDirectSoundStream *stream, s_mixbins const *mixbins);
extern "C" long __stdcall IDirectSoundStream_SetMixBinVolumes(
	IDirectSoundStream *stream, s_mixbins const *mixbins);
extern "C" long __stdcall IDirectSoundBuffer_SetMixBins(
	IDirectSoundBuffer *buffer, s_mixbins const *mixbins);
extern "C" long __stdcall IDirectSoundBuffer_SetMixBinVolumes(
	IDirectSoundBuffer *buffer, s_mixbins const *mixbins);

static inline long sound_decibels_to_volume(real decibels)
{
	return static_cast<long>(
		function_12aff0(-64.0f, 0.0f, decibels, true) * 6400.0f - 6400.0f);
}

static_assert(sizeof(s_mixbin_volume_pair) == 0x08,
	"mixbin volume pair size");
static_assert(sizeof(s_mixbins) == 0x08, "mixbin list header size");
static_assert(sizeof(s_mixbin_settings) == 0x48,
	"mixbin settings size");
static_assert(offsetof(s_mixbin_settings, mixbins) == 0x40,
	"mixbin settings header offset");
static_assert(sizeof(s_mixbin_list) == 0x44,
	"captured mixbin list size");

// @retail 0x21e230
void sound_mixbins_add(s_mixbin_settings *settings, long mixbin, real decibels)
{
	dword index = settings->mixbins.count;
	settings->pairs[index].mixbin = static_cast<dword>(mixbin);
	settings->pairs[index].volume = sound_decibels_to_volume(decibels);
	settings->mixbins.count++;
}

static void sound_mixbins_update_stream(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	IDirectSoundStream *stream)
{
	s_mixbin_volume_pair out_pairs[8];
	s_mixbins out_mixbins = { 0, out_pairs };
	long values[32];
	long out_count = 0;
	dword changed_mask = 0;
	dword seen_mask = 0;
	bool changed = false;

	if (list && list->count == static_cast<long>(settings->mixbins.count))
	{
		long count = list->count;
		for (long i = 0; i < count; i++)
		{
			long mixbin = static_cast<long>(settings->pairs[i].mixbin);
			long volume = settings->pairs[i].volume;
			if (list->pairs[i].mixbin != static_cast<dword>(mixbin))
				goto set_all;

			dword bit;
			if (list->pairs[i].volume != volume)
			{
				bit = 1u << mixbin;
				if (seen_mask & bit)
				{
					if (volume != values[i])
						goto set_all;
					if (changed_mask & bit)
						goto set_all;
				}
				changed_mask |= bit;
				out_count = static_cast<long>(out_mixbins.count);
				changed = true;
			}

			bit = 1u << mixbin;
			if (!(seen_mask & bit))
			{
				out_pairs[out_count].volume = volume;
				seen_mask |= bit;
				out_pairs[out_count].mixbin = static_cast<dword>(mixbin);
				out_count++;
				values[mixbin] = volume;
				out_mixbins.count = static_cast<dword>(out_count);
			}
		}
		if (changed)
			IDirectSoundStream_SetMixBinVolumes(stream, &out_mixbins);
		return;
	}

set_all:
	IDirectSoundStream_SetMixBins(stream, &settings->mixbins);
}

// @retail 0x2215d0
void function_2215d0(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	IDirectSoundStream *stream)
{
	sound_mixbins_update_stream(list, settings, stream);
}

static void sound_mixbins_update_buffer(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	IDirectSoundBuffer *buffer)
{
	s_mixbin_volume_pair out_pairs[8];
	s_mixbins out_mixbins = { 0, out_pairs };
	long values[32];
	long out_count = 0;
	dword changed_mask = 0;
	dword seen_mask = 0;
	bool changed = false;

	if (list && list->count == static_cast<long>(settings->mixbins.count))
	{
		long count = list->count;
		for (long i = 0; i < count; i++)
		{
			long mixbin = static_cast<long>(settings->pairs[i].mixbin);
			long volume = settings->pairs[i].volume;
			if (list->pairs[i].mixbin != static_cast<dword>(mixbin))
				goto set_all;

			dword bit;
			if (list->pairs[i].volume != volume)
			{
				bit = 1u << mixbin;
				if (seen_mask & bit)
				{
					if (volume != values[i])
						goto set_all;
					if (changed_mask & bit)
						goto set_all;
				}
				changed_mask |= bit;
				out_count = static_cast<long>(out_mixbins.count);
				changed = true;
			}

			bit = 1u << mixbin;
			if (!(seen_mask & bit))
			{
				out_pairs[out_count].volume = volume;
				seen_mask |= bit;
				out_pairs[out_count].mixbin = static_cast<dword>(mixbin);
				out_count++;
				values[mixbin] = volume;
				out_mixbins.count = static_cast<dword>(out_count);
			}
		}
		if (changed)
			IDirectSoundBuffer_SetMixBinVolumes(buffer, &out_mixbins);
		return;
	}

set_all:
	IDirectSoundBuffer_SetMixBins(buffer, &settings->mixbins);
}

// @retail 0x2216f0
void function_2216f0(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	IDirectSoundBuffer *buffer)
{
	sound_mixbins_update_buffer(list, settings, buffer);
}

#endif
