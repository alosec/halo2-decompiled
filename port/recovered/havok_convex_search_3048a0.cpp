// @flags /O2 /GL-
// Havok (third-party physics library): the search for the closest points of
// two convex shapes (a GJK-style search over a few vertices of each shape,
// the second's placed in the first's frame by the query transform), and two
// helpers it calls:
//   0x3048a0  the search: from the vertices the pair's state names, the
//             simplex of first and second vertices is reduced towards the
//             closest feature until a separating direction is found, then
//             each shape's support vertex along it extends the simplex while
//             that gains more than a tolerance (doubled every round). The
//             direction, normalized, and the distance along it go out, and
//             the vertices kept go back into the pair's state. Nonzero when
//             the shapes overlap (the exact search 0x303dd0 then decides,
//             unless the caller only wants the overlap)
//   0x303540  of a tetrahedron's three faces through its fourth vertex, the
//             one a point lies farthest outside (by squared distance), or -1
//             when it lies inside them all; near ties go to 0x303110
//   0x303110  the tie break: the first face left whose neighbours do not
//             both lie beyond the point
// The search's state lives in the caller's class_3048a0 (0x16c bytes); the
// segment and triangle tests it shares with 0x303360/0x3037a0
// (src/unknown_303360.cpp) read the same object.
#include <math.h>
#include <string.h>
#include "convex_pair_2ee2e0.h"

// ---------------------------------------------------------------------------
// plain four-float vectors (the retail code's SSE registers)

struct vec_3048a0
{
	float x;
	float y;
	float z;
	float w;
};

static __forceinline vec_3048a0 sub_3048a0(vec_3048a0 const &a, vec_3048a0 const &b)
{
	vec_3048a0 r = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
	return r;
}

static __forceinline vec_3048a0 scale_3048a0(vec_3048a0 const &a, float s)
{
	vec_3048a0 r = {a.x * s, a.y * s, a.z * s, a.w * s};
	return r;
}

static __forceinline vec_3048a0 negate_3048a0(vec_3048a0 const &a)
{
	vec_3048a0 r = {-a.x, -a.y, -a.z, -a.w};
	return r;
}

// the sum of the first three terms of the product, added x, then y, then z
static __forceinline float dot_3048a0(vec_3048a0 const &a, vec_3048a0 const &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// a.yzx * b.zxy - a.zxy * b.yzx (the fourth term comes out zero)
static __forceinline vec_3048a0 cross_3048a0(vec_3048a0 const &a, vec_3048a0 const &b)
{
	vec_3048a0 r = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, a.w * b.w - a.w * b.w};
	return r;
}

// the sign bit, as movmskps reads it
static __forceinline bool negative_3048a0(float value)
{
	unsigned long bits;
	memcpy(&bits, &value, sizeof(bits));
	return (bits & 0x80000000) != 0;
}

// the vertex id kept in the low half of the fourth term
static __forceinline unsigned short id_3048a0(vec_3048a0 const &vertex)
{
	unsigned short id;
	memcpy(&id, &vertex.w, sizeof(id));
	return id;
}

// a point placed by the transform: axes[0] * x + axes[1] * y + (axes[2] * z
// + position), in all four terms
static __forceinline vec_3048a0 place_3048a0(c_query_transform const *transform, vec_3048a0 const &p)
{
	vec_3048a0 const *axes = (vec_3048a0 const *)transform->axes;
	vec_3048a0 const &position = *(vec_3048a0 const *)&transform->position;
	vec_3048a0 r;
	r.x = (axes[0].x * p.x + axes[1].x * p.y) + (axes[2].x * p.z + position.x);
	r.y = (axes[0].y * p.x + axes[1].y * p.y) + (axes[2].y * p.z + position.y);
	r.z = (axes[0].z * p.x + axes[1].z * p.y) + (axes[2].z * p.z + position.z);
	r.w = (axes[0].w * p.x + axes[1].w * p.y) + (axes[2].w * p.z + position.w);
	return r;
}

