#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "havok_reference.h"
// @flags /O2 /arch:SSE /Gr

class c_contact_query_allocator
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2() = 0;
    virtual void slot3() = 0;
    virtual void *allocate(long, long) = 0;
};

struct c_contact_query_bounds_info
{
    long filter;
    void *shape;
    long unknown08;
    s_havok_array properties;
    byte unknown18[8];
    hkVector4 lower, upper;
    c_contact_query_bounds_info();
    __forceinline ~c_contact_query_bounds_info()
    {
        if (!(properties.capacity_and_flags & 0x80000000))
            g_480118->allocate((long)properties.data, (properties.capacity_and_flags & 0x7fffffff) * 8, 0x12);
    }
};

struct s_contact_body_view;
class c_contact_query_bounds_volume
{
public:
    c_contact_query_bounds_volume(c_contact_query_bounds_info const *);
    byte unknown00[0x80];
    s_contact_body_view **bodies;
    long count;
    byte unknown88[8];
    static void *operator new(size_t arg_0)
    {
        void *local_0 = ((c_contact_query_allocator *)g_480118)->allocate(arg_0, 0x2c);
        ((c_havok_reference_counted *)local_0)->allocation_size = (word)arg_0;
        return local_0;
    }
};

class c_contact_query_world
{
public:
    void *add(void *);
    void remove(void *);
};

class c_2784c1 : public c_havok_reference_counted
{
public:
    c_2784c1() { reference_count = 1; }
};
class c_2784c2
{
public:
    virtual ~c_2784c2() {}
    virtual void function_2784c2() {}
};
class c_2784c3
{
public:
    virtual void function_2784c3() {}
    virtual void function_2784c4() {}
};
class c_2784c4
{
public:
    void function_30cb30(void *arg_0);
};
#include "lane_a_overlap_listener.h"
class c_2784c0 : public c_2784c1, public c_2784c2, public c_2784c3
{
public:
    c_2784c0(c_2784c4 *arg_0);
    c_2784c4 *field_10;
};

PRIVATE __forceinline void function_278575(c_2784c4 *arg_0,
    c_2784c3 *arg_1, c_contact_query_bounds_info *arg_2)
{
    c_contact_query_bounds_volume *local_0 = new c_contact_query_bounds_volume(arg_2);
    ((c_2784c5 *)local_0)->function_315910(arg_1);
    local_0 = (c_contact_query_bounds_volume *)((c_contact_query_world *)arg_0)->add(local_0);
    havok_reference_remove((c_havok_reference_counted *)local_0);
}

// @retail 0x2784c0
c_2784c0::c_2784c0(c_2784c4 *arg_0)
{
    reference_count++;
    field_10 = arg_0;
    arg_0->function_30cb30((c_2784c2 *)this);
    c_contact_query_bounds_info local_0;
    real *local_1 = (real *)field_10;
    local_0.lower.set(local_1[0x220 / 4], -100000000.0f, -100000000.0f);
    local_0.upper.set(100000000.0f, 100000000.0f, 100000000.0f);
    function_278575(arg_0, this, &local_0);
    local_0.lower.set(-100000000.0f, -100000000.0f, -100000000.0f);
    local_0.upper.set(local_1[0x210 / 4], 100000000.0f, 100000000.0f);
    function_278575(arg_0, this, &local_0);
    local_0.lower.set(-100000000.0f, -100000000.0f, -100000000.0f);
    local_0.upper.set(100000000.0f, local_1[0x214 / 4], 100000000.0f);
    function_278575(arg_0, this, &local_0);
    local_0.lower.set(-100000000.0f, local_1[0x224 / 4], -100000000.0f);
    local_0.upper.set(100000000.0f, 100000000.0f, 100000000.0f);
    function_278575(arg_0, this, &local_0);
    local_0.lower.set(-100000000.0f, -100000000.0f, -100000000.0f);
    local_0.upper.set(100000000.0f, 100000000.0f, local_1[0x218 / 4]);
    function_278575(arg_0, this, &local_0);
    local_0.lower.set(-100000000.0f, -100000000.0f, local_1[0x228 / 4]);
    local_0.upper.set(100000000.0f, 100000000.0f, 100000000.0f);
    function_278575(arg_0, this, &local_0);
}
