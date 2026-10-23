// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_11cb00.h"

long function_11f9a0(box3f const *rectangle,
	long maximum_vertex_count, point3f vertices[]);

typedef point3f rectangle3d_edge[2];

PRIVATE __forceinline void function_11f771(box3f *arg_0, point3f const *arg_1)
{
	if (arg_0->x0 > arg_1->x)
		arg_0->x0 = arg_1->x;
	if (arg_1->x > arg_0->x1)
		arg_0->x1 = arg_1->x;
	if (arg_0->y0 > arg_1->y)
		arg_0->y0 = arg_1->y;
	if (arg_1->y > arg_0->y1)
		arg_0->y1 = arg_1->y;
	if (arg_0->z0 > arg_1->z)
		arg_0->z0 = arg_1->z;
	if (arg_1->z > arg_0->z1)
		arg_0->z1 = arg_1->z;
}

// @retail 0x11f770
box3f *function_11f770(box3f *rectangle,
	long point_count, point3f const points[])
{
	for (long i = 0; i < point_count; i++)
	{
		function_11f771(rectangle, &points[i]);
	}
	return rectangle;
}

// @retail 0x11fa40
long function_11fa40(box3f const *rectangle,
	long maximum_edge_count, rectangle3d_edge edges[])
{
	long edge_vertices[12][2] =
	{
		{ 0, 2 }, { 2, 3 }, { 3, 1 }, { 1, 0 },
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
		{ 4, 5 }, { 5, 7 }, { 7, 6 }, { 6, 4 }
	};
	point3f vertices[8];
	function_11f9a0(rectangle, 8, vertices);
	/* Retail always emits twelve edges; the capacity argument is unused. */
	for (long i = 0; i < 12; i++)
	{
		edges[i][0] = vertices[edge_vertices[i][0]];
		edges[i][1] = vertices[edge_vertices[i][1]];
	}
	return 12;
}