// ---------------------------------------------------------------------------
// 0x303110 and 0x303540

// the two other faces of each face, in turn (the bytes at 0x4136b0 and
// 0x4136b2: {2, 0, 1, 2, 0})
static signed char const g_other_4136b0[5] = {2, 0, 1, 2, 0};

// `dots` holds the point's (signed) heights over the faces opposite the
// first three vertices; a face whose two neighbours the point lies over has
// one of them ruled out
// @retail 0x303110
int __cdecl function_303110(vec_3048a0 const *vertices, vec_3048a0 const *point, float *dots)
{
	for (int i = 0; i < 3; i++)
	{
		int const next = g_other_4136b0[i + 2];
		int const previous = g_other_4136b0[i];
		if (!(dots[next] >= 0.0f))
		{
			continue;
		}
		if (!(dots[previous] >= 0.0f))
		{
			continue;
		}
		vec_3048a0 const base = vertices[i];
		vec_3048a0 const to_last = sub_3048a0(vertices[3], base);
		vec_3048a0 const to_next = sub_3048a0(vertices[next], base);
		vec_3048a0 const to_previous = sub_3048a0(vertices[previous], base);
		vec_3048a0 const to_point = sub_3048a0(*point, base);
		float const point_next = dot_3048a0(to_point, to_next);
		float const point_previous = dot_3048a0(to_point, to_previous);
		float const last_next = dot_3048a0(to_last, to_next);
		float const last_previous = dot_3048a0(to_last, to_previous);
		if (!(last_next * point_previous > last_previous * point_next))
		{
			dots[next] = -1.0f;
		}
		else
		{
			dots[previous] = -1.0f;
		}
	}
	if (dots[0] > 0.0f)
	{
		return 0;
	}
	if (dots[1] > 0.0f)
	{
		return 1;
	}
	if (dots[2] > 0.0f)
	{
		return 2;
	}
	return -1;
}

// retail passes the vertices and the point in registers; `sign` turns the
// faces' normals round (the tetrahedron's vertices may come from either
// shape)
// @retail 0x303540
int __cdecl function_303540(vec_3048a0 const *vertices, vec_3048a0 const *point, float sign)
{
	vec_3048a0 const &v0 = vertices[0];
	vec_3048a0 const &v1 = vertices[1];
	vec_3048a0 const &v2 = vertices[2];
	vec_3048a0 const &v3 = vertices[3];
	// the normals of the faces opposite the first three vertices
	vec_3048a0 normals[3];
	normals[0] = cross_3048a0(sub_3048a0(v2, v1), sub_3048a0(v3, v1));
	normals[1] = cross_3048a0(sub_3048a0(v0, v2), sub_3048a0(v3, v2));
	normals[2] = cross_3048a0(sub_3048a0(v1, v0), sub_3048a0(v3, v0));
	vec_3048a0 const offset = scale_3048a0(sub_3048a0(*point, v3), sign);
	float dots[4];
	dots[0] = dot_3048a0(normals[0], offset);
	dots[1] = dot_3048a0(normals[1], offset);
	dots[2] = dot_3048a0(normals[2], offset);
	dots[3] = dots[2];

	// each height squared (keeping its sign) over the normal's length
	// squared; FLT_MAX (0x7f7fffff) for a degenerate face
	float heights[3];
	for (int i = 0; i < 3; i++)
	{
		float const length_squared = dot_3048a0(normals[i], normals[i]);
		if (length_squared == 0.0f)
		{
			heights[i] = 3.40282347e+38f;
		}
		else
		{
			heights[i] = dots[i] * fabsf(dots[i]) / length_squared;
		}
	}

	// the largest height, its face, and the next largest (ties keep the
	// later face)
	int largest_index = 1;
	float largest = heights[1];
	float smaller = heights[0];
	if (heights[0] > heights[1])
	{
		smaller = heights[1];
		largest = heights[0];
		largest_index = 0;
	}
	float next_largest;
	if (!(largest > heights[2]))
	{
		largest_index = 2;
		next_largest = largest;
		largest = heights[2];
	}
	else
	{
		next_largest = !(smaller > heights[2]) ? heights[2] : smaller;
	}

	// inside every face (1e-6, 0x358637be at 0x45df1c)
	if (largest < 1e-6f)
	{
		return -1;
	}
	// clearly the farthest (1.1, 0x3f8ccccd at 0x45dc80)
	if (largest > 1.1f * next_largest)
	{
		return largest_index;
	}
	return function_303110(vertices, point, dots);
}

