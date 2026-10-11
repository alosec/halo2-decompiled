// Exact process-lifetime WMA stream codec object, its nine-slot retail
// virtual interface, and its registered staged initialization/disposal.
// Direct initialization avoids a host CRT constructor.  Decoder construction,
// processing, and release remain genuine Xbox media backend operations.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_stream;
struct s_sound_cache_usage;
struct s_sound_chunk;
struct XMEDIAPACKET;

// The retail WMA table shares three empty base-codec methods.  Keep their
// exact virtual ABIs while preserving the recovered no-op behavior.
void h2_wma_codec_mark_cache_usage(
	void *self, s_sound_stream *stream, s_sound_cache_usage *usage)
	__asm__("?mark_cache_usage@c_sound_stream_codec@@UAAXPAUs_sound_stream@@PAUs_sound_cache_usage@@@Z");
void h2_wma_codec_mark_cache_usage(
	void *self, s_sound_stream *stream, s_sound_cache_usage *usage)
{
	(void)self;
	(void)stream;
	(void)usage;
}

void h2_wma_codec_packet_failed(
	void *self, s_sound_stream *stream, XMEDIAPACKET *packet)
	__asm__("?packet_failed@c_sound_stream_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z");
void h2_wma_codec_packet_failed(
	void *self, s_sound_stream *stream, XMEDIAPACKET *packet)
{
	(void)self;
	(void)stream;
	(void)packet;
}

void h2_wma_codec_chunk_finished(
	void *self, s_sound_stream *stream, s_sound_chunk *chunk, long unused)
	__asm__("?chunk_finished@c_sound_stream_codec@@UAAXPAUs_sound_stream@@PAUs_sound_chunk@@J@Z");
void h2_wma_codec_chunk_finished(
	void *self, s_sound_stream *stream, s_sound_chunk *chunk, long unused)
{
	(void)self;
	(void)stream;
	(void)chunk;
	(void)unused;
}

extern void h2_wma_codec_attach(void *, s_sound_stream *)
	__asm__("?attach@c_wma_codec@@UAAXPAUs_sound_stream@@@Z");
extern void h2_wma_codec_start(void *, s_sound_stream *)
	__asm__("?start@c_wma_codec@@UAAXPAUs_sound_stream@@@Z");
extern bool h2_wma_codec_get_packet(
	void *, s_sound_stream *, XMEDIAPACKET *)
	__asm__("?get_packet@c_wma_codec@@UAA_NPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z");
extern void h2_wma_codec_packet_submitted(
	void *, s_sound_stream *, XMEDIAPACKET *)
	__asm__("?packet_submitted@c_wma_codec@@UAAXPAUs_sound_stream@@PAUXMEDIAPACKET@@@Z");
extern void h2_wma_codec_stop(void *)
	__asm__("?stop@c_wma_codec@@UAAXXZ");
extern bool h2_wma_codec_can_submit(void *, s_sound_stream *, bool)
	__asm__("?can_submit@c_wma_codec@@UAA_NPAUs_sound_stream@@_N@Z");

typedef void (*s_wma_codec_method)(void);

// Retail vtable 0x45711c.
s_wma_codec_method const h2_wma_codec_vtable[9] =
{
	reinterpret_cast<s_wma_codec_method>(h2_wma_codec_attach),
	reinterpret_cast<s_wma_codec_method>(h2_wma_codec_start),
	reinterpret_cast<s_wma_codec_method>(h2_wma_codec_mark_cache_usage),
	reinterpret_cast<s_wma_codec_method>(h2_wma_codec_get_packet),
	reinterpret_cast<s_wma_codec_method>(h2_wma_codec_packet_submitted),
	reinterpret_cast<s_wma_codec_method>(h2_wma_codec_packet_failed),
	reinterpret_cast<s_wma_codec_method>(h2_wma_codec_chunk_finished),
	reinterpret_cast<s_wma_codec_method>(h2_wma_codec_stop),
	reinterpret_cast<s_wma_codec_method>(h2_wma_codec_can_submit),
};

struct s_47f0d0
{
	s_wma_codec_method const *vtable;
	s_sound_stream *stream;
	void *decoder;
	dword position;
	dword packet_size;
	byte *buffer;
	dword packet_status[2];
};

static_assert(sizeof(s_47f0d0) == 0x20, "WMA codec object size");
static_assert(offsetof(s_47f0d0, packet_size) == 0x10,
	"WMA codec packet-size alias offset");
static_assert(offsetof(s_47f0d0, buffer) == 0x14,
	"WMA codec buffer offset");
static_assert(offsetof(s_47f0d0, packet_status) == 0x18,
	"WMA codec status offset");
static_assert(sizeof(h2_wma_codec_vtable) == 0x24,
	"WMA codec vtable size");

s_47f0d0 g_47f0d0 =
{
	h2_wma_codec_vtable,
	NULL,
	NULL,
	0,
	0,
	NULL,
	{ 0, 0 },
};

void function_2ae1d0(s_47f0d0 *codec);

// Registered in retail callback table 0x453c00.
// @retail 0x21ebc0
void __stdcall function_21ebc0(long stage)
{
	if (stage == 1)
		function_2ae1d0(&g_47f0d0);
}

// The packet buffer belongs to the staged physical arena; disposal clears
// publication only and does not pretend to free that arena allocation.
// @retail 0x21ebe0
void function_21ebe0(void)
{
	if (g_47f0d0.buffer)
		g_47f0d0.buffer = NULL;
}

#endif
