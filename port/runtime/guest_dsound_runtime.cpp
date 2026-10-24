/* DirectSound, the WMA in-memory decoder and the XHV voice engine for whole
 * images, played by the host mixer.
 *
 * Every object keeps the state the game sets (volumes, mix bins, pitch, 3D
 * parameters, play/pause state, effect data) and reports it back, so the
 * recovered sound driver runs its normal logic. Each audible change is also
 * sent to the host mixer as an h2_audio_command (h2/audio_protocol.h,
 * host_h2_audio_command), which decodes and mixes buffer data and stream
 * packets straight from guest memory.
 *
 * Timing is the host's: the mixer reports how far each voice has played
 * (host_h2_audio_status), in real time even headless.
 *  - stream packets complete once the mixer has read the last of their data;
 *    the completions (and their callbacks) run only in DirectSoundDoWork,
 *    which the driver calls every frame, so callbacks never re-enter Process
 *    or GetStatus. Flush drops the mixer's queue before the packets complete
 *    with E_ABORT.
 *  - a non-looping buffer with data stops when the mixer reaches its end;
 *    looping buffers and buffers without data (mix-in and effect-input
 *    buffers) play until stopped.
 * Without a host voice (an older host), packets complete at the next DoWork.
 *
 * Not sent (kept only for the game to read back): filters, LFOs, envelopes,
 * I3DL2 source settings, cone and doppler settings, DSP effect data; the WMA
 * decoder is a silent stand-in.
 *
 * Written from public DirectSound/XMediaObject semantics and the recovered
 * call sites (src/unknown_21e330.cpp, src/unknown_2ae170.cpp, src/network_voice.cpp
 * and their *_native.cpp replacements); see port/include/xdk/dsound.h. */
#include "h2/host_abi.h"
#include "h2/audio_protocol.h"
#include <xtl.h>
#include <dsound.h>
#include <xhv.h>
#include <math.h>
#include <new>
#include <string.h>

/* the library's count of mixer deadline misses (function_21ec00 compares it
 * each frame); the host mixer never misses one */
extern "C" DWORD g_dwDirectSoundDeltaPanicCount;
DWORD g_dwDirectSoundDeltaPanicCount = 0;

namespace {

constexpr HRESULT k_e_abort = (HRESULT)0x80004004L;  /* flushed packet status */
constexpr DWORD k_stream_status_playing = 0x00010000;  /* function_21ec00 tests bit 16 */
constexpr DWORD k_max_mixbins = 8;
constexpr DWORD k_mixbin_count = 32;
constexpr DWORD k_max_buffers = 256;
constexpr DWORD k_max_streams = 160;      /* the driver keeps up to 0x80 channels */
constexpr DWORD k_max_packets = 8;        /* per stream; READY follows dwMaxAttachedPackets */
constexpr DWORD k_max_decoders = 64;
constexpr DWORD k_effect_count = 15;      /* function_191420 expects a 15-effect image */
constexpr DWORD k_effect_state_dwords = 0x100;
constexpr DWORD k_buffer_arena_size = 0x40000;
constexpr double k_pitch_base_rate = 48000.0;
constexpr DWORD k_buffer_voice_base = 1;       /* host voice ids: buffers */
constexpr DWORD k_stream_voice_base = 0x1000;  /* host voice ids: streams */
constexpr DWORD k_flags_3d = 0x00000010;       /* DSBCAPS_CTRL3D */
constexpr DWORD k_flags_mixin = 0x00003000;    /* DSBCAPS_FXIN | DSBCAPS_MIXIN */

/* ---- the host mixer ---- */

h2_audio_u32 guest_address(const void *pointer)
{
    return static_cast<h2_audio_u32>(reinterpret_cast<DWORD>(pointer));
}

void audio_begin(h2_audio_command *command, DWORD kind, DWORD voice)
{
    memset(command, 0, sizeof(*command));
    command->kind = kind;
    command->voice = voice;
}

void audio_send(const h2_audio_command *command)
{
    host_h2_audio_command(command);
}

void audio_simple(DWORD kind, DWORD voice, DWORD flags)
{
    h2_audio_command command;
    audio_begin(&command, kind, voice);
    command.flags = flags;
    audio_send(&command);
}

bool audio_status(DWORD voice, h2_audio_status *status)
{
    memset(status, 0, sizeof(*status));
    return host_h2_audio_status(voice, status) != 0;
}

struct mixbin_state
{
    DWORD count;
    DSMIXBINVOLUMEPAIR pairs[k_max_mixbins];
};

void mixbins_set(mixbin_state *state, LPCDSMIXBINS mixbins)
{
    state->count = 0;
    if (!mixbins || !mixbins->lpMixBinVolumePairs)
        return;
    DWORD count = mixbins->dwMixBinCount < k_max_mixbins ? mixbins->dwMixBinCount : k_max_mixbins;
    memcpy(state->pairs, mixbins->lpMixBinVolumePairs, count * sizeof(DSMIXBINVOLUMEPAIR));
    state->count = count;
}

/* A voice created without mix bins gets the default assignment the game's
 * startup relies on: c_sound_effects::initialize (src/unknown_2ae170.cpp)
 * only sets the volumes of bins 6..9 (and 10 on a 3D voice) on effect
 * buffers it created without mix bins, so a mono voice starts on the four 3D
 * speaker bins (and the I3DL2 send when 3D); a stereo voice starts on front
 * left and right. */
void mixbins_default(mixbin_state *state, DWORD channels, bool spatial)
{
    static const DWORD k_mono[5] = { 6, 7, 8, 9, 10 };
    state->count = 0;
    if (channels == 2)
    {
        state->pairs[0].dwMixBin = 0;
        state->pairs[1].dwMixBin = 1;
        state->count = 2;
    }
    else
    {
        state->count = spatial ? 5 : 4;
        for (DWORD i = 0; i < state->count; i++)
            state->pairs[i].dwMixBin = k_mono[i];
    }
    for (DWORD i = 0; i < state->count; i++)
        state->pairs[i].lVolume = 0;
}

/* SetMixBinVolumes changes the volume of bins already assigned, by bin */
void mixbins_set_volumes(mixbin_state *state, LPCDSMIXBINS mixbins)
{
    if (!mixbins || !mixbins->lpMixBinVolumePairs)
        return;
    for (DWORD i = 0; i < mixbins->dwMixBinCount; i++)
    {
        const DSMIXBINVOLUMEPAIR &pair = mixbins->lpMixBinVolumePairs[i];
        for (DWORD j = 0; j < state->count; j++)
            if (state->pairs[j].dwMixBin == pair.dwMixBin)
                state->pairs[j].lVolume = pair.lVolume;
    }
}

void mixbins_get(const mixbin_state *state, LPDSVOICEPROPS props)
{
    memset(props, 0, sizeof(*props));
    props->dwMixBinCount = state->count;
    memcpy(props->MixBinVolumePairs, state->pairs, state->count * sizeof(DSMIXBINVOLUMEPAIR));
}

void audio_fill_mixbins(h2_audio_command *command, const mixbin_state *state)
{
    command->mixbin_count = state->count;
    for (DWORD i = 0; i < state->count; i++)
    {
        command->mixbins[i].bin = state->pairs[i].dwMixBin;
        command->mixbins[i].volume = state->pairs[i].lVolume;
    }
}

void audio_send_mixbins(DWORD voice, const mixbin_state *state)
{
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SET_MIXBINS, voice);
    audio_fill_mixbins(&command, state);
    audio_send(&command);
}

void audio_send_volume(DWORD voice, LONG volume, DWORD headroom)
{
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SET_VOLUME, voice);
    command.volume = volume;
    command.headroom = (h2_audio_s32)headroom;
    audio_send(&command);
}

/* the playback clock of one voice: frames per second and bytes per frame */
struct voice_format
{
    WAVEFORMATEX wfx;
    DWORD samples_per_block;
    bool pitch_set;
    LONG pitch;

