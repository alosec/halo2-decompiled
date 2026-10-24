/* UNKNOWN_11CC90.H: 2d vector type */

#ifndef UNKNOWN_11CC90_H
#define UNKNOWN_11CC90_H

#include "unknown_0259d0.h"
#include <math.h>

union vector2f
{
	real n[2];
	struct { real i, j; };
};

/* the length of a 2-D vector, and the vector made unit length (0 when it is
   shorter than 0.0001); src/unknown_1b0cd0.cpp and src/unknown_1ff010.cpp
   share them, so they live here */
inline real magnitude2d(vector2f const *v)
{
	return (real)sqrt(v->i * v->i + v->j * v->j);
}

inline real normalize2d_fast(vector2f *v)
{
	real magnitude = magnitude2d(v);

	if (fabs(magnitude) < 0.0001f)
		return 0.0f;

	real scale = 1.0f / magnitude;

	v->i *= scale;
	v->j *= scale;
	return magnitude;
}

#endif
