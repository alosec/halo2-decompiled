// Exact constructor-free XHV callback object and engine lifetime isolated from
// network_voice.cpp, whose broad recovered source requires unavailable Xbox
// voice and online headers.  Every engine operation remains a genuine XHV
// backend import; this source never fabricates engine creation or success.

#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

namespace XHVNamespace
{
struct CXHVEngine;
}

struct XHV_SR_ITEM;

enum XHV_VOICE_COMMUNICATOR_STATUS
{
	_XHV_VOICE_COMMUNICATOR_STATUS_INSERTED = 0,
	_XHV_VOICE_COMMUNICATOR_STATUS_REMOVED = 1,
};

typedef void *XHV_PROCESSING_MODE;

struct XHV_VOICE_MASK
{
	real robotic_value;
	real whisper_value;
	real pitch_scale;
	real spectral_energy_weight;
};

struct XHV_RUNTIME_PARAMS
{
	dword maximum_remote_talkers;
	dword maximum_local_talkers;
	dword maximum_compressed_buffers;
	dword flags;
	void *effect_image_description;
	dword effects_start_index;
	dword out_of_sync_threshold;
	long custom_vad_provided;
	long headphone_always_on;
};

struct XHV_LOCAL_TALKER_STATUS
{
	XHV_VOICE_COMMUNICATOR_STATUS communicator_status;
};

typedef void (*s_voice_callback_method)(void);

class c_voice_xhv
{
public:
	s_voice_callback_method const *title_vtable;
	bool initialized;
	byte unknown05[3];
	long mode;
	long unknown0c;
	XHVNamespace::CXHVEngine *engine;
	XHV_VOICE_MASK masks[4];
	long port_modes[4];
	bool communicator_present[4];
	dword *voice_mail_sizes[4];
	dword *voice_mail_durations[4];
	bool voice_mail_active[4];
	bool chat_data_ready[4];
};

struct s_voice_globals
{
	long unknown00;
	long unknown04;
	long unknown08;
	long unknown0c;
	long mode;
	long session_kind;
	long type;
	long pool_mode;
	long unknown20;
	long unknown24[0x30];
	void *pool;
	void *pool2;
	bool use_pool2;
	byte unknowned;
	word unknownee;
	word unknownf0[16];
	word unknown110[16];
	long port_states[4];
	bool initialized;
	byte unknown141[3];
};

struct s_voice_effects
{
	void *description;
};

struct s_game_options_view
{
	byte unknown00[8];
	long state;
};

static_assert(sizeof(XHV_PROCESSING_MODE) * 4 == 0x10,
	"XHV processing-mode storage size");
static_assert(sizeof(XHV_VOICE_MASK) == 0x10, "XHV voice mask size");
static_assert(sizeof(XHV_RUNTIME_PARAMS) == 0x24,
	"XHV runtime parameters size");
static_assert(offsetof(c_voice_xhv, initialized) == 0x04,
	"voice XHV initialized offset");
static_assert(offsetof(c_voice_xhv, mode) == 0x08,
	"voice XHV mode offset");
static_assert(offsetof(c_voice_xhv, unknown0c) == 0x0c,
	"voice XHV flags offset");
static_assert(offsetof(c_voice_xhv, engine) == 0x10,
	"voice XHV engine offset");
static_assert(offsetof(c_voice_xhv, masks) == 0x14,
	"voice XHV masks offset");
static_assert(offsetof(c_voice_xhv, port_modes) == 0x54,
	"voice XHV port modes offset");
static_assert(offsetof(c_voice_xhv, communicator_present) == 0x64,
	"voice XHV communicator offset");
static_assert(offsetof(c_voice_xhv, voice_mail_sizes) == 0x68,
	"voice XHV mail sizes offset");
static_assert(offsetof(c_voice_xhv, voice_mail_durations) == 0x78,
	"voice XHV mail durations offset");
static_assert(offsetof(c_voice_xhv, voice_mail_active) == 0x88,
	"voice XHV mail-active offset");
static_assert(offsetof(c_voice_xhv, chat_data_ready) == 0x8c,
	"voice XHV chat-ready offset");
static_assert(sizeof(c_voice_xhv) == 0x90, "voice XHV object size");
static_assert(sizeof(s_voice_globals) == 0x144, "voice globals size");
static_assert(offsetof(s_voice_globals, pool_mode) == 0x1c,
	"voice pool mode offset");
static_assert(offsetof(s_voice_globals, unknownee) == 0xee,
	"voice active mask offset");
static_assert(offsetof(s_voice_globals, initialized) == 0x140,
	"voice initialized offset");
static_assert(offsetof(s_game_options_view, state) == 8,
	"game options state offset");

void function_53a20(dword port, void *data, dword size)
	__asm__("?function_53a20@@YGXKPAXK@Z");

long h2_voice_local_chat_data_ready(
	c_voice_xhv *xhv, dword port, dword size, void *data)
	__asm__("?LocalChatDataReady@c_voice_xhv@@UAGJKKPAX@Z");
