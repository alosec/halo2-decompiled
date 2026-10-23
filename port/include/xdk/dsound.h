#ifndef H2_PORT_XDK_DSOUND_H
#define H2_PORT_XDK_DSOUND_H

/* Clean-room DirectSound declarations for compiling the recovered audio code
 * (MS-ABI frontend, 32-bit pointers).
 *
 * Provenance: nothing here is copied from a Microsoft Xbox SDK/XDK header,
 * leaked material or GPL emulator code. Every declaration comes from one of:
 *   - public PC DirectSound 8 / Win32 documentation (WAVEFORMATEX, DS3D_*,
 *     DSBCAPS_CTRL3D, WAVE_FORMAT_PCM, the I3DL2 reverb parameter list,
 *     E_PENDING, COM calling-convention macros);
 *   - the recovered sources in src/ and include/ (names, call sites, field
 *     accesses);
 *   - the layouts and constants asserted by the native replacements in
 *     src/sound_*_native.cpp and src/voice_*_native.cpp, and the retail
 *     executable itself (argument counts, vtable slot offsets and constants
 *     read from orig/game/default.xbe disassembly).
 * Values and names not verified against the retail executable are marked
 * PROVISIONAL. */

#include <xtl.h>

/* ---- Win32 base types and macros that xtl.h does not (yet) provide ---- */

#ifndef H2_PORT_HAVE_LPVOID
#define H2_PORT_HAVE_LPVOID
typedef void *LPVOID;
#endif
#ifndef H2_PORT_HAVE_LPCVOID
#define H2_PORT_HAVE_LPCVOID
typedef const void *LPCVOID;
#endif
#ifndef H2_PORT_HAVE_LPDWORD
#define H2_PORT_HAVE_LPDWORD
typedef DWORD *LPDWORD;
#endif
#ifndef H2_PORT_HAVE_LPLONG
#define H2_PORT_HAVE_LPLONG
typedef LONG *LPLONG;
#endif
#ifndef H2_PORT_HAVE_LPCSTR
#define H2_PORT_HAVE_LPCSTR
typedef const char *LPCSTR;
#endif
#ifndef H2_PORT_HAVE_FLOAT
#define H2_PORT_HAVE_FLOAT
typedef float FLOAT;
#endif
#ifndef H2_PORT_HAVE_ULONG
#define H2_PORT_HAVE_ULONG
typedef unsigned long ULONG;
#endif
#ifndef H2_PORT_HAVE_LONGLONG
#define H2_PORT_HAVE_LONGLONG
typedef long long LONGLONG;
#endif
#ifndef H2_PORT_HAVE_REFERENCE_TIME
#define H2_PORT_HAVE_REFERENCE_TIME
typedef LONGLONG REFERENCE_TIME;
typedef REFERENCE_TIME *LPREFERENCE_TIME;
#endif

#ifndef VOID
#define VOID void
#endif
#ifndef CALLBACK
#define CALLBACK __stdcall
#endif
#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE __stdcall
#endif
#ifndef STDMETHOD
#define STDMETHOD(method) virtual HRESULT STDMETHODCALLTYPE method
#endif
#ifndef STDMETHOD_
#define STDMETHOD_(type, method) virtual type STDMETHODCALLTYPE method
#endif
#ifndef PURE
#define PURE = 0
#endif
#ifndef E_NOTIMPL
#define E_NOTIMPL ((HRESULT)0x80004001L)
#endif
#ifndef E_PENDING
#define E_PENDING ((HRESULT)0x8000000AL)
#endif

/* ---- wave formats ---- */

#ifndef H2_PORT_HAVE_WAVEFORMATEX
#define H2_PORT_HAVE_WAVEFORMATEX
/* Public Win32 layout (18 bytes, byte packed); the recovered code clears
 * exactly 0x12 bytes of a wave format. */
