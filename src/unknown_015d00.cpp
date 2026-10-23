// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_015D00.CPP: reserving room in the D3D push buffer (built with SSE,
   apart from its neighbours in unknown_0158f0.cpp) */

#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"

namespace D3D
{
	struct CDevice
	{
		void LazySetStateVB(dword value);
		static dword m_PushSegmentSize;
	};

	dword volatile *WINAPI MakeRequestedSpace(dword fallback_size, dword requested_size);
}

/* retail calls it out of line */
__declspec(noinline) bool function_015d00(dword **out, long count);

// @retail 0x15d00
bool function_015d00(dword **out, long count)
{
	bool result = true;
	if (count < 0x800)
	{
		((D3D::CDevice *)D3D__pDevice)->LazySetStateVB(0);
		dword *const *fields = (dword *const *)D3D__pDevice;
		dword *push;
		if (fields[0] + count + 1 < fields[1] + 0x80)
			push = fields[0];
		else
		{
			dword fallback_size = D3D::CDevice::m_PushSegmentSize;
			dword full_size = fallback_size;
			dword requested_size = count * 4 + 0x208;
			fallback_size >>= 1;
			fallback_size = requested_size > fallback_size ? requested_size : fallback_size;
			full_size = requested_size > full_size ? requested_size : full_size;
			push = (dword *)D3D::MakeRequestedSpace(fallback_size, full_size);
		}
		*out = push;
	}
	else
	{
		*out = NULL;
		result = false;
	}
	return result;
}
