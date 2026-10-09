#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT
struct s_278371;
class c_278370
{
public:
    virtual void function_278370(s_278371 *event);
};

extern "C" void h2_native_secondary_listener_event(c_278370 *listener,
    s_278371 *event)
{
    listener->c_278370::function_278370(event);
}

extern "C" void h2_native_secondary_listener_idle(c_278370 *, void *)
{
}

typedef void (*h2_native_secondary_listener_method)();
// Retail table 0x0045a72c has these three slots. The adjacent 0x000a9ef0
// entry begins a different table and is not part of this four-byte listener.
extern "C" h2_native_secondary_listener_method const
    g_h2_native_secondary_listener_vtable[] =
{
    (h2_native_secondary_listener_method)h2_native_secondary_listener_event,
    (h2_native_secondary_listener_method)h2_native_secondary_listener_idle,
    (h2_native_secondary_listener_method)h2_native_secondary_listener_idle,
};

extern "C" dword g_h2_native_secondary_listener_storage;
dword g_h2_native_secondary_listener_storage;

extern "C" c_278370 *h2_native_havok_secondary_listener_construct()
{
    g_h2_native_secondary_listener_storage =
        (dword)g_h2_native_secondary_listener_vtable;
    return (c_278370 *)&g_h2_native_secondary_listener_storage;
}
#endif
