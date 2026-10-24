// @flags /O2 /GL-
// Havok (third-party physics library): the exact search 0x3048a0 hands over
// to when the two convex shapes overlap (or come closer than its tolerance):
//   0x303dd0  the simplex the closest-point search kept (its first shape's
//             vertices, the second's in its own frame, and their
//             differences) seeds the penetration query 0x30a6b0 (the
//             expanding hull of 0x309b50). Of the triangle of vertex pairs
//             that query leaves, repeated vertices are dropped on each side,
//             the second's are placed again by the transform, and the two
//             features are cut down to the parts that meet (0x303b80) and
//             reduced, as the search does, to the closest features (the
//             counts and the last second count are what the contact code
//             0x3032c0 reads next). The search direction then becomes the
//             penetration direction turned round. The query's status comes
//             back (0 found, 1 out of room, 2 failed); a query that leaves a
//             single vertex pair gives the direction (1, 0, 0) and one
//             vertex each.
// The search state is the caller's class_3048a0 (0x16c bytes), read with the
// layout port/recovered/havok_convex_search_3048a0.cpp gives it.
#include <math.h>
#include <string.h>
#include "convex_pair_2ee2e0.h"

// ---------------------------------------------------------------------------
// plain four-float vectors (the retail code's SSE registers)

struct vec_303dd0
{
	float x;
	float y;
	float z;
	float w;
};

static __forceinline vec_303dd0 sub_303dd0(vec_303dd0 const &a, vec_303dd0 const &b)
{
	vec_303dd0 r = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
	return r;
}

// the sign bits flipped (xorps with 0x80000000 at 0x4124a0)
static __forceinline vec_303dd0 negate_303dd0(vec_303dd0 const &a)
{
	vec_303dd0 r = {-a.x, -a.y, -a.z, -a.w};
	return r;
}

// the sum of the first three terms of the product, added x, then y, then z
static __forceinline float dot_303dd0(vec_303dd0 const &a, vec_303dd0 const &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// a.yzx * b.zxy - a.zxy * b.yzx (the fourth term comes out zero)
static __forceinline vec_303dd0 cross_303dd0(vec_303dd0 const &a, vec_303dd0 const &b)
{
	vec_303dd0 r = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, a.w * b.w - a.w * b.w};
	return r;
}

// the sign bit, as movmskps reads it
static __forceinline bool negative_303dd0(float value)
{
	unsigned long bits;
	memcpy(&bits, &value, sizeof(bits));
	return (bits & 0x80000000) != 0;
}

// a point placed by the transform: axes[0] * x + axes[1] * y + (axes[2] * z
// + position), in all four terms
static __forceinline vec_303dd0 place_303dd0(c_query_transform const *transform, vec_303dd0 const &p)
{
	vec_303dd0 const *axes = (vec_303dd0 const *)transform->axes;
	vec_303dd0 const &position = *(vec_303dd0 const *)&transform->position;
	vec_303dd0 r;
	r.x = (axes[0].x * p.x + axes[1].x * p.y) + (axes[2].x * p.z + position.x);
	r.y = (axes[0].y * p.x + axes[1].y * p.y) + (axes[2].y * p.z + position.y);
	r.z = (axes[0].z * p.x + axes[1].z * p.y) + (axes[2].z * p.z + position.z);
	r.w = (axes[0].w * p.x + axes[1].w * p.y) + (axes[2].w * p.z + position.w);
	return r;
}

// the zero vector (src/unknown_308830.cpp)
extern __m128 g_4801e0;

// whether two points differ in any of their first three terms: some
// |a - b| (andps with 0x7fffffff at 0x455940) compares greater than the zero
// vector (cmpltps, so a NaN difference counts as equal)
static __forceinline bool differ_303dd0(vec_303dd0 const &a, vec_303dd0 const &b)
{
	vec_303dd0 const d = sub_303dd0(a, b);
	return (g_4801e0.m128_f32[0] < fabsf(d.x)) | (g_4801e0.m128_f32[1] < fabsf(d.y)) |
		(g_4801e0.m128_f32[2] < fabsf(d.z));
}

// ---------------------------------------------------------------------------
// the search state (class_3048a0, as havok_convex_search_3048a0.cpp lays it
// out)

