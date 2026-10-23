// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_widget_quad_2b11;
struct s_float_rect;
struct s_4e6950;
extern s_4e6950 g_4e6950;
extern color3f *g_46871c;
void function_1392c5(long);
void function_22a664(s_widget_quad_2b11 const *, s_float_rect const *, long, long, long);

struct s_22bbca
{
	short field_0, field_2, field_4, field_6;
};

struct s_4e6950
{
	color3f field_0;
	byte field_c[0xc];
	real field_18, field_1c;
	byte field_20[0x1c];
	real field_3c;
	byte field_40[0x40];
};

inline void point2f_set(point2f *point, real x, real y)
{
	point->x = x;
	point->y = y;
}

// @retail 0x22bbca
void function_22bbca(s_22bbca const *arg_0, long arg_1, long arg_2, long arg_3,
	long arg_4, real arg_5)
{
	point2f local_0[4];
	real local_1[4];
	(void)&arg_1; (void)&arg_2; (void)&arg_3; (void)&arg_4;
	if (arg_1 != NONE && arg_2 != NONE && arg_4 != NONE && arg_5 > 0.0f)
	{
		local_1[0] = 0.0f;
		local_1[1] = 1.0f;
		local_1[2] = 0.0f;
		local_1[3] = 1.0f;
		for (long local_2 = 0; local_2 < 4; ++local_2)
		{
			bool local_3 = local_2 > 1;
			bool local_4 = local_2 == 1 || local_2 == 2;
			point2f_set(&local_0[local_2], local_4 ? (real)arg_0->field_6 : (real)arg_0->field_2,
				local_3 ? (real)arg_0->field_4 : (real)arg_0->field_0);
		}
		g_4e6950.field_3c = arg_5;
		g_4e6950.field_0 = *g_46871c;
		g_4e6950.field_18 = 0.0f;
		g_4e6950.field_1c = arg_5;
		function_1392c5(arg_4);
		function_22a664((s_widget_quad_2b11 const *)local_0,
			(s_float_rect const *)local_1, arg_2, arg_3, arg_1);
	}
}
