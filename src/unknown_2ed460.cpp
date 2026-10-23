// @flags /O2 /GL-
// Havok (third-party physics library): passing a query on with the two sides
// swapped, then turning round the normals of the results it added.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

struct description_2ecc80;

struct hit_2ed460
{
	__m128 field_0;
	__m128 normal;
	__m128 field_20;
};

struct hits_2ed460
{
	hit_2ed460 *data;
	long size;
};

class class_2ecc80
{
public:
	void function_2ecc80(description_2ecc80 const *description, long arg_1, long arg_2, long arg_3);
	void function_2ed460(long arg_1, description_2ecc80 const *description, long arg_3, hits_2ed460 **hits);
};

// @retail 0x2ed460
void class_2ecc80::function_2ed460(long arg_1, description_2ecc80 const *description, long arg_3, hits_2ed460 **hits)
{
	long i = (*hits)->size;
	function_2ecc80(description, arg_1, arg_3, (long)hits);
	for (; i < (*hits)->size; i++)
	{
		(*hits)->data[i].normal = _mm_xor_ps((*hits)->data[i].normal, *(const __m128 *)g_4124a0);
	}
}