#pragma pack(push, 1)
typedef struct tWAVEFORMATEX {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    WORD cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;
#pragma pack(pop)
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;
#endif

/* The stream-format setup (retail 0x21e410) stores wSamplesPerBlock right
 * after the 18-byte header. */
#pragma pack(push, 1)
typedef struct xbox_adpcmwaveformat_tag {
    WAVEFORMATEX wfx;
    WORD wSamplesPerBlock;
} XBOXADPCMWAVEFORMAT, *PXBOXADPCMWAVEFORMAT, *LPXBOXADPCMWAVEFORMAT;
#pragma pack(pop)

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 0x0001
#endif
/* retail 0x21e410 writes 0x0069 as the format tag of ADPCM streams */
#define WAVE_FORMAT_XBOX_ADPCM 0x0069

/* ---- media packets (XMediaObject) ---- */

/* 0x18 bytes, field order from the native stream/codec views */
typedef struct _XMEDIAPACKET {
    LPVOID pvBuffer;
    DWORD dwMaxSize;
    LPDWORD pdwCompletedSize;
    LPDWORD pdwStatus;
    HANDLE hCompletionEvent;
    REFERENCE_TIME *prtTimestamp;
} XMEDIAPACKET, *PXMEDIAPACKET, *LPXMEDIAPACKET;
typedef const XMEDIAPACKET *LPCXMEDIAPACKET;

#define XMEDIAPACKET_STATUS_SUCCESS S_OK          /* retail compares against 0 */
#define XMEDIAPACKET_STATUS_PENDING E_PENDING     /* retail compares against 0x8000000a */

/* stream completion callback (retail 0x220730 is one): stream context,
 * packet context, packet status */
typedef void (CALLBACK *LPFNXMEDIAOBJECTCALLBACK)(LPVOID pStreamContext,
                                                  LPVOID pPacketContext,
                                                  DWORD dwStatus);

/* ---- mix bins ---- */

typedef struct _DSMIXBINVOLUMEPAIR {
    DWORD dwMixBin;
    LONG lVolume;
} DSMIXBINVOLUMEPAIR, *LPDSMIXBINVOLUMEPAIR;
typedef const DSMIXBINVOLUMEPAIR *LPCDSMIXBINVOLUMEPAIR;

typedef struct _DSMIXBINS {
    DWORD dwMixBinCount;
    LPCDSMIXBINVOLUMEPAIR lpMixBinVolumePairs;
} DSMIXBINS, *LPDSMIXBINS;
typedef const DSMIXBINS *LPCDSMIXBINS;

/* ---- buffer and stream descriptions ---- */

/* 0x18 bytes: native s_buffer_description (retail 0x220a70, 0x220b90) */
typedef struct _DSBUFFERDESC {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    LPWAVEFORMATEX lpwfxFormat;
    LPCDSMIXBINS lpMixBins;
    DWORD dwInputMixBin;
} DSBUFFERDESC, *LPDSBUFFERDESC;
typedef const DSBUFFERDESC *LPCDSBUFFERDESC;

/* 0x18 bytes: native DSSTREAMDESC view (retail 0x21f4e0) */
typedef struct _DSSTREAMDESC {
    DWORD dwFlags;
    DWORD dwMaxAttachedPackets;
    LPWAVEFORMATEX lpwfxFormat;
    LPFNXMEDIAOBJECTCALLBACK lpfnCallback;
    LPVOID lpvContext;
    LPCDSMIXBINS lpMixBins;
} DSSTREAMDESC, *LPDSSTREAMDESC;
typedef const DSSTREAMDESC *LPCDSSTREAMDESC;

/* 0x10 bytes: the driver globals keep it at 0x2aa0, the device at 0x2ab0.
 * PROVISIONAL: the recovered code never reads individual fields. */
typedef struct _DSCAPS {
    DWORD adwCaps[4];
} DSCAPS, *LPDSCAPS;

/* What GetVoiceProperties fills. The recovered code reads it as a mix-bin
 * count followed by eight volume pairs (0x44 bytes, as the native port's
 * s_mixbin_list). PROVISIONAL: retail may define further trailing fields. */
typedef struct _DSVOICEPROPS {
    DWORD dwMixBinCount;
    DSMIXBINVOLUMEPAIR MixBinVolumePairs[8];
} DSVOICEPROPS, *LPDSVOICEPROPS;

/* ---- filters, LFOs, envelopes (sizes asserted by the native port) ---- */

/* 0x18 bytes */
typedef struct _DSFILTERDESC {
    DWORD dwMode;
    DWORD dwQCoefficient;
    DWORD adwCoefficients[4];
} DSFILTERDESC, *LPDSFILTERDESC;
typedef const DSFILTERDESC *LPCDSFILTERDESC;

/* retail filter-mode values (src/sound_channel_parameters_native.cpp) */
#define DSFILTER_MODE_BYPASS 0x00000000
#define DSFILTER_MODE_DLS2 0x00000001
#define DSFILTER_MODE_PARAMEQ 0x00000002
#define DSFILTER_MODE_MULTI 0x00000003

/* 0x18 bytes */
typedef struct _DSLFODESC {
    DWORD dwLFO;
    DWORD dwDelay;
    DWORD dwDelta;
    LONG lPitchModulation;
    LONG lFilterCutOffRange;
    LONG lAmplitudeModulation;
} DSLFODESC, *LPDSLFODESC;
typedef const DSLFODESC *LPCDSLFODESC;

/* the recovered code uses LFO 1 for pitch and LFO 0 for the combined one */
#define DSLFO_MULTI 0      /* PROVISIONAL name */
#define DSLFO_PITCH 1      /* PROVISIONAL name */

/* 0x28 bytes; attack at 0x0c, release at 0x18, sustain at 0x1c (native
 * src/sound_stream_envelope_native.cpp) */
typedef struct _DSENVELOPEDESC {
    DWORD dwEG;
    DWORD dwMode;
    DWORD dwDelay;
    DWORD dwAttack;
    DWORD dwHold;
    DWORD dwDecay;
    DWORD dwRelease;
    DWORD dwSustain;
    DWORD dwReserved20;    /* PROVISIONAL: unused by the recovered code */
    DWORD dwReserved24;    /* PROVISIONAL: unused by the recovered code */
} DSENVELOPEDESC, *LPDSENVELOPEDESC;
typedef const DSENVELOPEDESC *LPCDSENVELOPEDESC;

#define DSEG_AMPLITUDE 1        /* retail 0x2ae7a0 */
#define DSEG_MODE_ATTACK 2      /* retail 0x2ae7a0 */

/* ---- I3DL2 ---- */

typedef struct _DSI3DL2OBSTRUCTION {    /* PROVISIONAL type name */
    LONG lHFLevel;
    FLOAT flLFRatio;
} DSI3DL2OBSTRUCTION;

typedef struct _DSI3DL2OCCLUSION {      /* PROVISIONAL type name */
    LONG lHFLevel;
    FLOAT flLFRatio;
} DSI3DL2OCCLUSION;

/* 0x24 bytes, order from native s_i3dl2_buffer */
typedef struct _DSI3DL2BUFFER {
    LONG lDirect;
    LONG lDirectHF;
    LONG lRoom;
    LONG lRoomHF;
    FLOAT flRoomRolloffFactor;
    DSI3DL2OBSTRUCTION Obstruction;
    DSI3DL2OCCLUSION Occlusion;
} DSI3DL2BUFFER, *LPDSI3DL2BUFFER;
typedef const DSI3DL2BUFFER *LPCDSI3DL2BUFFER;

/* 0x30 bytes; the public I3DL2 reverb parameter list, order verified by the
 * native s_i3dl2_reverb_description */
typedef struct _DSI3DL2LISTENER {
    LONG lRoom;
    LONG lRoomHF;
    FLOAT flRoomRolloffFactor;
    FLOAT flDecayTime;
    FLOAT flDecayHFRatio;
    LONG lReflections;
    FLOAT flReflectionsDelay;
    LONG lReverb;
    FLOAT flReverbDelay;
    FLOAT flDiffusion;
    FLOAT flDensity;
    FLOAT flHFReference;
} DSI3DL2LISTENER, *LPDSI3DL2LISTENER;
typedef const DSI3DL2LISTENER *LPCDSI3DL2LISTENER;

/* ---- the DSP effects image ---- */

/* 0x20 bytes per effect; only the state segment (at 0x08) and its size (at
 * 0x0c) are read by the recovered code (native s_effect_map, offset by the
 * 8-byte image header). Other field names are PROVISIONAL. */
typedef struct _DSEFFECTMAP {
    LPVOID lpvCodeSegment;      /* PROVISIONAL */
    DWORD dwCodeSize;           /* PROVISIONAL */
    LPVOID lpvStateSegment;
    DWORD dwStateSize;
    LPVOID lpvYMemorySegment;   /* PROVISIONAL */
    DWORD dwYMemorySize;        /* PROVISIONAL */
    LPVOID lpvScratchSegment;   /* PROVISIONAL */
    DWORD dwScratchSize;        /* PROVISIONAL */
} DSEFFECTMAP, *LPDSEFFECTMAP;

typedef struct _DSEFFECTIMAGEDESC {
    DWORD dwEffectCount;        /* at 0 (retail 0x191420 compares it to 15) */
    DWORD dwTotalScratchSize;   /* PROVISIONAL name */
    DSEFFECTMAP aEffectMaps[1]; /* dwEffectCount entries */
} DSEFFECTIMAGEDESC, *LPDSEFFECTIMAGEDESC;

/* 8 bytes (native s_effect_image_location) */
typedef struct _DSEFFECTIMAGELOC {
    DWORD dwI3DL2ReverbIndex;
    DWORD dwCrosstalkIndex;
} DSEFFECTIMAGELOC, *LPDSEFFECTIMAGELOC;

#define XAUDIO_DOWNLOADFX_XBESECTION 0x00000001   /* retail value */
#define DSFX_IMMEDIATE 0x00000000                 /* retail value */
#define DSFX_DEFERRED 0x00000001                  /* PROVISIONAL */

/* high-level effect parameters passed to XAudioSetEffectData */
#define DSFX_EFFECT_TYPE_I3DL2REVERB 12           /* retail 0x21ece0 */

typedef struct _DSFX_HIGH_LEVEL_EFFECT_DESCRIPTION {
    DWORD effectType;
    union {
        DSI3DL2LISTENER I3DL2Reverb;
    };  /* PROVISIONAL: other effect kinds may enlarge the union */
} DSFX_HIGH_LEVEL_EFFECT_DESCRIPTION, *LPDSFX_HIGH_LEVEL_EFFECT_DESCRIPTION;
typedef const DSFX_HIGH_LEVEL_EFFECT_DESCRIPTION *LPCDSFX_HIGH_LEVEL_EFFECT_DESCRIPTION;

/* ---- flags and values ---- */

/* 3D apply mode (public DirectSound values; native port writes 0 and 1) */
#define DS3D_IMMEDIATE 0x00000000
#define DS3D_DEFERRED 0x00000001

/* buffer creation flags (retail 0x220a70 creates CTRL3D|MIXIN = 0x2010;
 * retail 0x220b90 uses FXIN = 0x1000 and FXIN2 = 0x2000) */
#define DSBCAPS_CTRL3D 0x00000010
#define DSBCAPS_FXIN 0x00001000
#define DSBCAPS_FXIN2 0x00002000
#define DSBCAPS_MIXIN 0x00002000

#define DSBPLAY_LOOPING 0x00000001          /* retail silence buffer */

#define DSBPAUSE_RESUME 0x00000000          /* retail value */
#define DSBPAUSE_PAUSE 0x00000001           /* retail value */

#define DSBSTATUS_PLAYING 0x00000001        /* retail value */

#define DSSTREAMPAUSE_RESUME 0x00000000     /* retail value */
#define DSSTREAMPAUSE_PAUSE 0x00000001      /* retail value */
#define DSSTREAMPAUSE_SYNCHPLAYBACK 0x00000002   /* retail 0x21f430 */

#define DSSTREAMSTATUS_READY 0x00000001     /* retail value */
#define DSSTREAMSTATUS_PAUSED 0x00020000    /* retail 0x2ae4b0 */

/* retail 0x2ae4b0 passes ASYNC|ENVELOPE == 3; the split is PROVISIONAL */
#define DSSTREAMFLUSHEX_ASYNC 0x00000001
#define DSSTREAMFLUSHEX_ENVELOPE 0x00000002

/* ---- interfaces ---- */

#ifdef __cplusplus
struct IDirectSound;
struct IDirectSoundBuffer;
struct IDirectSoundStream;
struct XMediaObject;
#else
typedef struct IDirectSound IDirectSound;
typedef struct IDirectSoundBuffer IDirectSoundBuffer;
typedef struct IDirectSoundStream IDirectSoundStream;
typedef struct XMediaObject XMediaObject;
#endif
typedef struct IDirectSound *LPDIRECTSOUND;
typedef struct IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;
typedef struct IDirectSoundStream *LPDIRECTSOUNDSTREAM;
typedef struct XMediaObject *LPXMEDIAOBJECT;

/* WMA in-memory decoder read callback (wma_codec_read_callback in
 * src/unknown_2ae170.cpp):
 * context, byte offset, byte count, out data pointer; returns bytes read */
typedef DWORD (CALLBACK *LPFNWMAXMODATACALLBACK)(LPVOID pContext, DWORD dwOffset,
                                                DWORD dwByteCount, LPVOID *ppvData);

#ifdef __cplusplus
extern "C" {
#endif

/* Library entry points. Parameter lists follow the recovered call sites and
 * the native port's import declarations; retail argument counts were checked
 * for StopEx (4 dwords) and FlushEx (4 dwords). */
HRESULT WINAPI DirectSoundCreate(const void *pcGuidDevice, LPDIRECTSOUND *ppDS, LPVOID pUnkOuter);
HRESULT WINAPI DirectSoundCreateBuffer(LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer);
void WINAPI DirectSoundDoWork(void);

ULONG WINAPI IDirectSound_Release(LPDIRECTSOUND pDirectSound);
HRESULT WINAPI IDirectSound_GetCaps(LPDIRECTSOUND pDirectSound, LPDSCAPS pdsc);
HRESULT WINAPI IDirectSound_CreateSoundBuffer(LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pdsbd,
                                              LPDIRECTSOUNDBUFFER *ppBuffer, LPVOID pUnkOuter);
HRESULT WINAPI IDirectSound_CreateSoundStream(LPDIRECTSOUND pDirectSound, LPCDSSTREAMDESC pdssd,
                                              LPDIRECTSOUNDSTREAM *ppStream, LPVOID pUnkOuter);
HRESULT WINAPI IDirectSound_GetSpeakerConfig(LPDIRECTSOUND pDirectSound, LPDWORD pdwSpeakerConfig);
HRESULT WINAPI IDirectSound_SetMixBinHeadroom(LPDIRECTSOUND pDirectSound, DWORD dwMixBin, DWORD dwHeadroom);
HRESULT WINAPI IDirectSound_SetEffectData(LPDIRECTSOUND pDirectSound, DWORD dwEffectIndex, DWORD dwOffset,
                                          LPCVOID pvData, DWORD dwDataSize, DWORD dwApply);
HRESULT WINAPI IDirectSound_GetEffectData(LPDIRECTSOUND pDirectSound, DWORD dwEffectIndex, DWORD dwOffset,
                                          LPVOID pvData, DWORD dwDataSize);
HRESULT WINAPI IDirectSound_SetPosition(LPDIRECTSOUND pDirectSound, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
HRESULT WINAPI IDirectSound_SetOrientation(LPDIRECTSOUND pDirectSound, FLOAT xFront, FLOAT yFront, FLOAT zFront,
                                           FLOAT xTop, FLOAT yTop, FLOAT zTop, DWORD dwApply);
HRESULT WINAPI IDirectSound_SetDistanceFactor(LPDIRECTSOUND pDirectSound, FLOAT flDistanceFactor, DWORD dwApply);
HRESULT WINAPI IDirectSound_SetDopplerFactor(LPDIRECTSOUND pDirectSound, FLOAT flDopplerFactor, DWORD dwApply);
HRESULT WINAPI IDirectSound_SetRolloffFactor(LPDIRECTSOUND pDirectSound, FLOAT flRolloffFactor, DWORD dwApply);
HRESULT WINAPI IDirectSound_CommitDeferredSettings(LPDIRECTSOUND pDirectSound);
/* retail 0x21f430 tail-calls it with the DirectSound object only */
HRESULT WINAPI IDirectSound_SynchPlayback(LPDIRECTSOUND pDirectSound);

ULONG WINAPI IDirectSoundBuffer_Release(LPDIRECTSOUNDBUFFER pBuffer);
HRESULT WINAPI IDirectSoundBuffer_SetBufferData(LPDIRECTSOUNDBUFFER pBuffer, LPVOID pvBufferData, DWORD dwBufferBytes);
HRESULT WINAPI IDirectSoundBuffer_SetCurrentPosition(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwPlayCursor);
HRESULT WINAPI IDirectSoundBuffer_Lock(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwOffset, DWORD dwBytes,
                                       LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
                                       LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);
HRESULT WINAPI IDirectSoundBuffer_Play(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwReserved1, DWORD dwReserved2, DWORD dwFlags);
HRESULT WINAPI IDirectSoundBuffer_Pause(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwPause);
HRESULT WINAPI IDirectSoundBuffer_StopEx(LPDIRECTSOUNDBUFFER pBuffer, REFERENCE_TIME rtTimeStamp, DWORD dwFlags);
HRESULT WINAPI IDirectSoundBuffer_GetStatus(LPDIRECTSOUNDBUFFER pBuffer, LPDWORD pdwStatus);
HRESULT WINAPI IDirectSoundBuffer_SetVolume(LPDIRECTSOUNDBUFFER pBuffer, LONG lVolume);
HRESULT WINAPI IDirectSoundBuffer_SetHeadroom(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwHeadroom);
HRESULT WINAPI IDirectSoundBuffer_SetMixBins(LPDIRECTSOUNDBUFFER pBuffer, LPCDSMIXBINS pMixBins);
HRESULT WINAPI IDirectSoundBuffer_SetMixBinVolumes(LPDIRECTSOUNDBUFFER pBuffer, LPCDSMIXBINS pMixBins);
HRESULT WINAPI IDirectSoundBuffer_SetFilter(LPDIRECTSOUNDBUFFER pBuffer, LPCDSFILTERDESC pFilterDesc);
HRESULT WINAPI IDirectSoundBuffer_SetI3DL2Source(LPDIRECTSOUNDBUFFER pBuffer, LPCDSI3DL2BUFFER pds3db, DWORD dwApply);
HRESULT WINAPI IDirectSoundBuffer_SetPosition(LPDIRECTSOUNDBUFFER pBuffer, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
HRESULT WINAPI IDirectSoundBuffer_SetMaxDistance(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flMaxDistance, DWORD dwApply);
HRESULT WINAPI IDirectSoundBuffer_SetMinDistance(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flMinDistance, DWORD dwApply);
HRESULT WINAPI IDirectSoundBuffer_SetRolloffFactor(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flRolloffFactor, DWORD dwApply);
HRESULT WINAPI IDirectSoundBuffer_SetDopplerFactor(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flDopplerFactor, DWORD dwApply);
HRESULT WINAPI IDirectSoundBuffer_SetConeOutsideVolume(LPDIRECTSOUNDBUFFER pBuffer, LONG lConeOutsideVolume, DWORD dwApply);
HRESULT WINAPI IDirectSoundBuffer_GetVoiceProperties(LPDIRECTSOUNDBUFFER pBuffer, LPDSVOICEPROPS pVoiceProps);

ULONG WINAPI IDirectSoundStream_Release(LPDIRECTSOUNDSTREAM pStream);
HRESULT WINAPI IDirectSoundStream_GetStatus(LPDIRECTSOUNDSTREAM pStream, LPDWORD pdwStatus);
HRESULT WINAPI IDirectSoundStream_Process(LPDIRECTSOUNDSTREAM pStream, LPXMEDIAPACKET pInputPacket,
                                          LPXMEDIAPACKET pOutputPacket);
HRESULT WINAPI IDirectSoundStream_Discontinuity(LPDIRECTSOUNDSTREAM pStream);
HRESULT WINAPI IDirectSoundStream_Flush(LPDIRECTSOUNDSTREAM pStream);
HRESULT WINAPI IDirectSoundStream_FlushEx(LPDIRECTSOUNDSTREAM pStream, REFERENCE_TIME rtTimeStamp, DWORD dwFlags);
HRESULT WINAPI IDirectSoundStream_Pause(LPDIRECTSOUNDSTREAM pStream, DWORD dwPause);
HRESULT WINAPI IDirectSoundStream_SetHeadroom(LPDIRECTSOUNDSTREAM pStream, DWORD dwHeadroom);
HRESULT WINAPI IDirectSoundStream_SetVolume(LPDIRECTSOUNDSTREAM pStream, LONG lVolume);
HRESULT WINAPI IDirectSoundStream_SetPitch(LPDIRECTSOUNDSTREAM pStream, LONG lPitch);
HRESULT WINAPI IDirectSoundStream_SetMixBins(LPDIRECTSOUNDSTREAM pStream, LPCDSMIXBINS pMixBins);
HRESULT WINAPI IDirectSoundStream_SetMixBinVolumes(LPDIRECTSOUNDSTREAM pStream, LPCDSMIXBINS pMixBins);
HRESULT WINAPI IDirectSoundStream_SetFilter(LPDIRECTSOUNDSTREAM pStream, LPCDSFILTERDESC pFilterDesc);
HRESULT WINAPI IDirectSoundStream_SetLFO(LPDIRECTSOUNDSTREAM pStream, LPCDSLFODESC pLFODesc);
HRESULT WINAPI IDirectSoundStream_SetEG(LPDIRECTSOUNDSTREAM pStream, LPCDSENVELOPEDESC pEnvelopeDesc);
HRESULT WINAPI IDirectSoundStream_SetOutputBuffer(LPDIRECTSOUNDSTREAM pStream, LPDIRECTSOUNDBUFFER pOutputBuffer);
HRESULT WINAPI IDirectSoundStream_GetVoiceProperties(LPDIRECTSOUNDSTREAM pStream, LPDSVOICEPROPS pVoiceProps);

HRESULT WINAPI XAudioDownloadEffectsImage(LPCSTR pszImageName, LPDSEFFECTIMAGELOC pEffectLoc,
                                          DWORD dwFlags, LPDSEFFECTIMAGEDESC *ppImageDesc);
/* the third argument is always NULL in the recovered code: PROVISIONAL */
HRESULT WINAPI XAudioSetEffectData(DWORD dwEffectIndex, LPCDSFX_HIGH_LEVEL_EFFECT_DESCRIPTION pDesc,
                                   LPVOID pvReserved);

HRESULT WINAPI WmaCreateInMemoryDecoder(LPFNWMAXMODATACALLBACK pfnCallback, LPVOID pvContext,
                                        DWORD dwYieldRate, LPWAVEFORMATEX pwfxCompressed,
                                        LPXMEDIAOBJECT *ppMediaObject);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* A media object: called through its vtable. Retail slots: 1 Release
 * (0x2ae170, +0x04), 3 GetStatus (0x2ae4b0, +0x0c), 4 Process (0x2ae820,
 * +0x10), 5 Discontinuity (0x2ae820, +0x14), 6 Flush (0x2ae6f0, +0x18).
 * Slots 0 and 2 are not called by the recovered code: PROVISIONAL. */
struct XMediaObject
{
    STDMETHOD_(ULONG, AddRef)() PURE;                           /* slot 0, PROVISIONAL */
    STDMETHOD_(ULONG, Release)() PURE;                          /* slot 1 */
    STDMETHOD(GetInfo)(LPVOID pInfo) PURE;                      /* slot 2, PROVISIONAL */
    STDMETHOD(GetStatus)(LPDWORD pdwStatus) PURE;               /* slot 3 */
    STDMETHOD(Process)(LPXMEDIAPACKET pInputPacket, LPXMEDIAPACKET pOutputPacket) PURE; /* slot 4 */
    STDMETHOD(Discontinuity)() PURE;                            /* slot 5 */
    STDMETHOD(Flush)() PURE;                                    /* slot 6 */
};

/* A DirectSound stream is a media object; its DirectSound-specific methods
 * are direct calls to the IDirectSoundStream_* entry points (retail 0x2201f0
 * and 0x21fa80 call them directly). */
struct IDirectSoundStream : public XMediaObject
{
    HRESULT FlushEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags) { return IDirectSoundStream_FlushEx(this, rtTimeStamp, dwFlags); }
    HRESULT Pause(DWORD dwPause) { return IDirectSoundStream_Pause(this, dwPause); }
    HRESULT SetHeadroom(DWORD dwHeadroom) { return IDirectSoundStream_SetHeadroom(this, dwHeadroom); }
    HRESULT SetVolume(LONG lVolume) { return IDirectSoundStream_SetVolume(this, lVolume); }
    HRESULT SetPitch(LONG lPitch) { return IDirectSoundStream_SetPitch(this, lPitch); }
    HRESULT SetMixBins(LPCDSMIXBINS pMixBins) { return IDirectSoundStream_SetMixBins(this, pMixBins); }
    HRESULT SetMixBinVolumes(LPCDSMIXBINS pMixBins) { return IDirectSoundStream_SetMixBinVolumes(this, pMixBins); }
    HRESULT SetFilter(LPCDSFILTERDESC pFilterDesc) { return IDirectSoundStream_SetFilter(this, pFilterDesc); }
    HRESULT SetLFO(LPCDSLFODESC pLFODesc) { return IDirectSoundStream_SetLFO(this, pLFODesc); }
    HRESULT SetEG(LPCDSENVELOPEDESC pEnvelopeDesc) { return IDirectSoundStream_SetEG(this, pEnvelopeDesc); }
    HRESULT SetOutputBuffer(LPDIRECTSOUNDBUFFER pOutputBuffer) { return IDirectSoundStream_SetOutputBuffer(this, pOutputBuffer); }
    HRESULT GetVoiceProperties(LPDSVOICEPROPS pVoiceProps) { return IDirectSoundStream_GetVoiceProperties(this, pVoiceProps); }
};

/* The DirectSound object and buffers: every method is a direct call (retail
 * 0x21e4b0, 0x21fa80, 0x21eae0), so these carry no vtable. */
struct IDirectSound
{
    ULONG Release() { return IDirectSound_Release(this); }
    HRESULT GetCaps(LPDSCAPS pdsc) { return IDirectSound_GetCaps(this, pdsc); }
    HRESULT CreateSoundBuffer(LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer, LPVOID pUnkOuter)
    { return IDirectSound_CreateSoundBuffer(this, pdsbd, ppBuffer, pUnkOuter); }
    HRESULT CreateSoundStream(LPCDSSTREAMDESC pdssd, LPDIRECTSOUNDSTREAM *ppStream, LPVOID pUnkOuter)
    { return IDirectSound_CreateSoundStream(this, pdssd, ppStream, pUnkOuter); }
    HRESULT GetSpeakerConfig(LPDWORD pdwSpeakerConfig) { return IDirectSound_GetSpeakerConfig(this, pdwSpeakerConfig); }
    HRESULT SetMixBinHeadroom(DWORD dwMixBin, DWORD dwHeadroom) { return IDirectSound_SetMixBinHeadroom(this, dwMixBin, dwHeadroom); }
    HRESULT SetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPCVOID pvData, DWORD dwDataSize, DWORD dwApply)
    { return IDirectSound_SetEffectData(this, dwEffectIndex, dwOffset, pvData, dwDataSize, dwApply); }
    HRESULT GetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPVOID pvData, DWORD dwDataSize)
    { return IDirectSound_GetEffectData(this, dwEffectIndex, dwOffset, pvData, dwDataSize); }
    HRESULT SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) { return IDirectSound_SetPosition(this, x, y, z, dwApply); }
    HRESULT SetOrientation(FLOAT xFront, FLOAT yFront, FLOAT zFront, FLOAT xTop, FLOAT yTop, FLOAT zTop, DWORD dwApply)
    { return IDirectSound_SetOrientation(this, xFront, yFront, zFront, xTop, yTop, zTop, dwApply); }
    HRESULT SetDistanceFactor(FLOAT flDistanceFactor, DWORD dwApply) { return IDirectSound_SetDistanceFactor(this, flDistanceFactor, dwApply); }
    HRESULT SetDopplerFactor(FLOAT flDopplerFactor, DWORD dwApply) { return IDirectSound_SetDopplerFactor(this, flDopplerFactor, dwApply); }
    HRESULT SetRolloffFactor(FLOAT flRolloffFactor, DWORD dwApply) { return IDirectSound_SetRolloffFactor(this, flRolloffFactor, dwApply); }
    HRESULT CommitDeferredSettings() { return IDirectSound_CommitDeferredSettings(this); }
    HRESULT SynchPlayback() { return IDirectSound_SynchPlayback(this); }
};

