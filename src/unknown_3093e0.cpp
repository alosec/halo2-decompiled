// @flags /O2 /GL-
// Havok (third-party physics library): steps of a closest-point search
// between two shapes over a simplex of support points: the better of two
// opposite support points along a direction, a first support point along the
// offset between the shapes, and the second simplex point taken from the
// first.
// (Its own file: in 0x309300's file the compiler inlines that callee.)
#include <math.h>
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// a packed vector with native copy operations
struct vector_3093e0
{
	__m128 value;

	vector_3093e0() {}
	vector_3093e0(__m128 initial) : value(initial) {}
	vector_3093e0(vector_3093e0 const &other) : value(other.value) {}
	vector_3093e0 &operator=(vector_3093e0 const &other)
	{
		value = other.value;
		return *this;
	}
	operator __m128() const { return value; }
};

// a support point: the first shape's point, the second's in its own frame,
// and that point in the first shape's frame
struct support_309300
{
	vector_3093e0 first;
	vector_3093e0 second;
	vector_3093e0 second_placed;
};

class shape_309300;

// the supporting point of the pair along a direction (0x309300)
class pair_309300
{
public:
	__m128 axes[3];
	__m128 position;
	shape_309300 *first;
	shape_309300 *second;

	void function_309300(__m128 const *direction, support_309300 *out) const;
};

struct vertex_309200
{
	__m128 first;
	__m128 second;
	__m128 third;
};

struct simplex_309200;

// a support point added to the simplex (0x309200)
class class_309200
{
public:
	char unknown0[0x48];
	simplex_309200 *simplex;
	char unknown4c[4];
	__m128 *differences;

	void add(vertex_309200 const *vertex);
};

// a support point and how far along the search direction it lies
struct scored_support_3093e0
{
	support_309300 support;
	float distance;
};

// the result of the first step: the direction searched, the point found,
// its distance and a fixed one half
struct first_step_3094d0
{
	__m128 direction;
	__m128 point;
	float distance;
	float field_24;
};

// the one-byte answer of a step, returned through a hidden pointer
class answer_3094d0
{
public:
	answer_3094d0() {}
	answer_3094d0(bool value_) : value((char)value_) {}

	char value;
};

// three-term dot product of two vectors
inline float dot3_3093e0(__m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return result;
}

// the length of a vector's first three terms
inline float length3_3094d0(__m128 const &v)
{
	__m128 const squares = _mm_mul_ps(v, v);
	float result;
	_mm_store_ss(&result, _mm_sqrt_ss(_mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares))));
	return result;
}

class search_3093e0
{
public:
	__m128 axes[3];
	__m128 position;
	shape_309300 *first;
	shape_309300 *second;
	simplex_309200 *simplex;
	long field_4c;
	__m128 *differences;

	void function_3093e0(__m128 const *direction, __m128 const *origin, scored_support_3093e0 *out);
	answer_3094d0 function_3094d0(first_step_3094d0 *out);
	void function_309610();
	void function_309700();
};

// the support points along a direction and against it, keeping in out the
// one that lies further along its own direction from the origin
// @retail 0x3093e0
void search_3093e0::function_3093e0(__m128 const *direction, __m128 const *origin, scored_support_3093e0 *out)
{
	__m128 const opposite = _mm_xor_ps(*direction, *(__m128 const *)g_4124a0);
	support_309300 along;
	((pair_309300 *)this)->function_309300(direction, &along);
	((pair_309300 *)this)->function_309300(&opposite, &out->support);
	float const along_distance = dot3_3093e0(_mm_sub_ps(_mm_sub_ps(along.first, along.second_placed), *origin), *direction);
	float const against_distance = dot3_3093e0(_mm_sub_ps(_mm_sub_ps(out->support.first, out->support.second_placed), *origin), opposite);
	if (along_distance >= against_distance)
	{
		out->support = along;
		out->distance = along_distance;
	}
	else
	{
		out->distance = against_distance;
	}
}
// the first simplex point, found along the offset from the second shape to
// the first (or straight up when they nearly coincide); true with the result
// filled in when that point lies at the origin
// @retail 0x3094d0
answer_3094d0 search_3093e0::function_3094d0(first_step_3094d0 *out)
{
	__m128 direction = _mm_xor_ps(position, *(__m128 const *)g_4124a0);
	if (dot3_3093e0(direction, direction) <= 0.001f)
	{
		direction.m128_f32[0] = 0.0f;
		direction.m128_f32[1] = 1.0f;
		direction.m128_f32[2] = 0.0f;
		direction.m128_f32[3] = 0.0f;
	}
	support_309300 support;
	((pair_309300 *)this)->function_309300(&direction, &support);
	((class_309200 *)this)->add((vertex_309200 const *)&support);
	__m128 const *difference = differences;
	if (dot3_3093e0(*difference, *difference) <= 0.001f * 0.001f)
	{
		out->distance = length3_3094d0(_mm_load_ps(difference->m128_f32));
		out->direction = direction;
		out->point = support.first;
		out->field_24 = 0.5f;
		return true;
	}
	return false;
}

