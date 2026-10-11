// Exact dormant looping silence-buffer construction isolated from the broader
// sound-driver source.  All buffer operations remain genuine DirectSound
// boundaries and the backing bytes retain recovered static-pool ownership.

#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

struct IDirectSoundBuffer;

struct s_wave_format
{
	word format_tag;
	word channels;
	dword samples_per_second;
	dword average_bytes_per_second;
	word block_align;
	word bits_per_sample;
	word extra_size;
};

struct s_mixbin_volume_pair
{
	dword mixbin;
	long volume;
};

struct s_mixbins
{
	dword count;
	s_mixbin_volume_pair *pairs;
};

struct s_buffer_description
{
	dword size;
	dword flags;
	dword buffer_bytes;
	s_wave_format *format;
	s_mixbins *mixbins;
	dword input_mixbin;
};

extern dword g_510800_pool_base;
extern long g_510804_pool_size;
extern dword g_510808_pool_checksum;

void function_163ba0(dword *checksum, void const *data, long data_size);

extern "C" long __stdcall DirectSoundCreateBuffer(
	s_buffer_description *description,
	IDirectSoundBuffer **buffer);
extern "C" long __stdcall IDirectSoundBuffer_SetBufferData(
	IDirectSoundBuffer *buffer, void *data, dword byte_count);
extern "C" long __stdcall IDirectSoundBuffer_SetHeadroom(
	IDirectSoundBuffer *buffer, long headroom);
extern "C" long __stdcall IDirectSoundBuffer_SetVolume(
	IDirectSoundBuffer *buffer, long volume);
extern "C" long __stdcall IDirectSoundBuffer_Play(
	IDirectSoundBuffer *buffer, dword reserved1, dword reserved2, dword flags);

static_assert(sizeof(s_wave_format) == 0x14, "natural wave format size");
static_assert(offsetof(s_wave_format, extra_size) == 0x10,
	"natural wave format extra-size offset");
static_assert(sizeof(s_mixbin_volume_pair) == 0x08, "mixbin pair size");
static_assert(sizeof(s_mixbins) == 0x08, "mixbin list size");
static_assert(sizeof(s_buffer_description) == 0x18,
	"DirectSound buffer description size");
static_assert(offsetof(s_buffer_description, format) == 0x0c,
	"buffer description format offset");
static_assert(offsetof(s_buffer_description, input_mixbin) == 0x14,
	"buffer description input-mixbin offset");

// @retail 0x221490
void function_221490(IDirectSoundBuffer **buffer_reference)
{
	static const dword k_silence_buffer_size = 1000;
	static const dword k_play_looping = 1;

	s_buffer_description description = {};
	s_wave_format format = {};
	s_mixbin_volume_pair pair;
	s_mixbins mixbins;
	IDirectSoundBuffer *buffer = NULL;

	format.format_tag = 1;
	format.channels = 1;
	format.samples_per_second = k_silence_buffer_size;
	format.average_bytes_per_second = k_silence_buffer_size;
	format.block_align = 1;
	format.bits_per_sample = 8;
	format.extra_size = 0;

	description.size = sizeof(description);
	description.buffer_bytes = 0;
	description.format = &format;
	description.mixbins = &mixbins;
	description.input_mixbin = 0;

	mixbins.count = 1;
	mixbins.pairs = &pair;
	pair.mixbin = 0xe;
	pair.volume = 0;

	DirectSoundCreateBuffer(&description, &buffer);

	byte *top = reinterpret_cast<byte *>(g_510800_pool_base) +
		g_510804_pool_size;
	byte *memory = reinterpret_cast<byte *>(
		(reinterpret_cast<dword>(top) + 3) & ~3);
	long aligned_size = (memory - top) + k_silence_buffer_size;
	g_510804_pool_size += aligned_size;
	function_163ba0(&g_510808_pool_checksum, &aligned_size,
		sizeof(aligned_size));

	memset(memory, 0, k_silence_buffer_size);
	IDirectSoundBuffer_SetBufferData(buffer, memory, k_silence_buffer_size);
	IDirectSoundBuffer_SetHeadroom(buffer, 0);
	IDirectSoundBuffer_SetVolume(buffer, -10000);
	IDirectSoundBuffer_Play(buffer, 0, 0, k_play_looping);

	*buffer_reference = buffer;
}

#endif
