#include "unknown_11c920.h"
#include <new>

#ifdef H2_NATIVE_PORT
class c_interface_277890
{
public:
    c_interface_277890();
    virtual ~c_interface_277890();
    byte field_4[0x28 - 4];
};
static_assert(sizeof(c_interface_277890) == 0x28,
    "primary Havok listener retail layout");

extern "C" alignas(16) byte g_h2_native_primary_listener_storage[0x28];
alignas(16) byte g_h2_native_primary_listener_storage[0x28];

extern "C" c_interface_277890 *h2_native_havok_primary_listener_construct()
{
    return new (g_h2_native_primary_listener_storage) c_interface_277890;
}
#endif
