// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description
// out, and passing a call on with the object's transform applied first.
#include "unknown_182d90.h"
#if H2_WHOLE_IMAGE
/* the bounds' tolerance is a float (the retail slot's callers push one); on
   the port's ARM guest a float travels apart from the integer arguments */
typedef float h2_bounds_tolerance;
#else
typedef long h2_bounds_tolerance;
#endif

struct class_2df1f0
{
	char unknown0[8];
	long field_8;

	void function_2df210(long *value) const;
};

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

// @retail 0x2de810
void class_2de810::function_2de810(s_2de810 *out) const
{
	((class_2df1f0 const *)this)->function_2df210(&out->field_0);
	out->field_4 = (long)target;
	out->field_10 = field_10;
	out->field_20 = field_20;
	out->field_30 = field_30;
	out->field_40 = field_40;
}

// @retail 0x2de850
void class_2de810::function_2de850(s_extent_transform const *transform, h2_bounds_tolerance arg_1, long arg_2)
{
	s_extent_transform combined;
	combined.compose(transform, (s_extent_transform const *)&field_10);
	target->slot6(&combined, arg_1, arg_2);
}
