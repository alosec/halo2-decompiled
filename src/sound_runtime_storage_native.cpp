// Source-local process-lifetime storage for sound publications whose complete
// recovered startup and teardown paths are assembled in the dormant sound
// cluster. These zero-initialized globals do not provide the static arena,
// pools, allocator vtables, codec objects, or an audio backend.

#include "unknown_11c920.h"

struct s_sound_voice;
struct s_record_pool;
struct s_sound_cache_allocator;
struct s_voice_effects;
struct s_bink_sound_settings;

long g_4e6374;
s_sound_voice *g_4e6378;
s_record_pool *g_502104;
dword g_502108;
s_sound_cache_allocator *g_50210c;
s_voice_effects *g_510c90;
void *g_51ebe0;
s_bink_sound_settings *g_51ebe4;
