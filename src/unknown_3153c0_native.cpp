#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT
class c_native_transition_callback
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2(void *item, void const *context) = 0;
};

struct s_native_transition_item
{
    void *table;
    byte unknown04[0x10];
    c_native_transition_callback *callback;
    byte unknown18[0x28];
    void const *payload;
    real direction;
};
static_assert(sizeof(s_native_transition_item) == 0x48,
    "native Havok transition item layout");

struct s_native_transition_context
{
    dword unknown00;
    dword unknown04;
    dword type;
    dword unknown0c;
};
static_assert(sizeof(s_native_transition_context) == 0x10,
    "native Havok transition context layout");

extern "C" void *g_h2_native_transition_pending_table;
extern "C" void *g_h2_native_transition_complete_table;
extern "C" s_native_transition_context const g_h2_native_transition_context;
void *g_h2_native_transition_pending_table;
void *g_h2_native_transition_complete_table;
extern "C" s_native_transition_context const g_h2_native_transition_context =
    { 0, 0, 0x28, 0 };

// Native reconstruction of retail 0x3153c0. The Xbox routine recognizes one
// library-owned table, records the payload and direction, invokes callback
// slot 2 with a type-0x28 context, then installs the completed table.
void __cdecl function_3153c0(void *owner, void *selected,
    void const *payload)
{
    if (!owner || !g_h2_native_transition_pending_table ||
        !g_h2_native_transition_complete_table)
        return;

    byte *owner_bytes = (byte *)owner;
    s_native_transition_item *item =
        (s_native_transition_item *)(owner_bytes + 0x10);
    if (item->table != g_h2_native_transition_pending_table || !item->callback)
        return;

    item->direction = *(void **)(owner_bytes + 0x1c) == selected ? -1.0f : 1.0f;
    item->payload = payload;
    item->callback->slot2(item, &g_h2_native_transition_context);
    item->table = g_h2_native_transition_complete_table;
}
#endif
