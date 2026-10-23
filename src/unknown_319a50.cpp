// @flags /O2 /GL-
// Havok (third-party physics library): keeps the reciprocal of the largest
// diagonal element of a 3x3 matrix stored in rows of four floats (0 when
// that element is not positive).

inline float larger_319a50(float a, float b)
{
	return a > b ? a : b;
}

class body_319a50
{
public:
	char unknown0[0x10];
	float reciprocal;

	void function_319a50(float const *rows);
};

// @retail 0x319a50
void body_319a50::function_319a50(float const *rows)
{
	float const largest = larger_319a50(larger_319a50(rows[0], rows[5]), rows[10]);
	if (largest > 0.0f)
	{
		reciprocal = 1.0f / largest;
	}
	else
	{
		reciprocal = 0.0f;
	}
}
