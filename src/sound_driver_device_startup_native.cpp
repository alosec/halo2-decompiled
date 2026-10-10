// Exact dormant DirectSound device/bootstrap phase isolated from the broader
// driver startup.  Allocation, device creation, capability queries, speaker
// configuration, and every DirectSound state change remain genuine lifetime
// operations; partial failure is intentionally left for ordered teardown.

#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;

struct s_driver_device_view
{
	bool initialized;
	bool surround;
	byte unknown0002[0x2aa0 - 0x0002];
	byte caps[0x10];
	IDirectSound *direct_sound;
	byte unknown2ab4[0x2ad8 - 0x2ab4];
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
extern dword g_510800_pool_base;
extern long g_510804_pool_size;
extern dword g_510808_pool_checksum;

void function_163ba0(dword *checksum, void const *data, long data_size);

extern "C" long __stdcall DirectSoundCreate(
	void *device_guid, IDirectSound **direct_sound, void *outer);
extern "C" long __stdcall IDirectSound_GetCaps(
	IDirectSound *direct_sound, void *caps);
extern "C" long __stdcall IDirectSound_SetDistanceFactor(
	IDirectSound *direct_sound, real factor, dword apply);
extern "C" long __stdcall IDirectSound_SetDopplerFactor(
	IDirectSound *direct_sound, real factor, dword apply);
extern "C" long __stdcall IDirectSound_GetSpeakerConfig(
	IDirectSound *direct_sound, dword *speaker_flags);
extern "C" long __stdcall IDirectSound_SetMixBinHeadroom(
	IDirectSound *direct_sound, dword mixbin, long headroom);
extern "C" long __stdcall IDirectSound_SetRolloffFactor(
	IDirectSound *direct_sound, real factor, dword apply);
extern "C" void __stdcall DirectSoundDoWork(void);

static_assert(sizeof(s_driver_device_view) == 0x2ad8,
	"sound driver globals size");
static_assert(offsetof(s_driver_device_view, caps) == 0x2aa0,
	"sound driver capability offset");
static_assert(offsetof(s_driver_device_view, direct_sound) == 0x2ab0,
	"sound driver DirectSound offset");

// Source-local exact phase of retail function_21e4b0.
bool sound_driver_device_startup(void)
{
	static const dword k_immediate = 0;
	static const dword k_deferred = 1;

	byte *top = reinterpret_cast<byte *>(g_510800_pool_base) +
		g_510804_pool_size;
	byte *memory = reinterpret_cast<byte *>(
		(reinterpret_cast<dword>(top) + 3) & ~3);
	long allocated_size = (memory - top) + sizeof(s_driver_device_view);
	g_510804_pool_size += allocated_size;
	dword checksum = g_510808_pool_checksum;
	function_163ba0(&checksum, &allocated_size, sizeof(allocated_size));
	g_510808_pool_checksum = checksum;
	memset(memory, 0, sizeof(s_driver_device_view));
	g_51ebe4 = reinterpret_cast<s_bink_sound_settings *>(memory);

	s_driver_device_view *globals =
		reinterpret_cast<s_driver_device_view *>(g_51ebe4);
	globals->initialized = false;
	if (DirectSoundCreate(NULL, &globals->direct_sound, NULL) < 0 ||
		IDirectSound_GetCaps(globals->direct_sound, globals->caps) < 0 ||
		IDirectSound_SetDistanceFactor(globals->direct_sound, 3.048f,
			k_immediate) < 0 ||
		IDirectSound_SetDopplerFactor(globals->direct_sound, 0.0f,
			k_immediate) < 0)
	{
		return false;
	}

	dword speaker_flags;
	if (IDirectSound_GetSpeakerConfig(globals->direct_sound,
		&speaker_flags) < 0)
	{
		return false;
	}
	globals->surround = (speaker_flags & 0xffff) == 2 &&
		(speaker_flags & 0x10000);
	for (long mixbin = 0; mixbin <= 31; mixbin++)
		IDirectSound_SetMixBinHeadroom(globals->direct_sound, mixbin, 0);
	IDirectSound_SetRolloffFactor(globals->direct_sound, 0.0f, k_deferred);
	DirectSoundDoWork();
	return true;
}

#endif
