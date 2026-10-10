// Exact dormant top-level sound-driver startup composed from source-owned
// phases.  Live device, DSP image, listener, stream, voice, effect, submix,
// and impulse ownership remains genuine; any failed required phase executes
// the exact source-owned ordered teardown.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;
struct s_sound_listener_environment;

struct s_sound_driver_counts
{
	short channels[3];
	byte unknown06[6];
	short voices;
};

struct s_sound_listener
{
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown24[0x0c];
	long environment_count;
	s_sound_listener_environment const *environments;
};

struct s_driver_startup_view
{
	bool initialized;
	byte unknown0001[0x2aaf];
	IDirectSound *direct_sound;
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
extern vector3f *g_4687a8;
extern vector3f *g_4687b0;

bool sound_driver_device_startup(void);
bool function_191300(IDirectSound *direct_sound);
void function_220790(s_sound_listener const *listener);
bool sound_driver_channels_startup(s_sound_driver_counts const *counts);
void sound_driver_effect_buffers_startup(void);
void function_21eae0(void);

static_assert(sizeof(s_sound_driver_counts) == 0x0e,
	"sound driver counts size");
static_assert(offsetof(s_sound_driver_counts, voices) == 0x0c,
	"sound driver voice-count input offset");
static_assert(sizeof(s_sound_listener) == 0x38, "sound listener size");
static_assert(offsetof(s_sound_listener, environment_count) == 0x30,
	"listener environment-count offset");
static_assert(offsetof(s_driver_startup_view, direct_sound) == 0x2ab0,
	"driver DirectSound offset");

// @retail 0x21e4b0
bool function_21e4b0(s_sound_driver_counts const *counts)
{
	bool result = false;
	if (sound_driver_device_startup())
	{
		s_driver_startup_view *globals =
			reinterpret_cast<s_driver_startup_view *>(g_51ebe4);
		if (function_191300(globals->direct_sound))
		{
			s_sound_listener listener;
			memset(&listener, 0, sizeof(listener));
			listener.forward = *g_4687a8;
			listener.up = *g_4687b0;
			function_220790(&listener);
			result = sound_driver_channels_startup(counts);
			sound_driver_effect_buffers_startup();
		}
	}

	s_driver_startup_view *globals =
		reinterpret_cast<s_driver_startup_view *>(g_51ebe4);
	if (result)
		globals->initialized = true;
	else
		function_21eae0();
	return result;
}

#endif