long h2_voice_local_chat_data_ready(
	c_voice_xhv *xhv, dword port, dword size, void *data)
{
	if (size > 0)
	{
		xhv->chat_data_ready[port] = true;
		function_53a20(port, data, size);
	}
	return 0;
}

long h2_voice_communicator_status_update(c_voice_xhv *xhv, dword port,
	XHV_VOICE_COMMUNICATOR_STATUS status)
	__asm__("?CommunicatorStatusUpdate@c_voice_xhv@@UAGJKW4XHV_VOICE_COMMUNICATOR_STATUS@@@Z");
long h2_voice_communicator_status_update(c_voice_xhv *xhv, dword port,
	XHV_VOICE_COMMUNICATOR_STATUS status)
{
	if (status == _XHV_VOICE_COMMUNICATOR_STATUS_INSERTED)
		xhv->communicator_present[port] = true;
	else if (status == _XHV_VOICE_COMMUNICATOR_STATUS_REMOVED)
		xhv->communicator_present[port] = false;
	return 0;
}

long h2_voice_speech_recognized(
	c_voice_xhv *, dword, XHV_SR_ITEM *, dword)
	__asm__("?SpeechRecognized@c_voice_xhv@@UAGJKPAUXHV_SR_ITEM@@K@Z");
long h2_voice_speech_recognized(
	c_voice_xhv *, dword, XHV_SR_ITEM *, dword)
{
	return static_cast<long>(0x80004001u);
}

long h2_voice_microphone_raw_data_ready(
	c_voice_xhv *, dword, dword, void *, long *)
	__asm__("?MicrophoneRawDataReady@c_voice_xhv@@UAGJKKPAXPAH@Z");
long h2_voice_microphone_raw_data_ready(
	c_voice_xhv *, dword, dword, void *, long *)
{
	return static_cast<long>(0x80004001u);
}

long h2_voice_mail_data_ready(
	c_voice_xhv *xhv, dword port, dword duration, dword size)
	__asm__("?VoiceMailDataReady@c_voice_xhv@@UAGJKKK@Z");
long h2_voice_mail_data_ready(
	c_voice_xhv *xhv, dword port, dword duration, dword size)
{
	if (xhv->voice_mail_sizes[port])
		*xhv->voice_mail_sizes[port] = size;
	if (xhv->voice_mail_durations[port])
		*xhv->voice_mail_durations[port] = duration;
	xhv->voice_mail_active[port] = false;
	return 0;
}

long h2_voice_mail_stopped(c_voice_xhv *xhv, dword port)
	__asm__("?VoiceMailStopped@c_voice_xhv@@UAGJK@Z");
long h2_voice_mail_stopped(c_voice_xhv *xhv, dword port)
{
	xhv->voice_mail_active[port] = false;
	return 0;
}

// Retail callback table 0x45095c.  Its custom internal name is not a game
// ABI; the object and all six methods retain their exact Microsoft ABIs.
extern s_voice_callback_method const h2_voice_xhv_callback_vtable[6] =
{
	reinterpret_cast<s_voice_callback_method>(h2_voice_local_chat_data_ready),
	reinterpret_cast<s_voice_callback_method>(h2_voice_communicator_status_update),
	reinterpret_cast<s_voice_callback_method>(h2_voice_speech_recognized),
	reinterpret_cast<s_voice_callback_method>(h2_voice_microphone_raw_data_ready),
	reinterpret_cast<s_voice_callback_method>(h2_voice_mail_data_ready),
	reinterpret_cast<s_voice_callback_method>(h2_voice_mail_stopped),
};

c_voice_xhv g_476fc8 =
{
	h2_voice_xhv_callback_vtable,
	false, {0, 0, 0},
	0, 0, NULL,
	{}, {0, 0, 0, 0}, {false, false, false, false},
	{NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL},
	{false, false, false, false}, {false, false, false, false},
};

XHV_PROCESSING_MODE g_52731c[4];

extern s_voice_globals g_4c9878;
extern s_voice_effects *g_510c90;
extern s_game_options_view *g_4e6948;

extern "C" long __stdcall XHVEngineCreate(
	XHV_RUNTIME_PARAMS *parameters, XHVNamespace::CXHVEngine **engine)
	__asm__("_XHVEngineCreate@8");
extern "C" long __stdcall xhv_enable_processing_mode(
	XHVNamespace::CXHVEngine *engine, XHV_PROCESSING_MODE mode)
	__asm__("?EnableProcessingMode@CXHVEngine@XHVNamespace@@QAGJPAX@Z");
extern "C" long __stdcall xhv_set_callback_interface(
	XHVNamespace::CXHVEngine *engine, c_voice_xhv *callback)
	__asm__("_XHVEngine_SetCallbackInterface@8");
extern "C" long __stdcall xhv_set_max_playback_streams_count(
	XHVNamespace::CXHVEngine *engine, dword count)
	__asm__("?SetMaxPlaybackStreamsCount@CXHVEngine@XHVNamespace@@QAGJK@Z");
