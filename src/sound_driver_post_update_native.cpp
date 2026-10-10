// Exact dormant sound-driver post-update isolated from the broader Xbox
// driver source.  Effects, reverb, occlusion, submix, stream advancement, and
// deferred DirectSound operations remain genuine mutable backend work.

#include "unknown_11c920.h"
#include "unknown_218850.h"
#include <stddef.h>
#include <math.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;
struct IDirectSoundBuffer;
struct s_sound_stream;

class c_sound_driver_effects
{
public:
	virtual void initialize(void *settings, void *buffers, bool rear) = 0;
	virtual void dispose() = 0;
	virtual void set_i3dl2(void *parameters, long unused) = 0;
};

struct s_sound_stream
{
	byte unknown00;
	char state : 3;
	char channel_count : 3;
	char unknown01_6 : 2;
	char chunk_count : 3;
	char started_count : 3;
	char unknown02_6 : 2;
	byte unknown03;
	byte unknown04[0x18];
	s_sound_chunk *chunks[2];
	void *stream;
	long unknown28;
	long offset;
	void *codec;
};

struct s_sound_driver_reverb
{
	byte unknown00[8];
	real room;
	real room_hf;
	real room_rolloff;
	real decay_time;
	real decay_hf_ratio;
	real reflections;
	real reflections_delay;
	real reverb;
	real reverb_delay;
	real diffusion;
	real density;
	real hf_reference;
	byte unknown38[0x10];
};

struct s_sound_driver_occlusion
{
	real minimum;
	real maximum;
	real scale;
	real offset;
};

struct s_sound_effect_parameters
{
	bool dirty;
	byte unknown01[3];
	long room;
	long room_hf;
	long direct;
	long direct_hf;
	real unknown70;
	long unknown74;
	real unknown78;
	long unknown7c;
	real unknown80;
	long unknown84;
	real unknown88;
};

struct s_i3dl2_reverb_description
{
	long room;
	long room_hf;
	real room_rolloff_factor;
	real decay_time;
	real decay_hf_ratio;
	long reflections;
	real reflections_delay;
	long reverb;
	real reverb_delay;
	real diffusion;
	real density;
	real hf_reference;
};

struct s_high_level_effect_description
{
	dword effect_type;
	s_i3dl2_reverb_description reverb;
};

