// Exact dormant sound-driver listener publication isolated from broader Xbox
// driver update code.  Position and orientation remain genuine deferred
// DirectSound operations; reverb and occlusion state remain authentic mutable
// driver storage.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <stddef.h>
#include <math.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;

struct s_sound_driver_reverb
{
	byte data[0x48];
};

struct s_sound_driver_occlusion
{
	real minimum;
	real maximum;
	real scale;
	real offset;
};

struct s_sound_listener_environment
{
	s_sound_driver_reverb const *reverb;
	s_sound_driver_occlusion occlusion;
	real scale;
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

struct s_driver_listener_view
{
	bool unknown0000;
	byte unknown0001[0x2917];
	point3f listener_position;
	vector3f listener_forward;
	vector3f listener_up;
	byte unknown293c[0x14];
	s_sound_driver_reverb reverbs[2];
	real reverb_scales[2];
	s_sound_driver_occlusion occlusions[2];
	bool reverb_dirty[2];
	bool occlusion_dirty[2];
	byte unknown2a0c[0xa4];
	IDirectSound *direct_sound;
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
extern s_sound_driver_reverb g_44a2a0;

extern "C" long __stdcall IDirectSound_SetPosition(
	IDirectSound *direct_sound, real x, real y, real z, dword apply);
extern "C" long __stdcall IDirectSound_SetOrientation(
	IDirectSound *direct_sound,
	real forward_x, real forward_y, real forward_z,
	real up_x, real up_y, real up_z,
	dword apply);

static_assert(sizeof(s_sound_driver_reverb) == 0x48,
	"driver reverb size");
static_assert(sizeof(s_sound_driver_occlusion) == 0x10,
	"driver occlusion size");
static_assert(sizeof(s_sound_listener_environment) == 0x18,
	"listener environment size");
static_assert(sizeof(s_sound_listener) == 0x38, "listener size");
static_assert(offsetof(s_sound_listener, environment_count) == 0x30,
	"listener environment-count offset");
static_assert(offsetof(s_driver_listener_view, listener_position) == 0x2918,
	"driver listener-position offset");
static_assert(offsetof(s_driver_listener_view, reverbs) == 0x2950,
	"driver reverb offset");
static_assert(offsetof(s_driver_listener_view, reverb_scales) == 0x29e0,
	"driver reverb-scale offset");
static_assert(offsetof(s_driver_listener_view, occlusions) == 0x29e8,
	"driver occlusion offset");
static_assert(offsetof(s_driver_listener_view, direct_sound) == 0x2ab0,
	"driver DirectSound offset");

static inline bool sound_real_equal(real a, real b, real epsilon)
{
	return fabs(a - b) < epsilon;
}

// @retail 0x220790
void function_220790(s_sound_listener const *listener)
{
	static const dword k_deferred = 1;
	s_driver_listener_view *globals =
		reinterpret_cast<s_driver_listener_view *>(g_51ebe4);

	if (!globals->unknown0000)
	{
		IDirectSound_SetPosition(globals->direct_sound,
			listener->position.x, listener->position.z, listener->position.y,
			k_deferred);
		globals->listener_position = listener->position;
	}

	if (!(sound_real_equal(listener->forward.i, globals->listener_forward.i,
			0.05f) &&
		sound_real_equal(listener->forward.j, globals->listener_forward.j,
			0.05f) &&
		sound_real_equal(listener->forward.k, globals->listener_forward.k,
			0.05f) &&
		sound_real_equal(listener->up.i, globals->listener_up.i, 0.05f) &&
		sound_real_equal(listener->up.j, globals->listener_up.j, 0.05f) &&
		sound_real_equal(listener->up.k, globals->listener_up.k, 0.05f)) ||
		!globals->unknown0000)
	{
		IDirectSound_SetOrientation(globals->direct_sound,
			listener->forward.i, listener->forward.k, listener->forward.j,
			listener->up.i, listener->up.k, listener->up.j, k_deferred);
		globals = reinterpret_cast<s_driver_listener_view *>(g_51ebe4);
		globals->listener_forward = listener->forward;
		globals->listener_up = listener->up;
	}

	long environment_count = listener->environment_count < 2 ?
		listener->environment_count : 2;
	long i;
	for (i = 0; i < environment_count; i++)
	{
		s_sound_listener_environment const *environment =
			&listener->environments[i];
		if (memcmp(environment->reverb, &globals->reverbs[i],
				sizeof(s_sound_driver_reverb)) != 0 || !globals->unknown0000)
		{
			globals->reverbs[i] = *environment->reverb;
			globals->reverb_dirty[i] = true;
		}
		if (memcmp(&globals->occlusions[i], &environment->occlusion,
				sizeof(s_sound_driver_occlusion)) != 0 ||
			!globals->unknown0000)
		{
			globals->occlusions[i] = environment->occlusion;
			globals->occlusion_dirty[i] = true;
		}
		if (!sound_real_equal(globals->reverb_scales[i], environment->scale,
				0.0001f) || !globals->unknown0000)
		{
			globals->reverb_scales[i] = environment->scale;
			globals->occlusion_dirty[i] = true;
		}
	}

	for (; i < 2; i++)
	{
		if (memcmp(&g_44a2a0, &globals->reverbs[i],
				sizeof(s_sound_driver_reverb)) != 0 || !globals->unknown0000)
		{
			globals->reverbs[i] = g_44a2a0;
			globals->reverb_dirty[i] = true;
		}
		if (!(fabs(globals->reverb_scales[i]) < 0.0001f) ||
			!globals->unknown0000)
		{
			globals->reverb_scales[i] = 0.0f;
			globals->occlusion_dirty[i] = true;
		}
	}
}

#endif