extern "C" long __stdcall xhv_register_local_talker(
	XHVNamespace::CXHVEngine *engine, dword port)
	__asm__("?RegisterLocalTalker@CXHVEngine@XHVNamespace@@QAGJK@Z");
extern "C" long __stdcall xhv_set_processing_mode(
	XHVNamespace::CXHVEngine *engine, dword port, XHV_PROCESSING_MODE mode)
	__asm__("?SetProcessingMode@CXHVEngine@XHVNamespace@@QAGJKPAX@Z");
extern "C" long __stdcall xhv_get_local_talker_status(
	XHVNamespace::CXHVEngine *engine, dword port,
	XHV_LOCAL_TALKER_STATUS *status)
	__asm__("?GetLocalTalkerStatus@CXHVEngine@XHVNamespace@@QAGJKPAU_XHV_LOCAL_TALKER_STATUS@@@Z");

// @retail 0x556a0
void voice_xhv_reset_masks(c_voice_xhv *xhv)
{
	for (long port = 0; port < 4; port++)
	{
		xhv->masks[port].robotic_value = -1.0f;
		xhv->masks[port].whisper_value = -1.0f;
		xhv->masks[port].pitch_scale = -1.0f;
		xhv->masks[port].spectral_energy_weight = -1.0f;
	}
}

// @retail 0x55720
void voice_xhv_reset_port_modes(c_voice_xhv *xhv)
{
	long const mode = xhv->mode == 1 ? 3 : 2;
	for (long port = 0; port < 4; port++)
		xhv->port_modes[port] = mode;
}

// @retail 0x55780
bool voice_xhv_get_runtime_parameters(
	XHV_RUNTIME_PARAMS *parameters, c_voice_xhv *xhv)
{
	void *const effects = g_510c90->description;
	if (!effects)
		return false;

	memset(parameters, 0, sizeof(*parameters));
	if (xhv->mode == 2)
	{
		parameters->maximum_remote_talkers = 15;
		parameters->maximum_compressed_buffers = 5;
	}
	parameters->maximum_local_talkers = 4;
	parameters->flags = 0;
	parameters->effect_image_description = effects;
	parameters->effects_start_index = 11;
	parameters->out_of_sync_threshold = 32;
	parameters->custom_vad_provided = 0;
	parameters->headphone_always_on = 0;
	return true;
}

// @retail 0x55810
bool voice_xhv_create(c_voice_xhv *xhv)
{
	XHV_RUNTIME_PARAMS parameters;
	bool result = voice_xhv_get_runtime_parameters(&parameters, xhv);
	if (result && XHVEngineCreate(&parameters, &xhv->engine) >= 0)
	{
		long error = 0;
		xhv->unknown0c = xhv->mode == 1 ? 0x0b : 0x0f;
		for (dword mode = 0; mode < 4; mode++)
		{
			if (xhv->unknown0c & (1 << mode))
				error = xhv_enable_processing_mode(
					xhv->engine, g_52731c[mode]);
		}
		if (error >= 0)
		{
			error = xhv_set_callback_interface(xhv->engine, xhv);
			if (xhv->unknown0c & 4)
				xhv_set_max_playback_streams_count(xhv->engine, 15);
			if (error >= 0)
			{
				for (dword port = 0; port < 4; port++)
				{
					XHV_LOCAL_TALKER_STATUS status;
					xhv_register_local_talker(xhv->engine, port);
					xhv_set_processing_mode(
						xhv->engine, port, g_52731c[xhv->port_modes[port]]);
					xhv_get_local_talker_status(xhv->engine, port, &status);
					xhv->communicator_present[port] =
						status.communicator_status ==
						_XHV_VOICE_COMMUNICATOR_STATUS_INSERTED;
					xhv->chat_data_ready[port] = false;
					xhv->voice_mail_active[port] = false;
				}
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x55080
void voice_xhv_initialize(c_voice_xhv *xhv, long mode)
{
	if (!xhv->initialized)
	{
		xhv->engine = NULL;
		xhv->mode = mode;
		voice_xhv_reset_masks(xhv);
		voice_xhv_reset_port_modes(xhv);
		xhv->initialized = voice_xhv_create(xhv);
	}
}

// @retail 0x53510
void voice_start_engine(void)
{
	if (g_4c9878.initialized)
	{
		long const mode = g_4c9878.pool_mode;
		if (mode == 2 || (g_4e6948->state == 1 && mode == 1))
		{
			if (!g_476fc8.initialized)
				voice_xhv_initialize(&g_476fc8, mode);
		}
	}
}

void voice_xhv_dispose(c_voice_xhv *xhv);

// @retail 0x53550
void voice_stop_engine(void)
{
	if (g_4c9878.initialized && g_476fc8.initialized)
	{
		voice_xhv_dispose(&g_476fc8);
		g_4c9878.unknownee = 0;
	}
}

#endif
