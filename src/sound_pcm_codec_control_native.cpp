// Exact compact PCM codec submission policy isolated from the broader Xbox
// DirectSound stream implementation.  This reads genuine queue state only;
// it does not fabricate a packet or stream progress.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_stream
{
	byte unknown00;
	char state : 3;
	char channel_count : 3;
	char unknown01_6 : 2;
	char chunk_count : 3;
	char started_count : 3;
	char unknown02_6 : 2;
	byte unknown03[0x31];
};

class c_pcm_codec
{
public:
	virtual bool can_submit(s_sound_stream *stream, bool submitted);
};

static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(sizeof(c_pcm_codec) == 0x04, "PCM codec size");

// @retail 0x2aeb00
bool c_pcm_codec::can_submit(s_sound_stream *stream, bool submitted)
{
	(void)submitted;
	return stream->chunk_count > stream->started_count;
}

#endif
