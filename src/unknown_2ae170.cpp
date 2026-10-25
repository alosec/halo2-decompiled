// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2AE170.CPP: streamed sounds: the WMA and PCM stream codecs, the
   DirectSound stream wrappers that feed them cached sound chunks */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "globals.h"
#include "unknown_01e930.h"
#include "unknown_053310.h"
#include "unknown_218850.h"
#include "unknown_2ae170.h"
#include "unknown_21e230.h"

/* the sound globals (unknown_01e930.h), viewed for their DirectSound object */
struct s_sound_globals_view
{
	byte unknown0000[0x2ab0];
	IDirectSound *direct_sound;
};

#define WMA_PACKET_SIZE 0x10000
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define TEST_FLAG(flags, bit) (((flags) >> (bit)) & 1)

class c_wma_codec : public c_sound_stream_codec
{
public:
	virtual void attach(s_sound_stream *stream);
	virtual void start(s_sound_stream *stream);
	virtual bool get_packet(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void packet_submitted(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void stop();
	virtual bool can_submit(s_sound_stream *stream, bool submitted);

	void release_decoder();
	dword read(dword offset, dword byte_count, void **data);

	s_sound_stream *m_stream;
	XMediaObject *m_decoder;
	dword m_position;
	dword m_packet_size;
	byte *m_buffer;
	dword m_packet_status[2];
};

class c_pcm_codec : public c_sound_stream_codec
{
public:
	virtual void mark_cache_usage(s_sound_stream *stream, s_sound_cache_usage *usage);
	virtual bool get_packet(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void packet_submitted(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void packet_failed(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void chunk_finished(s_sound_stream *stream, s_sound_chunk *chunk, long unused);
	virtual bool can_submit(s_sound_stream *stream, bool submitted);
};

PRIVATE DWORD CALLBACK wma_codec_read_callback(LPVOID context, DWORD offset, DWORD byte_count, LPVOID *data);

// @retail 0x2ae170
void c_wma_codec::release_decoder()
{
	if (m_decoder)
	{
		m_decoder->Release();
		m_decoder = NULL;
		m_stream->started_count = m_stream->chunk_count;
		while (m_stream->chunk_count > 0)
		{
			sound_stream_release_chunk(m_stream, m_stream->chunks[0]);
		}
	}
}

struct s_47f0d0;

// @retail 0x2ae1d0
void function_2ae1d0(s_47f0d0 *codec_view)
{
	c_wma_codec *codec = (c_wma_codec *)codec_view;
	long buffer;

	PHYSICAL_MEMORY_ALLOCATE(buffer, 2 * WMA_PACKET_SIZE);
	codec->m_buffer = (byte *)buffer;
	long packet = 0;
	do
	{
		codec->m_packet_status[packet] = 0;
		++packet;
	} while (packet < 2);
}

// @retail 0x2ae230
void c_wma_codec::attach(s_sound_stream *stream)
{
	m_stream = stream;
}

// @retail 0x2ae240
void c_wma_codec::start(s_sound_stream *stream)
{
	WAVEFORMATEX format;

	release_decoder();
	stream->started_count++;
	WmaCreateInMemoryDecoder(wma_codec_read_callback, this, 1, &format, &m_decoder);
}

// @retail 0x2ae290
bool c_wma_codec::get_packet(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	bool result = false;
	long packet_index = NONE;
	long index;

	for (index = 0; index < 2; index++)
	{
		if (m_packet_status[index] != XMEDIAPACKET_STATUS_PENDING)
		{
			packet_index = index;
			break;
		}
	}

	if (packet_index != NONE)
	{
		dword total = 0;
		byte *buffer = m_buffer + (packet_index << 16);

		while (total < m_packet_size)
		{
			WAVEFORMATEX format;
			XMEDIAPACKET output;
			DWORD completed;

			output.pvBuffer = buffer + total;
			output.dwMaxSize = m_packet_size - total;
			output.pdwCompletedSize = &completed;
			output.pdwStatus = NULL;
			output.hCompletionEvent = NULL;
			output.prtTimestamp = NULL;
			if (FAILED(m_decoder->Process(NULL, &output)))
			{
				break;
			}

			m_position += completed;
			total += completed;
			if (completed < output.dwMaxSize)
			{
				release_decoder();
				m_position = 0;
			}

			if (stream->started_count >= stream->chunk_count || stream->started_count != 0)
			{
				break;
			}

			stream->started_count++;
			WmaCreateInMemoryDecoder(wma_codec_read_callback, this, 1, &format, &m_decoder);
		}

		result = total > 0;
		if (result)
		{
			packet->pvBuffer = buffer;
			packet->dwMaxSize = total;
			packet->pdwCompletedSize = NULL;
			packet->pdwStatus = &m_packet_status[packet_index];
			packet->hCompletionEvent = NULL;
			packet->prtTimestamp = NULL;
		}
	}

	return result;
}

// @retail 0x2ae3d0
void c_wma_codec::packet_submitted(s_sound_stream *stream, XMEDIAPACKET *packet)
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
	{
		entry->reference_count--;
	}

	dword remaining = SOUND_CHUNK_SIZE(chunk) - offset;
	dword count = remaining > byte_count ? byte_count : remaining;
	return count > 0 ? (remaining > byte_count ? byte_count : remaining) : 0;
}

// @retail 0x2ae490
PRIVATE DWORD CALLBACK wma_codec_read_callback(LPVOID context, DWORD offset, DWORD byte_count, LPVOID *data)
{
	return ((c_wma_codec *)context)->read(offset, byte_count, data);
}

// @retail 0x2ae500
void sound_stream_add_chunk(s_sound_stream *stream, s_sound_chunk *chunk)
{
	stream->unknown03_3 = 0;
	switch (stream->state)
	{
	case 0:
	case 1:
		stream->state = 2;
		break;
	case 2:
		stream->state = 3;
		break;
	case 3:
		goto add_chunk;
	}

add_chunk:
	if (stream->chunk_count < k_maximum_stream_chunks)
	{
		function_218850(NONE, chunk, 4);
		stream->chunks[stream->chunk_count] = chunk;
		stream->chunk_count++;
	}

	if (stream->started_count == 0)
	{
		stream->codec->start(stream);
	}
}

// @retail 0x2ae590
void sound_stream_release_chunk(s_sound_stream *stream, s_sound_chunk *chunk)
{
	stream->started_count--;
	stream->chunk_count--;
	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(chunk->cache_index);
	entry->lock_count--;
	memmove(&stream->chunks[0], &stream->chunks[1], stream->chunk_count * sizeof(s_sound_chunk *));
	memset(&stream->chunks[stream->chunk_count], 0, (k_maximum_stream_chunks - stream->chunk_count) * sizeof(s_sound_chunk *));

	switch (stream->chunk_count)
	{
	case 0:
		stream->state = 1;
		break;
	case 1:
		stream->state = 2;
		break;
	}
}

// @retail 0x2ae660
bool sound_stream_create(s_sound_stream *stream, c_sound_stream_codec *codec, DSSTREAMDESC *description, short channel_count)
{
	stream->codec = codec;
	codec->attach(stream);
	description->dwFlags |= 0x20000000;
	IDirectSound *direct_sound = ((s_sound_globals_view *)g_51ebe4)->direct_sound;
	bool success = false;
	if (SUCCEEDED(IDirectSound_CreateSoundStream(direct_sound, description, &stream->stream, NULL)))
	{
		success = true;
	}
	IDirectSoundStream_SetHeadroom(stream->stream, 0);
	stream->channel_count = channel_count == 2 ? 1 : channel_count;
	stream->chunk_count = 0;
	stream->started_count = 0;
	stream->unknown28 = NONE;
	memset(stream->chunks, 0, sizeof(stream->chunks));

	return success;
}

static inline void sound_stream_halt(s_sound_stream *stream)
{
	if (stream->state == 1)
	{
		DWORD status;
		stream->stream->GetStatus(&status);
		stream->stream->Flush();
		stream->codec->stop();
		stream->state = 0;
		sound_stream_release_chunks(stream);
		stream->flushing = 0;
	}
}

// @retail 0x2ae6f0
void sound_stream_stop(s_sound_stream *stream)
{
	sound_stream_halt(stream);
	if (stream->started_count != 0)
	{
		stream->started_count = 0;
	}
	if (stream->chunk_count != 0)
	{
		stream->chunk_count = 0;
	}
}

// @retail 0x2ae750
void sound_stream_reset(s_sound_stream *stream)
{
	sound_stream_set_envelope(stream, 0.0f, 0.25f);
	sound_stream_halt(stream);
	sound_stream_flush(stream);
}

// @retail 0x2ae7a0
void sound_stream_set_envelope(s_sound_stream *stream, real attack, real release)
{
	DSENVELOPEDESC envelope = { 0 };

	envelope.dwEG = DSEG_AMPLITUDE;
	envelope.dwMode = DSEG_MODE_ATTACK;
	envelope.dwAttack = (DWORD)(attack * 93.75f);
	envelope.dwRelease = (DWORD)(release * 93.75f);
	envelope.dwSustain = 0xff;
	IDirectSoundStream_SetEG(stream->stream, &envelope);
}

// @retail 0x2ae820
void sound_stream_update(s_sound_stream *stream)
{
	/* Retail keeps both flags in addressable stack slots. */
	bool submitted = false;
	bool const *submitted_reference = &submitted;
	bool success;
	bool ready;
	bool const *ready_reference = &ready;

	do
	{
		DWORD status;
		XMEDIAPACKET packet;

		success = false;
		if (((1 << stream->state) & ((1 << 2) | (1 << 3))) &&
			SUCCEEDED(stream->stream->GetStatus(&status)))
		{
			ready = (status & DSSTREAMSTATUS_READY) != 0;
			if (*ready_reference &&
				stream->codec->can_submit(stream, *submitted_reference) &&
				stream->codec->get_packet(stream, &packet))
			{
				success = SUCCEEDED(stream->stream->Process(&packet, NULL));
				if (success)
					stream->codec->packet_submitted(stream, &packet);
				else
					stream->codec->packet_failed(stream, &packet);
				if (success)
					submitted = true;
			}
		}
	}
	while (success);

	if (submitted)
		stream->stream->Discontinuity();
}

// @retail 0x2ae8e0
void sound_stream_release_chunks(s_sound_stream *stream)
{
	while (stream->chunk_count > 0)
	{
		s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(stream->chunks[stream->chunk_count - 1]->cache_index);
		entry->lock_count--;
		stream->chunk_count--;
	}
}

static inline void bit_vector_or_flag(dword *vector, long index)
{
	vector[index >> 5] |= 1 << (index & 0x1f);
}

// @retail 0x2ae940
void c_pcm_codec::mark_cache_usage(s_sound_stream *stream, s_sound_cache_usage *usage)
{
	for (long index = 0; index < stream->chunk_count; index++)
	{
		if (index < stream->started_count)
		{
			s_sound_chunk *chunk = stream->chunks[index];
			usage->counts[chunk->cache_index & 0xffff]++;
			bit_vector_or_flag(usage->flags, chunk->cache_index & 0xffff);
		}
	}
}

// @retail 0x2ae9d0
bool c_pcm_codec::get_packet(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	s_sound_chunk *chunk = stream->chunks[stream->started_count];
	byte *data = sound_cache_chunk_get_data(chunk);

	long offset = MIN(stream->offset, MAX((long)SOUND_CHUNK_SIZE(chunk) - 0x48, 0));

	packet->pdwCompletedSize = NULL;
	packet->pdwStatus = NULL;
	packet->prtTimestamp = NULL;
	packet->pvBuffer = data + offset;
	packet->hCompletionEvent = (HANDLE)chunk;
	packet->dwMaxSize = SOUND_CHUNK_SIZE(chunk) - offset;

	return true;
}

// @retail 0x2aea50
void c_pcm_codec::packet_submitted(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	stream->started_count++;
	stream->offset = 0;
}

// @retail 0x2aea80
void c_pcm_codec::packet_failed(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(stream->chunks[stream->started_count]->cache_index);
	if (entry->reference_count)
	{
		entry->reference_count--;
	}
}

// @retail 0x2aeac0
void c_pcm_codec::chunk_finished(s_sound_stream *stream, s_sound_chunk *chunk, long unused)
{
	sound_stream_release_chunk(stream, chunk);

	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(chunk->cache_index);
	if (entry->reference_count)
	{
		entry->reference_count--;
	}
}

// @retail 0x2aeb00
bool c_pcm_codec::can_submit(s_sound_stream *stream, bool submitted)
{
	return stream->chunk_count > stream->started_count;
}

/* ---- sound effects sends (vtable 0x457140, instance 0x47f088) ---- */

/* the effect send buffers, and the mix bins of the three sends that bypass
   them */
struct s_sound_effect_buffers
{
	long mixbin00;
	long mixbin04;
	long mixbin08;
	IDirectSoundBuffer *buffer0c;
	IDirectSoundBuffer *buffer10;
	IDirectSoundBuffer *buffer14;
	IDirectSoundBuffer *buffer18;
	IDirectSoundBuffer *buffer1c;
	IDirectSoundBuffer *buffer20;
};

/* send levels in decibels */
struct s_sound_effect_settings
{
	real level00;
	real level04;
	real level08;
	real level0c;
	real level10;
	real gain;
	real unknown18;
	real level1c;
	real level20;
	real level24;
	real level28;
	real level2c;
	byte unknown30[0x10];
};

struct s_sound_i3dl2_parameters
{
	DSI3DL2BUFFER i3dl2;
	long cutoff;
	real gain;
};

struct s_sound_send_parameters
{
	union
	{
		dword flags;
		struct
		{
			dword flag0 : 1;
			dword : 2;
			dword flag3 : 1;
			dword flag4 : 1;
			dword : 27;
		};
	};
	real level;
	real level_offset;
	real gain0c;
	real gain10;
	byte unknown14[4];
	real left_gain;
	real right_gain;
	real rear_left_gain;
	real rear_right_gain;
};

struct s_sound_mixbin_source
{
	byte unknown00[0x12];
	short mixbin;
	byte unknown14[0x1c];
	dword flags;
	word speaker_mode;
};

class c_sound_effects
{
public:
	virtual void initialize(s_sound_effect_settings *settings, s_sound_effect_buffers *buffers, bool rear);
	virtual void v1() {}
	virtual void set_i3dl2(s_sound_i3dl2_parameters *parameters, long unused);
	virtual void add_effect_sends(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins);
	virtual void add_send(long mixbin, long mode, real gain, s_mixbin_settings *mixbins);
	virtual void add_source_mixbins(s_sound_mixbin_source *source, bool rear, long mode, s_mixbin_settings *mixbins);
	virtual void add_sends(s_sound_send_parameters *parameters, long unused, long mode, s_mixbin_settings *mixbins);

	void add_effect_sends_direct(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins);
	void add_effect_sends_split(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins);

	s_sound_effect_settings m_settings;
	s_sound_effect_buffers *m_buffers;
};


/* Retail stream mixing expands the mix-bin append operation here. */
__forceinline void sound_stream_append_mixbin(s_mixbin_settings *settings, long mixbin, real decibels)
{
	settings->pairs[settings->mixbins.dwMixBinCount].dwMixBin = mixbin;
	settings->pairs[settings->mixbins.dwMixBinCount].lVolume = sound_decibels_to_volume(decibels);
	settings->mixbins.dwMixBinCount++;
}

// score 578 (was 583): frame 0xd8 versus 0x94; constant conversions still coalesce.
// First-buffer input 578; late workspaces 583; muted-send loop 576;
// native rear predicate 584; workspace role names 583; input plus loop 565
// (not kept: the loop's index formula only reproduces 6, 8, 7, 9).
// score 552 (was 578): frame 0xd8 versus 0x94; repeated constant conversions still coalesce.
// Native settings copy 574; last-two scopes 616; direct routing table 552; pair table 553;
// constant append helper 552; variable append helper 552; last workspace scope 552.
// j91: frame 0xd8 versus 0x94 and repeated constant conversions remain; shared routing storage reaches 0xa4 but loses.
// front level table 613; back level table 566; both level tables 635; shared level table 635; shared routing
// workspace 647
// score 463 (was 552): Frame 0xdc versus 0x94 and conversion reuse remain; first/second/both rear calls 579/532/583, first/second table pairs 542/509, second pair with call 491, both pairs 480, both pairs with call 463 (plain).
// score 463 (was 463): first/third scopes reach frame 0x94 but score 490; pointer traversal 463;
// byte table 462; byte counters 458 (instruction sequence worsens); common counter 463;
// recovered-frame front/rear snapshots 546. Original loops and native workspaces kept.
// @retail 0x2aeb20
void c_sound_effects::initialize(s_sound_effect_settings *settings, s_sound_effect_buffers *buffers, bool rear)
{
	static long const muted_mixbins[4] = {6, 8, 7, 9};
	s_mixbin_settings mixbins;
	s_mixbin_settings second;
	s_mixbin_settings third;

	m_buffers = buffers;
	m_settings = *settings;

	sound_mixbins_initialize(&mixbins);
	c_sound_effects::add_send(6, 0, m_settings.level00, &mixbins);
	if (rear)
		c_sound_effects::add_send(8, 0, m_settings.level04, &mixbins);
	else
		c_sound_effects::add_send(8, 0, -64.0f, &mixbins);
	for (long first_pair_index = 2; first_pair_index < 4; ++first_pair_index)
		c_sound_effects::add_send(muted_mixbins[first_pair_index], 0, -64.0f, &mixbins);
	IDirectSoundBuffer_SetMixBinVolumes(buffers->buffer0c, &mixbins.mixbins);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer0c, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer18, -6400);

	sound_mixbins_initialize(&second);
	for (long second_pair_index = 0; second_pair_index < 2; ++second_pair_index)
		c_sound_effects::add_send(muted_mixbins[second_pair_index], 0, -64.0f, &second);
	c_sound_effects::add_send(7, 0, m_settings.level00, &second);
	c_sound_effects::add_send(9, 0, rear ? m_settings.level04 : -64.0f, &second);
	IDirectSoundBuffer_SetMixBinVolumes(m_buffers->buffer10, &second.mixbins);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer10, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer1c, -6400);

	sound_mixbins_initialize(&third);
	for (long mixbin_index = 0; mixbin_index < 4; ++mixbin_index)
		c_sound_effects::add_send(muted_mixbins[mixbin_index], 0, -64.0f, &third);
	c_sound_effects::add_send(10, 0, 0.0f, &third);
	IDirectSoundBuffer_SetMixBinVolumes(m_buffers->buffer20, &third.mixbins);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer20, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer14, -6400);
}

/* the level of the effect sends: none in mode 0 */
inline real const &sound_effects_send_level(c_sound_effects *effects, s_sound_send_parameters *parameters, long mode)
{
	return mode == 1 ? (TEST_FLAG(parameters->flags, 6) ? g_44f70c : effects->m_settings.level2c) : effects->m_settings.level10;
}

// score 527 (was 532): frame 4 versus 12; gain spills still differ.
// Base snapshot 514 (caller lost); bin snapshots 511; both 484; direct
// completed-path return 532; offset reads 554; retail-order base 514;
// base/bin snapshots 484; base/primary snapshots 504 (caller losses).
// global optimization off 364 (caller lost); inline depth one 532.
// short destinations 531; boolean routing extraction 527.
// scalar home names 532; selector bitfield 527 (split 276).
// both named flag fields 524 (+3; split 276); routing bitfield 527.
// score 515 (was 527): frame four versus twelve and scalar/SSE homes remain;
// frame pointer 559 (dispatcher lost); ordinary route first 532; repeated-send loop 541;
// front setting snapshot 521; rear setting snapshot 520; both 515.
// score 507 (was 515): frame four versus twelve and gain spill/SSE homes remain;
// per-use primary selection 467 (dispatcher lost); byte destinations 507;
// reversed mode cases 515; named special gains 515; conditional mode arms 532.
// score 460 (was 507): frame twelve agrees; selected-level and gain SSE spills still differ.
// ordinary gain snapshots 460; late secondary setting 517; native send copy 522 (dispatcher lost);
// alternate ordinary route first 507; special input snapshot 464; combined snapshots 455 (dispatcher lost).
// @retail 0x2aefe0
void c_sound_effects::add_effect_sends_direct(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins)
{
	if ((bool)((parameters->flags >> 2) & 1))
	{
		bool alternate = TEST_FLAG(parameters->flags, 3) != 0;
		byte left = alternate ? 0 : 6;
		byte right = alternate ? 1 : 7;
		real const &level = rear ? m_settings.level1c : m_settings.level24;
		real level2 = rear ? m_settings.level20 : m_settings.level28;
		sound_stream_append_mixbin(mixbins, left, level + parameters->level);
		sound_stream_append_mixbin(mixbins, right, parameters->level + level);
		add_send(2, mode, level2 + parameters->level, mixbins);
		add_send(10, mode, parameters->level_offset, mixbins);
	}
	else
	{
		real send_level = sound_effects_send_level(this, parameters, mode);
		real offset = parameters->level_offset + parameters->level;
		real left_gain = parameters->gain0c;
		real right_gain = parameters->gain10;
		if (!TEST_FIELD_BIT(parameters->flag3))
		{
			sound_stream_append_mixbin(mixbins, m_buffers->mixbin00, left_gain + (send_level + parameters->level));
			sound_stream_append_mixbin(mixbins, m_buffers->mixbin04, right_gain + (parameters->level + send_level));
			long mixbin08 = m_buffers->mixbin08;
			switch (mode)
			{
			case 0:
				sound_mixbins_add(mixbins, mixbin08, offset);
				break;
			case 1:
				sound_mixbins_add(mixbins, mixbin08, offset + m_settings.gain);
				sound_mixbins_add(mixbins, mixbin08, offset + m_settings.gain);
				break;
			}
		}
		else
		{
			real front_level = m_settings.level00;
			if (front_level > -64.0f)
			{
				sound_mixbins_add(mixbins, 0, left_gain + (front_level + (parameters->level + send_level)));
				sound_mixbins_add(mixbins, 1, right_gain + (front_level + (parameters->level + send_level)));
			}
			if (rear)
			{
				real rear_level = m_settings.level04;
				if (rear_level > -64.0f)
				{
					sound_mixbins_add(mixbins, 4, left_gain + (parameters->level + send_level + rear_level));
					sound_mixbins_add(mixbins, 5, right_gain + (parameters->level + send_level + rear_level));
				}
			}
			switch (mode)
			{
			case 0:
				sound_mixbins_add(mixbins, 10, offset);
				break;
			case 1:
				sound_mixbins_add(mixbins, 10, left_gain + offset);
				sound_mixbins_add(mixbins, 10, right_gain + offset);
				break;
			}
		}
	}
}

// score 274 (was 283): frame 0x1c versus 0x18; mode occupies EBP.
// Front pair loop 308; rear pair loop 323; both 363; all short/word
// selectors 274; byte selectors 275; only spilled short/word selectors 280.
// score 274 (was 274): frame 0x1c versus 0x18 and input-level/mode homes remain.
// Shared append front 392, rear 394, tail 430 (dispatcher lost in all three);
// whole-function depth zero 673 (dispatcher lost), depth one 274.
// score 270 (was 274): frame and input-level/mode homes remain;
// front setting snapshot 270; rear 279; both 275; frame pointer 312;
// global optimization off 281 (dispatcher lost).
// score 264 (was 270): frame 0x1c versus 0x18 and input-level/mode homes remain;
// word routing predicate 264; named routing reads 270; native-word home names 270;
// front field reference 267 and both field references 267 (three-point gains rejected).
// @retail 0x2af450
void c_sound_effects::add_effect_sends_split(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins)
{
	real send_level = sound_effects_send_level(this, parameters, mode);
	real offset = parameters->level_offset;
	word alternate = TEST_FLAG(parameters->flags, 3) != 0;
	word left = alternate ? 0 : 6;
	volatile word right = alternate ? 1 : 7;
	volatile word rear_left = alternate ? 4 : 8;
	volatile word rear_right = alternate ? 5 : 9;
	real level = parameters->level;

	real front_level = m_settings.level08;
	if (front_level > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, left, parameters->gain0c + (front_level + (send_level + level)));
		sound_stream_append_mixbin(mixbins, right, parameters->gain10 + (front_level + (level + send_level)));
	}
	if (rear && m_settings.level0c > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, rear_left, parameters->gain0c + (m_settings.level0c + (level + send_level)));
		sound_stream_append_mixbin(mixbins, rear_right, parameters->gain10 + (m_settings.level0c + (level + send_level)));
	}
	switch (mode)
	{
	case 0:
		sound_stream_append_mixbin(mixbins, 10, offset);
		break;
	case 1:
		sound_stream_append_mixbin(mixbins, 10, parameters->gain0c + offset);
		sound_stream_append_mixbin(mixbins, 10, parameters->gain10 + offset);
		break;
	}
}