    void set(LPCWAVEFORMATEX format)
    {
        memset(&wfx, 0, sizeof(wfx));
        samples_per_block = 1;
        pitch_set = false;
        pitch = 0;
        if (!format)
            return;
        wfx = *format;
        if (wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
        {
            const XBOXADPCMWAVEFORMAT *adpcm = reinterpret_cast<const XBOXADPCMWAVEFORMAT *>(format);
            samples_per_block = adpcm->wSamplesPerBlock ? adpcm->wSamplesPerBlock : 64;
        }
    }

    /* the format's rate until SetPitch, then 48 kHz scaled by pitch/4096
       octaves, the convention the driver's pitch offsets use */
    double frames_per_second() const
    {
        if (pitch_set)
            return k_pitch_base_rate * pow(2.0, (double)pitch / 4096.0);
        return (double)wfx.nSamplesPerSec;
    }

    void describe(h2_audio_format *format) const
    {
        format->tag = wfx.wFormatTag;
        format->channels = wfx.nChannels;
        format->samples_per_second = wfx.nSamplesPerSec;
        format->block_align = wfx.nBlockAlign;
        format->bits_per_sample = wfx.wBitsPerSample;
        format->samples_per_block = samples_per_block;
    }
};

void audio_send_frequency(DWORD voice, const voice_format &format)
{
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SET_FREQUENCY, voice);
    command.frequency = (float)format.frames_per_second();
    audio_send(&command);
}

struct voice_3d
{
    FLOAT position[3];
    FLOAT min_distance;
    FLOAT max_distance;
    FLOAT rolloff_factor;
    FLOAT doppler_factor;
    LONG cone_outside_volume;
    DSI3DL2BUFFER i3dl2;
};

/* ---- buffers ---- */

constexpr DWORD k_buffer_status_playing = DSBSTATUS_PLAYING;

struct h2_ds_buffer : IDirectSoundBuffer
{
    bool in_use;
    ULONG references;
    DWORD flags;
    DWORD input_mixbin;
    voice_format format;
    BYTE *data;
    DWORD data_size;
    DWORD cursor;
    bool playing;
    bool looping;
    bool paused;
    LONG volume;
    DWORD headroom;
    mixbin_state mixbins;
    DSFILTERDESC filter;
    voice_3d spatial;
};

h2_ds_buffer g_buffers[k_max_buffers];
BYTE g_buffer_arena[k_buffer_arena_size];
DWORD g_buffer_arena_used = 0;

h2_ds_buffer *buffer_from(LPDIRECTSOUNDBUFFER pointer)
{
    h2_ds_buffer *buffer = static_cast<h2_ds_buffer *>(pointer);
    if (buffer < g_buffers || buffer >= g_buffers + k_max_buffers || !buffer->in_use)
        return nullptr;
    return buffer;
}

DWORD buffer_voice(const h2_ds_buffer *buffer)
{
    return k_buffer_voice_base + (DWORD)(buffer - g_buffers);
}

void buffer_send_data(h2_ds_buffer *buffer)
{
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SET_DATA, buffer_voice(buffer));
    command.address = guest_address(buffer->data);
    command.size = buffer->data ? buffer->data_size : 0;
    audio_send(&command);
}

void buffer_send_3d(h2_ds_buffer *buffer)
{
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SET_3D, buffer_voice(buffer));
    memcpy(command.position, buffer->spatial.position, sizeof(command.position));
    command.min_distance = buffer->spatial.min_distance;
    command.max_distance = buffer->spatial.max_distance;
    command.rolloff = buffer->spatial.rolloff_factor;
    audio_send(&command);
}

/* the host's view of whether the buffer still plays (a non-looping buffer
 * stops at its end) and where its cursor is */
void buffer_sync(h2_ds_buffer *buffer)
{
    h2_audio_status status;
    if (!audio_status(buffer_voice(buffer), &status))
        return;
    buffer->playing = (status.flags & H2_AUDIO_STATUS_PLAYING) != 0;
    buffer->cursor = status.cursor;
}

HRESULT buffer_create(LPCDSBUFFERDESC description, LPDIRECTSOUNDBUFFER *result)
{
    if (!result)
        return E_INVALIDARG;
    *result = nullptr;
    if (!description)
        return E_INVALIDARG;

    h2_ds_buffer *buffer = nullptr;
    for (DWORD i = 0; i < k_max_buffers && !buffer; i++)
        if (!g_buffers[i].in_use)
            buffer = &g_buffers[i];
    if (!buffer)
        return E_OUTOFMEMORY;

    BYTE *data = nullptr;
    DWORD size = description->dwBufferBytes;
    if (size)
    {
        DWORD offset = (g_buffer_arena_used + 15) & ~15u;
        if (offset > k_buffer_arena_size || size > k_buffer_arena_size - offset)
            return E_OUTOFMEMORY;
        data = g_buffer_arena + offset;
        memset(data, 0, size);
        g_buffer_arena_used = offset + size;
    }

    memset(buffer, 0, sizeof(*buffer));
    buffer->in_use = true;
    buffer->references = 1;
    buffer->flags = description->dwFlags;
    buffer->input_mixbin = description->dwInputMixBin;
    buffer->format.set(description->lpwfxFormat);
    buffer->data = data;
    buffer->data_size = size;
    buffer->spatial.max_distance = 1000000000.0f;
    buffer->spatial.min_distance = 1.0f;
    buffer->spatial.rolloff_factor = 1.0f;
    buffer->spatial.doppler_factor = 1.0f;
    bool spatial = (buffer->flags & k_flags_3d) != 0;
    if (description->lpMixBins)
        mixbins_set(&buffer->mixbins, description->lpMixBins);
    else
        mixbins_default(&buffer->mixbins, buffer->format.wfx.nChannels, spatial);
    *result = buffer;

    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_CREATE, buffer_voice(buffer));
    command.flags = H2_AUDIO_VOICE_BUFFER | (spatial ? H2_AUDIO_VOICE_3D : 0) |
                    ((buffer->flags & k_flags_mixin) ? H2_AUDIO_VOICE_MIXIN : 0);
    command.input_mixbin = buffer->input_mixbin;
    buffer->format.describe(&command.format);
    command.frequency = (float)buffer->format.frames_per_second();
    audio_fill_mixbins(&command, &buffer->mixbins);
    audio_send(&command);
    if (data)
        buffer_send_data(buffer);
    if (spatial)
        buffer_send_3d(buffer);
    return S_OK;
}

/* ---- streams ---- */

struct queued_packet
{
    XMEDIAPACKET packet;
};

struct h2_ds_stream final : IDirectSoundStream
{
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT STDMETHODCALLTYPE GetInfo(LPVOID pInfo) override;
    HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus) override;
    HRESULT STDMETHODCALLTYPE Process(LPXMEDIAPACKET pInputPacket, LPXMEDIAPACKET pOutputPacket) override;
    HRESULT STDMETHODCALLTYPE Discontinuity() override;
    HRESULT STDMETHODCALLTYPE Flush() override;

    bool in_use;
    ULONG references;
    DWORD flags;
    DWORD max_attached;
    LPFNXMEDIAOBJECTCALLBACK callback;
    LPVOID context;
    voice_format format;
    queued_packet queue[k_max_packets];
    DWORD queue_head;
    DWORD queue_count;
    DWORD submitted;        /* packets sent to the host */
    DWORD retired;          /* packets completed here; trails the host's count */
    bool paused;
    bool synch_paused;      /* DSSTREAMPAUSE_SYNCHPLAYBACK: waits for SynchPlayback */
    bool flush_pending;     /* FlushEx(ASYNC): packets drop at the next DoWork */
    LONG volume;
    DWORD headroom;
    mixbin_state mixbins;
    DSFILTERDESC filter;
    DSLFODESC lfo[2];
    DSENVELOPEDESC envelope[2];
    LPDIRECTSOUNDBUFFER output_buffer;
};

alignas(h2_ds_stream) unsigned char g_stream_storage[k_max_streams][sizeof(h2_ds_stream)];
bool g_stream_constructed[k_max_streams];