// ---------------------------------------------------------------------------
// 0x3048a0

// the shapes' methods the search calls: the support vertex along a
// direction (slot 12) and the vertices of a list of ids (slot 13), each
// vertex with its id in the low half of its fourth term
class convex_3048a0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	virtual void support(vec_3048a0 const *direction, vec_3048a0 *out) const {}
	virtual void vertices(unsigned short const *ids, long count, vec_3048a0 *out) const {}
};

// the pair's search state (pair_state_2fefd0): the ids of the vertices
// kept, first shape's then second's, how many of each, and how many each
// shape can give at most
struct state_3048a0
{
	unsigned short ids[4];
	signed char first_count;
	signed char second_count;
	signed char first_limit;
	signed char second_limit;
};

// the search object (class_3048a0, class_303360 in src/unknown_303360.cpp)
struct search_3048a0
{
	long first_count;
	long second_count;
	// the second count when the search last stopped, read by the next
	// triangle test
	long last_second_count;
	// set when the caller only asks whether the shapes overlap: the exact
	// search (0x303dd0) is skipped
	bool overlap_only;
	char unknownd[3];
	// which shape's segment end the triangle test last moved (1 the
	// first's, -1 the second's): that shape is not extended this round
	long moved_side;
	char unknown14[0xc];
	vec_3048a0 first[4];
	vec_3048a0 first_sources[4];
	vec_3048a0 second[4];
	vec_3048a0 second_sources[4];
	vec_3048a0 direction;
	vec_3048a0 closest_points[3];
};

typedef char assert_search_3048a0_first[(__builtin_offsetof(search_3048a0, first) == 0x20) ? 1 : -1];
typedef char assert_search_3048a0_second[(__builtin_offsetof(search_3048a0, second) == 0xa0) ? 1 : -1];
typedef char assert_search_3048a0_direction[(__builtin_offsetof(search_3048a0, direction) == 0x120) ? 1 : -1];
typedef char assert_search_3048a0_points[(__builtin_offsetof(search_3048a0, closest_points) == 0x130) ? 1 : -1];
typedef char assert_state_3048a0[(sizeof(state_3048a0) == 0xc) ? 1 : -1];

// the tests src/unknown_303360.cpp and src/unknown_308590.cpp recover
struct triangle_303360;
struct segment_3037a0;
struct triangle_3037a0;
struct closest_points_308590;

class class_303360
{
public:
	int function_303360(__m128 const *point, triangle_303360 *triangle, int orient);
	int function_3037a0(segment_3037a0 *segment, triangle_3037a0 *triangle, int *segment_count, int *facet_total, bool own_plane, long value);
};

int __cdecl function_308590(__m128 const *first_start, __m128 const *first_direction, __m128 const *second_start, __m128 const *second_direction, closest_points_308590 *result);

// the exact search for overlapping shapes (not recovered)
extern "C" int h2_retail_303dd0(class_3048a0 *search, convex_2fefd0 const *first, convex_2fefd0 const *second,
	c_query_transform const *transform, pair_state_2fefd0 *state);

// for the three sign bits 0x303360 gives (which edges of a triangle the
// point lies outside), the one edge (0..2), or a negative code for two
// edges (-6, -7, -8) or none (-1) (the bytes at 0x4136a8)
static signed char const g_edge_4136a8[8] = {-1, 0, 1, -6, 2, -7, -8, -1};

