#ifndef UNKNOWN_182D90_H
#define UNKNOWN_182D90_H

#include "unknown_11c920.h"
#if !H2_NATIVE_PORT
#include <xmmintrin.h>
#endif

struct s_extent_transform
{
	__m128 rows[4];
	void compose(s_extent_transform const *a, s_extent_transform const *b);
};

#endif
