// Exact process-lifetime voice globals and process initializer isolated from
// network_voice.cpp.  These are the complete zero-initialized publications
// initialized by function_53210; no stage pool or XHV success is fabricated.

#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

class c_memory_source;

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

struct s_voice_player_values
{
	bool enabled;
	byte unknown01[3];
	long values[16];
};

struct s_voice_channel
{
	bool active;
	byte unknown01[3];
	long unknown04;
	word player_mask;
	byte unknown0a[2];
	long index;
	dword data[0x48];
	long values[16];
};

struct s_voice_channels
{
	bool initialized;
	byte unknown01[3];
	s_voice_channel channels[16];
};

struct s_voice_player_settings
{
	bool initialized;
	byte unknown01[3];
	long unknown04[16];
	long unknown44[16];
	long unknown84[16];
	long unknownc4;
	long unknownc8[16];
	long unknown108[16];
	long unknown148[16];
	long unknown188;
	long unknown18c;
	bool unknown190;
	byte unknown191[3];
	long unknown194[16];
	long unknown1d4[16];
};

struct s_voice_routing
{
	bool enabled;
	byte unknown01[3];
	s_voice_player_settings settings;
};

static_assert(sizeof(s_voice_globals) == 0x144, "voice globals size");
static_assert(offsetof(s_voice_globals, pool) == 0xe4,
	"voice primary pool offset");
static_assert(offsetof(s_voice_globals, pool2) == 0xe8,
	"voice secondary pool offset");
static_assert(offsetof(s_voice_globals, use_pool2) == 0xec,
	"voice pool selector offset");
static_assert(offsetof(s_voice_globals, initialized) == 0x140,
	"voice initialized offset");
static_assert(sizeof(s_voice_player_values) == 0x44,
	"voice player values size");
static_assert(sizeof(s_voice_channel) == 0x170, "voice channel size");
static_assert(offsetof(s_voice_channel, index) == 0x0c,
	"voice channel index offset");
static_assert(offsetof(s_voice_channel, data) == 0x10,
	"voice channel data offset");
static_assert(offsetof(s_voice_channel, values) == 0x130,
	"voice channel values offset");
static_assert(sizeof(s_voice_channels) == 0x1704,
	"voice channels size");
static_assert(sizeof(s_voice_player_settings) == 0x214,
	"voice player settings size");
static_assert(offsetof(s_voice_player_settings, unknown188) == 0x188,
	"voice settings value offset");
static_assert(offsetof(s_voice_player_settings, initialized) == 0,
	"voice settings initialized offset");
static_assert(sizeof(s_voice_routing) == 0x218, "voice routing size");

c_memory_source *g_46dd54;
s_voice_globals g_4c9878;
s_voice_player_values g_5259b8;
s_voice_channels g_525a00;
s_voice_routing g_527104;

// @retail 0x56080
void voice_channels_initialize(s_voice_channels *channels)
{
	for (long i = 0; i < 16; i++)
	{
		s_voice_channel *const channel = &channels->channels[i];
		channel->index = i;
		channel->active = true;
		channel->unknown04 = 0;
		channel->player_mask = 0;
		memset(channel->data, 0, sizeof(channel->data));
		memset(channel->values, 0, sizeof(channel->values));
	}
	channels->initialized = true;
}

// @retail 0x56ae0
void voice_player_settings_initialize(s_voice_player_settings *settings)
{
	memset(settings->unknown04, 0, sizeof(settings->unknown04));
	memset(settings->unknown44, 0, sizeof(settings->unknown44));
	memset(settings->unknown84, 0, sizeof(settings->unknown84));
	memset(settings->unknownc8, 0, sizeof(settings->unknownc8));
	memset(settings->unknown108, 0, sizeof(settings->unknown108));
	memset(settings->unknown194, 0, sizeof(settings->unknown194));
	memset(settings->unknown1d4, 0, sizeof(settings->unknown1d4));
	memset(settings->unknown148, 0, sizeof(settings->unknown148));
	settings->unknown18c = -1000;
	settings->unknown188 = -80;
	settings->unknown190 = true;
	settings->initialized = true;
}

// @retail 0x53210
void function_53210(void)
{
	memset(&g_4c9878, 0, sizeof(g_4c9878));
	memset(g_5259b8.values, 0, sizeof(g_5259b8.values));
	g_4c9878.unknown00 = NONE;
	g_4c9878.unknown04 = 0;
	g_4c9878.mode = 0;
	g_4c9878.session_kind = 0;
	g_4c9878.pool_mode = 0;
	g_4c9878.type = 0;
	g_4c9878.pool = NULL;
	g_4c9878.pool2 = NULL;
	g_4c9878.use_pool2 = false;
	g_5259b8.enabled = true;
	g_527104.enabled = true;
	voice_channels_initialize(&g_525a00);
	voice_player_settings_initialize(&g_527104.settings);
	g_4c9878.initialized = true;
}

#endif
