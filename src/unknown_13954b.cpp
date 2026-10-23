// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_13954B.CPP: updates and draws the current user's hud for one
   frame, and the motion sensor at its screen position */

#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <string.h>

struct s_510c4c;
extern s_510c4c *g_510c4c;

/* the parts of the new hud's game state (g_510c4c) read here */
struct s_13954b_hud_view
{
	byte unknown000[0x1bc];
	dword field_1bc;
	byte unknown1c0[0x1d8 - 0x1c0];
	real scale;
};

/* a user's hud state (0x270 bytes, unknown_139296.cpp) */
struct s_13954b_user_hud
{
	byte unknown000[0x3c];
	short field_3c;
	byte unknown03e[0x270 - 0x3e];
};

/* as unknown_139296.cpp defines it */
struct s_4e69d0
{
	long indices[7];
	byte unknown1c[0x270 - 0x1c];
};
extern s_4e69d0 g_4e69d0[4];

/* the motion sensor's screen offset for each screen mode */
struct s_screen_offset
{
	short x;
	short y;
};

s_screen_offset const g_47ff40[3] = { { 41, -42 }, { 31, -31 }, { 31, -31 } };

bool function_13939b();
void function_13ac42(long index);
long function_14de70(long local_player_index);
real function_1591e0(long local_index);
void function_139296(long user_index, real value);
bool __stdcall function_44370(long mode);
bool function_15dea0(void);
bool function_161b60(long player_index);
void function_13992a(byte *arg_1, long arg_2);
bool function_1393f3(long arg_1, byte *arg_2);
void function_1394b9(long arg_1, byte *arg_2);
bool function_15fef0(long player_index);
long function_13a690(long mode);
void function_1396c7(long a, point2f *point);
void motion_sensor_render(long local_player_index, short const *origin);
void function_15ea50(long player_index);
void function_200462(long arg_0);
void function_1e6fe0(long local_player_index);
void function_159880(void);
void function_445d0(void);

#pragma inline_depth(0)
/* "standard" stops LTCG assuming an 8-aligned entry: it gives retail's frame
   (0x27c) and late ebx/edi/esi saves, and keeps 0x1917eb matching */
// @retail 0x13954b standard
void function_13954b(void)
{
	if (function_13939b())
	{
		long index = g_4b9ed8;
		long player = function_14de70(index);
		function_139296(index, function_1591e0(index) * ((s_13954b_hud_view *)g_510c4c)->scale);
		function_44370(0x18);
		if (!function_15dea0() || function_161b60(player))
		{
			s_13954b_user_hud hud;
			function_13992a((byte *)&hud, index);
			if (function_1393f3(index, (byte *)&hud))
			{
				function_1394b9(index, (byte *)&hud);
				memcpy(&g_4e69d0[index], &hud, sizeof(hud));
			}
			else
			{
				function_1394b9(index, (byte *)&g_4e69d0[index]);
			}
			if (function_15fef0(player) && hud.field_3c == NONE)
			{
				long mode = function_13a690(4);
				point2f point;
				function_1396c7(2, &point);
				short origin[2];
				origin[0] = (short)(long)((real)g_47ff40[mode].x + point.x - (real)g_4b9dd2);
				origin[1] = (short)(long)((real)g_47ff40[mode].y + point.y - (real)g_4b9dd0);
				if (index != NONE)
					motion_sensor_render(index, origin);
			}
			function_15ea50(index);
			function_200462(index);
			function_1e6fe0(index);
		}
		function_159880();
		function_445d0();
		((s_13954b_hud_view *)g_510c4c)->field_1bc = 0;
	}
	else
	{
		function_13ac42(g_4b9ed8);
	}
}