struct search_303dd0
{
	long first_count;
	long second_count;
	// the second count when the search last stopped, read by the next
	// triangle test
	long last_second_count;
	bool overlap_only;
	char unknownd[3];
	long moved_side;
	char unknown14[0xc];
	vec_303dd0 first[4];
	vec_303dd0 first_sources[4];
	vec_303dd0 second[4];
	vec_303dd0 second_sources[4];
	vec_303dd0 direction;
	vec_303dd0 closest_points[3];
};

typedef char assert_search_303dd0_moved[(__builtin_offsetof(search_303dd0, moved_side) == 0x10) ? 1 : -1];
typedef char assert_search_303dd0_first[(__builtin_offsetof(search_303dd0, first) == 0x20) ? 1 : -1];
typedef char assert_search_303dd0_first_sources[(__builtin_offsetof(search_303dd0, first_sources) == 0x60) ? 1 : -1];
typedef char assert_search_303dd0_second[(__builtin_offsetof(search_303dd0, second) == 0xa0) ? 1 : -1];
typedef char assert_search_303dd0_second_sources[(__builtin_offsetof(search_303dd0, second_sources) == 0xe0) ? 1 : -1];
typedef char assert_search_303dd0_direction[(__builtin_offsetof(search_303dd0, direction) == 0x120) ? 1 : -1];
typedef char assert_search_303dd0_points[(__builtin_offsetof(search_303dd0, closest_points) == 0x130) ? 1 : -1];
typedef char assert_search_303dd0_size[(sizeof(search_303dd0) <= sizeof(class_3048a0)) ? 1 : -1];

// the penetration query's simplex (simplex_309b50): each vertex pair's first
// shape vertex, its second shape vertex (in that shape's frame) four entries
// on, and how many pairs
struct __declspec(align(16)) simplex_303dd0
{
	vec_303dd0 points[8];
	long count;
};

// what the penetration query gives back (first_step_3094d0): the direction
// (from the second shape into the first), the point, the depth
struct __declspec(align(16)) penetration_303dd0
{
	vec_303dd0 direction;
	vec_303dd0 point;
	float distance;
	float field_24;
};

// the simplex's vertex differences (first less second placed), read by the
// query as __m128s
struct __declspec(align(16)) differences_303dd0
{
	vec_303dd0 values[4];
};

typedef char assert_simplex_303dd0_count[(__builtin_offsetof(simplex_303dd0, count) == 0x80) ? 1 : -1];
typedef char assert_penetration_303dd0_distance[(__builtin_offsetof(penetration_303dd0, distance) == 0x20) ? 1 : -1];

// ---------------------------------------------------------------------------
// the helpers (all recovered elsewhere)

// the penetration query, timed (src/unknown_30a6b0.cpp): the two shapes, the
// transform, the simplex, its differences and the result
struct transform_30a6b0;
long __cdecl function_30a6b0(long arg_0, long arg_1, transform_30a6b0 const *transform, long arg_3, long arg_4, long arg_5);

// the face of a tetrahedron a point lies farthest beyond, or -1
// (port/recovered/havok_convex_search_3048a0.cpp)
struct vec_3048a0;
int __cdecl function_303540(vec_3048a0 const *vertices, vec_3048a0 const *point, float sign);

// the triangle and segment tests (src/unknown_303360.cpp), the two features
// cut down to the parts that meet (src/unknown_303b80.cpp), and the closest
// points of two segments (src/unknown_308590.cpp)
struct triangle_303360;
struct segment_3037a0;
struct triangle_3037a0;
struct closest_points_308590;

class class_303360
{
public:
	int function_303360(__m128 const *point, triangle_303360 *triangle, int orient);
	int function_3037a0(segment_3037a0 *segment, triangle_3037a0 *triangle, int *segment_count, int *facet_total, bool own_plane, long value);
	void function_303b80();
};

int __cdecl function_308590(__m128 const *first_start, __m128 const *first_direction, __m128 const *second_start, __m128 const *second_direction, closest_points_308590 *result);

// the two other corners of each corner, in turn (0x4136b0, 0x4136b2)
static signed char const g_other_303dd0[5] = {2, 0, 1, 2, 0};