// a point outside one edge of a triangle (the vertex `edge`, and `a`, `b`
// the other two): the triangle drops to the edge the point lies beside, or
// to a vertex
static __forceinline void reduce_triangle_3048a0(vec_3048a0 *vertices, vec_3048a0 *sources, long *count,
	vec_3048a0 const &point, int edge)
{
	int const a = g_other_4136b0[edge];
	int b = g_other_4136b0[edge + 2];
	vec_3048a0 const to_point = sub_3048a0(point, vertices[edge]);
	vec_3048a0 const to_b = sub_3048a0(vertices[b], vertices[edge]);
	vec_3048a0 const to_a = sub_3048a0(vertices[a], vertices[edge]);
	*count = 2;
	if (negative_3048a0(dot_3048a0(to_point, to_a)))
	{
		vertices[a] = vertices[2];
		if (sources)
		{
			sources[a] = sources[2];
		}
		if (negative_3048a0(dot_3048a0(to_point, to_b)))
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

// the vertex 0x303360's sign bits drop when the point lies outside two edges
// (or none)
static __forceinline int dropped_vertex_3048a0(int code)
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
// (Retail keeps the first shape's ids in its vertices' fourth terms; its
// sources at 0x60 are only moved about.)
static __forceinline bool point_segment_3048a0(vec_3048a0 *segment, vec_3048a0 *segment_sources, long *count,
	vec_3048a0 const &point, bool point_is_first, vec_3048a0 *direction)
{
	vec_3048a0 const end = segment[1];
	vec_3048a0 const edge = sub_3048a0(end, segment[0]);
	vec_3048a0 const from_start = sub_3048a0(segment[0], point);
	vec_3048a0 const from_end = sub_3048a0(end, point);
	bool const end_side = negative_3048a0(dot_3048a0(from_end, edge));
	bool const start_side = negative_3048a0(dot_3048a0(from_start, edge));
	if (end_side != start_side)
	{
		vec_3048a0 const across = cross_3048a0(from_end, from_start);
		*direction = point_is_first ? cross_3048a0(across, edge) : cross_3048a0(edge, across);
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

// the support vertex of `shape` along `direction` extends the simplex when
// it lies more than the tolerance past every vertex kept: the test retail
// makes against the first vertex (more than), then against the third and
// second (not less than)
static __forceinline bool extends_3048a0(vec_3048a0 const &vertex, vec_3048a0 const *kept, long count,
	vec_3048a0 const &direction, float tolerance)
{
	float const first_gain = dot_3048a0(sub_3048a0(vertex, kept[0]), direction);
	if (!(fabsf(first_gain) * first_gain > tolerance))
	{
		return false;
	}
	if (count == 3)
	{
		float const gain = dot_3048a0(sub_3048a0(vertex, kept[2]), direction);
		if (fabsf(gain) * gain < tolerance)
		{
			return false;
		}
	}
	if (count == 2 || count == 3)
	{
		float const gain = dot_3048a0(sub_3048a0(vertex, kept[1]), direction);
		if (fabsf(gain) * gain < tolerance)
		{
			return false;
		}
	}
	return true;
}

// @retail 0x3048a0
int class_3048a0::function_3048a0(convex_2fefd0 const *first_shape, convex_2fefd0 const *second_shape, c_query_transform const *transform, pair_state_2fefd0 *pair_state, closest_3048a0 *closest)
{
	search_3048a0 *search = (search_3048a0 *)this;
	class_303360 *tests = (class_303360 *)this;
	state_3048a0 *state = (state_3048a0 *)pair_state;
	convex_3048a0 const *first = (convex_3048a0 const *)first_shape;
	convex_3048a0 const *second = (convex_3048a0 const *)second_shape;

	search->first_count = state->first_count;
	search->second_count = state->second_count;
	long iterations = 0;
	// 1e-10 (0x2edbe6ff), doubled every round
	float epsilon = 1.0e-10f;
	first->vertices(state->ids, search->first_count, search->first);
	second->vertices(&state->ids[search->first_count], search->second_count, search->second_sources);
	for (long i = 0; i < search->second_count; i++)
	{
		search->second[i] = place_3048a0(transform, search->second_sources[i]);
	}
	int status = 0;
	bool overlap;

next_round:
	epsilon += epsilon;
	iterations++;
	search->moved_side = 0;

reduce:
	switch (search->first_count * 8 | search->second_count)
	{
	case 0x0b:
		// a point and a triangle
		goto point_triangle;
	case 0x0c:
	{
		// a point and a tetrahedron: drop the face the point is farthest
		// beyond, then go on with the triangle
		int const face = function_303540(search->second, &search->first[0], -1.0f);
		if (face < 0)
		{
			goto overlapping;
		}
		search->second_count--;
		search->second[face] = search->second[3];
		search->second_sources[face] = search->second_sources[3];
		goto point_triangle;
	}
	case 0x19:
		// a triangle and a point
		goto triangle_point;
	case 0x21:
	{
		int const face = function_303540(search->first, &search->second[0], 1.0f);
		if (face < 0)
		{
			goto overlapping;
		}
		search->first_count--;
		search->first[face] = search->first[3];
		goto triangle_point;
	}
	case 0x13:
	{
		// a segment and a triangle
		int const result = tests->function_3037a0((segment_3037a0 *)search->first, (triangle_3037a0 *)search->second,
			(int *)&search->first_count, (int *)&search->second_count, search->last_second_count == 2, 1);
		if (result == 1)
		{
			goto overlapping;
		}
		if (result != 2)
		{
			goto separated;
		}
		goto segment_segment;
	}
	case 0x1a:
	{
		// a triangle and a segment
		int const result = tests->function_3037a0((segment_3037a0 *)search->second, (triangle_3037a0 *)search->first,
			(int *)&search->second_count, (int *)&search->first_count, search->last_second_count == 2, -1);
		if (result == 1)
		{
			goto overlapping;
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
		goto overlapping;
	}

point_triangle:
	{
		int const outside = tests->function_303360((__m128 const *)&search->first[0], (triangle_303360 *)search->second, 1);
		if (outside == 7)
		{
			goto separated;
		}
		int const edge = g_edge_4136a8[outside];
		if (edge >= 0)
		{
			reduce_triangle_3048a0(search->second, search->second_sources, &search->second_count, search->first[0], edge);
			goto reduce;
		}
		int const vertex = dropped_vertex_3048a0(edge);
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
		int const edge = g_edge_4136a8[outside];
		if (edge >= 0)
		{
			reduce_triangle_3048a0(search->first, 0, &search->first_count, search->second[0], edge);
			goto reduce;
		}
		int const vertex = dropped_vertex_3048a0(edge);
		search->first_count--;
		search->first[vertex] = search->first[2];
		goto segment_point;
	}

segment_segment:
	{
		vec_3048a0 const first_direction = sub_3048a0(search->first[1], search->first[0]);
		vec_3048a0 const second_direction = sub_3048a0(search->second[1], search->second[0]);
		int const ends = function_308590((__m128 const *)&search->first[0], (__m128 const *)&first_direction,
			(__m128 const *)&search->second[0], (__m128 const *)&second_direction,
			(closest_points_308590 *)search->closest_points);
		if (ends == 0)
		{
			// the closest points lie inside both segments: the direction
			// across both, turned from the first towards the second
			vec_3048a0 const across = cross_3048a0(first_direction, second_direction);
			search->direction = across;
			if (!negative_3048a0(dot_3048a0(across, sub_3048a0(search->first[0], search->second[0]))))
			{
				search->direction = negate_3048a0(across);
			}
			goto separated;
		}
		// each segment drops to the end the closest point lies at
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
	if (point_segment_3048a0(search->second, search->second_sources, &search->second_count, search->first[0], true,
		&search->direction))
	{
		search->last_second_count = search->second_count;
		goto stopped;
	}
	goto point_point;

segment_point:
	if (point_segment_3048a0(search->first, search->first_sources, &search->first_count, search->second[0], false,
		&search->direction))
	{
		search->last_second_count = search->second_count;
		goto stopped;
	}
	goto point_point;

point_point:
	search->direction = sub_3048a0(search->second[0], search->first[0]);

separated:
	search->last_second_count = search->second_count;

stopped:
	overlap = false;
	goto measure;

overlapping:
	overlap = true;

measure:
	{
		vec_3048a0 const &direction = search->direction;
		float const length_squared = dot_3048a0(direction, direction);
		float const distance = dot_3048a0(sub_3048a0(search->second[0], search->first[0]), direction);
		// FLT_EPSILON (0x34000000 at 0x41359c)
		if (overlap || !(epsilon * 1.1920929e-07f * length_squared < distance * distance * distance))
		{
			if (!search->overlap_only)
			{
				status = h2_retail_303dd0(this, first_shape, second_shape, transform, pair_state);
			}
			else
			{
				if (search->first_count + search->second_count > 4)
				{
					if (search->first_count > search->second_count)
					{
						search->first_count = 3;
						search->second_count = 1;
					}
					else
					{
						search->first_count = 1;
						search->second_count = 3;
					}
				}
				status = 3;
			}
			goto finish;
		}

		float const tolerance = length_squared * epsilon;
		bool const odd = (iterations & 1) != 0;
		// odd rounds try the first shape first, even rounds the second
		if (odd && search->first_count != state->first_limit && search->moved_side != 1)
		{
			first->support(&search->direction, &search->first[search->first_count]);
			if (extends_3048a0(search->first[search->first_count], search->first, search->first_count, search->direction, tolerance))
			{
				search->first_count++;
				goto next_round;
			}
		}
		if (search->second_count != state->second_limit && search->moved_side != -1)
		{
			// the direction back into the second shape's frame, turned round
			vec_3048a0 const *axes = (vec_3048a0 const *)transform->axes;
			vec_3048a0 local;
			local.x = -(axes[0].x * direction.x + axes[0].y * direction.y + axes[0].z * direction.z);
			local.y = -(axes[1].x * direction.x + axes[1].y * direction.y + axes[1].z * direction.z);
			local.z = -(axes[2].x * direction.x + axes[2].y * direction.y + axes[2].z * direction.z);
			local.w = local.z;
			vec_3048a0 *source = &search->second_sources[search->second_count];
			second->support(&local, source);
			if (extends_3048a0(*source, search->second_sources, search->second_count, local, tolerance))
			{
				search->second[search->second_count] = place_3048a0(transform, *source);
				search->second_count++;
				goto next_round;
			}
		}
		if (!odd && search->first_count != state->first_limit && search->moved_side != 1)
		{
			first->support(&search->direction, &search->first[search->first_count]);
			if (extends_3048a0(search->first[search->first_count], search->first, search->first_count, search->direction, tolerance))
			{
				search->first_count++;
				goto next_round;
			}
		}
	}

finish:
	{
		// the direction normalized (a reciprocal square root refined by one
		// Newton step), and the distance along it in the fourth term
		vec_3048a0 const direction = search->direction;
		vec_3048a0 const offset = sub_3048a0(search->second[0], search->first[0]);
		float const length_squared = dot_3048a0(direction, direction);
		float const estimate = 1.0f / sqrtf(length_squared);
		float const inverse = 0.5f * estimate * (3.0f - length_squared * estimate * estimate);
		vec_3048a0 *out = (vec_3048a0 *)&closest->point;
		*out = scale_3048a0(direction, inverse);
		out->w = dot_3048a0(offset, direction) * inverse;

		state->first_count = (signed char)search->first_count;
		state->second_count = (signed char)search->second_count;
		long i = 0;
		do
		{
			state->ids[i] = id_3048a0(search->first[i]);
			i++;
		} while (i < search->first_count);
		unsigned short *ids = &state->ids[i];
		long j = 0;
		do
		{
			*ids++ = id_3048a0(search->second_sources[j]);
			j++;
		} while (j < search->second_count);
	}
	return status;
}
