// Exact dormant DirectSound driver teardown isolated from the broader Xbox
// driver source.  Channel halting, interface release, decoder disposal, and
// ownership clearing remain genuine lifetime operations.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;
struct IDirectSoundBuffer;
struct IDirectSoundStream;

struct s_driver_stream
{
	byte unknown00[0x24];
	IDirectSoundStream *stream;
	byte unknown28[0x0c];
};

struct s_driver_voice
{
	byte unknown00[0x34];
	IDirectSoundBuffer *buffer;
	IDirectSoundBuffer *submix;
};

struct s_driver_globals_view
{
	bool initialized;
	byte unknown0001[3];
	short channel_count;
	short voice_count;
	byte unknown0008[4];
	s_driver_stream channels[0x80];
	byte unknown1a0c[0x0c];
	s_driver_voice voices[0x40];
	byte unknown2918[0x198];
	IDirectSound *direct_sound;
};

struct s_47f0d0;
class c_wma_codec
{
public:
	void release_decoder();
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
extern s_47f0d0 g_47f0d0;
extern void *g_510c3c;

void function_21f290(void);
extern "C" dword __stdcall IDirectSoundStream_Release(
	IDirectSoundStream *stream);
extern "C" dword __stdcall IDirectSoundBuffer_Release(
	IDirectSoundBuffer *buffer);
extern "C" dword __stdcall IDirectSound_Release(IDirectSound *direct_sound);

static_assert(sizeof(s_driver_stream) == 0x34, "driver stream size");
static_assert(offsetof(s_driver_stream, stream) == 0x24,
	"driver stream interface offset");
static_assert(sizeof(s_driver_voice) == 0x3c, "driver voice size");
static_assert(offsetof(s_driver_voice, buffer) == 0x34,
	"driver voice buffer offset");
static_assert(offsetof(s_driver_voice, submix) == 0x38,
	"driver voice submix offset");
static_assert(offsetof(s_driver_globals_view, channels) == 0x0c,
	"driver channel-array offset");
static_assert(offsetof(s_driver_globals_view, voices) == 0x1a18,
	"driver voice-array offset");
static_assert(offsetof(s_driver_globals_view, direct_sound) == 0x2ab0,
	"driver DirectSound offset");

// @retail 0x21eae0
void function_21eae0(void)
{
	function_21f290();
	s_driver_globals_view *globals =
		reinterpret_cast<s_driver_globals_view *>(g_51ebe4);

	for (long i = 0; i < globals->channel_count; i++)
	{
		IDirectSoundStream **stream = &globals->channels[i].stream;
		if (*stream)
		{
			IDirectSoundStream_Release(*stream);
			globals = reinterpret_cast<s_driver_globals_view *>(g_51ebe4);
			stream = &globals->channels[i].stream;
			*stream = NULL;
		}
	}
	globals->channel_count = 0;

	for (long i = 0; i < globals->voice_count; i++)
	{
		s_driver_voice *voice = &globals->voices[i];
		if (voice->buffer)
		{
			IDirectSoundBuffer_Release(voice->buffer);
			globals = reinterpret_cast<s_driver_globals_view *>(g_51ebe4);
			voice = &globals->voices[i];
			voice->buffer = NULL;
		}
		if (voice->submix)
		{
			IDirectSoundBuffer_Release(voice->submix);
			globals = reinterpret_cast<s_driver_globals_view *>(g_51ebe4);
			voice = &globals->voices[i];
			voice->submix = NULL;
		}
	}

	if (globals->direct_sound)
	{
		IDirectSound_Release(globals->direct_sound);
		globals = reinterpret_cast<s_driver_globals_view *>(g_51ebe4);
		globals->direct_sound = NULL;
	}

	reinterpret_cast<c_wma_codec *>(&g_47f0d0)->release_decoder();
	globals = reinterpret_cast<s_driver_globals_view *>(g_51ebe4);
	g_510c3c = NULL;
	globals->initialized = false;
}

#endif
