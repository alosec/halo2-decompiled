#include "unknown_11c920.h"

// @flags /O2 /Gr

class c_physics_collection_shape;

struct s_physics_collection_query;

class c_physics_collection_filter
{
public:
    virtual bool *accept(bool *accepted, s_physics_collection_query *query,
        c_physics_collection_shape *collection, long key);
};

struct s_physics_collection_query
{
    byte field_0[0x24];
    c_physics_collection_filter *filter;
};
static_assert(sizeof(s_physics_collection_query) == 0x28,
    "collection query layout");

struct s_physics_collection_hit
{
    float direction[4];
    dword key;
    dword value;
};
static_assert(sizeof(s_physics_collection_hit) == 0x18,
    "collection hit layout");

struct s_physics_collection_context
{
    void *shape;
    dword key;
    void *matrix;
    s_physics_collection_context *parent;
};
static_assert(sizeof(s_physics_collection_context) == 0x10,
    "collection context layout");

class c_physics_collection_child
{
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual bool *intersect(bool *accepted, s_physics_collection_query *query,
        s_physics_collection_hit *hit);
    virtual void forward(s_physics_collection_query *query,
        s_physics_collection_context *context, void *collector);
};

class c_physics_collection_shape
{
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual bool *function_2fbb70(bool *accepted,
        s_physics_collection_query *query, s_physics_collection_hit *hit);
    virtual void function_2fbe90(s_physics_collection_query *query,
        s_physics_collection_context *context, void *collector);
    virtual void v10();
    virtual long first_key();
    virtual long next_key(long key);
    virtual c_physics_collection_child *child(long key, void *storage);
};

static bool physics_collection_accept(c_physics_collection_shape *collection,
    s_physics_collection_query *query, long key)
{
    if (!query->filter)
        return true;
    bool accepted;
    query->filter->accept(&accepted, query, collection, key);
    return accepted;
}

// @retail 0x2fbb70
bool *c_physics_collection_shape::function_2fbb70(bool *accepted,
    s_physics_collection_query *query, s_physics_collection_hit *hit)
{
    bool any_hit = false;
    long key = first_key();
    while (key != NONE)
    {
        if (physics_collection_accept(this, query, key))
        {
            byte child_storage[0x100];
            c_physics_collection_child *shape = child(key, child_storage);
            bool child_hit;
            shape->intersect(&child_hit, query, hit);
            if (child_hit)
            {
                any_hit = true;
                hit->key = key;
            }
        }
        key = next_key(key);
    }
    *accepted = any_hit;
    return accepted;
}

// @retail 0x2fbe90
void c_physics_collection_shape::function_2fbe90(
    s_physics_collection_query *query,
    s_physics_collection_context *context, void *collector)
{
    long key = first_key();
    while (key != NONE)
    {
        if (physics_collection_accept(this, query, key))
        {
            byte child_storage[0x100];
            c_physics_collection_child *shape = child(key, child_storage);
            s_physics_collection_context local = {
                shape, (dword)key, context->matrix, context
            };
            shape->forward(query, &local, collector);
        }
        key = next_key(key);
    }
}
