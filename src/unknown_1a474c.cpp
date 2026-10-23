// @flags /O1 /Gr
#include "unknown_11c920.h"
#include "loop_allocator.h"

byte g_47d924[4];
s_loop_allocator *g_51e998;

// @retail 0x1a474c
void __stdcall function_1a474c(long stage)
{
	long size;
	s_loop_allocator *loop;

	if (function_18eea0(stage))
	{
		size = 0x60000;
	}
	else
	{
		size = (stage > 1) ? 0x18000 : 0x100000;
	}

	loop = function_18e1f0(size, (c_memory_source *)g_47d924, "ui memory pool");
	g_51e998 = loop;
	if (loop)
	{
		loop->field3c = 1;
		loop->field3d = 1;
	}
}

// @retail 0x1a479a
void function_1a479a(void)
{
	if (g_51e998)
	{
		function_18e230(g_51e998);
		g_51e998 = 0;
	}
}