/* one texture stage state set through the device's inline setter */

#ifndef SAMPLER_STATE_H
#define SAMPLER_STATE_H

#include "unknown_11c920.h"
#include <xtl.h>

/* the type comes by reference, so each call keeps its own update of the
   device's dirty flags instead of sharing one with the calls around it */
PRIVATE __forceinline void sampler_state_write(dword stage, D3DTEXTURESTAGESTATETYPE const &type, dword value)
{
    D3DDevice_SetTextureStageState(stage, type, value);
}

#endif
