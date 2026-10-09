#include "unknown_11c920.h"
#include "unknown_0259d0.h"

// @flags /O2 /arch:SSE /Gr /Ob1

struct s_bsp3d;
struct s_source_245400;
struct s_collection_245270;
struct s_line_list;
short function_1ddda0(s_bsp3d const *arg_0, long arg_1, point3f *arg_2);
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, short plane_index, plane3f *plane);
void function_245270(real thickness, short count, point3f const *points, plane3f const *plane,
	real height, long field_00, long field_04, long field_08, byte field_0c,
	byte field_0d, short field_0e, s_collection_245270 *collection);
void function_245400(long index, s_source_245400 const *source,
	transform4x3f const *matrix, real height, real radius, long field_00,
	long field_04, s_line_list *list);
void function_245540(long arg_0, s_source_245400 const *arg_1,
	transform4x3f const *arg_2, real arg_3, real arg_4,
	long arg_5, long arg_6, s_collection_245270 *arg_7);

struct s_245850
{
	short field_00;
	short field_02;
	byte field_04;
	byte field_05;
	short field_06;
};

#pragma inline_depth(0)
// @retail 0x245850
void function_245850(long arg_0, s_source_245400 const *arg_1,
	transform4x3f const *arg_2, real arg_3, real arg_4,
	long arg_5, long arg_6, s_collection_245270 *arg_7)
{
	s_245850 const *local_0 = &(*(s_245850 const *const *)((byte const *)arg_1 + 0x2c))[arg_0];
	point3f local_1[8];
	short local_2 = function_1ddda0((s_bsp3d const *)arg_1, arg_0, local_1);
	plane3f local_3;
	bsp3d_get_plane((s_bsp3d const *)arg_1, local_0->field_00, &local_3);
	if (arg_2)
	{
		for (short local_4 = 0; local_4 < local_2; ++local_4)
		{
			real local_5 = local_1[local_4].x;
			real local_6 = local_1[local_4].y;
			real local_7 = local_1[local_4].z;
			if (arg_2->scale != 1.f)
			{
				local_5 = arg_2->scale * local_5;
				local_6 = arg_2->scale * local_6;
				local_7 = arg_2->scale * local_7;
			}
			local_1[local_4].x = arg_2->up.i * local_7 + arg_2->left.i * local_6 + local_5 * arg_2->forward.i + arg_2->position.x;
			local_1[local_4].y = arg_2->forward.j * local_5 + arg_2->up.j * local_7 + arg_2->left.j * local_6 + arg_2->position.y;
			local_1[local_4].z = arg_2->forward.k * local_5 + arg_2->up.k * local_7 + arg_2->left.k * local_6 + arg_2->position.z;
		}
		real local_5 = local_3.i;
		real local_6 = local_3.j;
		real local_7 = local_3.k;
		local_3.i = arg_2->forward.i * local_5 + arg_2->up.i * local_7 + arg_2->left.i * local_6;
		local_3.j = arg_2->forward.j * local_5 + arg_2->up.j * local_7 + arg_2->left.j * local_6;
		local_3.k = arg_2->left.k * local_6 + arg_2->forward.k * local_5 + arg_2->up.k * local_7;
		local_3.d = arg_2->position.z * local_3.k + arg_2->position.y * local_3.j + arg_2->position.x * local_3.i + arg_2->scale * local_3.d;
	}
	function_245270(arg_4, local_2, local_1, &local_3, arg_3, arg_5, arg_6, arg_0,
		local_0->field_04, local_0->field_05, local_0->field_06, arg_7);
}
#pragma inline_depth(255)

struct s_245aa0
{
	long field_00;
	long field_04[256];
	long field_404;
	long field_408[256];
	long field_808;
	long field_80c[256];
};

// @retail 0x245aa0
void function_245aa0(s_245aa0 const *arg_0, s_source_245400 const *arg_1,
	transform4x3f const *arg_2, real arg_3, real arg_4,
	long arg_5, long arg_6, s_collection_245270 *arg_7)
{
	long local_0;
	for (local_0 = 0; local_0 < arg_0->field_808; ++local_0)
		function_245400(arg_0->field_80c[local_0], arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, (s_line_list *)arg_7);
	for (local_0 = 0; local_0 < arg_0->field_404; ++local_0)
		function_245540(arg_0->field_408[local_0], arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
	for (local_0 = 0; local_0 < arg_0->field_00; ++local_0)
		function_245850(arg_0->field_04[local_0], arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
}
