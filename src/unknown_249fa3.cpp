// @flags /O1 /Gr
#include "unknown_11c920.h"
#include "loop_allocator.h"
#include "unknown_19b516.h"

/* the characters whose glyphs are loaded ahead of time */
word const g_459a60[28] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0 };
word const g_459a98[28] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0 };
word const g_459ad0[12] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0 };

bool function_13ee20(word const *string, long font);
long g_470a60;
long g_51ea10;

void function_24a190(void *p);
void function_249fcc(c_unknown_249fa3 *p);
void function_24a150(void *p);

// @retail 0x18f3d4 deleting c_unknown_249fa3

// @retail 0x18f40e destructor c_unknown_249fa3

// @retail 0x249fa3
void c_unknown_249fa3::v1()
{
	g_470a60 = NONE;
	function_24a190(this);
	((c_widget *)this)->c_widget::v9();
	function_249fcc(this);
}

// @retail 0x249fc0
void c_unknown_249fa3::v2()
{
	g_470a60 = NONE;
	((c_widget *)this)->c_widget::v10();
}

// @retail 0x24a01f
void c_unknown_249fa3::v3()
{
	function_13ee20(g_459a98, 1);
	function_13ee20(g_459ad0, 1);
	function_13ee20(g_459a60, 1);

	if (g_51ea10 <= 0)
	{
		if ((field8c != NONE && g_54e5d0.profile_index == NONE) || (field8a && g_470a60 == NONE))
		{
			function_24a150(this);
			field8a = false;
		}
		field8c = g_54e5d0.profile_index;
		v24();
	}
	((c_widget *)this)->c_widget::v11();
}
/* ---- lane O ---- */

bool g_51ec90;

bool function_8d7c0(void);
void __stdcall function_148b27(long index);
extern "C" unsigned long __stdcall XGetAutoLogonFlag(void);

// @retail 0x24aad1
bool __stdcall function_24aad1(long)
{
	function_148b27(g_470a60);
	g_470a60 = NONE;
	return true;
}

/* true when the console should sign in automatically */
// @retail 0x24aae8
bool function_24aae8()
{
	bool result = false;

	if (!g_51ec90 && function_8d7c0() && XGetAutoLogonFlag() == 1)
		result = true;

	return result;
}