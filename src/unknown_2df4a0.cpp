// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description out,
// as a free function.
#include <xmmintrin.h>
#if H2_WHOLE_IMAGE
/* the bounds' tolerance is a float (the retail slot's callers push one); on
   the port's ARM guest a float travels apart from the integer arguments */
typedef float h2_bounds_tolerance;
#else
typedef long h2_bounds_tolerance;
#endif
#include "unknown_182d90.h"

struct s_2de810
{
	long field_0;
	long field_4;
	char field_8[8];
	__m128 field_10;
	__m128 field_20;
	__m128 field_30;
	__m128 field_40;
};

class class_2de850_target
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void slot4() = 0;
	virtual void slot5() = 0;
	virtual void slot6(s_extent_transform const *transform, h2_bounds_tolerance arg_1, long arg_2) = 0;
};

class class_2de810
{
public:
	char field_0[0xc];
	class_2de850_target *target;
	__m128 field_10;
	__m128 field_20;
	__m128 field_30;
	__m128 field_40;

	void function_2de810(s_2de810 *out) const;
	void function_2de850(s_extent_transform const *transform, h2_bounds_tolerance arg_1, long arg_2);
};

// @retail 0x2df4a0
void __cdecl function_2df4a0(class_2de810 const *object, s_2de810 *out)
{
	object->function_2de810(out);
}
