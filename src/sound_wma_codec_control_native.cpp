// Exact compact WMA codec control methods isolated from the broader decoder
// implementation.  These preserve object/stream state but do not create or
// emulate an Xbox decoder.

#include "unknown_11c920.h"
#include "unknown_218850.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

class c_sound_stream_codec;
struct XMEDIAPACKET;

struct s_sound_stream
{
	byte unknown00;
	char state : 3;
	char channel_count : 3;
	char unknown01_6 : 2;
	char chunk_count : 3;
	char started_count : 3;
	char unknown02_6 : 2;
	byte unknown03[0x19];
	s_sound_chunk *chunks[2];
	void *stream;
	long unknown28;
	long offset;
	c_sound_stream_codec *codec;
};

class c_wma_codec
{
public:
	virtual void attach(s_sound_stream *stream);
	virtual void packet_submitted(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void stop();
	virtual bool can_submit(s_sound_stream *stream, bool submitted);

	void release_decoder();
	dword read(dword offset, dword byte_count, void **data);

	s_sound_stream *m_stream;
	void *m_decoder;
	dword m_position;
	dword m_packet_size;
	byte *m_buffer;
	dword m_packet_status[2];
};

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, chunks) == 0x1c,
	"sound stream chunk-array offset");
static_assert(offsetof(s_sound_stream, offset) == 0x2c,
	"sound stream byte-offset field");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
static_assert(offsetof(c_wma_codec, m_stream) == 0x04,
	"WMA codec stream offset");
static_assert(offsetof(c_wma_codec, m_decoder) == 0x08,
	"WMA codec decoder offset");
#pragma clang diagnostic pop
static_assert(sizeof(c_wma_codec) == 0x20, "WMA codec size");

// @retail 0x2ae230
void c_wma_codec::attach(s_sound_stream *stream)
{
	m_stream = stream;
}

// @retail 0x2ae3d0
void c_wma_codec::packet_submitted(s_sound_stream *stream,
	XMEDIAPACKET *packet)
{
	stream->offset = 0;
}

// @retail 0x2ae3e0
void c_wma_codec::stop()
{
	release_decoder();
}

// @retail 0x2ae3f0
bool c_wma_codec::can_submit(s_sound_stream *stream, bool submitted)
{
	(void)stream;
	return m_decoder && !submitted;
}

// @retail 0x2ae410
dword c_wma_codec::read(dword offset, dword byte_count, void **data)
{
	s_sound_chunk *chunk = m_stream->chunks[m_stream->started_count - 1];
	byte *chunk_data = sound_cache_chunk_get_data(chunk);
	*data = offset <= SOUND_CHUNK_SIZE(chunk) ? chunk_data + offset : NULL;

	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(chunk->cache_index);
	if (entry->reference_count)
		entry->reference_count--;

	dword remaining = SOUND_CHUNK_SIZE(chunk) - offset;
	dword count = remaining > byte_count ? byte_count : remaining;
	return count > 0 ? (remaining > byte_count ? byte_count : remaining) : 0;
}

#endif
