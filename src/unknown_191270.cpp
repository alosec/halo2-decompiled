#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "crc.h"
#include "unknown_01e930.h"
#include "network_voice.h"

// @flags /O2 /Gr
#include "pool_storage.h"

/* UNKNOWN_191270.CPP: the voice and sound settings and their string ids */

/* network_voice.cpp */
void voice_xhv_dispose(c_voice_xhv *xhv);
void voice_start_engine(void);

/* the sound driver (unknown_221490.cpp) */
short sound_driver_voice_new(dword flags, dword input_mixbin);

/* the effects whose mix bins the DSP image routes, and the parameter of each
   that holds its input mix bin */
struct s_effect_mixbin
{
	long effect;
	long parameter;
};

const s_effect_mixbin g_444c38[5] =
{
	{ 1, 1 },
	{ 0xb, 1 },
	{ 0xc, 1 },
	{ 0xd, 1 },
	{ 0xe, 1 },
};

/* an effect's state as IDirectSound_GetEffectData reads it: its memory, then
   its input mix bin addresses */
struct s_effect_state
{
	DWORD scratch_offset;
	DWORD scratch_length;
	DWORD y_memory_offset;
	DWORD y_memory_length;
	DWORD flags;
	DWORD input_mixbins[10];
};

/* the mix bin whose address in the given range this is, or NONE */
static inline long mixbin_from_address(dword address, dword first, dword last)
{
	long result = NONE;
	dword pinned;

	if (address < first)
		pinned = first;
	else if (address > last)
		pinned = last;
	else
		pinned = address;
	if (pinned == address)
		result = (address - first) >> 5;

	return result;
}

/* creates a voice for each effect input mix bin of the DSP image */
// @retail 0x191420
void function_191420(LPDIRECTSOUND direct_sound)
{
	if (g_510c90->description->dwEffectCount == 15)
	{
		for (long i = 0; i < sizeof(g_444c38) / sizeof(g_444c38[0]); i++)
		{
			s_effect_state state;
			dword address;
			long mixbin;

			IDirectSound_GetEffectData(direct_sound, g_444c38[i].effect, 0, &state, sizeof(state));
			address = state.input_mixbins[g_444c38[i].parameter];
			mixbin = mixbin_from_address(address, 0xc00, 0xfe0);
			if (mixbin == NONE)
				mixbin = mixbin_from_address(address, 0x1400, 0x17e0);
			if ((1 << g_444c38[i].effect) & 2 && mixbin != NONE)
				sound_driver_voice_new(0, mixbin);
		}
	}
}
// @retail 0x1914f0
long function_1914f0(long string_handle)
{
	long result = NONE;

	switch (string_handle)
	{
	case 0x1300012d:
		result = 0xd;
		break;
	case 0x14000135:
		result = 0xe;
		break;
	case 0x1600013c:
		result = 0x1b;
		break;
	case 0x1600013d:
		result = 0x1c;
		break;
	case 0x1600013e:
		result = 0x1d;
		break;
	case 0x1600013f:
		result = 0x1e;
		break;
	}

	return result;
}

// @retail 0x191660
dword function_191660(long string_handle, long index)
{
	dword result = 0;

	if (index == NONE)
	{
		if (string_handle != 0x11000138)
		{
			if (string_handle != 0x17000137)
			{
				if (string_handle != 0x18000136)
				{
					if (string_handle == 0x1300012d)
						result = 2;
				}
				else
					result = 0x30;
			}
			else
				result = 0xc0;
		}
		else
			result = 0xf0;
	}
	else if (string_handle == 0x11000138)
	{
		switch (index)
		{
		case 0:
			result = 0x10;
			break;
		case 1:
			result = 0x20;
			break;
		case 2:
			result = 0x40;
			break;
		case 3:
			result = 0x80;
			break;
		}
	}

	return result;
}

s_voice_effects *g_510c90;

