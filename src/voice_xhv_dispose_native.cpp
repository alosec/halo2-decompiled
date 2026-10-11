// Exact XHV teardown isolated from network_voice.cpp, whose broad recovered
// source depends on unavailable Xbox voice and online headers.  All voice
// engine operations remain genuine XHV method imports.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

#pragma pack(push, 4)
struct _XUID
{
	long long qwUserID;
	dword dwUserFlags;
};
#pragma pack(pop)

namespace XHVNamespace
{
struct CXHVEngine;
}

extern "C" long xhv_unregister_local_talker(
	XHVNamespace::CXHVEngine *engine, dword port)
	__asm__("?UnregisterLocalTalker@CXHVEngine@XHVNamespace@@QAGJK@Z");
extern "C" long xhv_unregister_remote_talker(
	XHVNamespace::CXHVEngine *engine, _XUID user)
	__asm__("?UnregisterRemoteTalker@CXHVEngine@XHVNamespace@@QAGJU_XUID@@@Z");
extern "C" long xhv_get_remote_talkers(
	XHVNamespace::CXHVEngine *engine, dword *count, _XUID *users)
	__asm__("?GetRemoteTalkers@CXHVEngine@XHVNamespace@@QAGJPAKPAU_XUID@@@Z");
extern "C" dword xhv_release(XHVNamespace::CXHVEngine *engine)
	__asm__("?Release@CXHVEngine@XHVNamespace@@QAGKXZ");

class c_voice_xhv
{
public:
	void *title_vtable;
	bool initialized;
	byte unknown05[3];
	long mode;
	long unknown0c;
	XHVNamespace::CXHVEngine *engine;
};

static_assert(sizeof(_XUID) == 0x0c, "XUID size");
static_assert(offsetof(c_voice_xhv, initialized) == 0x04,
	"voice XHV initialized offset");
static_assert(offsetof(c_voice_xhv, unknown0c) == 0x0c,
	"voice XHV flags offset");
static_assert(offsetof(c_voice_xhv, engine) == 0x10,
	"voice XHV engine offset");

// @retail 0x552a0
void voice_xhv_unregister_remote_talkers(c_voice_xhv *xhv)
{
	if (xhv->initialized && (xhv->unknown0c & 4))
	{
		dword count;
		_XUID talkers[30];
		if (xhv_get_remote_talkers(xhv->engine, &count, talkers) >= 0)
		{
			for (dword i = 0; i < count; i++)
			{
				if (xhv->initialized && (xhv->unknown0c & 4))
				{
					_XUID user;
					user.qwUserID = static_cast<long>(talkers[i].qwUserID);
					user.dwUserFlags = 0;
					xhv_unregister_remote_talker(xhv->engine, user);
				}
			}
		}
	}
}

// @retail 0x550b0
void voice_xhv_dispose(c_voice_xhv *xhv)
{
	if (xhv->initialized)
	{
		for (dword port = 0; port < 4; port++)
			xhv_unregister_local_talker(xhv->engine, port);
		if (xhv->unknown0c & 4)
			voice_xhv_unregister_remote_talkers(xhv);
		if (xhv->engine)
		{
			xhv_release(xhv->engine);
			xhv->engine = 0;
		}
	}
	xhv->initialized = false;
}

#endif
