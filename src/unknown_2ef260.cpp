// @flags /O2 /GL-
// Havok (third-party physics library): a query's input: six values, two
// points with a value between them, a radius, a tolerance, and the two
// points grown by the radius.
#include <xmmintrin.h>

class query_2ef260
{
public:
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	__m128 from;
	__m128 to;
	__m128 grown_from;
	__m128 grown_to;
	float radius;
	float tolerance;

	query_2ef260(long a, long b, long c, long d, long e, long f, __m128 const &from_, long g, __m128 const &to_, float radius_);
};

// @retail 0x2ef260
query_2ef260::query_2ef260(long a, long b, long c, long d, long e, long f, __m128 const &from_, long g, __m128 const &to_, float radius_)
	: field_0(a), field_4(b), field_8(c), field_c(d), field_10(e), field_14(f), field_18(g)
{
	_mm_store_ps((float *)&from, from_);
	_mm_store_ps((float *)&to, to_);
	radius = radius_;
	tolerance = 0.01f;
	__m128 grow = _mm_set_ss(radius_);
	grow = _mm_shuffle_ps(grow, grow, 0);
	grown_from = _mm_add_ps(grow, from);
	grown_to = _mm_add_ps(grow, to);
}