// @retail 0x2af7b0
void c_sound_effects::add_effect_sends(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins)
{
	if (TEST_FIELD_BIT(parameters->flag0))
	{
		if (!TEST_FIELD_BIT(parameters->flag4))
			add_effect_sends_direct(parameters, rear, mode, mixbins);
		else
			add_effect_sends_split(parameters, rear, mode, mixbins);
	}
}

// @retail 0x2aef50
void c_sound_effects::set_i3dl2(s_sound_i3dl2_parameters *parameters, long unused)
{
	DSFILTERDESC filter;

	IDirectSoundBuffer_SetI3DL2Source(m_buffers->buffer20, &parameters->i3dl2, DS3D_DEFERRED);
	filter.dwMode = DSFILTER_MODE_DLS2;
	filter.dwQCoefficient = 0;
	filter.adwCoefficients[0] = sound_filter_frequency_coefficient((real)parameters->cutoff);
	filter.adwCoefficients[1] = sound_filter_gain_coefficient(parameters->gain);
	filter.adwCoefficients[2] = 0;
	filter.adwCoefficients[3] = 0;
	IDirectSoundBuffer_SetFilter(m_buffers->buffer0c, &filter);
	IDirectSoundBuffer_SetFilter(m_buffers->buffer10, &filter);
}