// @retail 0x191270
void function_191270(void)
{
	LPDIRECTSOUND direct_sound = (LPDIRECTSOUND)g_51ebe4->direct_sound;

	IDirectSound_SetEffectData(direct_sound, 4, 0x20, g_510c90->effects[0], 8, DSFX_IMMEDIATE);
	IDirectSound_SetEffectData(direct_sound, 5, 0x20, g_510c90->effects[1], 8, DSFX_IMMEDIATE);
	IDirectSound_SetEffectData(direct_sound, 6, 0x20, g_510c90->effects[2], 8, DSFX_IMMEDIATE);
	IDirectSound_SetEffectData(direct_sound, 7, 0x20, g_510c90->effects[3], 8, DSFX_IMMEDIATE);
	if (g_510c90)
	{
		g_510c90->previous_changed = 0;
		g_510c90->changed = 0;
	}
}

// @retail 0x191550
void function_191550(void)
{
	dword changed = g_510c90->previous_changed & ~g_510c90->changed;
	LPDIRECTSOUND direct_sound = (LPDIRECTSOUND)g_51ebe4->direct_sound;

	if (changed & 0x10)
		IDirectSound_SetEffectData(direct_sound, 4, 0x20, g_510c90->effects[0], 8, DSFX_IMMEDIATE);
	if (changed & 0x20)
		IDirectSound_SetEffectData(direct_sound, 5, 0x20, g_510c90->effects[1], 8, DSFX_IMMEDIATE);
	if (changed & 0x40)
		IDirectSound_SetEffectData(direct_sound, 6, 0x20, g_510c90->effects[2], 8, DSFX_IMMEDIATE);
	if (changed & 0x80)
		IDirectSound_SetEffectData(direct_sound, 7, 0x20, g_510c90->effects[3], 8, DSFX_IMMEDIATE);
	g_510c90->previous_changed = g_510c90->changed;
	g_510c90->changed = 0;
}

/* downloads the DSP image again, with the voice engine stopped */
// @retail 0x1915f0
void function_1915f0(void)
{
	if (XLoadSection("DSPImage"))
	{
		DSEFFECTIMAGELOC location;

		location.dwI3DL2ReverbIndex = 9;
		location.dwCrosstalkIndex = 10;
		if (g_4c9878.initialized && g_476fc8.initialized)
		{
			voice_xhv_dispose(&g_476fc8);
			g_4c9878.unknownEE = 0;
		}
		XAudioDownloadEffectsImage("DSPImage", &location, XAUDIO_DOWNLOADFX_XBESECTION, &g_510c90->description);
		voice_start_engine();
		XFreeSection("DSPImage");
	}
}
/* the static memory pool (unknown_221490.cpp) */

// @retail 0x191300
bool function_191300(LPDIRECTSOUND direct_sound)
{
	bool result = false;

	if (XLoadSection("DSPImage"))
	{
		byte *top = (byte *)(g_510804_pool_size + g_510800_pool_base);
		s_voice_effects *effects = (s_voice_effects *)(((dword)top + 3) & ~3);
		long aligned_size = ((byte *)effects - top) + sizeof(s_voice_effects);
		DSEFFECTIMAGELOC location;

		g_510804_pool_size += aligned_size;
		function_163ba0(&g_510808_pool_checksum, &aligned_size, sizeof(aligned_size));
		memset(effects->indices, 0xff, sizeof(effects->indices));
		short *changed = (short *)&effects->changed;
		*changed = 0;
		effects->previous_changed = 0;
		g_510c90 = effects;
		location.dwI3DL2ReverbIndex = 9;
		location.dwCrosstalkIndex = 10;
		if (SUCCEEDED(XAudioDownloadEffectsImage("DSPImage", &location, XAUDIO_DOWNLOADFX_XBESECTION, &effects->description)))
			result = true;
		XFreeSection("DSPImage");

		if (result)
		{
			IDirectSound_GetEffectData(direct_sound, 4, 0x20, g_510c90->effects[0], 8);
			IDirectSound_GetEffectData(direct_sound, 5, 0x20, g_510c90->effects[1], 8);
			IDirectSound_GetEffectData(direct_sound, 6, 0x20, g_510c90->effects[2], 8);
			IDirectSound_GetEffectData(direct_sound, 7, 0x20, g_510c90->effects[3], 8);
		}
	}

	return result;
}
