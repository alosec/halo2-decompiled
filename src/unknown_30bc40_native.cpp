#ifdef H2_NATIVE_PORT

#include "unknown_1cec30.h"

#include <stddef.h>

struct s_havok_simulation_island_active_view
{
    byte unknown00[0x29];
    byte active;
};

static_assert(offsetof(hkRigidBody, m_simulation_island) == 0x44,
    "hkRigidBody simulation-island offset must match retail");
static_assert(offsetof(s_havok_simulation_island_active_view, active) == 0x29,
    "Havok simulation-island active offset must match retail");

// @retail 0x30bc40
hkBool hkRigidBody::isActive(void) const
{
    hkBool result;
    result.m_bool = m_simulation_island
        ? ((s_havok_simulation_island_active_view *)m_simulation_island)->active
        : 0;
    return result;
}

#endif