h2_ds_stream *stream_slot(DWORD index)
{
    return reinterpret_cast<h2_ds_stream *>(g_stream_storage[index]);
}

h2_ds_stream *stream_from(LPDIRECTSOUNDSTREAM pointer)
{
    unsigned char *address = reinterpret_cast<unsigned char *>(static_cast<h2_ds_stream *>(pointer));
    if (address < g_stream_storage[0] || address >= g_stream_storage[0] + sizeof(g_stream_storage))
        return nullptr;
    DWORD index = (DWORD)(address - g_stream_storage[0]) / sizeof(h2_ds_stream);
    if (!g_stream_constructed[index] || !stream_slot(index)->in_use)
        return nullptr;
    return stream_slot(index);
}

DWORD stream_voice(h2_ds_stream *stream)
{
    unsigned char *address = reinterpret_cast<unsigned char *>(stream);
    return k_stream_voice_base + (DWORD)(address - g_stream_storage[0]) / sizeof(h2_ds_stream);
}

/* Callers pop the packet before completing it: the driver's callback
 * (sound_driver_stream_callback) releases the chunk and may submit more. */
void stream_complete(h2_ds_stream *stream, const XMEDIAPACKET &packet, HRESULT status, bool notify)
{
    if (packet.pdwCompletedSize)
        *packet.pdwCompletedSize = status == S_OK ? packet.dwMaxSize : 0;
    if (packet.pdwStatus)
        *packet.pdwStatus = (DWORD)status;
    /* With a stream callback the event field is the packet context (the PCM
     * codec stores its chunk there). Without one it would be an event to
     * signal; the title never submits such packets and the runtime has no
     * events yet. */
    if (notify && stream->callback)
        stream->callback(stream->context, packet.hCompletionEvent, (DWORD)status);
}

bool stream_pop(h2_ds_stream *stream, XMEDIAPACKET *packet)
{
    if (!stream->queue_count)
        return false;
    *packet = stream->queue[stream->queue_head].packet;
    stream->queue_head = (stream->queue_head + 1) % k_max_packets;
    stream->queue_count--;
    return true;
}

/* the host stops reading the packets first: their data may be released by
 * the completion callbacks */
void stream_flush_now(h2_ds_stream *stream, bool notify)
{
    stream->flush_pending = false;
    if (stream->queue_count)
        audio_simple(H2_AUDIO_FLUSH, stream_voice(stream), 0);
    stream->retired = stream->submitted;
    XMEDIAPACKET packet;
    while (stream->in_use && stream_pop(stream, &packet))
        stream_complete(stream, packet, k_e_abort, notify);
}

/* completes the packets the host has finished playing */
void stream_advance(h2_ds_stream *stream)
{
    if (stream->flush_pending)
        stream_flush_now(stream, true);
    if (!stream->queue_count)
        return;
    h2_audio_status status;
    DWORD completed = audio_status(stream_voice(stream), &status) ? status.packets_completed
                                                                  : stream->submitted;
    /* a callback may submit (the count grows) or flush (retired jumps to
       submitted); either way stop at the host's count */
    while (stream->in_use && stream->queue_count && (LONG)(completed - stream->retired) > 0)
    {
        XMEDIAPACKET packet;
        stream_pop(stream, &packet);
        stream->retired++;
        stream_complete(stream, packet, S_OK, true);
    }
}

ULONG h2_ds_stream::AddRef()
{
    return ++references;
}

ULONG h2_ds_stream::Release()
{
    return IDirectSoundStream_Release(this);
}

HRESULT h2_ds_stream::GetInfo(LPVOID)
{
    return E_NOTIMPL;
}

HRESULT h2_ds_stream::GetStatus(LPDWORD pdwStatus)
{
    return IDirectSoundStream_GetStatus(this, pdwStatus);
}

HRESULT h2_ds_stream::Process(LPXMEDIAPACKET pInputPacket, LPXMEDIAPACKET pOutputPacket)
{
    return IDirectSoundStream_Process(this, pInputPacket, pOutputPacket);
}

HRESULT h2_ds_stream::Discontinuity()
{
    return IDirectSoundStream_Discontinuity(this);
}

HRESULT h2_ds_stream::Flush()
{
    return IDirectSoundStream_Flush(this);
}

/* ---- the WMA in-memory decoder ---- */

/* The data is WMA in ASF, one whole file per sound chunk, resident while it
 * plays. The first Process hands the chunk to the host decoder
 * (host_h2_wma_open; FFmpeg when the host has it) and pulls 16-bit 44.1 kHz
 * stereo from it. Without a host decoder this is a silent stand-in: it pulls
 * the compressed data through the title's read callback as a decoder would
 * (the callback also drops cache references) and returns silence at an
 * assumed 128 kbit/s, so music lasts roughly its real length and ends when the
 * data does. */
constexpr DWORD k_wma_expansion = 11;

struct h2_wma_decoder final : XMediaObject
{
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT STDMETHODCALLTYPE GetInfo(LPVOID pInfo) override;
    HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus) override;
    HRESULT STDMETHODCALLTYPE Process(LPXMEDIAPACKET pInputPacket, LPXMEDIAPACKET pOutputPacket) override;
    HRESULT STDMETHODCALLTYPE Discontinuity() override;
    HRESULT STDMETHODCALLTYPE Flush() override;

    bool in_use;
    ULONG references;
    LPFNWMAXMODATACALLBACK read;
    LPVOID context;
    DWORD offset;
    DWORD owed;             /* silence still due for data already read */
    bool finished;
    bool opened;            /* the host decoder was tried */
    DWORD host;             /* the host decoder's stream, 0: silent */
};

alignas(h2_wma_decoder) unsigned char g_decoder_storage[k_max_decoders][sizeof(h2_wma_decoder)];
bool g_decoder_constructed[k_max_decoders];

h2_wma_decoder *decoder_slot(DWORD index)
{
    return reinterpret_cast<h2_wma_decoder *>(g_decoder_storage[index]);
}

ULONG h2_wma_decoder::AddRef()
{
    return ++references;
}

ULONG h2_wma_decoder::Release()
{
    if (references && --references)
        return references;
    if (host)
        host_h2_wma_close(host);
    host = 0;
    in_use = false;
    return 0;
}

HRESULT h2_wma_decoder::GetInfo(LPVOID)
{
    return E_NOTIMPL;
}

HRESULT h2_wma_decoder::GetStatus(LPDWORD pdwStatus)
{
    if (pdwStatus)
        *pdwStatus = DSSTREAMSTATUS_READY;
    return S_OK;
}

/* synchronous: fills the output packet and reports its size at once, as
 * c_wma_codec::get_packet expects (it reads the completed size right away) */
HRESULT h2_wma_decoder::Process(LPXMEDIAPACKET, LPXMEDIAPACKET output)
{
    if (!output)
        return E_INVALIDARG;
    /* The packet must come back full unless the data ended: the codec takes a
     * short packet as the end of the sound and releases the decoder. */
    DWORD produced = 0;
    BYTE *target = static_cast<BYTE *>(output->pvBuffer);
    if (!opened && read)
    {
        opened = true;
        LPVOID data = nullptr;
        DWORD got = read(context, 0, 0xffffffffu, &data);
        if (got && data)
        {
            host = host_h2_wma_open(data, got);
            if (host)
                offset = got;
        }
        else
        {
            finished = true;
        }
    }
    if (host)
    {
        produced = host_h2_wma_decode(host, target, output->dwMaxSize);
        if (produced < output->dwMaxSize)
        {
            host_h2_wma_close(host);
            host = 0;
            finished = true;
        }
    }
    while (produced < output->dwMaxSize)
    {
        if (!owed)
        {
            if (finished || !read)
                break;
            LPVOID data = nullptr;
            DWORD wanted = (output->dwMaxSize - produced + k_wma_expansion - 1) / k_wma_expansion;
            DWORD got = read(context, offset, wanted, &data);
            if (!got || !data)
            {
                finished = true;
                break;
            }
            if (got > wanted)
                got = wanted;
            offset += got;
            /* a host backend would decode these bytes here */
            owed = got * k_wma_expansion;
        }
        DWORD take = output->dwMaxSize - produced;
        if (take > owed)
            take = owed;
        memset(target + produced, 0, take);
        produced += take;
        owed -= take;
    }
    /* the last packet still ends on a whole 4-byte stereo frame */
    if (produced < output->dwMaxSize)
        produced &= ~3u;
    if (output->pdwCompletedSize)
        *output->pdwCompletedSize = produced;
    if (output->pdwStatus)
        *output->pdwStatus = S_OK;
    return S_OK;
}

