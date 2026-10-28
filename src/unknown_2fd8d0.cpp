// @flags /O2 /GL-
// Havok (third-party physics library): a convex shape whose vertices are kept
// four to a block, one lane each, copying its planes and its vertices, one
// vector each, out to a description.
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"

struct s_2fc4e0
{
	long field_0;
	long field_4;
};

class class_2fc4e0
{
public:
	void function_2fc4e0(s_2fc4e0 *out) const;
};

ELEMENTS_2D91D0_ALIGNED(vectors_2fd8d0, __m128);

struct description_2fd8d0 : s_2fc4e0
{
	vectors_2fd8d0 vertices;
	vectors_2fd8d0 planes;
};

// four vertices, one lane each
struct vertex_block_2fd8d0
{
	__m128 x;
	__m128 y;
	__m128 z;
};

// one lane of a vector
inline float lane_2fd8d0(__m128 v, int lane)
{
	switch (lane)
	{
	case 0:
		v = _mm_shuffle_ps(v, v, 0x00);
		break;
	case 1:
		v = _mm_shuffle_ps(v, v, 0x55);
		break;
	case 2:
		v = _mm_shuffle_ps(v, v, 0xaa);
		break;
	default:
		v = _mm_shuffle_ps(v, v, 0xff);
		break;
	}
	float value;
	_mm_store_ss(&value, v);
	return value;
}

// copies count vectors
inline void copy_2fd8d0(__m128 *destination, __m128 const *source, int count)
{
	for (int i = 0; i < count; i++)
	{
		destination[i] = source[i];
	}
}

// a vector of three floats and a zero
union point_2fd8d0
{
	__m128 quad;
	float xyz[4];

	point_2fd8d0(float x, float y, float z)
	{
		xyz[0] = x;
		xyz[1] = y;
		xyz[2] = z;
		xyz[3] = 0;
	}
};

class blocked_convex_2fd8d0
{
public:
	char unknown0[0x30];
	vertex_block_2fd8d0 *blocks;
	char unknown34[0xd0 - 0x34];
	int vertex_count;
	__m128 *planes;
	int planes_wanted;

	void function_2fd8d0(description_2fd8d0 *out) const;
};

// score 31 (was 31): register choice only: in the vertex loop retail keeps
// the block offset in edx and blocks in eax and switches on eax, ours the
// reverse (the planes branch now matches, with planes_wanted read into a local
// for the allocation). Other forms of the block address do not move it.
// Further trials: descending or high cases first 31; direct indexed inputs
// 29; a full block value 214; coordinate-array block 31; direct array
// inputs 29; existing output index helper 31; direct inputs with index helper 29.
// The early base load and EAX/EDX ownership still differ.
// Further trials: block shift 108; lane remainder 97; both 97; short
// selector 70; lane helper parameter order 31; forced lane helper inline 31;
// zero component first 31; short selector with zero first 70.
// Further trials: double lane result 31; pointer input 31; both 230;
// unsigned selector 31; conditional lane tests with signed/unsigned selector
// 80/80; output pointer traversal alone/conditional lane tests 121/116.
// @retail 0x2fd8d0
void blocked_convex_2fd8d0::function_2fd8d0(description_2fd8d0 *out) const
{
	((class_2fc4e0 const *)this)->function_2fc4e0(out);
	long capacity = out->planes.capacity_and_flags & 0x7fffffff;
	if (capacity < planes_wanted)
	{
		if ((out->planes.capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(out->planes.data, capacity * sizeof(__m128), 0x12);
		}
		int const wanted = planes_wanted;
		out->planes.data = (__m128 *)g_memory_2d88a0->allocate_block(wanted * sizeof(__m128), 0x12);
		out->planes.capacity_and_flags = planes_wanted;
	}
	out->planes.size = planes_wanted;
	copy_2fd8d0(out->planes.data, planes, planes_wanted);
	out->vertices.resize(vertex_count);
	for (int i = 0; i < vertex_count; i++)
	{
		int lane = i & 3;
		vertex_block_2fd8d0 const *block = &blocks[i / 4];
		point_2fd8d0 point(lane_2fd8d0(block->x, lane), lane_2fd8d0(block->y, lane), lane_2fd8d0(block->z, lane));
		out->vertices.data[i] = point.quad;
	}
}
