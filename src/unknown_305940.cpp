// @flags /O2 /GL-
// Havok (third-party physics library): replacing x, y and z with the
// reciprocals of their square roots, and clearing w.
#include <math.h>
#include <string.h>

// @retail 0x305940
void __cdecl function_305940(float *vector)
{
	vector[0] = 1.0f / (float)sqrt(vector[0] + 1.1920928955078125e-07f);
	vector[1] = 1.0f / (float)sqrt(vector[1] + 1.1920928955078125e-07f);
	vector[2] = 1.0f / (float)sqrt(vector[2] + 1.1920928955078125e-07f);
	memset(vector + 3, 0, 4);
}
