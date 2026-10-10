// Source-local native reconstruction of the material curve input/evaluation
// bridge used beneath function_174d0. The Xbox body remains in
// unknown_0167a0.cpp.

#include "unknown_11c920.h"
#include <math.h>

struct s_1b230_callback
{
    void *context;
    real (__stdcall *evaluate)(void *, long);
};

struct s_1b230_function
{
    long size;
    byte *data;
};

struct s_33a0b_view;
extern double g_4858a0;
extern s_33a0b_view *g_485a58;
real function_13b390(void const *function, real input, real range);

// @retail 0x1b230
real function_1b230(
    s_1b230_function const *definition, long input_index, long range_index, real period)
{
    real input = 0.0f;
    real range = 0.0f;
    s_1b230_callback *state = (s_1b230_callback *)g_485a58;
    if (state && state->evaluate)
    {
        input = state->evaluate(state->context, input_index);
        range = state->evaluate(state->context, range_index);
    }
    if (!input_index)
    {
        input = (real)(g_4858a0 / period);
        if (definition->data[0] != 3)
            input = (real)fmod((double)input, 1.0);
    }
    return function_13b390(definition, input, range);
}