HRESULT h2_wma_decoder::Discontinuity()
{
    return S_OK;
}

HRESULT h2_wma_decoder::Flush()
{
    if (host)
        host_h2_wma_close(host);
    host = 0;
    opened = false;
    offset = 0;
    owed = 0;
    finished = false;
    return S_OK;
}

/* ---- the device ---- */

struct h2_ds_device : IDirectSound
{
    bool created;
    ULONG references;
    DSCAPS caps;
    FLOAT position[3];
    FLOAT orientation[6];
    FLOAT distance_factor;
    FLOAT doppler_factor;
    FLOAT rolloff_factor;
    DWORD mixbin_headroom[k_mixbin_count];
    LPDSEFFECTIMAGEDESC effect_image;
};

h2_ds_device g_device;

/* The DSP image the title downloads holds effect code for the audio DSP; with
 * no DSP, the image is a header of k_effect_count effects whose state segments
 * are plain memory, so SetEffectData/GetEffectData round-trip and the title's
 * state comparisons (function_21f960) work. */
struct effect_image_storage
{
    DWORD dwEffectCount;
    DWORD dwTotalScratchSize;
    DSEFFECTMAP aEffectMaps[k_effect_count];
};
effect_image_storage g_effect_image;
DWORD g_effect_state[k_effect_count][k_effect_state_dwords];
DSFX_HIGH_LEVEL_EFFECT_DESCRIPTION g_effect_high_level[k_effect_count];

bool device_valid(LPDIRECTSOUND pointer)
{
    return pointer == &g_device && g_device.created;
}

/* the listener, as the mixer positions 3D voices against it */
void device_send_listener()
{
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SET_LISTENER, 0);
    memcpy(command.position, g_device.position, sizeof(command.position));
    memcpy(command.front, g_device.orientation, sizeof(command.front));
    memcpy(command.top, g_device.orientation + 3, sizeof(command.top));
    command.distance_factor = g_device.distance_factor;
    command.rolloff = g_device.rolloff_factor;
    audio_send(&command);
}

LPDSEFFECTMAP effect_map(DWORD index)
{
    if (!g_device.effect_image || index >= g_effect_image.dwEffectCount)
        return nullptr;
    return &g_effect_image.aEffectMaps[index];
}

/* ---- the voice engine ---- */

constexpr DWORD k_xhv_ports = 4;
constexpr DWORD k_xhv_max_remote = 30;   /* the title lists up to 30 talkers */

struct h2_xhv_engine : XHVNamespace::CXHVEngine
{
    bool created;
    ULONG references;
    ITitleXHV *callback;
    XHV_RUNTIME_PARAMS parameters;
    DWORD enabled_modes;
    DWORD max_playback_streams;
    bool local_registered[k_xhv_ports];
    XHV_PROCESSING_MODE port_mode[k_xhv_ports];
    XHV_VOICE_MASK mask[k_xhv_ports];
    DWORD remote_count;
    XUID remote[k_xhv_max_remote];
};

h2_xhv_engine g_xhv;

h2_xhv_engine *xhv_from(XHVNamespace::CXHVEngine *engine)
{
    return engine == &g_xhv && g_xhv.created ? &g_xhv : nullptr;
}

long xhv_find_remote(const XUID &xuid)
{
    for (DWORD i = 0; i < g_xhv.remote_count; i++)
        if (g_xhv.remote[i].qwUserID == xuid.qwUserID)
            return (long)i;
    return -1;
}

} // namespace

/* ---- DirectSound entry points ---- */

extern "C" HRESULT WINAPI DirectSoundCreate(const void *, LPDIRECTSOUND *ppDS, LPVOID)
{
    if (!ppDS)
        return E_INVALIDARG;
    if (!g_device.created)
    {
        memset(&g_device, 0, sizeof(g_device));
        g_device.created = true;
        g_device.distance_factor = 1.0f;
        g_device.doppler_factor = 1.0f;
        g_device.rolloff_factor = 1.0f;
        g_device.orientation[2] = 1.0f;
        g_device.orientation[4] = 1.0f;
        device_send_listener();
    }
    g_device.references++;
    *ppDS = &g_device;
    return S_OK;
}

extern "C" HRESULT WINAPI DirectSoundCreateBuffer(LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer)
{
    return buffer_create(pdsbd, ppBuffer);
}

/* The mixer's periodic work: retires the stream packets the host has played
 * (running their completion callbacks); buffers report the host's state when
 * asked. */
extern "C" void WINAPI DirectSoundDoWork(void)
{
    for (DWORD i = 0; i < k_max_streams; i++)
        if (g_stream_constructed[i] && stream_slot(i)->in_use)
            stream_advance(stream_slot(i));
}

extern "C" ULONG WINAPI IDirectSound_Release(LPDIRECTSOUND pDirectSound)
{
    if (!device_valid(pDirectSound))
        return 0;
    if (--g_device.references)
        return g_device.references;
    g_device.created = false;
    return 0;
}

