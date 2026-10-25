// @flags /O2 /GL-
// Havok (third-party physics library): walking from face to face across a
// hull's triangles, leaving each face by an edge that the point lies outside
// of, until no edge leads on or a face is reached twice.
#include <xmmintrin.h>

struct flag_30aac0
{
	char value;
	flag_30aac0() {}
	flag_30aac0(bool input) : value(input) {}
	operator bool() const { return value != 0; }
};

struct face_30aac0;

struct edge_30aac0
{
	__m128 const *vertex;
	edge_30aac0 const *next;
	edge_30aac0 const *twin;
	face_30aac0 *face;
};

struct face_30aac0
{
	__m128 normal;
	float distance;
	edge_30aac0 edges[3];
	char mark;

	void function_30a9d0();
};

// a face seen as the plane that narrows an interval
class slab_30a770
{
public:
	int function_30a770(__m128 const *point, float *low, float *high) const;
};

class class_30a810
{
public:
	float low;
	float high;

	void clear_marks();
	int function_30a880(int *count, edge_30aac0 const **found, face_30aac0 *face, __m128 const *point);
	face_30aac0 *function_30aac0(face_30aac0 *start);
	int function_30abe0(int *count, edge_30aac0 const **found, face_30aac0 *face, __m128 const *point);
};

// the face's plane from its three vertices: the unit normal of the edges'
// cross product, and the distance along it of the first vertex
// @retail 0x30a9d0
void face_30aac0::function_30a9d0()
{
	__m128 const v1 = *edges[1].vertex;
	__m128 const a = _mm_sub_ps(*edges[0].vertex, v1);
	__m128 const b = _mm_sub_ps(v1, *edges[2].vertex);
	normal = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9)));
	__m128 const value = normal;
	__m128 const squares = _mm_mul_ps(value, value);
	__m128 const sum = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
	__m128 const inverse = _mm_rsqrt_ss(sum);
	__m128 const correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, inverse), inverse));
	__m128 const scale = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), inverse), correction);
	normal = _mm_mul_ps(_mm_shuffle_ps(scale, scale, 0), normal);
	__m128 const product = _mm_mul_ps(normal, *edges[0].vertex);
	float d;
	_mm_store_ss(&d, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	distance = d;
}

// @retail 0x30aac0
face_30aac0 *class_30a810::function_30aac0(face_30aac0 *start)
{
	clear_marks();
	face_30aac0 *face = start;
	edge_30aac0 const *entered = 0;
	flag_30aac0 done = false;
	while (!done)
	{
		face->mark = 3;
		int i;
		for (i = 0; i < 3; i++)
		{
			edge_30aac0 const *edge = &face->edges[i];
			if (edge == entered)
			{
				continue;
			}
			__m128 const &a = *edge->vertex;
			__m128 const d = _mm_sub_ps(*edge->next->vertex, a);
			__m128 const n = face->normal;
			__m128 const c = _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(d, d, 0xc9), _mm_shuffle_ps(n, n, 0xd2)),
				_mm_mul_ps(_mm_shuffle_ps(d, d, 0xd2), _mm_shuffle_ps(n, n, 0xc9)));
			__m128 const p = _mm_mul_ps(c, a);
			float side;
			_mm_store_ss(&side, _mm_add_ss(_mm_shuffle_ps(p, p, 0xaa), _mm_add_ss(_mm_shuffle_ps(p, p, 0x55), p)));
			if (side < -1e-4f)
			{
				face_30aac0 *neighbour = edge->twin->face;
				if (!(start->distance > 0.0f) || !(start->distance * 0.9f > neighbour->distance))
				{
					entered = face->edges[i].twin;
					face = neighbour;
					done = neighbour->mark == 3;
					break;
				}
			}
		}
		if (i != 3)
		{
			continue;
		}
		break;
	}
	return face;
}

// starts the walk from a face with the whole interval ahead of the point;
// 2 when the face itself rules the interval out
// @retail 0x30abe0
int class_30a810::function_30abe0(int *count, edge_30aac0 const **found, face_30aac0 *face, __m128 const *point)
{
	*found = 0;
	*count = 0;
	high = 3.40282347e+38f;
	low = 1e-05f;
	char const mark = (char)((slab_30a770 const *)face)->function_30a770(point, &low, &high);
	face->mark = mark;
	if (mark == 1)
	{
		return function_30a880(count, found, face, point);
	}
	return 2;
}
