// @flags /O2 /GL-
// Havok (third-party physics library): adding a vertex to a simplex of up to
// four: the difference of its two points into the owner's array, and both
// points into the simplex.
#include <xmmintrin.h>

struct vertex_309200
{
	__m128 first;
	__m128 second;
	__m128 third;
};

struct simplex_309200
{
	__m128 first[4];
	__m128 second[4];
	int count;
};

class class_309200
{
public:
	char unknown0[0x48];
	simplex_309200 *simplex;
	char unknown4c[4];
	__m128 *differences;

	void add(vertex_309200 const *vertex);
};

// @retail 0x309200
void class_309200::add(vertex_309200 const *vertex)
{
	differences[simplex->count] = _mm_sub_ps(vertex->first, vertex->third);
	simplex->first[simplex->count] = vertex->first;
	simplex->second[simplex->count] = vertex->second;
	simplex->count++;
}
