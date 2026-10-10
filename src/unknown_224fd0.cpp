#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <math.h>
#include <stddef.h>

// @flags /O2 /arch:SSE /Gr

struct s_tag_data { long size; byte *address; };
struct s_particle_property
{
	short input_index, range_index;
	word modifier;
	short modifier_index;
	s_tag_data function;
};
struct s_draw_value_cache
{
	real values[17];
	dword field_44;
	void *field_48;
	void *field_4c;
	void const *field_50;
};
struct s_draw_index_span
{
	byte field_0[4];
	word first, count;
};
struct s_draw_mesh_definition
{
	long field_0;
	long orientation;
	byte field_8[0x20-8];
	s_particle_property scale_x, scale_y, scale_z, angle;
	byte field_60[0x80-0x60];
	long span_count;
	s_draw_index_span *spans;
	byte field_88[0x94-0x88];
	word *indices;
	byte field_98[0xd8-0x98];
	dword field_d8;
};
struct s_draw_mesh_context { s_draw_mesh_definition const *definition; long constant_index; };
struct s_draw_mesh_seed { byte field_0[0x1a]; word field_1a; };
struct s_frame_offset { point3f position; vector3f forward; vector3f up; };
extern s_frame_offset g_485618;
typedef char check_draw_mesh_mask[offsetof(s_draw_mesh_definition, field_d8)==0xd8?1:-1];

void __stdcall function_173ba0(dword mask, void *a, void *b, void const *c, real *values);
real function_246cd0(s_particle_property const *property, real const *values);
real function_30bf0(vector3f *v);
vector3f *function_11d000(vector3f const *v, vector3f *out);
matrix3x3 *function_142eb0(matrix3x3 const *a, matrix3x3 const *b, matrix3x3 *out);
void function_1cf50();

PRIVATE inline void draw_cross(vector3f const *a, vector3f const *b, vector3f *out)
{
	out->i = a->j*b->k-a->k*b->j;
	out->j = a->k*b->i-a->i*b->k;
	out->k = a->i*b->j-a->j*b->i;
}

PRIVATE __forceinline void draw_rotation(vector3f const *axis, real sine, real cosine, matrix3x3 *out)
{
	real i2=axis->i*axis->i, j2=axis->j*axis->j, k2=axis->k*axis->k;
	real ij=(1.0f-cosine)*axis->i*axis->j;
	real ik=(1.0f-cosine)*axis->k*axis->i;
	real jk=(1.0f-cosine)*axis->k*axis->j;
	out->forward.i=(1.0f-i2)*cosine+i2;
	out->left.i=ij-sine*axis->k;
	out->forward.j=ij+sine*axis->k;
	out->left.j=(1.0f-j2)*cosine+j2;
	out->forward.k=ik-sine*axis->j;
	out->up.i=ik+sine*axis->j;
	out->up.k=(1.0f-k2)*cosine+k2;
	out->up.j=jk-sine*axis->i;
	out->left.k=jk+sine*axis->i;
}

// @retail 0x224fd0
void __stdcall function_224fd0(s_draw_mesh_seed const *seed, s_draw_value_cache *values,
	point3f const *position, vector3f const *direction, real scale, color4f const *color,
	s_draw_mesh_context const *context)
{
	s_draw_mesh_seed const *const *seed_reference=&seed;
	s_draw_value_cache *const *values_reference=&values;
	s_draw_mesh_definition const *definition=context->definition;
	s_draw_index_span const *span=&definition->spans[(*seed_reference)->field_1a%definition->span_count];
	dword mask=definition->field_d8&0xf80f;
	function_173ba0(~values->field_44&mask,values->field_48,values->field_4c,values->field_50,(*values_reference)->values);
	values->field_44|=mask;
	real angle=function_246cd0(&definition->angle,values->values)*6.2831854820251465f;
	real sine=(real)sin(angle), cosine=(real)cos(angle);
	vector3f scaling;
	scaling.i=function_246cd0(&definition->scale_x,values->values)*scale;
	scaling.j=function_246cd0(&definition->scale_y,values->values)*scale;
	scaling.k=function_246cd0(&definition->scale_z,values->values)*scale;
	matrix3x3 rotation, basis;
#ifdef H2_NATIVE_PORT
	/* Avoid retaining a compiler-emitted switch table as a false external
	   provider while preserving the exact orientation-to-axis mapping. */
	__assume(definition->orientation >= 0 && definition->orientation <= 4);
	if (definition->orientation == 2 || definition->orientation == 4)
		draw_rotation(g_4687b0,sine,cosine,&rotation);
	else
		draw_rotation(g_4687ac,sine,cosine,&rotation);
#else
	switch(definition->orientation)
	{
	case 0: draw_rotation(g_4687ac,sine,cosine,&rotation); break;
	case 1: draw_rotation(g_4687ac,sine,cosine,&rotation); break;
	case 2: draw_rotation(g_4687b0,sine,cosine,&rotation); break;
	case 3: draw_rotation(g_4687ac,sine,cosine,&rotation); break;
	case 4: draw_rotation(g_4687b0,sine,cosine,&rotation); break;
	default: __assume(0);
	}
#endif
	rotation.forward.i*=scaling.i; rotation.forward.j*=scaling.i; rotation.forward.k*=scaling.i;
	rotation.left.i*=scaling.j; rotation.left.j*=scaling.j; rotation.left.k*=scaling.j;
	rotation.up.i*=scaling.k; rotation.up.j*=scaling.k; rotation.up.k*=scaling.k;
	switch(definition->orientation)
	{
	case 0:
		basis.forward.i=-g_485618.forward.i;
		basis.forward.j=-g_485618.forward.j;
		basis.forward.k=-g_485618.forward.k;
		basis.up=g_485618.up;
		draw_cross(&basis.up,&basis.forward,&basis.left);
		break;
	case 1:
	case 2:
		basis.up=*direction;
		function_30bf0(&basis.up);
		function_11d000(&basis.up,&basis.forward);
		draw_cross(&basis.up,&basis.forward,&basis.left);
		break;
	case 3:
	case 4:
		basis.up=*g_4687b0;
		vector3f facing=g_485618.forward;
		draw_cross(&basis.up,&facing,&basis.left);
		draw_cross(&basis.left,&basis.up,&basis.forward);
		break;
	}
	function_142eb0(&rotation,&basis,&rotation);
	word *indices=definition->indices;
	D3DDevice_SetVertexData4f(12,rotation.forward.i,rotation.left.i,rotation.up.i,position->x);
	D3DDevice_SetVertexData4f(13,rotation.forward.j,rotation.left.j,rotation.up.j,position->y);
	D3DDevice_SetVertexData4f(14,rotation.forward.k,rotation.left.k,rotation.up.k,position->z);
	if(context->constant_index!=NONE)
		D3DDevice_SetVertexData4f(context->constant_index,color->red,color->green,color->blue,color->alpha);
	word count=span->count, first=span->first;
	function_1cf50();
	D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLELIST,count,indices+first);
}
