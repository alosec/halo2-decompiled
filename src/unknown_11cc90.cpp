// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_11CC90.CPP: angles between vectors */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_11cc90.h"
#include <math.h>


PRIVATE __forceinline real function_11cd21(real arg_0);

// @retail 0x11cc90
real function_11cc90(
	vector2f const *a,
	vector2f const *b)
{
	real d = a->i * b->i + a->j * b->j;
	real c = d < -1.f ? -1.f : d > 1.f ? 1.f : d;
	real angle = function_11cd21(c);
	if (a->i * b->j - a->j * b->i < 0.f)
		return 0.f - angle;
	return angle;
}

PRIVATE __forceinline real function_11cd21(real arg_0)
{
	return (real)acos(arg_0 < -1.f ? -1.f : arg_0 > 1.f ? 1.f : arg_0);
}

// @retail 0x11cd20
real function_11cd20(
	vector2f const *a,
	vector2f const *b)
{
	real result = 0.f;
	real l = (b->i * b->i + b->j * b->j) * (a->i * a->i + a->j * a->j);
	if (l != 0.f)
	{
		real d = b->i * a->i + b->j * a->j;
		real c = d / l * d * 2.f - 1.f;
		c = c < -1.f ? -1.f : c > 1.f ? 1.f : c;
		result = function_11cd21(c) * 0.5f;
		if (d < 0.f)
			result = g_5476c4 - result;
	}
	return result;
}

// @retail 0x11ce20
real function_11ce20(
	vector3f const *a,
	vector3f const *b)
{
	real result = 0.f;
	real l = (b->i * b->i + b->j * b->j + b->k * b->k) * (a->i * a->i + a->j * a->j + a->k * a->k);
	if (l != 0.f)
	{
		real d = b->k * a->k + b->j * a->j + b->i * a->i;
		real c = d / l * d * 2.f - 1.f;
		c = c < -1.f ? -1.f : c > 1.f ? 1.f : c;
		result = function_11cd21(c) * 0.5f;
		if (d < 0.f)
			result = g_5476c4 - result;
	}
	return result;
}

// @retail 0x11cf50
real function_11cf50(vector3f const *a, vector3f const *b)
{
	real local_0;
	if (((long const *)a)[0] == ((long const *)b)[0] && ((long const *)a)[1] == ((long const *)b)[1] && ((long const *)a)[2] == ((long const *)b)[2])
		local_0 = 0.f;
	else
	{
		real d = a->k * b->k + a->j * b->j + a->i * b->i;
		real c = d < -1.f ? -1.f : d > 1.f ? 1.f : d;
		local_0 = function_11cd21(c);
	}
	return local_0;
}

// @retail 0x11d000
vector3f *function_11d000(
	vector3f const *v,
	vector3f *out)
{
	real x = (real)fabs(v->i);
	real y = (real)fabs(v->j);
	real z = (real)fabs(v->k);
	if (y >= x && z >= x)
	{
		out->i = 0.f;
		out->j = v->k;
		out->k = 0.f - v->j;
	}
	else if (z >= y)
	{
		out->i = 0.f - v->k;
		out->j = 0.f;
		out->k = v->i;
	}
	else
	{
		out->i = v->j;
		out->j = 0.f - v->i;
		out->k = 0.f;
	}
	return out;
}

// @retail 0x11d090
vector3f *function_11d090(
	vector3f const *v,
	vector3f *out)
{
	vector3f w;
	real length;
	w.i = v->j;
	w.j = 0.f - v->i;
	w.k = 0.f;
	length = (real)sqrt(w.j * w.j + w.i * w.i + w.k * w.k);
	if (!(0.0001f > fabs(length)))
	{
		real inv = 1.f / length;
		w.i = inv * w.i;
		w.j = inv * w.j;
		w.k = inv * w.k;
	}
	else
		length = 0.f;
	if (length == 0.f)
		w = *g_4687a8;
	real local_0 = w.i * v->j - v->i * w.j;
	real local_1 = v->i * w.k - v->k * w.i;
	real local_2 = v->k * w.j - w.k * v->j;
	out->i = local_2;
	out->j = local_1;
	out->k = local_0;
	return out;
}
