// @flags /O2 /GL-
// Havok (third-party physics library): adding one 3x4 matrix to another.
#include <xmmintrin.h>

class class_2da880
{
public:
	__m128 rows[3];

	void add(const class_2da880 &other);
};

// @retail 0x2da880
void class_2da880::add(const class_2da880 &other)
{
	rows[0] = _mm_add_ps(rows[0], other.rows[0]);
	rows[1] = _mm_add_ps(rows[1], other.rows[1]);
	rows[2] = _mm_add_ps(rows[2], other.rows[2]);
}