// for 0x303360's three sign bits, the one edge (0..2), or a negative code
// for two edges (-6, -7, -8) or none (-1) (0x4136a8)
static signed char const g_edge_303dd0[8] = {-1, 0, 1, -6, 2, -7, -8, -1};

// a point outside one edge of a triangle (the vertex `edge`, and `a`, `b`
// the other two): the triangle drops to the edge the point lies beside, or
// to a vertex
static __forceinline void reduce_triangle_303dd0(vec_303dd0 *vertices, vec_303dd0 *sources, long *count,
	vec_303dd0 const &point, int edge)
{
	int const a = g_other_303dd0[edge];
	int b = g_other_303dd0[edge + 2];
	vec_303dd0 const to_point = sub_303dd0(point, vertices[edge]);
	vec_303dd0 const to_b = sub_303dd0(vertices[b], vertices[edge]);
	vec_303dd0 const to_a = sub_303dd0(vertices[a], vertices[edge]);
	*count = 2;
	if (negative_303dd0(dot_303dd0(to_point, to_a)))
	{
		vertices[a] = vertices[2];
		if (sources)
		{
			sources[a] = sources[2];
		}
		if (negative_303dd0(dot_303dd0(to_point, to_b)))
		{
			if (b == *count)
			{
				b = a;
			}
			(*count)--;
			vertices[b] = vertices[*count];
			if (sources)
			{
				sources[b] = sources[*count];
			}
		}
	}
	else
	{
		vertices[b] = vertices[2];
		if (sources)
		{
			sources[b] = sources[2];
		}
	}
}

// the vertex dropped when the point lies outside two edges (or none)
static __forceinline int dropped_vertex_303dd0(int code)
{
	int vertex = code + 8;
	if (vertex > 3)
	{
		vertex = 1;
	}
	return vertex;
}

// a point and a segment: the direction across the segment (from the first
// shape towards the second) when the point lies beside it, else the segment
// drops to its end nearer the point. True when the direction was set.
static __forceinline bool point_segment_303dd0(vec_303dd0 *segment, vec_303dd0 *segment_sources, long *count,
	vec_303dd0 const &point, bool point_is_first, vec_303dd0 *direction)
{
	vec_303dd0 const end = segment[1];
	vec_303dd0 const edge = sub_303dd0(end, segment[0]);
	vec_303dd0 const from_start = sub_303dd0(segment[0], point);
	vec_303dd0 const from_end = sub_303dd0(end, point);
	bool const end_side = negative_303dd0(dot_303dd0(from_end, edge));
	bool const start_side = negative_303dd0(dot_303dd0(from_start, edge));
	if (end_side != start_side)
	{
		vec_303dd0 const across = cross_303dd0(from_end, from_start);
		*direction = point_is_first ? cross_303dd0(across, edge) : cross_303dd0(edge, across);
		return true;
	}
	if (end_side)
	{
		segment[0] = end;
		segment_sources[0] = segment_sources[1];
	}
	(*count)--;
	return false;
}

// ---------------------------------------------------------------------------
// 0x303dd0

