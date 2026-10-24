/* The guest DirectSound runtime's voice commands to the host mixer.
 *
 * port/runtime/guest_dsound_runtime.cpp keeps the Xbox DirectSound objects the
 * game uses and mirrors every audible change to the host mixer
 * (port/src/audio/) as one of these commands; the host mixes, and the guest
 * reads playback progress back with host_h2_audio_status, so buffer status
 * and stream packet completion follow what was actually played.
 *
 * The guest compiles this header as ILP32 and the host as LP64, so every field
 * is a fixed-size integer or float; sample data travels as 32-bit guest
 * addresses the host turns into host pointers itself (GuestArena).
 *
 * Voices. The guest names each voice with a nonzero id of its choosing. A
 * voice is one of
 *  - a data buffer: plays bytes in guest memory (SET_DATA), once or looping;
 *  - a stream: plays packets in submission order (SUBMIT); the host counts
 *    the packets it has finished reading (packets_completed), and a FLUSH
 *    drops the queue and counts every submitted packet as completed, so the
 *    guest's count of retired packets always matches the host's;
 *  - a submix bus: a buffer with H2_AUDIO_VOICE_MIXIN and no data. It sums
 *    the voices whose output is routed to it (SET_OUTPUT) and, when its
 *    input mix bin is an effect send (>= H2_AUDIO_FIRST_FX_MIXBIN), what
 *    other voices send to that bin; then it plays like any voice.
 *
 * Levels follow DirectSound: volumes and headroom in millibels (hundredths of
 * a decibel, 0 = unity, -10000 = silent), mix-bin volumes the same. Mix bins
 * use the Xbox numbering: 0 front left, 1 front right, 2 center, 3 LFE,
 * 4 back left, 5 back right, 6..9 the same four for 3D (crosstalk) output,
 * 10 the I3DL2 reverb send, 11..31 effect sends. The host folds 0..9 to
 * stereo; it has no effect DSP, so effect sends reach the speakers only
 * through submix buses reading them, and the reverb send is dropped.
 *
 * 3D: positions and orientations arrive in the coordinate system the game
 * passes to DirectSound (left-handed, y up); the host only uses relative
 * geometry, so any consistent frame works. */
#ifndef H2_AUDIO_PROTOCOL_H
#define H2_AUDIO_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int h2_audio_u32;
typedef int h2_audio_s32;

#define H2_AUDIO_PROTOCOL_VERSION 1
#define H2_AUDIO_MAX_MIXBINS 8
#define H2_AUDIO_MIXBIN_COUNT 32
#define H2_AUDIO_FIRST_FX_MIXBIN 11
#define H2_AUDIO_I3DL2_MIXBIN 10

/* sample formats (the WAVEFORMATEX tags) */
#define H2_AUDIO_FORMAT_PCM 0x0001u
#define H2_AUDIO_FORMAT_XBOX_ADPCM 0x0069u

enum h2_audio_command_kind {
    /* voice_flags, format, input_mixbin: makes a voice (replaces one with the
       same id) */
    H2_AUDIO_CREATE = 1,
    /* drops a voice; voices routed to it fall back to their own mix bins */
    H2_AUDIO_RELEASE = 2,
    /* buffer: address, size (bytes); the play cursor returns to 0 */
    H2_AUDIO_SET_DATA = 3,
    /* buffer: loop region offset, size (bytes; size 0 = the whole buffer) */
    H2_AUDIO_SET_LOOP_REGION = 4,
    /* buffer: play cursor = offset (bytes) */
    H2_AUDIO_SET_CURSOR = 5,
    /* buffer: starts playing; flags & H2_AUDIO_PLAY_LOOPING loops */
    H2_AUDIO_PLAY = 6,
    /* buffer: stops (keeps the cursor); stream: nothing (streams stop by
       running out of packets) */
    H2_AUDIO_STOP = 7,
    /* flags: 1 pauses, 0 resumes */
    H2_AUDIO_PAUSE = 8,
    /* stream: queues a packet of address, size bytes */
    H2_AUDIO_SUBMIT = 9,
    /* stream: drops every queued packet (all count as completed) */
    H2_AUDIO_FLUSH = 10,
    /* volume, headroom (millibels) */
    H2_AUDIO_SET_VOLUME = 11,
    /* frequency: playback rate in frames per second */
    H2_AUDIO_SET_FREQUENCY = 12,
    /* mixbin_count, mixbins: replaces the voice's mix-bin assignment */
    H2_AUDIO_SET_MIXBINS = 13,
    /* offset: the id of the submix bus that takes this voice's output
       instead of its mix bins (0 = none) */
    H2_AUDIO_SET_OUTPUT = 14,
    /* position, min_distance, max_distance, rolloff (a 3D voice) */
    H2_AUDIO_SET_3D = 15,
    /* listener: position, front, top, distance_factor, rolloff (device);
       voice is 0 */
    H2_AUDIO_SET_LISTENER = 16,
    /* device: mix bin offset gets headroom (millibels); voice is 0 */
    H2_AUDIO_SET_MIXBIN_HEADROOM = 17,
};

