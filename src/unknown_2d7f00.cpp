// @flags /O2 /GL-
#include "unknown_146a20.h"
#include "sized_reference_a9ef0.h"
class error_handler_2d97d0;
class reporter_2ff210;
void __cdecl function_2d7dd0();
void __cdecl function_2d73f0(error_handler_2d97d0 *replacement);
void __cdecl function_2d7460(reporter_2ff210 *replacement);
void __cdecl function_2d74d0(c_sized_reference_a9ef0 *replacement);
void __cdecl function_2d6fe0(c_havok_abstract_allocator *replacement);
unsigned char g_48014c;
// @retail 0x2d7f00
int __cdecl function_2d7f00()
{
	if (g_48014c == 1)
	{
		function_2d7dd0();
		function_2d73f0(0);
		function_2d7460(0);
		function_2d74d0(0);
		function_2d6fe0(0);
		g_48014c = 0;
	}
	return 0;
}
