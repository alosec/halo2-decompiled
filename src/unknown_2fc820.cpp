// @flags /O2 /GL-
// Havok (third-party physics library): copies an object's word and its
// vectors out, appending the vectors to an array.
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"
#include "word_holder_2df1f0.h"

ELEMENTS_2D91D0_ALIGNED(vectors_2fc820, __m128);

struct out_2fc820
{
	long field_0;
	vectors_2fc820 vectors;
};

class class_2fc820 : public class_2df1f0
{
public:
	long vector_count;
	__m128 vectors[1];

	void function_2fc820(out_2fc820 *out) const;
};

// @retail 0x2fc820
void class_2fc820::function_2fc820(out_2fc820 *out) const
{
	function_2df210(&out->field_0);
	for (long i = 0; i < vector_count; i++)
	{
		out->vectors.next_slot() = vectors[i];
	}
}
