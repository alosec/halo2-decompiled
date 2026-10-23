// @flags /O2 /GL-
// Havok (third-party physics library): setting a 3x3 matrix (rows of four
// floats) to the transpose of another.

class class_2da7a0
{
public:
	float rows[3][4];

	void set_transpose(const class_2da7a0 &other);
};

// @retail 0x2da7a0
void class_2da7a0::set_transpose(const class_2da7a0 &other)
{
	rows[0][0] = other.rows[0][0];
	rows[1][1] = other.rows[1][1];
	rows[2][2] = other.rows[2][2];
	rows[0][3] = 0.0f;
	rows[1][3] = 0.0f;
	rows[2][3] = 0.0f;
	rows[0][1] = other.rows[1][0];
	rows[1][0] = other.rows[0][1];
	rows[0][2] = other.rows[2][0];
	rows[2][0] = other.rows[0][2];
	rows[1][2] = other.rows[2][1];
	rows[2][1] = other.rows[1][2];
}
