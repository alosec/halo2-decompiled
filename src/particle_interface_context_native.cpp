#include "unknown_11c920.h"
#include "unknown_0259d0.h"

#if defined(H2_NATIVE_PORT)

struct s_interface_function_context
{
    long field_0;
    void *field_4;
    long field_8;
    byte untouched_c[4];
    vector3f field_10;
    vector3f field_1c;
    real field_28;
    real field_2c;
    vector3f field_30;
    vector3f field_3c;
    vector3f field_48;
    vector3f field_54;
    short field_60;
    byte untouched_62[0x12];
    byte field_74;
    byte untouched_75;
    byte field_76;
    byte field_77;
};
static_assert(sizeof(s_interface_function_context) == 0x78,
    "particle interface function context layout");

// @retail 0x2c3b0
// Retail evaluates x87 sin/cos over fixed constants. These are the exact
// resulting binary32 values, so native construction is deterministic and
// does not broaden ownership to a general transcendental evaluator.
void function_2c3b0(s_interface_function_context *context)
{
    real const value = 0.09000000357627869f;
    real const direction_i = 0.30901697278022766f;
    real const direction_k = -0.9510565400123596f;

    context->field_74 = 0;
    context->field_4 = 0;
    context->field_77 = 0;
    context->field_76 = 0;
    context->field_0 = NONE;
    context->field_8 = NONE;

    context->field_10.i = value;
    context->field_10.j = value;
    context->field_10.k = value;
    context->field_48.i = value;
    context->field_48.j = value;
    context->field_48.k = value;

    context->field_30.i = 0.11999999731779099f;
    context->field_30.j = 0.11999999731779099f;
    context->field_30.k = 0.11999999731779099f;
    context->field_28 = 0.40000000596046448f;
    context->field_2c = 0.40000000596046448f;

    context->field_3c.i = direction_i;
    context->field_3c.j = 0.0f;
    context->field_3c.k = direction_k;
    context->field_54.i = -direction_i;
    context->field_54.j = 0.0f;
    context->field_54.k = -direction_k;
    context->field_1c.i = context->field_3c.i;
    context->field_1c.j = context->field_3c.j;
    context->field_1c.k = context->field_3c.k;
    context->field_60 = NONE;
}

#endif