extern "C" HRESULT WINAPI IDirectSound_GetCaps(LPDIRECTSOUND pDirectSound, LPDSCAPS pdsc)
{
    if (!device_valid(pDirectSound) || !pdsc)
        return E_INVALIDARG;
    /* the title stores the caps but never reads a field */
    *pdsc = g_device.caps;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_CreateSoundBuffer(LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pdsbd,
                                                         LPDIRECTSOUNDBUFFER *ppBuffer, LPVOID)
{
    if (!device_valid(pDirectSound))
        return E_INVALIDARG;
    return buffer_create(pdsbd, ppBuffer);
}

extern "C" HRESULT WINAPI IDirectSound_CreateSoundStream(LPDIRECTSOUND pDirectSound, LPCDSSTREAMDESC pdssd,
                                                         LPDIRECTSOUNDSTREAM *ppStream, LPVOID)
{
    if (!ppStream)
        return E_INVALIDARG;
    *ppStream = nullptr;
    if (!device_valid(pDirectSound) || !pdssd)
        return E_INVALIDARG;

    DWORD index = k_max_streams;
    for (DWORD i = 0; i < k_max_streams && index == k_max_streams; i++)
        if (!g_stream_constructed[i] || !stream_slot(i)->in_use)
            index = i;
    if (index == k_max_streams)
        return E_OUTOFMEMORY;

    h2_ds_stream *stream = stream_slot(index);
    if (!g_stream_constructed[index])
    {
        new (stream) h2_ds_stream;
        g_stream_constructed[index] = true;
    }
    stream->in_use = true;
    stream->references = 1;
    stream->flags = pdssd->dwFlags;
    stream->max_attached = pdssd->dwMaxAttachedPackets ? pdssd->dwMaxAttachedPackets : 1;
    if (stream->max_attached > k_max_packets)
        stream->max_attached = k_max_packets;
    stream->callback = pdssd->lpfnCallback;
    stream->context = pdssd->lpvContext;
    stream->format.set(pdssd->lpwfxFormat);
    stream->queue_head = 0;
    stream->queue_count = 0;
    stream->submitted = 0;
    stream->retired = 0;
    stream->paused = false;
    stream->synch_paused = false;
    stream->flush_pending = false;
    stream->volume = 0;
    stream->headroom = 0;
    if (pdssd->lpMixBins)
        mixbins_set(&stream->mixbins, pdssd->lpMixBins);
    else
        mixbins_default(&stream->mixbins, stream->format.wfx.nChannels, false);
    memset(&stream->filter, 0, sizeof(stream->filter));
    memset(stream->lfo, 0, sizeof(stream->lfo));
    memset(stream->envelope, 0, sizeof(stream->envelope));
    stream->output_buffer = nullptr;
    *ppStream = stream;

    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_CREATE, stream_voice(stream));
    command.flags = H2_AUDIO_VOICE_STREAM;
    stream->format.describe(&command.format);
    command.frequency = (float)stream->format.frames_per_second();
    audio_fill_mixbins(&command, &stream->mixbins);
    audio_send(&command);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_GetSpeakerConfig(LPDIRECTSOUND pDirectSound, LPDWORD pdwSpeakerConfig)
{
    if (!device_valid(pDirectSound) || !pdwSpeakerConfig)
        return E_INVALIDARG;
    /* plain stereo: low word 2 without the 0x10000 bit the driver reads as
     * surround (sound_driver_device_startup) */
    *pdwSpeakerConfig = 2;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_SetMixBinHeadroom(LPDIRECTSOUND pDirectSound, DWORD dwMixBin, DWORD dwHeadroom)
{
    if (!device_valid(pDirectSound) || dwMixBin >= k_mixbin_count)
        return E_INVALIDARG;
    g_device.mixbin_headroom[dwMixBin] = dwHeadroom;
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SET_MIXBIN_HEADROOM, 0);
    command.offset = dwMixBin;
    command.headroom = (h2_audio_s32)dwHeadroom;
    audio_send(&command);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_SetEffectData(LPDIRECTSOUND pDirectSound, DWORD dwEffectIndex, DWORD dwOffset,
                                                     LPCVOID pvData, DWORD dwDataSize, DWORD)
{
    if (!device_valid(pDirectSound))
        return E_INVALIDARG;
    LPDSEFFECTMAP map = effect_map(dwEffectIndex);
    if (!map || !pvData || dwOffset > map->dwStateSize * 4 || dwDataSize > map->dwStateSize * 4 - dwOffset)
        return E_INVALIDARG;
    memcpy(static_cast<BYTE *>(map->lpvStateSegment) + dwOffset, pvData, dwDataSize);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_GetEffectData(LPDIRECTSOUND pDirectSound, DWORD dwEffectIndex, DWORD dwOffset,
                                                     LPVOID pvData, DWORD dwDataSize)
{
    if (!device_valid(pDirectSound))
        return E_INVALIDARG;
    LPDSEFFECTMAP map = effect_map(dwEffectIndex);
    if (!map || !pvData || dwOffset > map->dwStateSize * 4 || dwDataSize > map->dwStateSize * 4 - dwOffset)
        return E_INVALIDARG;
    memcpy(pvData, static_cast<const BYTE *>(map->lpvStateSegment) + dwOffset, dwDataSize);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_SetPosition(LPDIRECTSOUND pDirectSound, FLOAT x, FLOAT y, FLOAT z, DWORD)
{
    if (!device_valid(pDirectSound))
        return E_INVALIDARG;
    g_device.position[0] = x;
    g_device.position[1] = y;
    g_device.position[2] = z;
    device_send_listener();
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_SetOrientation(LPDIRECTSOUND pDirectSound, FLOAT xFront, FLOAT yFront, FLOAT zFront,
                                                      FLOAT xTop, FLOAT yTop, FLOAT zTop, DWORD)
{
    if (!device_valid(pDirectSound))
        return E_INVALIDARG;
    FLOAT values[6] = { xFront, yFront, zFront, xTop, yTop, zTop };
    memcpy(g_device.orientation, values, sizeof(values));
    device_send_listener();
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_SetDistanceFactor(LPDIRECTSOUND pDirectSound, FLOAT flDistanceFactor, DWORD)
{
    if (!device_valid(pDirectSound))
        return E_INVALIDARG;
    g_device.distance_factor = flDistanceFactor;
    device_send_listener();
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_SetDopplerFactor(LPDIRECTSOUND pDirectSound, FLOAT flDopplerFactor, DWORD)
{
    if (!device_valid(pDirectSound))
        return E_INVALIDARG;
    g_device.doppler_factor = flDopplerFactor;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSound_SetRolloffFactor(LPDIRECTSOUND pDirectSound, FLOAT flRolloffFactor, DWORD)
{
    if (!device_valid(pDirectSound))
        return E_INVALIDARG;
    g_device.rolloff_factor = flRolloffFactor;
    device_send_listener();
    return S_OK;
}

/* deferred 3D settings apply at once (the mixer takes each as it comes) */
extern "C" HRESULT WINAPI IDirectSound_CommitDeferredSettings(LPDIRECTSOUND pDirectSound)
{
    return device_valid(pDirectSound) ? S_OK : E_INVALIDARG;
}

/* starts every stream paused with DSSTREAMPAUSE_SYNCHPLAYBACK together */
extern "C" HRESULT WINAPI IDirectSound_SynchPlayback(LPDIRECTSOUND pDirectSound)
{
    if (!device_valid(pDirectSound))
        return E_INVALIDARG;
    for (DWORD i = 0; i < k_max_streams; i++)
    {
        h2_ds_stream *stream = stream_slot(i);
        if (g_stream_constructed[i] && stream->in_use && stream->synch_paused)
        {
            stream->synch_paused = false;
            stream->paused = false;
            audio_simple(H2_AUDIO_PAUSE, stream_voice(stream), 0);
        }
    }
    return S_OK;
}

/* ---- buffer entry points ---- */

extern "C" ULONG WINAPI IDirectSoundBuffer_Release(LPDIRECTSOUNDBUFFER pBuffer)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return 0;
    if (--buffer->references)
        return buffer->references;
    buffer->playing = false;
    /* the host lets go of the data (and of the streams routed here) first */
    audio_simple(H2_AUDIO_RELEASE, buffer_voice(buffer), 0);
    /* arena memory is not reused: the title creates its buffers once */
    buffer->in_use = false;
    return 0;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetBufferData(LPDIRECTSOUNDBUFFER pBuffer, LPVOID pvBufferData, DWORD dwBufferBytes)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->data = static_cast<BYTE *>(pvBufferData);
    buffer->data_size = pvBufferData ? dwBufferBytes : 0;
    buffer->cursor = 0;
    buffer_send_data(buffer);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetCurrentPosition(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwPlayCursor)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->cursor = dwPlayCursor < buffer->data_size ? dwPlayCursor : 0;
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SET_CURSOR, buffer_voice(buffer));
    command.offset = buffer->cursor;
    audio_send(&command);
    return S_OK;
}

/* the data is the title's memory (or the arena), so the lock is a view */
extern "C" HRESULT WINAPI IDirectSoundBuffer_Lock(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwOffset, DWORD dwBytes,
                                                  LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
                                                  LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer || !ppvAudioPtr1 || !pdwAudioBytes1)
        return E_INVALIDARG;
    *ppvAudioPtr1 = nullptr;
    *pdwAudioBytes1 = 0;
    if (ppvAudioPtr2)
        *ppvAudioPtr2 = nullptr;
    if (pdwAudioBytes2)
        *pdwAudioBytes2 = 0;
    if (!buffer->data || dwOffset >= buffer->data_size)
        return E_INVALIDARG;
    if (dwBytes > buffer->data_size)
        dwBytes = buffer->data_size;
    DWORD first = buffer->data_size - dwOffset;
    if (first > dwBytes)
        first = dwBytes;
    *ppvAudioPtr1 = buffer->data + dwOffset;
    *pdwAudioBytes1 = first;
    if (first < dwBytes && ppvAudioPtr2 && pdwAudioBytes2)
    {
        *ppvAudioPtr2 = buffer->data;
        *pdwAudioBytes2 = dwBytes - first;
    }
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_Play(LPDIRECTSOUNDBUFFER pBuffer, DWORD, DWORD, DWORD dwFlags)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->looping = (dwFlags & DSBPLAY_LOOPING) != 0;
    buffer->playing = true;
    audio_simple(H2_AUDIO_PLAY, buffer_voice(buffer), buffer->looping ? H2_AUDIO_PLAY_LOOPING : 0);
    return S_OK;
}

/* A paused buffer still reports playing: it resumes where it stopped. */
extern "C" HRESULT WINAPI IDirectSoundBuffer_Pause(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwPause)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->paused = dwPause == DSBPAUSE_PAUSE;
    audio_simple(H2_AUDIO_PAUSE, buffer_voice(buffer), buffer->paused ? 1 : 0);
    return S_OK;
}

/* stops at once; the timestamp and envelope-release flags are not modelled */
extern "C" HRESULT WINAPI IDirectSoundBuffer_StopEx(LPDIRECTSOUNDBUFFER pBuffer, REFERENCE_TIME, DWORD)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->playing = false;
    audio_simple(H2_AUDIO_STOP, buffer_voice(buffer), 0);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_GetStatus(LPDIRECTSOUNDBUFFER pBuffer, LPDWORD pdwStatus)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer || !pdwStatus)
        return E_INVALIDARG;
    buffer_sync(buffer);
    *pdwStatus = buffer->playing ? k_buffer_status_playing : 0;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetVolume(LPDIRECTSOUNDBUFFER pBuffer, LONG lVolume)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->volume = lVolume;
    audio_send_volume(buffer_voice(buffer), buffer->volume, buffer->headroom);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetHeadroom(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwHeadroom)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->headroom = dwHeadroom;
    audio_send_volume(buffer_voice(buffer), buffer->volume, buffer->headroom);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetMixBins(LPDIRECTSOUNDBUFFER pBuffer, LPCDSMIXBINS pMixBins)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    mixbins_set(&buffer->mixbins, pMixBins);
    audio_send_mixbins(buffer_voice(buffer), &buffer->mixbins);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetMixBinVolumes(LPDIRECTSOUNDBUFFER pBuffer, LPCDSMIXBINS pMixBins)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    mixbins_set_volumes(&buffer->mixbins, pMixBins);
    audio_send_mixbins(buffer_voice(buffer), &buffer->mixbins);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetFilter(LPDIRECTSOUNDBUFFER pBuffer, LPCDSFILTERDESC pFilterDesc)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer || !pFilterDesc)
        return E_INVALIDARG;
    buffer->filter = *pFilterDesc;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetI3DL2Source(LPDIRECTSOUNDBUFFER pBuffer, LPCDSI3DL2BUFFER pds3db, DWORD)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer || !pds3db)
        return E_INVALIDARG;
    buffer->spatial.i3dl2 = *pds3db;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetPosition(LPDIRECTSOUNDBUFFER pBuffer, FLOAT x, FLOAT y, FLOAT z, DWORD)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->spatial.position[0] = x;
    buffer->spatial.position[1] = y;
    buffer->spatial.position[2] = z;
    buffer_send_3d(buffer);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetMaxDistance(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flMaxDistance, DWORD)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->spatial.max_distance = flMaxDistance;
    buffer_send_3d(buffer);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetMinDistance(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flMinDistance, DWORD)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->spatial.min_distance = flMinDistance;
    buffer_send_3d(buffer);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetRolloffFactor(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flRolloffFactor, DWORD)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->spatial.rolloff_factor = flRolloffFactor;
    buffer_send_3d(buffer);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetDopplerFactor(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flDopplerFactor, DWORD)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->spatial.doppler_factor = flDopplerFactor;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_SetConeOutsideVolume(LPDIRECTSOUNDBUFFER pBuffer, LONG lConeOutsideVolume, DWORD)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer)
        return E_INVALIDARG;
    buffer->spatial.cone_outside_volume = lConeOutsideVolume;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundBuffer_GetVoiceProperties(LPDIRECTSOUNDBUFFER pBuffer, LPDSVOICEPROPS pVoiceProps)
{
    h2_ds_buffer *buffer = buffer_from(pBuffer);
    if (!buffer || !pVoiceProps)
        return E_INVALIDARG;
    mixbins_get(&buffer->mixbins, pVoiceProps);
    return S_OK;
}

