// Exact dormant DirectSound stream envelope policy.  The checked 0x28-byte
// Xbox descriptor and its public envelope identifiers were confirmed against
// the matching retail function; submission remains a genuine backend call.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_stream
{
	byte unknown00[0x24];
	void *stream;
	byte unknown28[0x0c];
};

struct s_direct_sound_envelope_desc
{
	dword eg;
	dword mode;
	dword delay;
	dword attack;
	dword hold;
	dword decay;
	dword release;
	dword sustain;
	dword unknown20;
	dword unknown24;
};

extern "C" long __stdcall IDirectSoundStream_SetEG(void *stream,
	s_direct_sound_envelope_desc *envelope);

static const dword k_direct_sound_envelope_amplitude = 1;
static const dword k_direct_sound_envelope_mode_attack = 2;

static_assert(sizeof(s_direct_sound_envelope_desc) == 0x28,
	"Xbox envelope descriptor size");
static_assert(offsetof(s_direct_sound_envelope_desc, attack) == 0x0c,
	"Xbox envelope attack offset");
static_assert(offsetof(s_direct_sound_envelope_desc, release) == 0x18,
	"Xbox envelope release offset");
static_assert(offsetof(s_direct_sound_envelope_desc, sustain) == 0x1c,
	"Xbox envelope sustain offset");
static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");

// @retail 0x2ae7a0
void sound_stream_set_envelope(s_sound_stream *stream, real attack, real release)
{
	s_direct_sound_envelope_desc envelope = {};

	envelope.eg = k_direct_sound_envelope_amplitude;
	envelope.mode = k_direct_sound_envelope_mode_attack;
	envelope.attack = (dword)(attack * 93.75f);
	envelope.release = (dword)(release * 93.75f);
	envelope.sustain = 0xff;
	IDirectSoundStream_SetEG(stream->stream, &envelope);
}

#endif
