// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_013450.CPP: depth range setup */

#include "unknown_11c920.h"
#include "globals.h"
#include "frame_storage.h"
#include "depth_parameters_storage.h"
#include <xtl.h>

struct s_cache_record;
struct s_cache_record_state
{
    long count;
    dword unknown04;
    s_cache_record *records;
    void *buffer;
    byte unknown10;
    bool available;
    byte unknown12[2];
};

extern s_cache_record_state g_4b6280;
extern real g_485adc, g_485ae0;
extern long g_4858b8;
extern bool g_485a74;
extern dword g_4b8344;

void function_14bc0(short index, short element, bool use_depth);
void function_14ac0(void);

#if 0 // Draft (also defines g_4b893c, which nothing else defines yet), 477 bytes (retail 474). Writing it costs matched 0x14ac0 (LTCG stack
// alignment: this body gets a 'push ecx' pad and 0x14ac0's frame grows 0x30 -> 0x34).
real g_4b893c;

// Retail 0x13450
bool function_13450(s_depth_parameters_13450 const *parameters)
{
    real high = g_485ad4.hi;
    real inverse = 1.0f / high;
    g_4850c4 = *parameters;
    real lower = inverse * g_485ad4.lo * 16777215.0f;
    g_485a74 = false;
    if (lower < 0.0f) lower = 0.0f;
    else if (lower > 16777215.0f) lower = 16777215.0f;
    g_485adc = lower;
    real upper = inverse * high * 16777215.0f;
    if (upper < 0.0f) upper = 0.0f;
    else if (upper > 16777215.0f) upper = 16777215.0f;
    g_485ae0 = upper;
    if (parameters->mode == 1 || parameters->mode == 2)
    {
        if (parameters->mode == 1)
        {
            real distance = parameters->distance * inverse * 16777215.0f;
            if (distance < 0.0f) distance = 0.0f;
            else if (distance > 16777215.0f) distance = 16777215.0f;
            g_485adc = distance;
        }
        else if (parameters->mode == 2)
        {
            real distance = parameters->distance * inverse * 16777215.0f;
            if (distance < 0.0f) distance = 0.0f;
            else if (distance > 16777215.0f) distance = 16777215.0f;
            g_485ae0 = distance;
            g_4858b8 = 18;
            function_14bc0(18, 0, true);
            if (g_485607 && !g_4858b8) function_14ac0();
            D3DDevice_Clear(0, 0, 0xf3, 0, 1.0f, 0);
        }
    }
    D3DDevice_SetDepthClipPlanes(g_485adc, g_485ae0, D3DSDCP_SET_VERTEXPROGRAM_PLANES);
    g_4b8344 = 0;
    D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0);
    g_4b6280.count = 0;
    g_4b6280.unknown04 = 0;
    g_4b6280.unknown10 = true;
    g_4b6280.available = true;
    g_4b893c = 0.0f;
    return true;
}
#endif
