/* Controllers for whole images: the host session (port/src/main.cpp) maps the
 * keyboard, mouse and a gamepad onto Xbox port 0; memory units and voice
 * peripherals are never present. */
#include "h2/host_abi.h"
#include <xtl.h>
#include <string.h>

struct h2_native_device_type
{
    DWORD kind;
};

XPP_DEVICE_TYPE h2_native_device_type_gamepad = {1};
XPP_DEVICE_TYPE h2_native_device_type_memory_unit = {2};
XPP_DEVICE_TYPE h2_native_device_type_voice_microphone = {3};
XPP_DEVICE_TYPE h2_native_device_type_voice_headphone = {4};

namespace {

constexpr DWORD k_error_success = 0;
constexpr DWORD k_error_device_not_connected = 1167;
constexpr unsigned int k_input_handle_base = 0x4b000000u;

DWORD g_reported_gamepads;

} // namespace

extern "C" void WINAPI XInitDevices(DWORD type_count, const XDEVICE_PREALLOC_TYPE *types)
{
    (void)type_count;
    (void)types;
}

extern "C" BOOL WINAPI XGetDeviceChanges(PXPP_DEVICE_TYPE type, DWORD *insertions, DWORD *removals)
{
    *insertions = 0;
    *removals = 0;
    if (type != XDEVICE_TYPE_GAMEPAD)
        return FALSE;
    const DWORD connected = host_h2_input_connected();
    *insertions = connected & ~g_reported_gamepads;
    *removals = g_reported_gamepads & ~connected;
    g_reported_gamepads = connected;
    return *insertions || *removals;
}

extern "C" HANDLE WINAPI XInputOpen(PXPP_DEVICE_TYPE type, DWORD port, DWORD slot, void *parameters)
{
    (void)slot;
    (void)parameters;
    if (type != XDEVICE_TYPE_GAMEPAD || port > 3 || !(host_h2_input_connected() & (1u << port)))
        return nullptr;
    return reinterpret_cast<HANDLE>(k_input_handle_base + port);
}

extern "C" void WINAPI XInputClose(HANDLE device)
{
    (void)device;
}

extern "C" DWORD WINAPI XInputGetCapabilities(HANDLE device, XINPUT_CAPABILITIES *capabilities)
{
    (void)device;
    memset(capabilities, 0, sizeof(*capabilities));
    capabilities->SubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
    return k_error_success;
}

extern "C" DWORD WINAPI XInputGetState(HANDLE device, XINPUT_STATE *state)
{
    const unsigned int port = reinterpret_cast<unsigned int>(device) - k_input_handle_base;
    if (port > 3 || !host_h2_input_state(port, state))
        return k_error_device_not_connected;
    return k_error_success;
}

extern "C" DWORD WINAPI XInputSetState(HANDLE device, XINPUT_FEEDBACK *feedback)
{
    /* rumble is accepted and completes at once */
    (void)device;
    feedback->Header.dwStatus = k_error_success;
    return k_error_success;
}
