// @flags /O2 /GL-
// A copy of six vectors, one at a time.
#include <xmmintrin.h>

struct class_2e7d80
{
	__m128 rows[6];

	class_2e7d80(const class_2e7d80 &other);
};

// @retail 0x2e7d80
class_2e7d80::class_2e7d80(const class_2e7d80 &other)
{
	rows[0] = other.rows[0];
	rows[1] = other.rows[1];
	rows[2] = other.rows[2];
	rows[3] = other.rows[3];
	rows[4] = other.rows[4];
	rows[5] = other.rows[5];
}
