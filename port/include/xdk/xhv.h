#ifndef H2_PORT_XDK_XHV_H
#define H2_PORT_XDK_XHV_H

/* Clean-room declarations of the Xbox voice (XHV) engine interface used by
 * the recovered voice-chat code (src/network_voice.cpp and its users).
 *
 * Provenance: nothing here is copied from a Microsoft Xbox SDK/XDK header,
 * leaked material or GPL emulator code. Declarations come from:
 *   - the recovered sources (method names and arguments at the call sites in
 *     src/network_voice.cpp, field names they access);
 *   - the native replacements src/voice_xhv_lifetime_native.cpp and
 *     src/voice_xhv_dispose_native.cpp (structure sizes asserted there, the
 *     retail callback vtable order at 0x45095c, and the parameter types
 *     encoded in the decorated names of the engine entry points they import);
 *   - data read from the retail executable (the voice masks at 0x43fe48).
 * Anything not verified against the retail executable is marked
 * PROVISIONAL. */

#include <xtl.h>      /* base types, and DirectSound (DSMIXBINS) via dsound.h */
#include <dsound.h>
#include <xonline.h>  /* XUID */

/* processing modes are opaque pointers (decorated as PAX by the engine) */
typedef void *XHV_PROCESSING_MODE;

/* PROVISIONAL: the recovered code only casts 0 and -1 to it */
typedef DWORD XHV_PLAYBACK_PRIORITY;

typedef enum _XHV_VOICE_COMMUNICATOR_STATUS {
    XHV_VOICE_COMMUNICATOR_STATUS_INSERTED = 0,   /* native port value */
    XHV_VOICE_COMMUNICATOR_STATUS_REMOVED = 1     /* native port value */
} XHV_VOICE_COMMUNICATOR_STATUS;

/* PROVISIONAL size: only the communicator status is read */
typedef struct _XHV_LOCAL_TALKER_STATUS {
    XHV_VOICE_COMMUNICATOR_STATUS communicatorStatus;
} XHV_LOCAL_TALKER_STATUS, *PXHV_LOCAL_TALKER_STATUS;

/* 0x10 bytes. PROVISIONAL field order: the recovered code assigns all four
 * fields the same value, so only the size is verified; the order follows the
 * native port's view. */
typedef struct _XHV_VOICE_MASK {
    FLOAT fRoboticValue;
    FLOAT fWhisperValue;
    FLOAT fPitchScale;
    FLOAT fSpecEnergyWeight;
} XHV_VOICE_MASK, *PXHV_VOICE_MASK;

#define XHV_VOICE_MASK_PARAM_DISABLED (-1.0f)     /* native port value */
/* Positional initializers matching the retail table at 0x43fe48 (the bytes,
 * not the meaning of each position, are verified). */
#define XHV_VOICE_MASK_NONE { -1.0f, -1.0f, -1.0f, -1.0f }
#define XHV_VOICE_MASK_ANONYMOUS { 0.95f, 0.5f, 0.35f, -1.0f }

/* 0x24 bytes, order from the native port's view */
typedef struct _XHV_RUNTIME_PARAMS {
    DWORD dwMaxRemoteTalkers;
    DWORD dwMaxLocalTalkers;
    DWORD dwMaxCompressedBuffers;
    DWORD dwFlags;
    LPDSEFFECTIMAGEDESC pEffectImageDesc;
    DWORD dwEffectsStartIndex;
    DWORD dwOutOfSyncThreshold;
    BOOL bCustomVADProvided;
    BOOL bHeadphoneAlwaysOn;
} XHV_RUNTIME_PARAMS, *PXHV_RUNTIME_PARAMS;

/* speech recognition results: never inspected by the title (opaque) */
struct XHV_SR_ITEM;

#ifdef __cplusplus

/* The title's callback interface. Slot order is the retail vtable at
 * 0x45095c (see src/voice_xhv_lifetime_native.cpp). */
