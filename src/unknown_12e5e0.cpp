// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <string.h>

struct s_fog_state;
struct s_fog_plane_view;
struct s_12efb0;
void function_12efb0(long arg_0, point3f const *arg_1, vector3f const *arg_2, s_12efb0 *arg_3);
bool function_12fea0(s_fog_state *state, long cluster_index, point3f const *point, vector3f const *normal);
void function_1301c0(s_fog_state *fog);
void function_1305d0(point3f const *point, long cluster_index, s_fog_plane_view *fog, bool force);
void function_130bb0(s_fog_state *fog);
long function_130de0(s_fog_state *fog);

struct s_12e5e0
{
	long field_0;
	color3f field_4;
	byte field_10;
	bool field_11;
	byte field_12[2];
	real field_14;
	bool field_18;
	byte field_19[3];
	color3f field_1c;
	real field_28;
	real field_2c;
	real field_30;
	color3f field_34;
	real field_40;
	real field_44;
	real field_48;
	byte field_4c[0xc];
	real field_58;
	color3f field_5c;
	real field_68;
	long field_6c;
	byte field_70[0x28];
	bool field_98;
	byte field_99[3];
	long field_9c;
	byte field_a0[0xc];
	real field_ac;
	byte field_b0[0x30];
	real field_e0;
	byte field_e4[8];
	real field_ec;
	byte field_f0[0x1c];
	real field_10c;
	byte field_110[4];
	bool field_114;
	byte field_115[3];
	real field_118;
	real field_11c;
};

struct s_12e5e1
{
	byte field_0 : 1;
	byte field_1 : 1;
	byte field_2 : 6;
};

bool g_4e64b8;
bool g_4e64b9;
bool g_4e64ba;
point3f g_4e64bc;
extern real g_4e6738;
extern real g_4e673c;

static __forceinline real function_12e5e1(real arg_0)
{
	if (arg_0 < 0.0f) arg_0 = 0.0f;
	else if (arg_0 > 1.0f) arg_0 = 1.0f;
	return arg_0;
}

// @retail 0x12e5e0
void function_12e5e0(long arg_1, byte *arg_6, point3f const *arg_2, vector3f const *arg_3, byte arg_4, bool arg_5)
{
	s_12e5e0 *arg_0 = (s_12e5e0 *)arg_6;
	memset(arg_0, 0, sizeof(*arg_0));
	arg_0->field_6c = NONE;
	arg_0->field_9c = NONE;
	arg_0->field_10 = arg_4;
	if (g_4e0348 && g_4e0350 && arg_1 != NONE && arg_5)
	{
		g_4e64bc = *arg_2;
		function_12efb0(arg_1, arg_2, arg_3, (s_12efb0 *)arg_0);
		bool local_0 = function_12fea0((s_fog_state *)arg_0, arg_1, arg_2, arg_3);
		function_1301c0((s_fog_state *)arg_0);
		function_1305d0(arg_2, arg_1, (s_fog_plane_view *)arg_0, local_0);
		function_130bb0((s_fog_state *)arg_0);
		long local_1 = function_130de0((s_fog_state *)arg_0);
		arg_0->field_0 = local_1;
		if (arg_4)
		{
			if (arg_0->field_68 < 1.0f)
			{
				arg_0->field_10 = true;
				arg_0->field_11 = arg_0->field_68 > 0.0f;
			}
			else
			{
				arg_0->field_10 = false;
				arg_0->field_4 = arg_0->field_5c;
				arg_0->field_58 = 1.0f;
			}
		}
		else if (arg_0->field_ac == 0.0f)
		{
			if (arg_0->field_28 == 1.0f && (arg_0->field_40 == 0.0f || arg_0->field_11c == 1.0f))
			{
				arg_0->field_4 = arg_0->field_1c;
				arg_0->field_14 = arg_0->field_30;
				arg_0->field_18 = true;
			}
			else if (arg_0->field_40 == 1.0f && (arg_0->field_28 == 0.0f || arg_0->field_11c == 0.0f))
			{
				arg_0->field_4 = arg_0->field_34;
				arg_0->field_14 = arg_0->field_48;
				arg_0->field_18 = true;
			}
			else if (arg_0->field_28 == 1.0f || arg_0->field_40 == 1.0f)
			{
				real local_3 = arg_0->field_28;
				real local_2 = arg_0->field_40;
				real local_7 = (1.0f - arg_0->field_11c) * local_2;
				real local_6 = arg_0->field_11c * local_3;
				real local_4 = (1.0f - local_7) * local_3;
				real local_5 = (1.0f - local_6) * local_2;
				color3f const &first = arg_0->field_1c;
				color3f const &second = arg_0->field_34;
				arg_0->field_4.red = function_12e5e1(local_4 * first.red + second.red * local_5);
				arg_0->field_4.green = function_12e5e1(local_4 * first.green + second.green * local_5);
				arg_0->field_4.blue = function_12e5e1(local_4 * first.blue + second.blue * local_5);
				arg_0->field_14 = arg_0->field_30 > arg_0->field_48 ? arg_0->field_30 : arg_0->field_48;
				arg_0->field_18 = true;
			}
		}
		if (local_1 == 3 && arg_0->field_9c != NONE && arg_0->field_10c == 0.0f && arg_0->field_118 == 0.0f)
		{
			if ((bool)((s_12e5e1 *)g_4e3b44[arg_0->field_9c & 0xffff].bytes)->field_0)
				arg_0->field_114 = true;
		}
		if (arg_0->field_6c != NONE)
			arg_0->field_98 = (bool)((s_12e5e1 *)g_4e3b44[arg_0->field_6c & 0xffff].bytes)->field_1;
		g_4e6738 = arg_0->field_e0;
		g_4e673c = arg_0->field_ec;
	}
	g_4e64b8 = false;
	g_4e64b9 = false;
	g_4e64ba = false;
}
