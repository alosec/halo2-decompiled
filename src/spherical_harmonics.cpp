// @flags /O2 /Gr /arch:SSE
/* SPHERICAL_HARMONICS.CPP: real spherical harmonics up to the fourth order
   (16 coefficients): the basis evaluated in a direction, and a directional
   light projected onto it */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

#define k_spherical_harmonics_pi 3.14159265f

/* the basis functions of the first order*order bands in a direction */
// score 59 (was 59): +0x50 x87/SSE schedule (the band-1 SSE load comes before result[0]'s x87
// product in ours), cubic Y/scalar homes and the retained roots differ: retail never consumes
// sqrt(2) or sqrt(105) at their last use (fmul st(1)/st(2), two fstp st(0) before the return;
// result[14] in source order), ours folds them in (fmulp st(2), result[14] reassociated around
// sqrt105). Tried: outer order return 123; cubic order return 123; roots before inputs 137; early
// Z square 128; first-band Z store first 128; repeated inline sqrt(2.0)/sqrt(105.0) 158-162; /Op 383
// (and 0x146020 stops matching); first-band product staging, first Y product, square-difference
// staging and quadratic mixed-product staging 123; cubic Y staging 129; mixed-root staging 123;
// final-root product chain 113 (651/660); linear plus final chain 113; XZ product chain 59 kept;
// second cubic root chain, coefficient 8/12/9/5/6 chains and root/edge combination 113; repaired
// XZ plus early coefficient 15 147/148; cubic X chain 139; first cubic root chain 59; repaired-prefix
// linear Y/Z/X/all values 59 each; late root-five/depth square 59/59; linear scope 59; later common
// root 59; isotropic result lifetime, band-two normalization, Z-square and cubic-axis/polynomial
// accumulators 59; repaired-XZ plus cubic-Y chain 137; external direction reference and both double
// root chains 59; repaired-prefix per-use common root 59; per-use mixed/both roots 78/78; completed
// cubic X, Y and paired results 59/59/59; paired results with per-use root 59; complete mixed result
// 59; first Y store before isotropic 59; linear factor after isotropic 143; quadratic root after
// squares 138; axis-root scope 59; depth/edge/axis/joint scalar homes 59 each; double or unrounded
// common root 158 (keeps band3_35 on x87); root rounded per factor 158; plus early coefficient 15
// 140; early coefficient 15 alone 147; retail store order 144; native float roots common/mixed/both
// 59/59/59, linear/quadratic 56/57, axis pair 59, cubic factor pair 158, every root 156, linear plus
// quadratic 54 (float constants, not retail's double ones), other rounded roots added 54; both early
// root boundaries with four/six cubic product chains 139/139; double sqrt(2)/sqrt(105)/sqrt(7) with
// (real) sqrt105 at both uses 80, at coefficient 10 or 14 only 82/82; double sqrt(2)/sqrt(105) with
// real sqrt(7) 82; coefficient 15 recomputing the root product 130; result[0] after result[3] 59;
// double sqrt(105)/sqrt(7) with real sqrt(2) 66; result[0] in double 64; double sqrt(105) alone 73;
// renamed spilled locals (slot names) 59.
// @retail 0x143360
void spherical_harmonics_evaluate_direction(vector3f const *direction, dword order, real *result)
{
	real x = direction->i;
	real y = direction->j;
	real z = direction->k;
	real inverse_sqrt_pi = (real)(1.0f / sqrt(k_spherical_harmonics_pi));
	real band1 = (real)(sqrt(3.0) * inverse_sqrt_pi);

	result[0] = inverse_sqrt_pi * 0.5f;
	result[1] = band1 * y * -0.5f;
	result[2] = band1 * z * 0.5f;
	result[3] = band1 * x * -0.5f;
	if (order > 2)
	{
		real sqrt15 = (real)sqrt(15.0);
		real band2 = sqrt15 * inverse_sqrt_pi;
		real xy = y * x;
		real xx = x * x;
		real yy = y * y;
		real xx_yy = xx - yy;
		real zz = z * z;
		real sqrt5;

		result[4] = xy * band2 * 0.5f;
		result[5] = band2 * z * y * -0.5f;
		sqrt5 = (real)sqrt(5.0);
		result[6] = sqrt5 * (zz * 3.0f - 1.0f) * inverse_sqrt_pi * 0.25f;
		real quadratic_xz = z * x;
		quadratic_xz *= band2;
		result[7] = quadratic_xz * -0.5f;
		result[8] = xx_yy * sqrt15 * inverse_sqrt_pi * 0.25f;
		if (order > 3)
		{
			real sqrt2 = (real)sqrt(2.0);
			real zz5 = zz * 5.0f;
			real zz5_1 = zz5 - 1.0f;
			real xx_3yy = xx - yy * 3.0f;
			real band3_35 = (real)(sqrt(35.0) * sqrt2);
			real sqrt105 = (real)sqrt(105.0);
			real sqrt7;
			real band3_21;

			band3_21 = (real)(sqrt(21.0) * sqrt2);
			result[11] = zz5_1 * band3_21 * inverse_sqrt_pi * y * -0.125f;
			result[13] = inverse_sqrt_pi * x * zz5_1 * band3_21 * -0.125f;
			result[9] = (xx * 3.0f - yy) * band3_35 * inverse_sqrt_pi * y * -0.125f;
			result[10] = xy * sqrt105 * inverse_sqrt_pi * z * 0.5f;
			sqrt7 = (real)sqrt(7.0);
			result[12] = sqrt7 * (zz5 - 3.0f) * inverse_sqrt_pi * z * 0.25f;
			real cubic_difference = inverse_sqrt_pi * z;
			cubic_difference *= sqrt105;
			cubic_difference *= xx_yy;
			result[14] = cubic_difference * 0.25f;
			result[15] = xx_3yy * band3_35 * inverse_sqrt_pi * x * -0.125f;
		}
	}
}

/* a directional light's colour in the basis of the first order*order bands */
// @retail 0x146020
bool spherical_harmonics_evaluate_directional_light(vector3f const *direction, dword order, real red, real green, real blue,
	real *red_result, real *green_result, real *blue_result)
{
	real coefficients[16];
	real normalization;
	real scale;
	dword count;
	dword i;

	spherical_harmonics_evaluate_direction(direction, order, coefficients);
	normalization = 0.75f;
	if (order > 2)
	{
		normalization = 1.0625f;
	}
	normalization = 3.141593f / normalization;
	count = order * order;

	scale = normalization * red;
	for (i = 0; i < count; i++)
	{
		red_result[i] = coefficients[i] * scale;
	}
	if (green_result)
	{
		scale = normalization * green;
		for (i = 0; i < count; i++)
		{
			green_result[i] = coefficients[i] * scale;
		}
	}
	if (blue_result)
	{
		scale = normalization * blue;
		for (i = 0; i < count; i++)
		{
			blue_result[i] = coefficients[i] * scale;
		}
	}
	return true;
}
