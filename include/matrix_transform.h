/* MATRIX_TRANSFORM.H: a vector rotated by a matrix, shared by several files */

#ifndef MATRIX_TRANSFORM_H
#define MATRIX_TRANSFORM_H

#include "unknown_0259d0.h"

/* the vector rotated by the matrix's forward, left and up axes (no translation) */
inline void matrix_transform_vector(transform4x3f const *matrix, vector3f const *vector, vector3f *result)
{
	real i = vector->i;
	real j = vector->j;
	real k = vector->k;

	result->i = matrix->forward.i * i + matrix->left.i * j + matrix->up.i * k;
	result->j = matrix->forward.j * i + matrix->left.j * j + matrix->up.j * k;
	result->k = matrix->forward.k * i + matrix->left.k * j + matrix->up.k * k;
}

#endif
