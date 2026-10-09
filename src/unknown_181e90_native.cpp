#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT
struct s_scale_definition
{
    byte unknown00[0x2c];
    real scale;
};
static_assert(sizeof(s_scale_definition) == 0x30,
    "Havok scale definition layout");

struct s_scale_owner
{
    byte unknown00[0x3c];
    s_scale_definition *definition;
    real get_inverse_scale() const;
};
static_assert(sizeof(s_scale_owner) == 0x40,
    "Havok scale owner layout");

// Retail 0x377750 copies the exact 0.0625f constant at 0x45df78 into this
// threshold. Native owns the equivalent initialized scalar directly.
real g_54e870 = 0.0625f;

// @retail 0x183dc0
real s_scale_owner::get_inverse_scale() const
{
    real scale = definition->scale;
    return scale != 0.0f ? 1.0f / scale : 0.0f;
}

// @retail 0x181e90
bool function_181e90(real numerator, real denominator)
{
    if (!(numerator < 0.001f) && !(denominator < 0.001f))
    {
        real ratio = numerator / denominator;
        real pinned = ratio < g_54e870 ? g_54e870 :
            (ratio > 16.0f ? 16.0f : ratio);
        if (pinned != ratio)
            return true;
    }
    return false;
}

static real scale_definition_inverse(s_scale_owner const *owner)
{
    real scale = owner->definition->scale;
    return scale == 0.0f ? 0.0f : 1.0f / scale;
}

// @retail 0x181ee0
void __cdecl function_181ee0(s_scale_owner const *a,
    s_scale_owner const *b, real *scale_a, real *scale_b)
{
    real inverse_a = scale_definition_inverse(a);
    real inverse_b = scale_definition_inverse(b);

    *scale_a = 1.0f;
    *scale_b = 1.0f;
    if (!(inverse_a < 0.001f) && !(inverse_b < 0.001f))
    {
        real ratio = inverse_a / inverse_b;
        if (ratio > 0.001f)
        {
            if (ratio < g_54e870)
                *scale_a = ratio / g_54e870;
            else if (ratio > 16.0f)
                *scale_a = 16.0f / ratio;
        }
    }
}
#endif
