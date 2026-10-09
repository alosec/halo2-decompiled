// Native endpoint-only reconstruction of the recovered motion-basis query.
// The Xbox implementation remains in unknown_181e90.cpp and reaches the
// general tag-function evaluator. This source evaluates only the exact 0/1
// endpoints requested by function_355e0, so spline2 endpoint behavior does
// not depend on retail's intermediate SSE rsqrtss approximation.

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>
#include <stddef.h>

struct s_tag_function_native
{
    long size;
    byte *data;
};

struct s_function_header_native
{
    byte type;
    byte flags;
    byte function_type[2];
    union
    {
        real_bounds bounds;
        dword colors[4];
    };
};

struct s_function_spline_native
{
    s_function_header_native header;
    point2f points[4];
    real coefficients[4];
};

struct s_355e0_curve_native
{
    real period;
    long input;
    real duration;
    s_tag_function_native function;
};

struct s_motion_entry_native
{
    long tag_index;
    short basis_index;
    short unknown06;
};

struct s_motion_basis_native
{
    vector3f first;
    vector3f second;
};

struct s_motion_globals_native
{
    byte unknown00[0x10];
    s_motion_entry_native *entries;
    byte unknown14[0xb4 - 0x14];
    long basis_count;
    s_motion_basis_native *bases;
};

static_assert(sizeof(s_tag_function_native) == 8, "tag function size");
static_assert(offsetof(s_function_header_native, bounds) == 4, "function bounds offset");
static_assert(sizeof(s_motion_entry_native) == 8, "motion entry size");
static_assert(offsetof(s_motion_globals_native, entries) == 0x10, "motion entries offset");
static_assert(offsetof(s_motion_globals_native, basis_count) == 0xb4, "motion basis count offset");
static_assert(offsetof(s_motion_globals_native, bases) == 0xb8, "motion bases offset");

extern byte g_46bd48[16][1024];
extern byte const g_468d48[12][1024];