// score 55 (was 55): our body is 12 bytes shorter: retail adds the gain with
// SSE (the gain's bits copied through ebx into a stack temp) while our
// union pun makes MSVC use x87; five more rewrites of the pun scored 55 or
// worse.
// score 55 (was 55): retail inlines the existing out-of-line mix bin append (its
// count++ is reused by the second send), ours the local copy. The existing
// append called directly stays out of line 252, 238; plain sums 102; one
// adjusted level 167; the gain parameter adjusted in place 167.
// @retail 0x2af800
__forceinline void c_sound_effects::add_send(long mixbin, long mode, real gain, s_mixbin_settings *mixbins)
{
	switch (mode)
	{
	case 0:
		sound_stream_append_mixbin(mixbins, mixbin, gain);
		break;
	case 1:
	{
		union s_send_level
		{
			real value;
			long bits;
		};
		long gain_bits = *(long *)&gain;
		s_send_level first;
		first.bits = gain_bits;
		sound_stream_append_mixbin(mixbins, mixbin, m_settings.gain + first.value);
		s_send_level second;
		second.bits = gain_bits;
		sound_stream_append_mixbin(mixbins, mixbin, m_settings.gain + second.value);
		break;
	}
	}
}

// score 213 (was 213): frame 4 versus retail's 8; retail computes the
// attenuation at run time and dispatches the speaker modes through a jump
// table. Two-pass channel loops that pick the bin with a ternary 145 (not
// kept: they only lengthen the code toward retail's size); loops over
// left + channel 245; word mode 208.
// score 166 (was 213): frame four versus eight; zero gain folds and mode dispatch differs.
// Direct bin loops: mode one 172, mode two 174, attenuation 200, fixed pair 214, all 166;
// dynamic pairs only 166, modes one/two 171, mode one/attenuation 169.
// score 156 (was 166): frame eight agrees; zero gain folding and mode dispatch remain;
// frame pointer 166; word counters 157; common counter 166; exclusive end bins 165;
// pair/quad capacity predicates 156.
// score 137 (was 156): Frame eight agrees; runtime zero attenuation and mode dispatch remain. Shared modes 168; adjacent endpoints 193; all byte endpoints 142; byte capacity 156; unsigned counters 156; combined shared modes 161; rear bytes 170; front bytes 137 (plain, exact length).
// @retail 0x2afb80
void c_sound_effects::add_source_mixbins(s_sound_mixbin_source *source, bool rear, long mode, s_mixbin_settings *mixbins)
{
	bool alternate = ((source->flags >> 3) & 1) != 0;
	long remaining = 8 - mixbins->mixbins.dwMixBinCount;
	bool pair_available = remaining >= 2;
	bool quad_available = remaining >= 4;
	real gain = 0.0f;
	unsigned char left = alternate ? 0 : 6;
	unsigned char right = alternate ? 1 : 7;
	long rear_left = alternate ? 4 : 8;
	long rear_right = alternate ? 5 : 9;

	if (source->speaker_mode == 1)
	{
		if (pair_available)
		{
			for (long mixbin = left; mixbin <= right; ++mixbin)
				sound_mixbins_add(mixbins, mixbin, gain);
		}
	}
	else if (source->speaker_mode == 2)
	{
		if (pair_available)
		{
			if (rear)
			{
				for (long mixbin = rear_left; mixbin <= rear_right; ++mixbin)
					sound_mixbins_add(mixbins, mixbin, gain);
			}
			else
			{
				for (long mixbin = left; mixbin <= right; ++mixbin)
					sound_mixbins_add(mixbins, mixbin, gain);
			}
		}
	}
	else if (source->speaker_mode == 3)
	{
		if (rear)
		{
			if (pair_available)
			{
				sound_mixbins_add(mixbins, 2, gain);
				sound_mixbins_add(mixbins, 3, gain);
			}
		}
		else if (quad_available)
		{
			for (long mixbin = left; mixbin <= right; ++mixbin)
			{
				sound_mixbins_add(mixbins, mixbin, gain - 6.0206f);
				sound_mixbins_add(mixbins, mixbin, g_44f710);
			}
		}
	}

	if (source->mixbin != NONE)
	{
		add_send(source->mixbin, mode, gain, mixbins);
	}
}