/* ---- stream entry points ---- */

/* Queued packets are dropped without callbacks: the driver releases its
 * streams at teardown, after its channel state is gone. */
extern "C" ULONG WINAPI IDirectSoundStream_Release(LPDIRECTSOUNDSTREAM pStream)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return 0;
    if (--stream->references)
        return stream->references;
    stream_flush_now(stream, false);
    audio_simple(H2_AUDIO_RELEASE, stream_voice(stream), 0);
    stream->in_use = false;
    return 0;
}

extern "C" HRESULT WINAPI IDirectSoundStream_GetStatus(LPDIRECTSOUNDSTREAM pStream, LPDWORD pdwStatus)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream || !pdwStatus)
        return E_INVALIDARG;
    DWORD status = 0;
    if (stream->queue_count < stream->max_attached)
        status |= DSSTREAMSTATUS_READY;
    if (stream->queue_count)
        status |= k_stream_status_playing;
    if (stream->paused)
        status |= DSSTREAMSTATUS_PAUSED;
    *pdwStatus = status;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_Process(LPDIRECTSOUNDSTREAM pStream, LPXMEDIAPACKET pInputPacket,
                                                     LPXMEDIAPACKET)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream || !pInputPacket)
        return E_INVALIDARG;
    if (stream->queue_count >= k_max_packets)
        return E_OUTOFMEMORY;

    queued_packet &slot = stream->queue[(stream->queue_head + stream->queue_count) % k_max_packets];
    slot.packet = *pInputPacket;
    stream->queue_count++;
    stream->submitted++;
    if (pInputPacket->pdwCompletedSize)
        *pInputPacket->pdwCompletedSize = 0;
    if (pInputPacket->pdwStatus)
        *pInputPacket->pdwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SUBMIT, stream_voice(stream));
    command.address = guest_address(pInputPacket->pvBuffer);
    command.size = pInputPacket->dwMaxSize;
    audio_send(&command);
    return S_OK;
}

/* the queue already plays to its end and then stops; nothing to mark */
extern "C" HRESULT WINAPI IDirectSoundStream_Discontinuity(LPDIRECTSOUNDSTREAM pStream)
{
    return stream_from(pStream) ? S_OK : E_INVALIDARG;
}

extern "C" HRESULT WINAPI IDirectSoundStream_Flush(LPDIRECTSOUNDSTREAM pStream)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return E_INVALIDARG;
    stream_flush_now(stream, true);
    return S_OK;
}

/* An asynchronous flush (the title always passes ASYNC|ENVELOPE) keeps the
 * stream playing until the next DoWork, standing in for the envelope release,
 * and then drops its packets; the timestamp needs a mixer clock. */
