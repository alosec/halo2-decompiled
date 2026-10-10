// Exact WMA decoder release policy isolated from the broader Xbox stream
// codec implementation.  The decoder and stream remain genuine mutable
// objects; this does not fabricate a codec or replace its Release operation.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_chunk;
struct s_sound_stream;

struct s_sound_stream_release_view
{
	byte unknown00;
	char state : 3;
	char channel_count : 3;
	char unknown01_6 : 2;
	char chunk_count : 3;
	char started_count : 3;
	char unknown02_6 : 2;
	byte unknown03_0 : 2;
	byte flushing : 1;
	byte unknown03_3 : 1;
	byte unknown03_4 : 4;
	byte unknown04[0x18];
	s_sound_chunk *chunks[2];
	byte unknown24[0x10];
};

struct s_xmedia_object_release_view
{
	void **vtable;
};

class c_wma_codec
{
public:
	void release_decoder();

	void *m_vtable;
	s_sound_stream_release_view *m_stream;
	s_xmedia_object_release_view *m_decoder;
	dword m_position;
	dword m_packet_size;
	byte *m_buffer;
	dword m_packet_status[2];
};

void sound_stream_release_chunk(
	s_sound_stream *stream,
	s_sound_chunk *chunk);

static_assert(sizeof(s_sound_stream_release_view) == 0x34,
	"sound stream size");
static_assert(offsetof(s_sound_stream_release_view, chunks) == 0x1c,
	"sound stream chunk array offset");
static_assert(offsetof(c_wma_codec, m_stream) == 0x04,
	"WMA codec stream offset");
static_assert(offsetof(c_wma_codec, m_decoder) == 0x08,
	"WMA codec decoder offset");
static_assert(sizeof(c_wma_codec) == 0x20,
	"WMA codec size");

// @retail 0x2ae170
void c_wma_codec::release_decoder()
{
	if (m_decoder)
	{
		typedef dword (__stdcall *release_method)(
			s_xmedia_object_release_view *object);
		release_method release =
			reinterpret_cast<release_method>(m_decoder->vtable[2]);
		release(m_decoder);
		m_decoder = NULL;
		m_stream->started_count = m_stream->chunk_count;
		while (m_stream->chunk_count > 0)
			sound_stream_release_chunk(
				reinterpret_cast<s_sound_stream *>(m_stream),
				m_stream->chunks[0]);
	}
}

#endif
