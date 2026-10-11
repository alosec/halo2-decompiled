// Exact process-lifetime PCM stream codec object and its nine-slot retail
// virtual interface.  The object is initialized directly, without relying on
// a host CRT constructor, and every non-empty method remains a recovered
// provider.  This owns dispatch only; it does not fabricate cache data,
// packet submission, or playback progress.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_stream;
struct s_sound_cache_usage;
struct s_sound_chunk;
struct XMEDIAPACKET;

// The retail PCM table shares the base codec's empty attach, start, and stop
// methods.  These source-local definitions preserve the exact virtual ABIs.
void h2_pcm_codec_attach(void *self, s_sound_stream *stream)
	__asm__("?attach@c_sound_stream_codec@@UAAXPAUs_sound_stream@@@Z");
void h2_pcm_codec_attach(void *self, s_sound_stream *stream)
{
	(void)self;
	(void)stream;
}

void h2_pcm_codec_start(void *self, s_sound_stream *stream)
	__asm__("?start@c_sound_stream_codec@@UAAXPAUs_sound_stream@@@Z");
void h2_pcm_codec_start(void *self, s_sound_stream *stream)
{
	(void)self;
	(void)stream;
}

void h2_pcm_codec_stop(void *self)
	__asm__("?stop@c_sound_stream_codec@@UAAXXZ");
void h2_pcm_codec_stop(void *self)
{
	(void)self;
}

extern void h2_pcm_codec_mark_cache_usage(
	void *, s_sound_stream *, s_sound_cache_usage *)
	__asm__("?mark_cache_usage@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUs_sound_cache_usage@@@Z");
extern bool h2_pcm_codec_get_packet(void *, s_sound_stream *, XMEDIAPACKET *)
	__asm__("?get_packet@c_pcm_codec@@UAA_NPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z");
extern void h2_pcm_codec_packet_submitted(
	void *, s_sound_stream *, XMEDIAPACKET *)
	__asm__("?packet_submitted@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z");
extern void h2_pcm_codec_packet_failed(
	void *, s_sound_stream *, XMEDIAPACKET *)
	__asm__("?packet_failed@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z");
extern void h2_pcm_codec_chunk_finished(
	void *, s_sound_stream *, s_sound_chunk *, long)
	__asm__("?chunk_finished@c_pcm_codec@@UAAXPAUs_sound_stream@@PAUs_sound_chunk@@J@Z");
extern bool h2_pcm_codec_can_submit(void *, s_sound_stream *, bool)
	__asm__("?can_submit@c_pcm_codec@@UAA_NPAUs_sound_stream@@_N@Z");

typedef void (*s_pcm_codec_method)(void);

// Retail vtable 0x45715c.  The custom internal symbol is deliberately not a
// game ABI; g_47f0f0 and all nine method symbols retain their exact ABIs.
s_pcm_codec_method const h2_pcm_codec_vtable[9] =
{
	reinterpret_cast<s_pcm_codec_method>(h2_pcm_codec_attach),
	reinterpret_cast<s_pcm_codec_method>(h2_pcm_codec_start),
	reinterpret_cast<s_pcm_codec_method>(h2_pcm_codec_mark_cache_usage),
	reinterpret_cast<s_pcm_codec_method>(h2_pcm_codec_get_packet),
	reinterpret_cast<s_pcm_codec_method>(h2_pcm_codec_packet_submitted),
	reinterpret_cast<s_pcm_codec_method>(h2_pcm_codec_packet_failed),
	reinterpret_cast<s_pcm_codec_method>(h2_pcm_codec_chunk_finished),
	reinterpret_cast<s_pcm_codec_method>(h2_pcm_codec_stop),
	reinterpret_cast<s_pcm_codec_method>(h2_pcm_codec_can_submit),
};

struct s_47f0f0
{
	s_pcm_codec_method const *vtable;
};

static_assert(sizeof(s_47f0f0) == 0x04, "PCM codec object size");
static_assert(sizeof(h2_pcm_codec_vtable) == 0x24,
	"PCM codec vtable size");

s_47f0f0 g_47f0f0 = { h2_pcm_codec_vtable };

#endif