static __forceinline real clamp_unit_native(real value)
{
    return 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static real periodic_endpoint_native(short function_type, real input)
{
    if (function_type == 0)
        return 1.0f;
    if (function_type < 0 || function_type >= 12)
        return 0.0f;

    real scaled = input * 36.57143f;
    long whole = (long)scaled;
    real fraction = scaled - (real)whole;
    long index = whole & 0x3ff;
    byte const *table = g_468d48[function_type];
    real a = table[index] * (1.0f / 255.0f);
    real b = table[(index + 1) & 0x3ff] * (1.0f / 255.0f);
    if ((1 << function_type) & 0xc0)
    {
        if (a > 0.75f && 0.25f > b)
            b += 1.0f;
        real result = (1.0f - fraction) * a + b * fraction;
        return result > 1.0f ? result - 1.0f : result;
    }
    return (1.0f - fraction) * a + b * fraction;
}

static real transition_endpoint_native(real endpoint, short curve)
{
    if (curve == 0)
        return endpoint;
    if (curve < 0 || curve >= 16)
        return 0.0f;
    return g_46bd48[curve][endpoint == 0.0f ? 0 : 0x3ff] * (1.0f / 255.0f);
}

static __forceinline real length_squared_native(point2f const *a, point2f const *b)
{
    real x = b->x - a->x;
    real y = b->y - a->y;
    return x * x + y * y;
}

static real tag_function_endpoint_native(s_tag_function_native const *function, real endpoint)
{
    if (!function || !function->data || function->size <= 0)
        return 0.0f;

    s_function_header_native const *header =
        (s_function_header_native const *)function->data;
    real const *parameters = (real const *)(header + 1);
    real result = 0.0f;

    switch (header->type)
    {
    case 0: // identity
        result = endpoint;
        break;
    case 1: // constant
        result = 0.0f;
        break;
    case 2: // transition
    {
        real x = transition_endpoint_native(endpoint, header->function_type[0]);
        result = (parameters[1] - parameters[0]) * x + parameters[0];
        break;
    }
    case 3: // periodic
    {
        real x = periodic_endpoint_native(header->function_type[0],
            parameters[0] * endpoint + parameters[1]);
        result = (parameters[3] - parameters[2]) * x + parameters[2];
        break;
    }
    case 4: // linear
        result = parameters[4] * endpoint + parameters[5];
        break;
    case 5: // linear key
    {
        real value = (endpoint - parameters[9]) * parameters[13];
        real t0 = clamp_unit_native(value);
        value = (endpoint - parameters[10]) * parameters[14];
        real t1 = clamp_unit_native(value);
        value = (endpoint - parameters[11]) * parameters[15];
        real t2 = clamp_unit_native(value);
        result = parameters[19] * t2 + parameters[18] * t1 +
            parameters[17] * t0 + parameters[16];
        break;
    }
    case 7: // spline
    case 10: // spline2
    {
        real t = endpoint;
        if (header->type == 10)
        {
            s_function_spline_native const *spline =
                (s_function_spline_native const *)header;
            real end_length_squared = length_squared_native(
                &spline->points[3], &spline->points[2]);
            if (end_length_squared > 0.0001f)
            {
                real start_length_squared = length_squared_native(
                    &spline->points[1], &spline->points[0]);
                if (fabs(start_length_squared) < 0.0001f)
                    t = 1.0f;
                // For endpoint 0 or 1, pow(endpoint, positive_ratio) is the
                // endpoint itself. Retail's rsqrtss approximation therefore
                // cannot affect this caller's result.
            }
        }
        real const *coefficients =
            ((s_function_spline_native const *)header)->coefficients;
        real t2 = t * t;
        real t3 = t2 * t;
        result = coefficients[1] * t2 + coefficients[2] * t +
            coefficients[0] * t3 + coefficients[3];
        break;
    }
    case 9: // exponent
    {
        real exponent = parameters[2];
        if (fabs(exponent) < 0.0001f ||
            (0.0f > exponent && endpoint == 0.0f))
            result = 1.0f;
        else
        {
            real powered = endpoint == 0.0f ? 0.0f : 1.0f;
            result = (parameters[1] - parameters[0]) * powered + parameters[0];
        }
        break;
    }
    default:
        return 0.0f;
    }
    return clamp_unit_native(result);
}

static real curve_endpoint_native(s_tag_function_native const *curve, real endpoint)
{
    real result = tag_function_endpoint_native(curve, endpoint);
    byte *data = curve->data;
    if (!(data[1] & 0xf0))
    {
        real lower = *(real *)(data + 4);
        real upper = *(real *)(data + 8);
        result = (upper - lower) * clamp_unit_native(result) + lower;
    }
    return result;
}

// @retail 0x355e0
bool function_355e0(long tag, long index, real *first, real *second)
{
    if (!first || !second || tag == NONE || !g_4e3b44)
        return false;
    byte *definition = *(byte **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
    if (!definition)
        return false;

    *first = 0.0f;
    *second = 0.0f;
    long count = 0;
    long selection = (*(short **)(definition + 0x60))[index * 2 + 1];
    if (selection != NONE)
    {
        word const *range = *(word **)(definition + 0x50) + selection;
        short const *entry = *(short **)(definition + 0x48) + (*range & 0x1ff) * 2;
        for (long i = 0; i < (*range >> 9); entry += 2, ++i)
        {
            if (entry[1] == 4 || entry[1] == 5)
            {
                s_355e0_curve_native const *curve =
                    *(s_355e0_curve_native **)(definition + 0x40) + entry[0];
                real duration = curve->duration != 0.0f ? curve->duration : 1.0f;
                real initial = curve_endpoint_native(&curve->function, 0.0f);
                real final = curve_endpoint_native(&curve->function, 1.0f);
                real slope = (final - initial) / duration;
                if (entry[1] == 4)
                    *first = slope;
                else
                    *second = slope;
                ++count;
            }
        }
    }
    return count > 0;
}

// @retail 0x181db0
bool function_181db0(long index, vector3f *result)
{
    s_motion_globals_native *globals = (s_motion_globals_native *)g_4e0348;
    if (!globals || !result || !globals->entries || !globals->bases)
        return false;
    s_motion_entry_native *entry = &globals->entries[index];
    if (entry->basis_index == NONE || globals->basis_count <= entry->basis_index)
        return false;

    real first;
    real second;
    bool valid = function_355e0(entry->tag_index, 0, &first, &second);
    if (!valid)
        valid = function_355e0(entry->tag_index, 1, &first, &second);
    if (!valid)
        return false;

    s_motion_basis_native *basis = &globals->bases[entry->basis_index];
    first = 0.0f - first;
    second = 0.0f - second;
    result->i = basis->second.i * second + basis->first.i * first;
    result->j = basis->second.j * second + basis->first.j * first;
    result->k = basis->second.k * second + basis->first.k * first;
    return true;
}
