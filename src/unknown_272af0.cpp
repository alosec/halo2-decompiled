#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
// @flags /O2 /arch:SSE /Gr

struct s_bsp3d;
long function_14a280(s_bsp3d *arg_0, long arg_2, point3f *arg_1);

struct s_272af0
{
	byte field_0[0x18];
	s_bsp3d *field_18;
	byte field_1c[0x14];
	byte *field_30;
};

__forceinline point3f *function_272af3(point3f const *arg_0, vector3f const *arg_1, real arg_2, point3f *arg_3)
{
	arg_3->x = arg_1->i * arg_2 + arg_0->x;
	arg_3->y = arg_1->j * arg_2 + arg_0->y;
	arg_3->z = arg_1->k * arg_2 + arg_0->z;
	return arg_3;
}

#pragma auto_inline(off)
// @retail 0x272af0
short __stdcall function_272af0(s_match_globals *arg_0, point3f const *arg_1)
{
	short local_1 = NONE;
	point3f local_0;
	long local_2 = function_14a280(((s_272af0 *)arg_0)->field_18, 0, function_272af3(arg_1, g_4687b0, 0.1f, &local_0));
	if (local_2 != NONE)
		local_1 = *(short *)(((s_272af0 *)arg_0)->field_30 + local_2 * 8);
	return local_1;
}
#pragma auto_inline(on)