struct IDirectSoundBuffer
{
    ULONG Release() { return IDirectSoundBuffer_Release(this); }
    HRESULT SetBufferData(LPVOID pvBufferData, DWORD dwBufferBytes) { return IDirectSoundBuffer_SetBufferData(this, pvBufferData, dwBufferBytes); }
    HRESULT SetCurrentPosition(DWORD dwPlayCursor) { return IDirectSoundBuffer_SetCurrentPosition(this, dwPlayCursor); }
    HRESULT Lock(DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
                 LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags)
    { return IDirectSoundBuffer_Lock(this, dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags); }
    HRESULT Play(DWORD dwReserved1, DWORD dwReserved2, DWORD dwFlags) { return IDirectSoundBuffer_Play(this, dwReserved1, dwReserved2, dwFlags); }
    HRESULT Pause(DWORD dwPause) { return IDirectSoundBuffer_Pause(this, dwPause); }
    HRESULT StopEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags) { return IDirectSoundBuffer_StopEx(this, rtTimeStamp, dwFlags); }
    HRESULT GetStatus(LPDWORD pdwStatus) { return IDirectSoundBuffer_GetStatus(this, pdwStatus); }
    HRESULT SetVolume(LONG lVolume) { return IDirectSoundBuffer_SetVolume(this, lVolume); }
    HRESULT SetHeadroom(DWORD dwHeadroom) { return IDirectSoundBuffer_SetHeadroom(this, dwHeadroom); }
    HRESULT SetMixBins(LPCDSMIXBINS pMixBins) { return IDirectSoundBuffer_SetMixBins(this, pMixBins); }
    HRESULT SetMixBinVolumes(LPCDSMIXBINS pMixBins) { return IDirectSoundBuffer_SetMixBinVolumes(this, pMixBins); }
    HRESULT SetFilter(LPCDSFILTERDESC pFilterDesc) { return IDirectSoundBuffer_SetFilter(this, pFilterDesc); }
    HRESULT SetI3DL2Source(LPCDSI3DL2BUFFER pds3db, DWORD dwApply) { return IDirectSoundBuffer_SetI3DL2Source(this, pds3db, dwApply); }
    HRESULT SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) { return IDirectSoundBuffer_SetPosition(this, x, y, z, dwApply); }
    HRESULT SetMaxDistance(FLOAT flMaxDistance, DWORD dwApply) { return IDirectSoundBuffer_SetMaxDistance(this, flMaxDistance, dwApply); }
    HRESULT SetMinDistance(FLOAT flMinDistance, DWORD dwApply) { return IDirectSoundBuffer_SetMinDistance(this, flMinDistance, dwApply); }
    HRESULT SetRolloffFactor(FLOAT flRolloffFactor, DWORD dwApply) { return IDirectSoundBuffer_SetRolloffFactor(this, flRolloffFactor, dwApply); }
    HRESULT SetDopplerFactor(FLOAT flDopplerFactor, DWORD dwApply) { return IDirectSoundBuffer_SetDopplerFactor(this, flDopplerFactor, dwApply); }
    HRESULT SetConeOutsideVolume(LONG lConeOutsideVolume, DWORD dwApply) { return IDirectSoundBuffer_SetConeOutsideVolume(this, lConeOutsideVolume, dwApply); }
    HRESULT GetVoiceProperties(LPDSVOICEPROPS pVoiceProps) { return IDirectSoundBuffer_GetVoiceProperties(this, pVoiceProps); }
};

