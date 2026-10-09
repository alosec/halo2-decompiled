#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT
extern "C" dword h2_native_havok_primary_listener_vtables[3];

class c_interface_277890
{
public:
    c_interface_277890();
private:
    byte storage[0x28];
};
static_assert(sizeof(c_interface_277890) == 0x28,
    "primary Havok listener layout");

// Exact scalar reconstruction of retail 0x277840. The three table addresses
// come only from the separately assembled ARM table object. The final retail
// function_278300 call is a no-op for this freshly initialized count/index
// pair, so native construction leaves no hidden callback or world dependency.
c_interface_277890::c_interface_277890()
{
    byte *self = storage;
    *(dword *)(self + 0x00) = h2_native_havok_primary_listener_vtables[2];
    *(dword *)(self + 0x04) = h2_native_havok_primary_listener_vtables[1];
    *(dword *)(self + 0x08) = 0;
    *(long *)(self + 0x0c) = 0;
    *(dword *)(self + 0x10) = 0x80000000;
    *(long *)(self + 0x14) = NONE;
    *(byte *)(self + 0x18) = 0;
    *(dword *)(self + 0x1c) = h2_native_havok_primary_listener_vtables[0];
    *(word *)(self + 0x22) = 1;
    *(dword *)(self + 0x24) = (dword)self;
}
#endif