// retail is __thiscall with four stack arguments (ret 0x10); `search` is
// ecx. The pair state is not read.
// @retail 0x303dd0
int __cdecl function_303dd0(class_3048a0 *search_object, convex_2fefd0 const *first_shape, convex_2fefd0 const *second_shape,
	c_query_transform const *transform, pair_state_2fefd0 *pair_state)
{
	search_303dd0 *search = (search_303dd0 *)search_object;
	class_303360 *tests = (class_303360 *)search_object;
	(void)pair_state;

	// the simplex the query starts from: the search's first vertex pair,
	// and (when either side has a triangle or more) the second side's
	// vertices against the first's first vertex, or the first side's
	// against the second's first, with a fourth pair from the other side
	// when there is one
	simplex_303dd0 simplex;
	differences_303dd0 differences;
	penetration_303dd0 penetration;
	differences.values[0] = sub_303dd0(search->first[0], search->second[0]);
	simplex.count = 4;
	simplex.points[0] = search->first[0];
	simplex.points[4] = search->second_sources[0];
	if (search->second_count >= 3)
	{
		differences.values[1] = sub_303dd0(search->first[0], search->second[1]);
		simplex.points[5] = search->second_sources[1];
		differences.values[2] = sub_303dd0(search->first[0], search->second[2]);
		simplex.points[1] = search->first[0];
		simplex.points[2] = search->first[0];
		simplex.points[6] = search->second_sources[2];
		if (search->second_count > 3)
		{
			differences.values[3] = sub_303dd0(search->first[0], search->second[3]);
			simplex.points[3] = search->first[0];
			simplex.points[7] = search->second_sources[3];
		}
		else if (search->first_count > 1)
		{
			differences.values[3] = sub_303dd0(search->first[1], search->second[0]);
			simplex.points[3] = search->first[1];
			simplex.points[7] = search->second_sources[0];
		}
		else
		{
			simplex.count = 3;
		}
	}
	else if (search->first_count >= 3)
	{
		differences.values[1] = sub_303dd0(search->first[1], search->second[0]);
		simplex.points[1] = search->first[1];
		differences.values[2] = sub_303dd0(search->first[2], search->second[0]);
		simplex.points[2] = search->first[2];
		simplex.points[5] = search->second_sources[0];
		simplex.points[6] = search->second_sources[0];
		if (search->first_count > 3)
		{
			differences.values[3] = sub_303dd0(search->first[3], search->second[0]);
			simplex.points[3] = search->first[3];
			simplex.points[7] = search->second_sources[0];
		}
		else if (search->second_count > 1)
		{
			differences.values[3] = sub_303dd0(search->first[0], search->second[1]);
			simplex.points[3] = search->first[0];
			simplex.points[7] = search->second_sources[1];
		}
		else
		{
			simplex.count = 3;
		}
	}
	else
	{
		simplex.count = 1;
	}

	int const status = (int)function_30a6b0((long)first_shape, (long)second_shape, (transform_30a6b0 const *)transform,
		(long)&simplex, (long)&differences, (long)&penetration);

	// the triangle of vertex pairs the query ends on
	search->first[0] = simplex.points[0];
	search->first[1] = simplex.points[1];
	search->first[2] = simplex.points[2];
	search->second_sources[0] = simplex.points[4];
	search->second_sources[1] = simplex.points[5];
	search->second_sources[2] = simplex.points[6];
	if (simplex.count == 1)
	{
		vec_303dd0 const x_axis = {1.0f, 0.0f, 0.0f, 0.0f};
		search->direction = x_axis;
		search->second_count = 1;
		search->first_count = 1;
		return status;
	}
	search->second_count = 3;
	search->first_count = 3;

	// repeated vertices dropped: the third when it repeats the second or the
	// first, then the first when it repeats the second
	if (!differ_303dd0(search->first[1], search->first[2]) || !differ_303dd0(search->first[0], search->first[2]))
	{
		search->first_count = 2;
	}
	if (!differ_303dd0(search->first[0], search->first[1]))
	{
		search->first_count--;
		search->first[0] = search->first[search->first_count];
	}
	if (!differ_303dd0(search->second_sources[1], search->second_sources[2]) ||
		!differ_303dd0(search->second_sources[0], search->second_sources[2]))
	{
		search->second_count--;
	}
	if (search->second_count >= 2 && !differ_303dd0(search->second_sources[0], search->second_sources[1]))
	{
		search->second_count--;
		search->second_sources[0] = search->second_sources[search->second_count];
	}
	for (long i = 0; i < search->second_count; i++)
	{
		search->second[i] = place_303dd0(transform, search->second_sources[i]);
	}
	if (search->first_count + search->second_count > 4)
	{
		tests->function_303b80();
	}

	// the features reduced to the closest, as the search does
	search->moved_side = 0;

reduce:
	switch (search->first_count * 8 | search->second_count)
	{
	case 0x0b:
		goto point_triangle;
	case 0x0c:
	{
		int const face = function_303540((vec_3048a0 const *)search->second, (vec_3048a0 const *)&search->first[0], -1.0f);
		if (face < 0)
		{
			goto finish;
		}
		search->second_count--;
		search->second[face] = search->second[3];
		search->second_sources[face] = search->second_sources[3];
		goto point_triangle;
	}
	case 0x19:
		goto triangle_point;
	case 0x21:
	{
		int const face = function_303540((vec_3048a0 const *)search->first, (vec_3048a0 const *)&search->second[0], 1.0f);
		if (face < 0)
		{
			goto finish;
		}
		search->first_count--;
		search->first[face] = search->first[3];
		goto triangle_point;
	}
	case 0x13:
	{
		int const result = tests->function_3037a0((segment_3037a0 *)search->first, (triangle_3037a0 *)search->second,
			(int *)&search->first_count, (int *)&search->second_count, search->last_second_count == 2, 1);
		if (result == 1)
		{
			goto finish;
		}
		if (result != 2)
		{
			goto separated;
		}
		goto segment_segment;
	}
	case 0x1a:
	{
		int const result = tests->function_3037a0((segment_3037a0 *)search->second, (triangle_3037a0 *)search->first,
			(int *)&search->second_count, (int *)&search->first_count, search->last_second_count == 2, -1);
		if (result == 1)
		{
			goto finish;
		}
		if (result != 2)
		{
			goto separated;
		}
		goto segment_segment;
	}
	case 0x12:
		goto segment_segment;
	case 0x0a:
		goto point_segment;
	case 0x11:
		goto segment_point;
	case 0x09:
		goto point_point;
	default:
		goto finish;
	}

point_triangle:
	{
		int const outside = tests->function_303360((__m128 const *)&search->first[0], (triangle_303360 *)search->second, 1);
		if (outside == 7)
		{
			goto separated;
		}
		int const edge = g_edge_303dd0[outside];
		if (edge >= 0)
		{
			reduce_triangle_303dd0(search->second, search->second_sources, &search->second_count, search->first[0], edge);
			goto reduce;
		}
		int const vertex = dropped_vertex_303dd0(edge);
		search->second_count--;
		search->second[vertex] = search->second[2];
		search->second_sources[vertex] = search->second_sources[2];
		goto point_segment;
	}

triangle_point:
	{
		int const outside = tests->function_303360((__m128 const *)&search->second[0], (triangle_303360 *)search->first, -1);
		if (outside == 7)
		{
			goto separated;
		}
		int const edge = g_edge_303dd0[outside];
		if (edge >= 0)
		{
			reduce_triangle_303dd0(search->first, 0, &search->first_count, search->second[0], edge);
			goto reduce;
		}
		int const vertex = dropped_vertex_303dd0(edge);
		search->first_count--;
		search->first[vertex] = search->first[2];
		goto segment_point;
	}

segment_segment:
	{
		vec_303dd0 const first_direction = sub_303dd0(search->first[1], search->first[0]);
		vec_303dd0 const second_direction = sub_303dd0(search->second[1], search->second[0]);
		int const ends = function_308590((__m128 const *)&search->first[0], (__m128 const *)&first_direction,
			(__m128 const *)&search->second[0], (__m128 const *)&second_direction,
			(closest_points_308590 *)search->closest_points);
		if (ends == 0)
		{
			vec_303dd0 const across = cross_303dd0(first_direction, second_direction);
			search->direction = across;
			if (!negative_303dd0(dot_303dd0(across, sub_303dd0(search->first[0], search->second[0]))))
			{
				search->direction = negate_303dd0(across);
			}
			goto separated;
		}
		if (ends & 1)
		{
			search->first[0] = search->first[1];
			search->first_count = 1;
		}
		else if (ends & 2)
		{
			search->first_count = 1;
		}
		if (ends & 4)
		{
			search->second[0] = search->second[1];
			search->second_sources[0] = search->second_sources[1];
			search->second_count = 1;
		}
		else if (ends & 8)
		{
			search->second_count = 1;
		}
		goto reduce;
	}

point_segment:
	if (point_segment_303dd0(search->second, search->second_sources, &search->second_count, search->first[0], true,
		&search->direction))
	{
		goto separated;
	}
	goto point_point;

segment_point:
	if (point_segment_303dd0(search->first, search->first_sources, &search->first_count, search->second[0], false,
		&search->direction))
	{
		goto separated;
	}
	goto point_point;

point_point:
	search->direction = sub_303dd0(search->second[0], search->first[0]);

separated:
	search->last_second_count = search->second_count;

finish:
	// whatever the reduction left there, the direction is the penetration
	// direction turned round
	search->direction = negate_303dd0(penetration.direction);
	return status;
}