extern "C" HRESULT WINAPI IDirectSoundStream_FlushEx(LPDIRECTSOUNDSTREAM pStream, REFERENCE_TIME, DWORD dwFlags)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return E_INVALIDARG;
    if (dwFlags & DSSTREAMFLUSHEX_ASYNC)
        stream->flush_pending = stream->queue_count != 0;
    else
        stream_flush_now(stream, true);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_Pause(LPDIRECTSOUNDSTREAM pStream, DWORD dwPause)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return E_INVALIDARG;
    if (dwPause == DSSTREAMPAUSE_RESUME)
    {
        stream->paused = false;
        stream->synch_paused = false;
    }
    else
    {
        /* what already played still completes at the next DoWork */
        stream->paused = true;
        stream->synch_paused = dwPause == DSSTREAMPAUSE_SYNCHPLAYBACK;
    }
    audio_simple(H2_AUDIO_PAUSE, stream_voice(stream), stream->paused ? 1 : 0);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_SetHeadroom(LPDIRECTSOUNDSTREAM pStream, DWORD dwHeadroom)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return E_INVALIDARG;
    stream->headroom = dwHeadroom;
    audio_send_volume(stream_voice(stream), stream->volume, stream->headroom);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_SetVolume(LPDIRECTSOUNDSTREAM pStream, LONG lVolume)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return E_INVALIDARG;
    stream->volume = lVolume;
    audio_send_volume(stream_voice(stream), stream->volume, stream->headroom);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_SetPitch(LPDIRECTSOUNDSTREAM pStream, LONG lPitch)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return E_INVALIDARG;
    stream->format.pitch = lPitch;
    stream->format.pitch_set = true;
    audio_send_frequency(stream_voice(stream), stream->format);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_SetMixBins(LPDIRECTSOUNDSTREAM pStream, LPCDSMIXBINS pMixBins)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return E_INVALIDARG;
    mixbins_set(&stream->mixbins, pMixBins);
    audio_send_mixbins(stream_voice(stream), &stream->mixbins);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_SetMixBinVolumes(LPDIRECTSOUNDSTREAM pStream, LPCDSMIXBINS pMixBins)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return E_INVALIDARG;
    mixbins_set_volumes(&stream->mixbins, pMixBins);
    audio_send_mixbins(stream_voice(stream), &stream->mixbins);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_SetFilter(LPDIRECTSOUNDSTREAM pStream, LPCDSFILTERDESC pFilterDesc)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream || !pFilterDesc)
        return E_INVALIDARG;
    stream->filter = *pFilterDesc;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_SetLFO(LPDIRECTSOUNDSTREAM pStream, LPCDSLFODESC pLFODesc)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream || !pLFODesc || pLFODesc->dwLFO > 1)
        return E_INVALIDARG;
    stream->lfo[pLFODesc->dwLFO] = *pLFODesc;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_SetEG(LPDIRECTSOUNDSTREAM pStream, LPCDSENVELOPEDESC pEnvelopeDesc)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream || !pEnvelopeDesc)
        return E_INVALIDARG;
    /* the title only sets the amplitude envelope (DSEG_AMPLITUDE == 1) */
    stream->envelope[pEnvelopeDesc->dwEG == DSEG_AMPLITUDE ? 0 : 1] = *pEnvelopeDesc;
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_SetOutputBuffer(LPDIRECTSOUNDSTREAM pStream, LPDIRECTSOUNDBUFFER pOutputBuffer)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream)
        return E_INVALIDARG;
    stream->output_buffer = pOutputBuffer;
    h2_ds_buffer *target = buffer_from(pOutputBuffer);
    h2_audio_command command;
    audio_begin(&command, H2_AUDIO_SET_OUTPUT, stream_voice(stream));
    command.offset = target ? buffer_voice(target) : 0;
    audio_send(&command);
    return S_OK;
}

extern "C" HRESULT WINAPI IDirectSoundStream_GetVoiceProperties(LPDIRECTSOUNDSTREAM pStream, LPDSVOICEPROPS pVoiceProps)
{
    h2_ds_stream *stream = stream_from(pStream);
    if (!stream || !pVoiceProps)
        return E_INVALIDARG;
    mixbins_get(&stream->mixbins, pVoiceProps);
    return S_OK;
}

/* ---- effects image ---- */

extern "C" HRESULT WINAPI XAudioDownloadEffectsImage(LPCSTR, LPDSEFFECTIMAGELOC, DWORD,
                                                     LPDSEFFECTIMAGEDESC *ppImageDesc)
{
    if (!ppImageDesc)
        return E_INVALIDARG;
    /* a reload (sound_driver_effect_refresh) resets the effect state, as a
     * fresh download of the image would */
    memset(&g_effect_image, 0, sizeof(g_effect_image));
    memset(g_effect_state, 0, sizeof(g_effect_state));
    g_effect_image.dwEffectCount = k_effect_count;
    for (DWORD i = 0; i < k_effect_count; i++)
    {
        g_effect_image.aEffectMaps[i].lpvStateSegment = g_effect_state[i];
        g_effect_image.aEffectMaps[i].dwStateSize = k_effect_state_dwords;
    }
    g_device.effect_image = reinterpret_cast<LPDSEFFECTIMAGEDESC>(&g_effect_image);
    *ppImageDesc = g_device.effect_image;
    return S_OK;
}

extern "C" HRESULT WINAPI XAudioSetEffectData(DWORD dwEffectIndex, LPCDSFX_HIGH_LEVEL_EFFECT_DESCRIPTION pDesc, LPVOID)
{
    if (!pDesc || dwEffectIndex >= k_effect_count)
        return E_INVALIDARG;
    /* kept for a future reverb; the I3DL2 listener is the only kind sent */
    g_effect_high_level[dwEffectIndex] = *pDesc;
    return S_OK;
}

extern "C" HRESULT WINAPI WmaCreateInMemoryDecoder(LPFNWMAXMODATACALLBACK pfnCallback, LPVOID pvContext,
                                                   DWORD, LPWAVEFORMATEX pwfxCompressed,
                                                   LPXMEDIAOBJECT *ppMediaObject)
{
    if (!ppMediaObject)
        return E_INVALIDARG;
    *ppMediaObject = nullptr;
    DWORD index = k_max_decoders;
    for (DWORD i = 0; i < k_max_decoders && index == k_max_decoders; i++)
        if (!g_decoder_constructed[i] || !decoder_slot(i)->in_use)
            index = i;
    if (index == k_max_decoders)
        return E_OUTOFMEMORY;

    h2_wma_decoder *decoder = decoder_slot(index);
    if (!g_decoder_constructed[index])
    {
        new (decoder) h2_wma_decoder;
        g_decoder_constructed[index] = true;
    }
    decoder->in_use = true;
    decoder->references = 1;
    decoder->read = pfnCallback;
    decoder->context = pvContext;
    decoder->offset = 0;
    decoder->owed = 0;
    decoder->finished = false;
    decoder->opened = false;
    decoder->host = 0;

    /* the caller passes an uninitialized local: report the decoded format */
    if (pwfxCompressed)
    {
        pwfxCompressed->wFormatTag = WAVE_FORMAT_PCM;
        pwfxCompressed->nChannels = 2;
        pwfxCompressed->nSamplesPerSec = 44100;
        pwfxCompressed->nAvgBytesPerSec = 176400;
        pwfxCompressed->nBlockAlign = 4;
        pwfxCompressed->wBitsPerSample = 16;
        pwfxCompressed->cbSize = 0;
    }
    *ppMediaObject = decoder;
    return S_OK;
}

/* ---- XHV voice engine ---- */

/* The engine exists so the voice code runs, but no communicator is ever
 * inserted: local talkers report removed, nobody talks, voice mail fails. */

extern "C" HRESULT WINAPI XHVEngineCreate(XHV_RUNTIME_PARAMS *pParams, XHVEngine **ppEngine)
{
    if (!ppEngine)
        return E_INVALIDARG;
    *ppEngine = nullptr;
    if (!pParams)
        return E_INVALIDARG;
    /* one engine: a create after a failed start (never released) replaces it */
    memset(&g_xhv, 0, sizeof(g_xhv));
    g_xhv.created = true;
    g_xhv.references = 1;
    g_xhv.parameters = *pParams;
    *ppEngine = &g_xhv;
    return S_OK;
}

extern "C" HRESULT WINAPI XHVEngine_SetCallbackInterface(XHVEngine *pEngine, ITitleXHV *pCallback)
{
    h2_xhv_engine *engine = xhv_from(pEngine);
    if (!engine)
        return E_INVALIDARG;
    engine->callback = pCallback;
    return S_OK;
}

namespace XHVNamespace {

ULONG __stdcall CXHVEngine::Release()
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine)
        return 0;
    if (--engine->references)
        return engine->references;
    engine->created = false;
    return 0;
}

HRESULT __stdcall CXHVEngine::EnableProcessingMode(XHV_PROCESSING_MODE mode)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine)
        return E_INVALIDARG;
    engine->enabled_modes++;
    (void)mode;
    return S_OK;
}

HRESULT __stdcall CXHVEngine::SetMaxPlaybackStreamsCount(DWORD dwCount)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine)
        return E_INVALIDARG;
    engine->max_playback_streams = dwCount;
    return S_OK;
}

