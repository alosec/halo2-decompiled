#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT
class hkEntity;

class c_native_contact_transition_callback
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2(void *item, void const *context) = 0;
};

struct s_native_contact_transition_item
{
    void *table;
    byte unknown04[8];
    void *owner;
    byte unknown10[4];
    c_native_contact_transition_callback *callback;
    byte unknown18[0x28];
    real scale_a;
    real scale_b;
};
static_assert(sizeof(s_native_contact_transition_item) == 0x48,
    "native Havok contact transition item layout");

struct s_native_contact_transition_context
{
    dword unknown00;
    dword unknown04;
    dword type;
    dword unknown0c;
    dword unknown10;
    real scale;
};
static_assert(sizeof(s_native_contact_transition_context) == 0x18,
    "native Havok contact transition context layout");

extern "C" void *g_h2_native_contact_transition_pending_table;
extern "C" void *g_h2_native_contact_transition_complete_table;
void *g_h2_native_contact_transition_pending_table;
void *g_h2_native_contact_transition_complete_table;

// Native reconstruction of retail 0x315340. The routine recognizes the
// library-owned pending table, orders the two scale values relative to the
// item's owner, invokes callback slot 2 with the exact type-0x88 context, and
// installs the completed table. Null native ownership fails closed.
void __cdecl function_315340(void *storage, hkEntity *entity_a, hkEntity *,
    real scale_a, real scale_b)
{
    if (!storage || !g_h2_native_contact_transition_pending_table ||
        !g_h2_native_contact_transition_complete_table)
        return;

    s_native_contact_transition_item *item =
        (s_native_contact_transition_item *)((byte *)storage + 0x10);
    if (item->table != g_h2_native_contact_transition_pending_table ||
        !item->callback)
        return;

    if (item->owner == entity_a)
    {
        item->scale_a = scale_a;
        item->scale_b = scale_b;
    }
    else
    {
        item->scale_a = scale_b;
        item->scale_b = scale_a;
    }

    s_native_contact_transition_context context =
        { 0, 0, 0x88, 0, 0, item->scale_a };
    item->callback->slot2(item, &context);
    item->table = g_h2_native_contact_transition_complete_table;
}
#endif