struct s_driver_post_update_view
{
	bool unknown0000;
	bool surround;
	byte unknown0002[2];
	short channel_count;
	byte unknown0006[6];
	s_sound_stream channels[0x80];
	byte unknown1a0c[0xf44];
	s_sound_driver_reverb reverbs[2];
	real reverb_scales[2];
	s_sound_driver_occlusion occlusions[2];
	bool reverb_dirty[2];
	bool occlusion_dirty[2];
	byte unknown2a0c[0x64];
	IDirectSoundBuffer *submix_buffers[8];
	byte unknown2a90[0x20];
	IDirectSound *direct_sound;
	c_sound_driver_effects *effects;
	byte unknown2ab8[0x20];
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
extern s_sound_effect_parameters g_47005c;
extern real g_468834;

long function_2197f0(real gain);
real function_12aff0(real minimum, real maximum, real value, bool enabled);
void sound_stream_update(s_sound_stream *stream);
void function_191550(void);
extern "C" long __stdcall XAudioSetEffectData(
	long effect_index, s_high_level_effect_description *description,
	void *unknown);
extern "C" long __stdcall IDirectSoundBuffer_SetVolume(
	IDirectSoundBuffer *buffer, long volume);
extern "C" long __stdcall IDirectSound_CommitDeferredSettings(
	IDirectSound *direct_sound);

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(sizeof(s_sound_driver_reverb) == 0x48,
	"driver reverb size");
static_assert(sizeof(s_sound_driver_occlusion) == 0x10,
	"driver occlusion size");
static_assert(sizeof(s_sound_effect_parameters) == 0x30,
	"sound effect parameters size");
static_assert(sizeof(s_i3dl2_reverb_description) == 0x30,
	"I3DL2 reverb description size");
static_assert(sizeof(s_high_level_effect_description) == 0x34,
	"high-level effect description size");
static_assert(sizeof(s_driver_post_update_view) == 0x2ad8,
	"sound driver globals size");
static_assert(offsetof(s_driver_post_update_view, channels) == 0x0c,
	"driver channel-array offset");
static_assert(offsetof(s_driver_post_update_view, reverbs) == 0x2950,
	"driver reverb offset");
static_assert(offsetof(s_driver_post_update_view, reverb_scales) == 0x29e0,
	"driver reverb-scale offset");
static_assert(offsetof(s_driver_post_update_view, occlusions) == 0x29e8,
	"driver occlusion offset");
static_assert(offsetof(s_driver_post_update_view, reverb_dirty) == 0x2a08,
	"driver reverb-dirty offset");
static_assert(offsetof(s_driver_post_update_view, occlusion_dirty) == 0x2a0a,
	"driver occlusion-dirty offset");
static_assert(offsetof(s_driver_post_update_view, submix_buffers) == 0x2a70,
	"driver submix-buffer offset");
static_assert(offsetof(s_driver_post_update_view, direct_sound) == 0x2ab0,
	"driver DirectSound offset");
static_assert(offsetof(s_driver_post_update_view, effects) == 0x2ab4,
	"driver effects offset");

static inline real real_from_bits(long value)
{
	union
	{
		long integer;
		real scalar;
	} converted = { value };
	return converted.scalar;
}

static inline real reverb_decibels_add(long first, long second)
{
	return static_cast<real>(static_cast<double>(real_from_bits(first)) +
		real_from_bits(second));
}

// @retail 0x21ece0
void function_21ece0(long effect_index, s_sound_driver_reverb const *reverb)
{
	static const dword k_i3dl2_reverb = 12;
	s_high_level_effect_description description;
	long gain = function_2197f0(g_468834);

	description.effect_type = k_i3dl2_reverb;
	description.reverb.room = static_cast<long>(function_12aff0(-64.0f, 0.0f,
		reverb_decibels_add(*reinterpret_cast<long const *>(&reverb->room), gain),
		true) * 6400.0f - 6400.0f);
	description.reverb.room_hf = static_cast<long>(function_12aff0(-64.0f, 0.0f,
		reverb_decibels_add(*reinterpret_cast<long const *>(&reverb->room_hf),
			gain), true) * 6400.0f - 6400.0f);
	description.reverb.room_rolloff_factor = reverb->room_rolloff;
	description.reverb.decay_time = reverb->decay_time;
	description.reverb.decay_hf_ratio = reverb->decay_hf_ratio;
	description.reverb.reflections = static_cast<long>(function_12aff0(
		-64.0f, 10.0f,
		reverb_decibels_add(
			*reinterpret_cast<long const *>(&reverb->reflections), gain),
		true) * 7400.0f - 6400.0f);
	description.reverb.reflections_delay = reverb->reflections_delay;
	description.reverb.reverb = static_cast<long>(function_12aff0(-64.0f, 20.0f,
		reverb_decibels_add(*reinterpret_cast<long const *>(&reverb->reverb),
			gain), true) * 8400.0f - 6400.0f);
	description.reverb.reverb_delay = reverb->reverb_delay;
	description.reverb.diffusion = reverb->diffusion * 100.0f;
	description.reverb.density = reverb->density * 100.0f;
	description.reverb.hf_reference = reverb->hf_reference;
	XAudioSetEffectData(effect_index, &description, NULL);
}

static inline real sound_occlusion_overlap(
	s_sound_driver_occlusion const *occlusion, real const *range)
{
	real upper = occlusion->maximum > range[1] ?
		range[1] : occlusion->maximum;
	real lower = occlusion->minimum > range[0] ?
		occlusion->minimum : range[0];
	real overlap = upper - lower;
	return overlap > 0.0f ? overlap : 0.0f;
}

static inline long sound_submix_volume(real gain)
{
	gain = 0.0f > gain ? 0.0f : gain > 1.0f ? 1.0f : gain;
	long volume;
	if (gain == 0.0f)
		volume = -6400;
	else
	{
		volume = static_cast<long>(log10(gain) * 2000.0f);
		if (volume < -6400)
			volume = -6400;
		else if (volume > 0)
			volume = 0;
	}
	return volume;
}

// @retail 0x21ee80
void function_21ee80(void)
{
	union
	{
		s_sound_effect_parameters parameters;
		real angles[4][6];
	} local;
	s_driver_post_update_view *globals =
		reinterpret_cast<s_driver_post_update_view *>(g_51ebe4);

	if (g_47005c.dirty)
	{
		globals->effects->set_i3dl2(&g_47005c.room, globals->surround);
		g_47005c.dirty = false;
	}

	for (long group = 0; group < 2; group++)
	{
		globals = reinterpret_cast<s_driver_post_update_view *>(g_51ebe4);
		if (globals->reverb_dirty[group])
		{
			function_21ece0(9 - group, &globals->reverbs[group]);
			globals = reinterpret_cast<s_driver_post_update_view *>(g_51ebe4);
			globals->reverb_dirty[group] = false;
		}
		if (globals->occlusion_dirty[group])
		{
			local.angles[0][0] = -0.785398185f;
			local.angles[0][1] = 2.3561945f;
			local.angles[0][2] = 5.49778748f;
			local.angles[0][3] = 6.28318548f;
			local.angles[0][4] = -6.28318548f;
			local.angles[0][5] = -3.92699099f;
			local.angles[1][0] = 0.785398185f;
			local.angles[1][1] = 3.92699099f;
			local.angles[1][2] = -5.49778748f;
			local.angles[1][3] = -2.3561945f;
			local.angles[1][4] = 0.0f;
			local.angles[1][5] = 0.0f;
			local.angles[2][0] = -2.3561945f;
			local.angles[2][1] = 0.785398185f;
			local.angles[2][2] = 3.92699099f;
			local.angles[2][3] = 6.28318548f;
			local.angles[2][4] = -6.28318548f;
			local.angles[2][5] = -5.49778748f;
			local.angles[3][0] = 2.3561945f;
			local.angles[3][1] = 5.49778748f;
			local.angles[3][2] = -3.92699099f;
			local.angles[3][3] = -0.785398185f;
			local.angles[3][4] = 0.0f;
			local.angles[3][5] = 0.0f;
			real const *ranges[4] = {
				local.angles[0], local.angles[2],
				local.angles[1], local.angles[3] };
			real scale = globals->reverb_scales[group];
			s_sound_driver_occlusion const *occlusion =
				&globals->occlusions[group];
			for (long channel = 0; channel < 4; channel++)
			{
				real overlap0 = sound_occlusion_overlap(
					occlusion, ranges[channel]);
				real overlap1 = sound_occlusion_overlap(
					occlusion, ranges[channel] + 2);
				real overlap2 = sound_occlusion_overlap(
					occlusion, ranges[channel] + 4);
				real fraction = (overlap2 + overlap1 + overlap0) *
					0.318309873f;
				real gain = ((fraction - occlusion->offset) *
					occlusion->scale + occlusion->offset) * scale;
				IDirectSoundBuffer_SetVolume(
					globals->submix_buffers[group * 4 + channel],
					sound_submix_volume(gain));
				globals = reinterpret_cast<s_driver_post_update_view *>(g_51ebe4);
			}
			globals->occlusion_dirty[group] = false;
		}
	}

	IDirectSound_CommitDeferredSettings(globals->direct_sound);
	globals = reinterpret_cast<s_driver_post_update_view *>(g_51ebe4);
	for (long channel = 0; channel < globals->channel_count; channel++)
	{
		s_sound_stream *stream = &globals->channels[channel];
		// Both retail state switches have four empty cases.
		switch (reinterpret_cast<s_sound_stream const volatile *>(stream)->state)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			break;
		}
		sound_stream_update(stream);
		switch (reinterpret_cast<s_sound_stream const volatile *>(stream)->state)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			break;
		}
		globals = reinterpret_cast<s_driver_post_update_view *>(g_51ebe4);
	}
	function_191550();
}

#endif
