// Exact compact WMA codec control methods isolated from the broader decoder
// implementation.  These preserve object/stream state but do not create or
// emulate an Xbox decoder.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_stream;

class c_wma_codec
{
public:
	virtual void attach(s_sound_stream *stream);
	virtual void stop();
	virtual bool can_submit(s_sound_stream *stream, bool submitted);

	void release_decoder();

	s_sound_stream *m_stream;
	void *m_decoder;
	dword m_position;
	dword m_packet_size;
	byte *m_buffer;
	dword m_packet_status[2];
};

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

#endif