HRESULT __stdcall CXHVEngine::RegisterLocalTalker(DWORD dwPort)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine || dwPort >= k_xhv_ports)
        return E_INVALIDARG;
    engine->local_registered[dwPort] = true;
    return S_OK;
}

HRESULT __stdcall CXHVEngine::UnregisterLocalTalker(DWORD dwPort)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine || dwPort >= k_xhv_ports)
        return E_INVALIDARG;
    engine->local_registered[dwPort] = false;
    return S_OK;
}

HRESULT __stdcall CXHVEngine::SetProcessingMode(DWORD dwPort, XHV_PROCESSING_MODE mode)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine || dwPort >= k_xhv_ports)
        return E_INVALIDARG;
    engine->port_mode[dwPort] = mode;
    return S_OK;
}

HRESULT __stdcall CXHVEngine::GetLocalTalkerStatus(DWORD dwPort, XHV_LOCAL_TALKER_STATUS *pStatus)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine || dwPort >= k_xhv_ports || !pStatus)
        return E_INVALIDARG;
    pStatus->communicatorStatus = XHV_VOICE_COMMUNICATOR_STATUS_REMOVED;
    return S_OK;
}

/* the title passes a 30-entry array */
HRESULT __stdcall CXHVEngine::GetRemoteTalkers(DWORD *pdwCount, XUID *pXuids)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine || !pdwCount)
        return E_INVALIDARG;
    *pdwCount = engine->remote_count;
    if (pXuids)
        memcpy(pXuids, engine->remote, engine->remote_count * sizeof(XUID));
    return S_OK;
}

HRESULT __stdcall CXHVEngine::UnregisterRemoteTalker(XUID xuid)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine)
        return E_INVALIDARG;
    long index = xhv_find_remote(xuid);
    if (index < 0)
        return E_FAIL;
    engine->remote_count--;
    memmove(&engine->remote[index], &engine->remote[index + 1],
            (engine->remote_count - (DWORD)index) * sizeof(XUID));
    return S_OK;
}

HRESULT __stdcall CXHVEngine::RegisterRemoteTalker(XUID xuid)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine)
        return E_INVALIDARG;
    if (xhv_find_remote(xuid) >= 0)
        return S_OK;
    DWORD limit = engine->parameters.dwMaxRemoteTalkers < k_xhv_max_remote
                      ? engine->parameters.dwMaxRemoteTalkers : k_xhv_max_remote;
    if (engine->remote_count >= limit)
        return E_FAIL;
    engine->remote[engine->remote_count++] = xuid;
    return S_OK;
}

BOOL __stdcall CXHVEngine::IsTalking(XUID)
{
    return FALSE;
}

HRESULT __stdcall CXHVEngine::SetVoiceMask(DWORD dwPort, const XHV_VOICE_MASK *pMask)
{
    h2_xhv_engine *engine = xhv_from(this);
    if (!engine || dwPort >= k_xhv_ports || !pMask)
        return E_INVALIDARG;
    engine->mask[dwPort] = *pMask;
    return S_OK;
}

HRESULT __stdcall CXHVEngine::SetMixBinMapping(XUID, DWORD, const DSMIXBINS *)
{
    return xhv_from(this) ? S_OK : E_INVALIDARG;
}

HRESULT __stdcall CXHVEngine::SetPlaybackPriority(XUID, DWORD, XHV_PLAYBACK_PRIORITY)
{
    return xhv_from(this) ? S_OK : E_INVALIDARG;
}

/* accepted and dropped: no playback device */
HRESULT __stdcall CXHVEngine::SubmitIncomingVoicePacket(XUID xuid, VOID *, DWORD)
{
    if (!xhv_from(this))
        return E_INVALIDARG;
    return xhv_find_remote(xuid) >= 0 ? S_OK : E_FAIL;
}

HRESULT __stdcall CXHVEngine::VoiceMailPlay(DWORD, DWORD, const BYTE *, BOOL)
{
    return E_FAIL;
}

HRESULT __stdcall CXHVEngine::VoiceMailRecord(DWORD, DWORD, DWORD, BYTE *)
{
    return E_FAIL;
}

HRESULT __stdcall CXHVEngine::VoiceMailStop(DWORD)
{
    return xhv_from(this) ? S_OK : E_INVALIDARG;
}

HRESULT __stdcall CXHVEngine::DoWork()
{
    return xhv_from(this) ? S_OK : E_INVALIDARG;
}

} // namespace XHVNamespace

/* The voice natives (src/voice_xhv_*_native.cpp) import the engine by its
 * x86 __stdcall decorated names; the members above get the ARM (__cdecl)
 * decorations network_voice.cpp uses. */
extern "C" HRESULT xhv_create_stdcall(XHV_RUNTIME_PARAMS *params, XHVEngine **engine)
    __asm__("_XHVEngineCreate@8");
extern "C" HRESULT xhv_create_stdcall(XHV_RUNTIME_PARAMS *params, XHVEngine **engine)
{
    return XHVEngineCreate(params, engine);
}

extern "C" HRESULT xhv_set_callback_stdcall(XHVEngine *engine, ITitleXHV *callback)
    __asm__("_XHVEngine_SetCallbackInterface@8");
extern "C" HRESULT xhv_set_callback_stdcall(XHVEngine *engine, ITitleXHV *callback)
{
    return XHVEngine_SetCallbackInterface(engine, callback);
}

#define H2_XHV_STDCALL(name, decorated, result, params, args) \
    extern "C" result xhv_stdcall_##name params __asm__(decorated); \
    extern "C" result xhv_stdcall_##name params { return engine->name args; }

H2_XHV_STDCALL(Release, "?Release@CXHVEngine@XHVNamespace@@QAGKXZ",
               ULONG, (XHVEngine *engine), ())
H2_XHV_STDCALL(EnableProcessingMode, "?EnableProcessingMode@CXHVEngine@XHVNamespace@@QAGJPAX@Z",
               HRESULT, (XHVEngine *engine, XHV_PROCESSING_MODE mode), (mode))
H2_XHV_STDCALL(SetMaxPlaybackStreamsCount, "?SetMaxPlaybackStreamsCount@CXHVEngine@XHVNamespace@@QAGJK@Z",
               HRESULT, (XHVEngine *engine, DWORD count), (count))
H2_XHV_STDCALL(RegisterLocalTalker, "?RegisterLocalTalker@CXHVEngine@XHVNamespace@@QAGJK@Z",
               HRESULT, (XHVEngine *engine, DWORD port), (port))
H2_XHV_STDCALL(UnregisterLocalTalker, "?UnregisterLocalTalker@CXHVEngine@XHVNamespace@@QAGJK@Z",
               HRESULT, (XHVEngine *engine, DWORD port), (port))
H2_XHV_STDCALL(SetProcessingMode, "?SetProcessingMode@CXHVEngine@XHVNamespace@@QAGJKPAX@Z",
               HRESULT, (XHVEngine *engine, DWORD port, XHV_PROCESSING_MODE mode), (port, mode))
H2_XHV_STDCALL(GetLocalTalkerStatus,
               "?GetLocalTalkerStatus@CXHVEngine@XHVNamespace@@QAGJKPAU_XHV_LOCAL_TALKER_STATUS@@@Z",
               HRESULT, (XHVEngine *engine, DWORD port, XHV_LOCAL_TALKER_STATUS *status), (port, status))
H2_XHV_STDCALL(GetRemoteTalkers, "?GetRemoteTalkers@CXHVEngine@XHVNamespace@@QAGJPAKPAU_XUID@@@Z",
               HRESULT, (XHVEngine *engine, DWORD *count, XUID *xuids), (count, xuids))
H2_XHV_STDCALL(UnregisterRemoteTalker, "?UnregisterRemoteTalker@CXHVEngine@XHVNamespace@@QAGJU_XUID@@@Z",
               HRESULT, (XHVEngine *engine, XUID xuid), (xuid))

#undef H2_XHV_STDCALL
