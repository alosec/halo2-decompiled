#ifdef H2_NATIVE_PORT

#include "unknown_11c920.h"

#include <stddef.h>

struct s_havok_shape_key_view
{
    byte unknown00[8];
    dword user_data;
};

struct s_havok_cd_body
{
    s_havok_shape_key_view *shape;
    long shape_key;
    byte unknown08[4];
    s_havok_cd_body *parent;
};

static_assert(offsetof(s_havok_shape_key_view, user_data) == 8,
    "Havok shape user-data offset must match retail");
static_assert(sizeof(s_havok_cd_body) == 0x10,
    "Havok collision-body layout must match retail");
static_assert(offsetof(s_havok_cd_body, parent) == 0xc,
    "Havok collision-body parent offset must match retail");

// @retail 0x1c55b0
long havok_cd_body_shape_key_get(s_havok_cd_body const *body)
{
    while (body->parent)
    {
        if (body->parent->shape->user_data == 0xcabcabb0)
            break;
        body = body->parent;
    }
    return body->parent ? body->shape_key : NONE;
}

#endif
