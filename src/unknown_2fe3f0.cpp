// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description out:
// the base's values, then its vector.
#include <xmmintrin.h>

struct s_2fc4e0
{
	long field_0;
	long field_4;
};

class class_2fc4e0
{
public:
	char field_0[0xc];
	long field_c;

	void function_2fc4e0(s_2fc4e0 *out) const;
};

struct s_2fe3f0 : s_2fc4e0
{
	char field_8[8];
	__m128 field_10;
};

class class_2fe3f0 : public class_2fc4e0
{
public:
	__m128 field_10;

	void function_2fe3f0(s_2fe3f0 *out) const;
};

// @retail 0x2fe3f0
void class_2fe3f0::function_2fe3f0(s_2fe3f0 *out) const
{
	function_2fc4e0(out);
	out->field_10 = field_10;
}
