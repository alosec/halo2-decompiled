// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "timed_effect.h"
#include <string.h>

struct s_object_list;
struct s_unknown_13bf00;
struct s_unknown_78;
extern s_object_list *g_4de2f4;
extern s_unknown_13bf00 *g_510c50;
extern s_unknown_78 *g_510c6c;
extern byte g_509415;
byte *g_50934c;

struct s_object_reset_flags
{
	byte unknown00[0x80];
	bool flag80;
	bool flag81;
};

struct s_interface_reset_flags
{
	real value;
	bool flag4;
	bool flag5;
};

// @retail 0x13ca80
void function_13ca80(void)
{
	s_object_reset_flags *objects = (s_object_reset_flags *)g_4de2f4;
	s_index_table *players = g_4e8c20;
	objects->flag81 = false;
	objects->flag80 = false;
	g_4e6948->flag11f8 = false;
	s_interface_reset_flags *interface_state = (s_interface_reset_flags *)g_510c50;
	interface_state->flag4 = false;
	players->unknown00[6] = false;
	g_4f55d0->unknown20 = true;
	interface_state->flag5 = false;
	if (g_5093e0)
	{
		memset(g_5093e0, 0, sizeof(*g_5093e0));
		g_5093e0->unknown180[0] = 1.0f;
		g_5093e0->unknown180[1] = 1.0f;
		g_5093e0->unknown180[2] = 1.0f;
		g_5093e0->unknown180[3] = 1.0f;
		g_5093e0->unknown3f8 = 1.0f;
		*(real *)g_5093e0->unknown190 = 0.0f;
	}
	*g_50934c = false;
	memset((byte *)g_510c6c + 4, 0, 4 * sizeof(long));
	g_509415 = false;
}

void function_f8190(void);

// @retail 0x13bff0
void function_13bff0(void)
{
	s_object_reset_flags *local_1 = (s_object_reset_flags *)g_4de2f4;
	s_ai_globals *local_2 = g_4f55d0;
	local_1->flag81 = true;
	local_1->flag80 = true;
	g_4e8c20->unknown00[6] = true;
	s_interface_reset_flags *local_3 = (s_interface_reset_flags *)g_510c50;
	local_2->unknown20 = false;
	local_3->flag4 = true;
	local_3->flag5 = true;
	function_f8190();
	*g_50934c = true;
	g_4e6948->flag11f8 = true;
	g_4ed288->flag240 = true;
	g_509415 = false;
}
