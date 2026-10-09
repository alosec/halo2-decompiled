// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_03bcb0.h"
#include "globals.h"
#include <xtl.h>

struct s_widget_quad_2b11;
struct s_float_rect;
struct s_interface_function_context;
extern s_interface_function_context g_5021d0;
struct s_4e6950
{
	byte field_0[0x28];
	long field_28, field_2c;
	byte field_30[0x50];
};
extern s_4e6950 g_4e6950;
void function_1be50(void);
void function_1bd50(void *);
void function_1bbf0(long, long, long, long, long, real);
void function_1cf50(void);
dword function_1cc30(long);
void function_1cd90(void);
#ifdef H2_NATIVE_PORT
extern "C" void h2_render_preview_quad(long bitmap_tag, long bitmap_index,
                                         long layer, void const *vertices);
#endif

struct s_22a664
{
	real field_0, field_4, field_8, field_c;
	real field_10, field_14;
	dword field_18;
};

PRIVATE __forceinline real function_22a665(byte const *arg_0, long arg_1)
{
    real local_0;
    if (*(word const *)(arg_0 + 0xe) & 0x10)
        local_0 = (real)*(short const *)(arg_0 + arg_1);
    else
        local_0 = 1.0f;
    return local_0;
}

// @retail 0x22a664
void function_22a664(s_widget_quad_2b11 const *arg_0, s_float_rect const *arg_1,
	long arg_2, long arg_3, long arg_4)
{
	s_22a664 local_0[4];
	(void)&arg_2; (void)&arg_3; (void)&arg_4;
	byte *local_1 = *(byte **)(g_4e3b44[arg_2 & 0xffff].bytes + 0x48) + arg_3 * 0x74;
#ifndef H2_NATIVE_PORT
	D3DTexture *local_8 = function_3bcb0((s_bitmap_data *)local_1);
	if (local_8)
#else
	/* The native host owns texture lookup from the retail tag/index pair. */
	if (true)
#endif
	{
		real local_2 = function_22a665(local_1, 4);
		real local_3 = function_22a665(local_1, 6);
		real const *local_4 = (real const *)arg_0;
		point2f const *local_5 = (point2f const *)arg_1;
		local_0[0].field_0 = local_5[0].x;
		local_0[0].field_4 = local_5[0].y;
		local_0[1].field_0 = local_5[1].x;
		local_0[1].field_4 = local_5[1].y;
		local_0[2].field_0 = local_5[2].x;
		local_0[2].field_4 = local_5[2].y;
		local_0[3].field_0 = local_5[3].x;
		local_0[3].field_4 = local_5[3].y;
		local_0[0].field_8 = local_0[1].field_8 = local_0[2].field_8 = local_0[3].field_8 = 16777215.0f;
		local_0[0].field_c = local_0[1].field_c = local_0[2].field_c = local_0[3].field_c = 16777215.0f;
		local_0[0].field_10 = local_4[0] * local_2;
		local_0[0].field_14 = local_0[1].field_14 = local_4[2] * local_3;
		local_0[1].field_10 = local_0[2].field_10 = local_4[1] * local_2;
		local_0[2].field_14 = local_0[3].field_14 = local_4[3] * local_3;
		local_0[3].field_10 = local_0[0].field_10;
		local_0[0].field_18 = local_0[1].field_18 = local_0[2].field_18 = local_0[3].field_18 = 0xffffffff;
#ifdef H2_NATIVE_PORT
		h2_render_preview_quad(arg_2, arg_3, arg_4, local_0);
#else
		g_4e6950.field_28 = arg_2;
		g_4e6950.field_2c = arg_3;
		function_1be50();
		function_1bd50(0);
		function_1bd50(&g_5021d0);
		function_1bbf0(arg_4, 0, 1, 0, 0, 100.0f);
		function_1cf50();
		function_1cc30(20);
		function_1cd90();
		D3DDevice_Begin(D3DPT_TRIANGLEFAN);
		D3DDevice_SetVertexDataColor(9, 0xffffffff);
		for (long local_6 = 0; local_6 < 4; ++local_6)
		{
			D3DDevice_SetVertexData2f(3, local_0[local_6].field_10, local_0[local_6].field_14);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, local_0[local_6].field_0,
				local_0[local_6].field_4, local_0[local_6].field_8, local_0[local_6].field_c);
		}
		D3DDevice_End();
#endif
	}
}
