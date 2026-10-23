/* NORMALIZE3D.H: the vector scale and normalization several files expand inline */

#ifndef NORMALIZE3D_H
#define NORMALIZE3D_H

#include "unknown_0259d0.h"
#include <math.h>

/* v scaled by s into result (v and result may be the same vector) */
static __inline vector3f *vector3f_scale(vector3f const *v, real s, vector3f *result)
{
	result->i = s * v->i;
	result->j = s * v->j;
	result->k = s * v->k;
	return result;
}

/* scales v to unit length and returns its old length, or 0 for a vector too short to normalize */
static __inline real normalize3d(vector3f *v)
{
	real m = (real)sqrt(dot3f(v, v));
	if (!(fabs(m) < 0.0001f))
		vector3f_scale(v, 1.f / m, v);
	else
		m = 0.f;
	return m;
}

#endif
