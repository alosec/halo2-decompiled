#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "normalize3d.h"
// @flags /O2 /arch:SSE /Gr

struct s_294050
{
	byte field_0[0x12];
	short field_12;
	point3f field_14;
	real field_20;
	short field_24;
};

struct s_294051
{
	byte field_0[0x54];
	real field_54;
	real field_58;
	byte field_5c[0x10];
	real field_6c;
	real field_70;
};

point3f *function_b9dd0(long arg_0, point3f *arg_1);
real function_30bf0(vector3f *arg_0);

__forceinline real function_2941a7(vector3f const *vector)
{
	return (real)sqrt(vector->i * vector->i + (vector->j * vector->j + vector->k * vector->k));
}

__forceinline real function_29419d(vector3f *arg_0)
{
	real local_0 = function_2941a7(arg_0);
	if (!(fabs(local_0) < 0.0001f))
		vector3f_scale(arg_0, 1.0f / local_0, arg_0);
	else
		local_0 = 0.0f;
	return local_0;
}

__forceinline void function_294240(vector3f *arg_0, real arg_1, vector3f const *arg_2)
{
	arg_0->i = arg_1 * arg_2->i + arg_0->i;
	arg_0->j = arg_2->j * arg_1 + arg_0->j;
	arg_0->k = arg_2->k * arg_1 + arg_0->k;
}

// @retail 0x294050
void function_294050(s_294050 const *arg_0, s_294051 const *arg_1, point3f const *arg_2,
	vector3f *arg_3, real *arg_4, real *arg_5)
{
	*arg_3 = *g_4687a4;
	real local_0 = 0.0f;
	real local_1 = 3.0f;
	if (arg_1->field_6c > 0.0f)
		local_1 = arg_1->field_6c;
	if (arg_0->field_12 > 0)
	{
		s_record_pool_iterator local_2;
		local_2.data = g_4e8c24;
		local_2.index = NONE;
		byte *local_3;
		while ((local_3 = data_iterator_next_inlined(&local_2)) != NULL)
		{
			long local_4 = *(long *)(local_3 + 0x2c);
			if (local_4 != NONE)
			{
				point3f local_5;
				function_b9dd0(local_4, &local_5);
				vector3f local_6;
				vector3d_from_points3d(arg_2, &local_5, &local_6);
				real local_7 = function_29419d(&local_6);
				real local_8;
				if (local_1 > local_7)
					local_8 = 1.0f;
				else if (local_1 * 2.0f > local_7)
					local_8 = (local_1 * 2.0f - local_7) / local_1;
				else
					continue;
				if (local_8 > 0.0f)
				{
					function_294240(arg_3, 0.0f - local_8, &local_6);
					if (local_8 > local_0)
						local_0 = local_8;
				}
			}
		}
	}
	if (arg_0->field_24 > 0)
	{
		vector3f local_9;
		vector3d_from_points3d(arg_2, &arg_0->field_14, &local_9);
		real local_10 = function_30bf0(&local_9);
		real local_11 = arg_0->field_20 * local_1;
		real local_12 = 0.0f;
		if (local_11 > local_10)
			local_12 = 1.0f;
		else if (local_11 * 2.0f > local_10)
			local_12 = (local_11 * 2.0f - local_10) / local_11;
		if (local_12 > 0.0f)
		{
			local_12 = arg_0->field_20 * local_12;
			function_294240(arg_3, 0.0f - local_12, &local_9);
			if (local_12 > local_0)
				local_0 = local_12;
		}
	}
	if (local_0 > 0.0f)
	{
		function_30bf0(arg_3);
		real local_13 = arg_1->field_70 * local_0;
		arg_3->i *= local_13;
		arg_3->j *= local_13;
		arg_3->k *= local_13;
		*arg_4 = local_0;
		real local_14 = local_0 > 1.0f ? 1.0f : local_0;
		*arg_5 = (arg_1->field_58 - arg_1->field_54) * local_14 + arg_1->field_54;
	}
}
