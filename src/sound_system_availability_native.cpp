#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

struct s_4e6380;
extern s_4e6380 *g_4e6380;

struct s_native_sound_system_availability_view
{
    byte unknown00[0x78];
    bool initialized;
    bool hardware_available;
    bool enabled;
};

static_assert(offsetof(s_native_sound_system_availability_view, initialized) == 0x78,
    "sound system initialized offset");
static_assert(offsetof(s_native_sound_system_availability_view, hardware_available) == 0x79,
    "sound system hardware offset");
static_assert(offsetof(s_native_sound_system_availability_view, enabled) == 0x7a,
    "sound system enabled offset");

// @retail 0x125a60
long sound_system_available(void)
{
    s_native_sound_system_availability_view const *const sound_system =
        reinterpret_cast<s_native_sound_system_availability_view const *>(g_4e6380);
    return sound_system->initialized && sound_system->hardware_available &&
        sound_system->enabled;
}

#endif