struct ITitleXHV
{
    STDMETHOD(LocalChatDataReady)(DWORD dwPort, DWORD dwSize, VOID *pData) PURE;                  /* slot 0 */
    STDMETHOD(CommunicatorStatusUpdate)(DWORD dwPort, XHV_VOICE_COMMUNICATOR_STATUS status) PURE; /* slot 1 */
    STDMETHOD(SpeechRecognized)(DWORD dwPort, XHV_SR_ITEM *pItems, DWORD dwItemCount) PURE;       /* slot 2 */
    STDMETHOD(MicrophoneRawDataReady)(DWORD dwPort, DWORD dwSize, VOID *pData,
                                      BOOL *pVoiceDetected) PURE;                                 /* slot 3 */
    STDMETHOD(VoiceMailDataReady)(DWORD dwPort, DWORD dwDuration, DWORD dwSize) PURE;             /* slot 4 */
    STDMETHOD(VoiceMailStopped)(DWORD dwPort) PURE;                                               /* slot 5 */
};

namespace XHVNamespace {

/* The engine: non-virtual __stdcall members (the native port imports them
 * by their decorated names). Signatures marked verified come from those
 * decorations; the rest follow the recovered call sites (PROVISIONAL). */
struct CXHVEngine
{
    ULONG __stdcall Release();                                                   /* verified */
    HRESULT __stdcall EnableProcessingMode(XHV_PROCESSING_MODE mode);            /* verified */
    HRESULT __stdcall SetMaxPlaybackStreamsCount(DWORD dwCount);                 /* verified */
    HRESULT __stdcall RegisterLocalTalker(DWORD dwPort);                         /* verified */
    HRESULT __stdcall UnregisterLocalTalker(DWORD dwPort);                       /* verified */
    HRESULT __stdcall SetProcessingMode(DWORD dwPort, XHV_PROCESSING_MODE mode); /* verified */
    HRESULT __stdcall GetLocalTalkerStatus(DWORD dwPort,
                                           XHV_LOCAL_TALKER_STATUS *pStatus);   /* verified */
    HRESULT __stdcall GetRemoteTalkers(DWORD *pdwCount, XUID *pXuids);           /* verified */
    HRESULT __stdcall UnregisterRemoteTalker(XUID xuid);                         /* verified */
    HRESULT __stdcall RegisterRemoteTalker(XUID xuid);                           /* PROVISIONAL */
    BOOL __stdcall IsTalking(XUID xuid);                                         /* PROVISIONAL */
    HRESULT __stdcall SetVoiceMask(DWORD dwPort, const XHV_VOICE_MASK *pMask);   /* PROVISIONAL */
    HRESULT __stdcall SetMixBinMapping(XUID xuid, DWORD dwPort,
                                       const DSMIXBINS *pMixBins);               /* PROVISIONAL */
    HRESULT __stdcall SetPlaybackPriority(XUID xuid, DWORD dwPort,
                                          XHV_PLAYBACK_PRIORITY priority);       /* PROVISIONAL */
    HRESULT __stdcall SubmitIncomingVoicePacket(XUID xuid, VOID *pvData,
                                                DWORD dwSize);                   /* PROVISIONAL */
    HRESULT __stdcall VoiceMailPlay(DWORD dwPort, DWORD dwSize, const BYTE *pbData,
                                    BOOL fForce);                                /* PROVISIONAL */
    HRESULT __stdcall VoiceMailRecord(DWORD dwPort, DWORD dwMaxTime, DWORD dwBufferSize,
                                      BYTE *pbBuffer);                           /* PROVISIONAL */
    HRESULT __stdcall VoiceMailStop(DWORD dwPort);                               /* PROVISIONAL */
    HRESULT __stdcall DoWork();                                                  /* PROVISIONAL */

    /* a plain entry point taking the engine (XHVEngine_SetCallbackInterface) */
    inline HRESULT SetCallbackInterface(ITitleXHV *pCallback);
};

} /* namespace XHVNamespace */

typedef XHVNamespace::CXHVEngine XHVEngine;
typedef XHVEngine *PXHVENGINE;
typedef XHVEngine *LPXHVENGINE;

extern "C" {
HRESULT WINAPI XHVEngineCreate(XHV_RUNTIME_PARAMS *pParams, XHVEngine **ppEngine);
HRESULT WINAPI XHVEngine_SetCallbackInterface(XHVEngine *pEngine, ITitleXHV *pCallback);
}

inline HRESULT XHVNamespace::CXHVEngine::SetCallbackInterface(ITitleXHV *pCallback)
{
    return XHVEngine_SetCallbackInterface(this, pCallback);
}

static_assert(sizeof(XHV_VOICE_MASK) == 0x10, "retail voice mask layout");
#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(XHV_RUNTIME_PARAMS) == 0x24, "retail runtime parameter layout");
#endif

#endif /* __cplusplus */

#endif