/* voice_flags */
#define H2_AUDIO_VOICE_BUFFER 0x0001u
#define H2_AUDIO_VOICE_STREAM 0x0002u
#define H2_AUDIO_VOICE_3D 0x0004u      /* positioned against the listener */
#define H2_AUDIO_VOICE_MIXIN 0x0008u   /* a submix bus (no data of its own) */

/* PLAY flags */
#define H2_AUDIO_PLAY_LOOPING 0x0001u

struct h2_audio_format {
    h2_audio_u32 tag;                /* H2_AUDIO_FORMAT_* */
    h2_audio_u32 channels;           /* 1 or 2 */
    h2_audio_u32 samples_per_second;
    h2_audio_u32 block_align;        /* bytes per frame (PCM) or block (ADPCM) */
    h2_audio_u32 bits_per_sample;    /* 8 or 16 (PCM), 4 (ADPCM) */
    h2_audio_u32 samples_per_block;  /* ADPCM: 64 */
};

struct h2_audio_mixbin {
    h2_audio_u32 bin;
    h2_audio_s32 volume;             /* millibels */
};

struct h2_audio_command {
    h2_audio_u32 kind;               /* h2_audio_command_kind */
    h2_audio_u32 voice;              /* the guest's voice id (0: the device) */
    h2_audio_u32 flags;              /* CREATE: voice_flags; PLAY, PAUSE */
    h2_audio_u32 address;            /* SET_DATA, SUBMIT: guest address */
    h2_audio_u32 size;               /* SET_DATA, SUBMIT, SET_LOOP_REGION: bytes */
    h2_audio_u32 offset;             /* SET_CURSOR, SET_LOOP_REGION, SET_OUTPUT,
                                        SET_MIXBIN_HEADROOM */
    h2_audio_s32 volume;             /* SET_VOLUME */
    h2_audio_s32 headroom;           /* SET_VOLUME, SET_MIXBIN_HEADROOM */
    h2_audio_u32 input_mixbin;       /* CREATE */
    float frequency;                 /* SET_FREQUENCY, CREATE (initial) */
    float position[3];               /* SET_3D, SET_LISTENER */
    float front[3];                  /* SET_LISTENER */
    float top[3];                    /* SET_LISTENER */
    float min_distance;              /* SET_3D */
    float max_distance;              /* SET_3D */
    float rolloff;                   /* SET_3D, SET_LISTENER */
    float distance_factor;           /* SET_LISTENER */
    struct h2_audio_format format;   /* CREATE */
    h2_audio_u32 mixbin_count;       /* CREATE, SET_MIXBINS */
    struct h2_audio_mixbin mixbins[H2_AUDIO_MAX_MIXBINS];
};

/* status flags */
#define H2_AUDIO_STATUS_PLAYING 0x0001u  /* a buffer still playing */
#define H2_AUDIO_STATUS_PAUSED 0x0002u

struct h2_audio_status {
    h2_audio_u32 flags;
    h2_audio_u32 cursor;             /* buffer: play cursor (bytes) */
    h2_audio_u32 packets_submitted;  /* stream: packets ever submitted */
    h2_audio_u32 packets_completed;  /* stream: packets finished (or flushed) */
    h2_audio_u32 frames_low;         /* source frames played since create */
    h2_audio_u32 frames_high;
};

#ifdef __cplusplus
}
#endif

#endif