// the support points along the first simplex point's offset and against it;
// the one further from that offset is added to the simplex
// @retail 0x309610
void search_3093e0::function_309610()
{
	__m128 const *direction = differences;
	support_309300 along;
	((pair_309300 *)this)->function_309300(direction, &along);
	__m128 const along_offset = _mm_sub_ps(_mm_sub_ps(along.first, along.second_placed), differences[0]);
	float const along_distance = dot3_3093e0(along_offset, along_offset);
	__m128 const opposite = _mm_xor_ps(*direction, *(__m128 const *)g_4124a0);
	support_309300 against;
	((pair_309300 *)this)->function_309300(&opposite, &against);
	__m128 const against_offset = _mm_sub_ps(_mm_sub_ps(against.first, against.second_placed), differences[0]);
	float const against_distance = dot3_3093e0(against_offset, against_offset);
	if (along_distance >= against_distance)
	{
		((class_309200 *)this)->add((vertex_309200 const *)&along);
	}
	else
	{
		((class_309200 *)this)->add((vertex_309200 const *)&against);
	}
}

// a vector scaled to unit length, by the reciprocal square root refined once
__forceinline void normalize_309700(__m128 &v)
{
	__m128 const squares = _mm_mul_ps(v, v);
	__m128 const sum = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
	__m128 const inverse = _mm_rsqrt_ss(sum);
	__m128 const correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, inverse), inverse));
	__m128 const scale = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), inverse), correction);
	v = _mm_mul_ps(_mm_shuffle_ps(scale, scale, 0), v);
}

// two directions square to the edge between the first two simplex points
// (one in the plane of the edge's smallest term, one square to both), and
// the support point along either that lies further out added to the simplex
// @retail 0x309700
void search_3093e0::function_309700()
{
	vector_3093e0 const edge = _mm_sub_ps(differences[0], differences[1]);
	int smallest = 0;
	int second = 1;
	int third = 2;
	float least = fabsf(edge.value.m128_f32[0]);
	float const size_1 = fabsf(edge.value.m128_f32[1]);
	float const size_2 = fabsf(edge.value.m128_f32[2]);
	if (size_1 < least)
	{
		second = 0;
		least = size_1;
		smallest = 1;
	}
	if (size_2 < least)
	{
		third = smallest;
		smallest = 2;
	}
	vector_3093e0 square;
	square.value.m128_f32[smallest] = 0.0f;
	square.value.m128_f32[second] = edge.value.m128_f32[third];
	square.value.m128_f32[third] = -edge.value.m128_f32[second];
	vector_3093e0 cross = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(edge, edge, 0xc9), _mm_shuffle_ps(square, square, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(edge, edge, 0xd2), _mm_shuffle_ps(square, square, 0xc9)));
	normalize_309700(square.value);
	normalize_309700(cross.value);
	scored_support_3093e0 first;
	function_3093e0(&square.value, differences, &first);
	scored_support_3093e0 second_support;
	function_3093e0(&cross.value, differences, &second_support);
	if (first.distance > second_support.distance)
	{
		((class_309200 *)this)->add((vertex_309200 const *)&first);
	}
	else
	{
		((class_309200 *)this)->add((vertex_309200 const *)&second_support);
	}
}
