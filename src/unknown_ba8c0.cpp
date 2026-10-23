// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_0bad50.h"

bool object_dispatch_query_function_value(long object_index, long name, real *value, bool *enabled);
bool function_10aac0(long object_index, long name, real *value, long *index);
real function_13bb90(s_tag_data const *function, real input, real range);
bool __stdcall function_bab40(long object_index, long name, real *value);


#pragma inline_depth(0)
// @retail 0xba8c0
bool function_ba8c0(s_object_function_ab const *function, long object_index, real *out)
{
    bool implicit_enable = !(bool)((function->flags >> 1) & 1);
    bool enabled = false;
    real value = 0.0f;
    bool periodic = function->curve.address[0] == 3;
    long input_name = function->input_name;
    s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    if ((bool)(((dword)object->flags_c0 >> 9) & 1))
    {
        long index;
        if (function_10aac0(object_index, input_name, out, &index))
            return true;
    }
    object_dispatch_query_function_value(object_index, function->input_name, &value, &enabled);
    if ((enabled || implicit_enable) && !periodic)
    {
        if (function->flags & 1)
            value = 1.0f - value;
        value = function_13bb90(&function->curve, value, 1.0f);
        if (implicit_enable)
            enabled = value > 0.0f;
    }
    else if (periodic)
    {
        real time = g_510c54->game_time * g_510c54->rate;
        if (function->flags & 8)
        {
            dword random = ((dword)object_index * 0x19660d + 0x3c6ef35f) >> 16;
            time += random * 0.000015259021893143654f;
        }
        value *= function_13bb90(&function->curve, time, 1.0f);
    }
    if (function->scale_name)
    {
        real scale;
        bool scale_enabled = function_bab40(object_index, function->scale_name, &scale);
        value = scale * value;
        enabled = enabled && scale_enabled;
    }
    if (enabled && function->threshold > 0.0f)
        enabled = value > function->threshold;
    if (function->flags & 4)
        enabled = true;
    if (function->enable_name && enabled)
    {
        real ignored = 0.0f;
        bool gate = false;
        object_dispatch_query_function_value(object_index, function->enable_name, &ignored, &gate);
        if (!gate)
            enabled = false;
    }
    *out = !enabled ? 0.0f : value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
    return enabled;
}
#pragma inline_depth()