// score 208 (was 208): eight-byte frame and inherited x87 send sums differ.
// Levels before flag 208; scalar names 208; depth one 208;
// size preference 449; shared gain loop 384.
// With a per-function precision pragma (173, not kept: pragmas other than a file-scope inline_depth
// steer the compiler) the frame is four bytes and the late level sum remains.
// Native channel bit 208; const samples 208; immutable input 208; bit/input 208;
// scoped precision pragma 173; global optimization off 229; frame pointer 225; precision/early sum 224.
// score 200 (was 208): frame 0x10 versus incoming-slot reuse; inherited x87 send sums differ.
// Entry front/rear/all gain samples 210/200/203; individual left/right/rear-left/rear-right
// samples 208/212/206/202; all samples with early send sum 215.
// @retail 0x2af920
void c_sound_effects::add_sends(s_sound_send_parameters *parameters, long unused, long mode, s_mixbin_settings *mixbins)
{
	bool alternate = TEST_FLAG(parameters->flags, 3) != 0;
	real level_offset = parameters->level_offset;
	real base_level = parameters->level;
	real rear_left_gain = parameters->rear_left_gain;
	real rear_right_gain = parameters->rear_right_gain;

	sound_stream_append_mixbin(mixbins, alternate ? 0 : 6, parameters->left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 1 : 7, parameters->right_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 4 : 8, rear_left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 5 : 9, rear_right_gain);
	c_sound_effects::add_send(10, mode, level_offset + base_level, mixbins);
}