static_assert(sizeof(WAVEFORMATEX) == 0x12, "public wave format layout");
static_assert(sizeof(XBOXADPCMWAVEFORMAT) == 0x14, "retail ADPCM format layout");
#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(XMEDIAPACKET) == 0x18, "retail media packet layout");
#endif
static_assert(sizeof(DSMIXBINVOLUMEPAIR) == 0x08, "retail mix bin pair layout");
#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(DSMIXBINS) == 0x08, "retail mix bin list layout");
#endif
#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(DSBUFFERDESC) == 0x18, "retail buffer description layout");
#endif
#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(DSSTREAMDESC) == 0x18, "retail stream description layout");
#endif
static_assert(sizeof(DSCAPS) == 0x10, "retail caps layout");
static_assert(sizeof(DSVOICEPROPS) == 0x44, "recovered voice properties view");
static_assert(sizeof(DSFILTERDESC) == 0x18, "retail filter description layout");
static_assert(sizeof(DSLFODESC) == 0x18, "retail LFO description layout");
static_assert(sizeof(DSENVELOPEDESC) == 0x28, "retail envelope description layout");
static_assert(__builtin_offsetof(DSENVELOPEDESC, dwAttack) == 0x0c, "retail envelope attack offset");
static_assert(__builtin_offsetof(DSENVELOPEDESC, dwRelease) == 0x18, "retail envelope release offset");
static_assert(__builtin_offsetof(DSENVELOPEDESC, dwSustain) == 0x1c, "retail envelope sustain offset");
static_assert(sizeof(DSI3DL2BUFFER) == 0x24, "retail I3DL2 source layout");
static_assert(sizeof(DSI3DL2LISTENER) == 0x30, "retail I3DL2 reverb layout");
#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(DSEFFECTMAP) == 0x20, "retail effect map stride");
#endif
#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(DSEFFECTIMAGEDESC, aEffectMaps) + __builtin_offsetof(DSEFFECTMAP, lpvStateSegment) == 0x10,
              "retail effect state segment offset");
#endif
static_assert(sizeof(DSEFFECTIMAGELOC) == 0x08, "retail effect image location layout");

#endif /* __cplusplus */

#endif
